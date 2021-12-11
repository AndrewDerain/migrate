#pragma once

#include "content.h"
#include <thread>


_MANGO_PACK_PUSH
_MANGO_DETAIL_NAMESPACE_BEGIN
namespace interprocess
{
    namespace eipc
    {
        /// @brief
        /// @note 多线程安全性说明
        ///     1) 不要让多个 _basic_memory_cyclic_buffer 指向同一个内存区域，否则无法保证多线程安全
        ///     2) 本缓冲区是无锁缓冲区，仅单生产者单消费者模式下（各使用一个线程，
        ///        总共两个线程访问）可以保证多线程安全
        class _basic_lock_free_cyclic_queue
        {
        public:
            enum class write_result_t
            {
                noerror,
                not_ready_to_use,
                buffer_is_full,
                other_error
            };

            enum class read_result_t
            {
                noerror,
                not_ready_to_use,
                nodata,
                other_error
            };

        public:
            _basic_lock_free_cyclic_queue(_basic_lock_free_cyclic_queue&) = delete;
            _basic_lock_free_cyclic_queue& operator=(_basic_lock_free_cyclic_queue&) = delete;
            _basic_lock_free_cyclic_queue& operator=(_basic_lock_free_cyclic_queue&&) = delete;

            _basic_lock_free_cyclic_queue() :
                m_buffer_size(0),
                m_head_data(nullptr),
                m_buffer_start_address(nullptr),
                m_buffer_end_address(nullptr),
                m_data_start_address(nullptr) {}

            /// @brief 如果使用此类的一方不负责初始化 start_address 所指向的内存
            ///     块那么应该使用这个函数来配置该此类。
            /// @param start_address 此类使用的缓冲区内存块起始地址
            /// @param buffer_size 此类使用的缓冲区内存块大小, 不得小于 48 字节
            /// @note 本类不负责对 start_address 所指向的地址进行释放
            void config(void* start_address, size_t buffer_size);

            /// @brief 如果使用此类的一方负责初始化 start_address 所指向的内存块
            ///     那么应该使用这个函数来配置该内存块，并初始化数据，此类内部会调
            ///     用 config 函数。
            /// @param start_address 此类使用的缓冲区内存块起始地址
            /// @param buffer_size 此类使用的缓冲区内存块大小, 不得小于 40 字节
            /// @note 本类不负责对 start_address 所指向的地址进行释放
            void init(void* start_address, size_t buffer_size);

            /// @brief 此类使用的缓冲区内存起始地址
            inline void* buffer_start_address() const;

            /// @brief 缓冲区的大小
            inline size_t buffer_size() const;

            /// @brief 写入数据
            write_result_t write(tag_t tag, size_t size, void* data);

            /// @param con 传入的空指针，此函数会将缓存中的数据地址直接赋值给这个指针
            ///      而不是拷贝一份
            /// @param 调用者使用完 con 之后需要调用 fast_read_release 函数来将 con
            ///      所指向的缓冲区中的内存重新标记为可用的
            read_result_t fast_read(content** con);

            void fast_read_release(content* con);

            void read_block(bool& quit_flag);

            inline bool is_empty();

        private:
            struct head_data;

        private:
            head_data* m_head_data;

        private:
            size_t m_buffer_size;

            char* m_buffer_start_address;
            char* m_buffer_end_address;
            char* m_data_start_address;

        private:
            inline char* to_pointer(offset_t offset);

            inline offset_t to_offset(char* address);
        };


        struct _basic_lock_free_cyclic_queue::head_data
        {
        public:
            bool ready_to_use; //< 队列的可用状态

            // 当 reader_ptr == writer_ptr 时，表示
            // 当前没有需要读取的数据
            // 当 reader_ptr != writer_ptr 时，表示
            // 当前有数据需要被读取
            offset_t reader_ofs;  //< 读指针
            offset_t writer_ofs;  //< 写指针

            offset_t data_end_ofs; //< 可读数据结束地址
        };

        void _basic_lock_free_cyclic_queue::config(void* start_address, size_t buffer_size)
        {
            m_buffer_size = buffer_size;

            m_buffer_start_address = reinterpret_cast<char*>(start_address);
            m_head_data = reinterpret_cast<head_data*>(m_buffer_start_address);
            m_data_start_address = m_buffer_start_address + sizeof(head_data);
            m_buffer_end_address = m_buffer_start_address + m_buffer_size;
        }

        void _basic_lock_free_cyclic_queue::init(void* start_address, size_t buffer_size)
        {
            memset(start_address, 0, buffer_size);
            config(start_address, buffer_size);

            // m_head_data->writer_ptr == m_head_data->reader_ptr
            // 时表示当前没有需要读取的数据
            m_head_data->writer_ofs = to_offset(m_data_start_address);
            m_head_data->reader_ofs = m_head_data->writer_ofs;
            m_head_data->data_end_ofs = to_offset(m_buffer_end_address);
            m_head_data->ready_to_use = true;
        }

        void* _basic_lock_free_cyclic_queue::buffer_start_address() const
        {
            return reinterpret_cast<void*>(m_buffer_start_address);
        }

        size_t _basic_lock_free_cyclic_queue::buffer_size() const
        {
            return m_buffer_size;
        }

        _basic_lock_free_cyclic_queue::write_result_t
            _basic_lock_free_cyclic_queue::write(tag_t tag, size_t size, void* data)
        {
            if (!m_head_data->ready_to_use)
                return write_result_t::not_ready_to_use;

            write_result_t result = write_result_t::noerror;

            char* const reader_ptr = to_pointer(m_head_data->reader_ofs);
            char* writer_ptr = to_pointer(m_head_data->writer_ofs);
            char* data_end_addr = to_pointer(m_head_data->data_end_ofs);

            // 新数据包的总大小
            size_t data_pkg_size = sizeof(content) + size;

            // 边界检查
            if (reader_ptr <= writer_ptr)
            {
                size_t rest_data_size = data_end_addr - writer_ptr;  // 可能有BUG
                size_t rest_buff_size = m_buffer_end_address - writer_ptr;

                if (rest_data_size < data_pkg_size)
                {
                    data_end_addr = m_buffer_end_address;
                }

                if (rest_buff_size < data_pkg_size)
                {
                    data_end_addr = writer_ptr;

                    // 只有当所有数据都被读取完了的时候，才允许
                    // reader_ptr = m_head_data->writer_ptr
                    // 所以当 reader_ptr 与 m_head_data->writer_ptr
                    // 之间的数据块大小 刚好等于 data_pkg_size 时，不允许写入数据
                    size_t supsize = reader_ptr - m_data_start_address;
                    if (supsize > data_pkg_size)
                        writer_ptr = m_data_start_address;
                    else
                        result = write_result_t::buffer_is_full;
                }
            }
            else // if (reader_ptr > m_head_data->writer_ptr)
            {
                size_t rest_size = reader_ptr - writer_ptr;
                // 只有当所有数据都被读取完了的时候，才允许
                // reader_ptr = m_head_data->writer_ptr
                // 所以当 reader_ptr 与 m_head_data->writer_ptr
                // 之间的数据块大小 刚好等于 data_pkg_size 时，不允许写入数据
                if (rest_size <= data_pkg_size)
                {
                    result = write_result_t::buffer_is_full;
                }
            }

            if (result == write_result_t::noerror)
            {
                // 写入数据
                char* data_addr = writer_ptr + sizeof(content);
                content* item_ptr = reinterpret_cast<content*>(writer_ptr);
                item_ptr->m_tag = tag;
                item_ptr->m_size = size;
                item_ptr->m_beg_addr = m_buffer_start_address;
                item_ptr->m_data = to_offset((char*)memcpy(data_addr, data, size));

                writer_ptr += data_pkg_size;
            }

            m_head_data->data_end_ofs = to_offset(data_end_addr);
            m_head_data->writer_ofs = to_offset(writer_ptr);

            return result;
        }

        _basic_lock_free_cyclic_queue::read_result_t
            _basic_lock_free_cyclic_queue::fast_read(content** con)
        {
            *con = nullptr;

            if (!m_head_data->ready_to_use)
                return read_result_t::not_ready_to_use;

            read_result_t result = read_result_t::noerror;

            char* reader_ptr = to_pointer(m_head_data->reader_ofs);
            char* const writer_ptr = to_pointer(m_head_data->writer_ofs);
            char* const data_end_addr = to_pointer(m_head_data->data_end_ofs);

            if (reader_ptr > writer_ptr)
            {
                if (reader_ptr >= data_end_addr)
                {
                    reader_ptr = m_data_start_address;
                }
            }

            if (reader_ptr == writer_ptr)
                result = read_result_t::nodata;

            // 读取数据
            if (result == read_result_t::noerror)
            {
                *con = reinterpret_cast<content*>(reader_ptr);
                (*con)->m_beg_addr = m_buffer_start_address;
            }

            m_head_data->reader_ofs = to_offset(reader_ptr);

            return result;
        }

        void _basic_lock_free_cyclic_queue::fast_read_release(content* con)
        {
            if (con != nullptr)
            {
                char* reader_ptr = to_pointer(m_head_data->reader_ofs);
                char* const writer_ptr = to_pointer(m_head_data->writer_ofs);
                char* const data_end_addr = to_pointer(m_head_data->data_end_ofs);

                // 新数据包的总大小
                size_t data_pkg_size = sizeof(content) + con->m_size;

                if (reader_ptr < writer_ptr)
                {
                    reader_ptr += data_pkg_size;
                }
                else if (reader_ptr > writer_ptr)
                {
                    size_t size = data_end_addr - reader_ptr;
                    if (size > data_pkg_size)
                        reader_ptr += data_pkg_size;
                    else if (size == data_pkg_size)
                        reader_ptr = m_data_start_address;
                }

                m_head_data->reader_ofs = to_offset(reader_ptr);
            }
        }

        void _basic_lock_free_cyclic_queue::read_block(bool& quit_flag)
        {
            while (!quit_flag && is_empty())
            {
                if (is_empty()) std::this_thread::sleep_for(std::chrono::microseconds(10));
                else return;
            }
        }

        inline bool _basic_lock_free_cyclic_queue::is_empty()
        {
            return (m_head_data->writer_ofs == m_head_data->reader_ofs);
        }
        inline char* _basic_lock_free_cyclic_queue::to_pointer(offset_t offset)
        {
            return m_buffer_start_address + offset;
        }

        inline offset_t	_basic_lock_free_cyclic_queue::to_offset(char* address)
        {
            return address - m_buffer_start_address;
        }
    }
}
_MANGO_DETAIL_NAMESPACE_END
_MANGO_PACK_POP

_MANGO_PACK_PUSH
_MANGO_NAMESPACE_BEGIN
namespace interprocess
{
    namespace eipc
    {
        typedef _mangodetail::interprocess::eipc::_basic_lock_free_cyclic_queue lock_free_cyclic_queue;
    }
}
_MANGO_NAMESPACE_END
_MANGO_PACK_POP
