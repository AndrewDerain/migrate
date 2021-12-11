#pragma once

#include "protocol.h"
#include "lock_free_cyclic_queue.h"

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <memory>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>


_MANGO_PACK_PUSH
_MANGO_DETAIL_NAMESPACE_BEGIN
namespace interprocess
{
    namespace eipc
    {
        using namespace mango::interprocess::eipc;

        /// @brief 本机进程间通信对象
        /// 创建此对象的线程与读取数据的线程可以不为同一个
        class basic_eipc
        {
        public:
            typedef boost::interprocess::mapped_region         mapped_region;
            typedef boost::interprocess::shared_memory_object  shared_memory_object;

            typedef lock_free_cyclic_queue::read_result_t    read_result_t;
            typedef	lock_free_cyclic_queue::write_result_t   write_result_t;

        protected:
            /// @brief 客户端缓冲区，服务端写，客户端读
            lock_free_cyclic_queue m_first_buffer_queue;
            /// @brief 服务端缓冲区，服务端读，客户端写
            lock_free_cyclic_queue m_second_buffer_queue;

        private:
            // message queue
            std::string m_mq_name; // name of message queue
            boost::interprocess::message_queue* m_mq;

            // shared memory object
            std::string m_smo_name; // name of shared memory object
            shared_memory_object m_smo;
            mapped_region m_first_region;
            mapped_region m_second_region;

        public:
            basic_eipc()
            {
                m_mq = nullptr;
                m_mq_name = "";
                m_smo_name = "";
            }
            ~basic_eipc()
            {
                //_message_queue_release();
                //_shared_memory_release();
            }

        protected:
            std::string smo_name() { return m_smo_name; }
            std::string mq_name() { return m_mq_name; }

            bool _open_shared_memory(std::string smo_name,
                                     size_t first_buffer_size,
                                     size_t second_buffer_size)
            {
                m_smo_name = smo_name;

                // create shared memory object
                m_smo = shared_memory_object(
                    boost::interprocess::open_only,
                    m_smo_name.c_str(),
                    boost::interprocess::mode_t::read_write);

                // set or check size
                size_t smo_size = 0;
                if (m_smo.get_size(smo_size))
                {
                    if (smo_size < (first_buffer_size + second_buffer_size))
                        return false;
                }
                else return false;

                // bind region
                m_first_region = mapped_region(m_smo,
                                               boost::interprocess::mode_t::read_write,
                                               0, first_buffer_size);

                m_second_region = mapped_region(m_smo,
                                                boost::interprocess::mode_t::read_write,
                                                first_buffer_size, second_buffer_size);

                // set queue
                m_first_buffer_queue.config(m_first_region.get_address(), m_first_region.get_size());
                m_second_buffer_queue.config(m_second_region.get_address(), m_second_region.get_size());

                return true;
            }

            bool _create_shared_memory(std::string smo_name,
                                       size_t first_buffer_size,
                                       size_t second_buffer_size)
            {
                m_smo_name = smo_name;
                shared_memory_object::remove(m_smo_name.c_str());

                // create shared memory object
                m_smo = shared_memory_object(
                    boost::interprocess::create_only,
                    m_smo_name.c_str(),
                    boost::interprocess::mode_t::read_write);

                size_t smo_size = first_buffer_size + second_buffer_size;
                m_smo.truncate(smo_size);

                // check size
                smo_size = 0;
                if (m_smo.get_size(smo_size))
                {
                    if (smo_size < (first_buffer_size + second_buffer_size))
                        return false;
                }
                else return false;

                // bind region
                m_first_region = mapped_region(m_smo,
                                               boost::interprocess::mode_t::read_write,
                                               0, first_buffer_size);

                m_second_region = mapped_region(m_smo,
                                                boost::interprocess::mode_t::read_write,
                                                first_buffer_size, second_buffer_size);

                // set queue
                m_first_buffer_queue.init(m_first_region.get_address(), m_first_region.get_size());
                m_second_buffer_queue.init(m_second_region.get_address(), m_second_region.get_size());
                return true;
            }

            bool _open_message_queue(std::string message_queue_name,
                                     unsigned long long max_num_msg = 50,
                                     unsigned long long max_msg_size = sizeof(protocol::package))
            {
                m_mq_name = message_queue_name;

                m_mq = new boost::interprocess::message_queue(
                    boost::interprocess::open_only,
                    message_queue_name.c_str());

                if (!m_mq) return false;

                return true;
            }

            bool _create_message_queue(std::string message_queue_name,
                                       unsigned long long max_num_msg = 50,
                                       unsigned long long max_msg_size = sizeof(protocol::package))
            {
                m_mq_name = message_queue_name;
                boost::interprocess::message_queue::remove(m_mq_name.c_str());

                m_mq = new boost::interprocess::message_queue(
                    boost::interprocess::create_only,
                    message_queue_name.c_str(),
                    max_num_msg, max_msg_size);

                if (!m_mq) return false;

                return true;
            }

            void _send_to_mq(_mangodetail::interprocess::eipc::protocol::package& msg)
            {
                m_mq->send(&msg, sizeof(msg), 0);
            }

            void _recive_from_mq(_mangodetail::interprocess::eipc::protocol::package& msg,
                                 boost::ulong_long_type& recvd_size,
                                 unsigned int priority)
            {
                m_mq->receive(&msg, sizeof(msg), recvd_size, priority);
            }

            void _shared_memory_release()
            {
                if (m_smo_name != "")
                    shared_memory_object::remove(m_smo_name.c_str());
                m_smo_name = "";
            }

            void _message_queue_release()
            {
                if (m_mq_name != "")
                    boost::interprocess::message_queue::remove(m_mq_name.c_str());
                m_mq_name = "";

                if (m_mq) delete m_mq;
                m_mq = nullptr;
            }
        };
    }
}
_MANGO_DETAIL_NAMESPACE_END
_MANGO_PACK_POP
