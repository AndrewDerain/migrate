#pragma once

#include "basic_eipc.h"

_MANGO_PACK_PUSH
_MANGO_NAMESPACE_BEGIN
namespace interprocess
{
    namespace eipc
    {
        class client :
            public _mangodetail::interprocess::eipc::basic_eipc
        {
        public:
            client()
            {
                m_inited = false;
            }
            ~client()
            {
                disconnect();
            }

        public:
            /// @brief 
            /// @param server_name 此名字是共享内存对象的名字，即便不是同一个进程，也不能重复
            /// @param client_name 此名字是共享内存对象的名字，即便不是同一个进程，也不能重复
            /// @param buffer_size 共享内存大小
            /// @return 如果连接正常，返回 true
            bool connect(std::string server_name,
                         std::string client_name,
                         size_t buffer_size)
            {
                bool result = true;

                size_t first_buffer_size = buffer_size / 2;
                size_t second_buffer_size = buffer_size - first_buffer_size;

                return _connect(server_name, client_name, first_buffer_size, second_buffer_size);
            }

            bool disconnect()
            {
                if (m_inited)
                {
                    _mangodetail::interprocess::eipc::protocol::package msg = {};
                    msg.req_disconnect.use_this(name());
                    _send_to_mq(msg);
                    _shared_memory_release();
                    m_inited = false;
                }
                return true;
            }

            inline std::string name()
            {
                return smo_name();
            }

            /// @brief 写入数据
            template<typename _Ty>
            inline write_result_t write(tag_t tag, _Ty& data)
            {
                return this->write(tag, sizeof(_Ty), &data);
            }

            /// @brief 写入数据
            inline write_result_t write(tag_t tag, size_t size, void* data)
            {
                return m_second_buffer_queue.write(tag, size, data);
            }

            /// @brief 读取数据
            inline read_result_t fast_read(content_ptr& con)
            {
                return m_first_buffer_queue.fast_read(&con);
            }

            /// @brief 读取数据
            inline read_result_t fast_read(content_ptr& con, bool block, bool& block_quit_flag)
            {
                if (block) { m_first_buffer_queue.read_block(block_quit_flag); }
                return m_first_buffer_queue.fast_read(&con);
            }

            inline void fast_read_release(content_ptr con)
            {
                return m_first_buffer_queue.fast_read_release(con);
            }

        private:
            bool m_inited;

        private:
            bool _connect(std::string server_name,
                          std::string client_name,
                          size_t first_buffer_size,
                          size_t second_buffer_size)
            {
                bool result = true;

                if (m_inited == false)
                {
                    if (first_buffer_size < 50) first_buffer_size = 50;
                    if (second_buffer_size < 50) second_buffer_size = 50;

                    result = result && _create_shared_memory(
                        client_name, first_buffer_size, second_buffer_size);

                    result = result && _open_message_queue(server_name);

                    if (result == false) return result;
                }

                m_inited = true;

                _mangodetail::interprocess::eipc::protocol::package msg = {};
                msg.req_connect.use_this(client_name, first_buffer_size, second_buffer_size);

                _send_to_mq(msg);

                return result;
            }
        };
    } // namespace eipc
} // namespace interprocess
_MANGO_NAMESPACE_END
_MANGO_PACK_POP
