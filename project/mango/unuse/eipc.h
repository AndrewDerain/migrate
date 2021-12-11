#pragma once

#include "lock_free_cyclic_queue.h"
#include "../utility/fixed_string.hpp"

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
        using namespace mango::interprocess;

        typedef lock_free_cyclic_queue::size_t           size_t;
        typedef lock_free_cyclic_queue::offset_t         offset_t;


        // 通信协议
        namespace protocol
        {
            enum class id
            {
                server_exit,
                req_connect,
                req_disconnect
            };

            struct alignas(8) server_exit_field
            {
                id msg_id;

                void use_this() { msg_id = id::server_exit; }
            };

            struct alignas(8) req_connect_field
            {
                id msg_id;

                char name[1024];
                size_t first_buffer_size;
                size_t second_buffer_size;

                /// @brief 
                /// @param client_name 客户端的名字，不超过 1024 字节
                /// @param first_buffer_size   第一段缓冲区大小
                /// @param second_buffer_size  第二段缓冲区大小
                void use_this(std::string client_name,
                              size_t first_buf_size,
                              size_t second_buf_size)
                {
                    msg_id = id::req_connect;

                    first_buffer_size = first_buf_size;
                    second_buffer_size = second_buf_size;

                    mango::utility::shadow(name) = client_name.c_str();
                }
            };

            struct alignas(8) req_disconnect_field
            {
                id msg_id;

                char name[1024];

                void use_this(std::string client_name)
                {
                    msg_id = id::req_disconnect;
                    mango::utility::shadow(name) = client_name.c_str();
                }
            };

            union alignas(8) package
            {
                id msg_id;

                server_exit_field    server_exit;
                req_connect_field    req_connect;
                req_disconnect_field req_disconnect;
            };
        } // end of protocol


        /// @brief 客户端角色
        enum role
        {
            server,
            client,
            session
        };


        /// @brief 本机进程间通信对象
        /// 创建此对象的线程与读取数据的线程可以不为同一个
        class basic_eipc
        {
        public:
            typedef boost::interprocess::mapped_region         mapped_region;
            typedef boost::interprocess::shared_memory_object  shared_memory_object;

            typedef lock_free_cyclic_queue::tag_t            tag_t;
            typedef lock_free_cyclic_queue::size_t           size_t;
            typedef lock_free_cyclic_queue::offset_t         offset_t;
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
            typedef lock_free_cyclic_queue::content* content_ptr;

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


        class session :
            public _mangodetail::interprocess::eipc::basic_eipc
        {
        public:
            typedef lock_free_cyclic_queue::content* content_ptr;

        public:
            session()
            {
                m_inited = false;
            }
            ~session()
            {
                disconnect();
            }

        public:
            /// @brief 
            /// @param server_name 此名字是服务端的名字，即便不是同一个进程，也不能重复
            /// @param client_name 此名字是客户端对象的名字，即便不是同一个进程，也不能重复
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
                return m_first_buffer_queue.write(tag, size, data);
            }

            /// @brief 读取数据
            inline read_result_t fast_read(content_ptr& con)
            {
                return m_second_buffer_queue.fast_read(&con);
            }

            /// @brief 读取数据
            inline read_result_t fast_read(content_ptr& con, bool block, bool& block_quit_flag)
            {
                if (block) { m_second_buffer_queue.read_block(block_quit_flag); }
                return m_second_buffer_queue.fast_read(&con);
            }

            inline void fast_read_release(content_ptr con)
            {
                return m_second_buffer_queue.fast_read_release(con);
            }

        private:
            friend class server;

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

                    result = result && _open_shared_memory(
                        client_name, first_buffer_size, second_buffer_size);

                    result = result && _open_message_queue(server_name);

                    if (result == false) return result;
                }

                m_inited = true;

                return result;
            }
        };


        class server :
            protected _mangodetail::interprocess::eipc::basic_eipc
        {
        public:
            typedef lock_free_cyclic_queue::content          content;

        public:
            server() { m_inited = false; }

            ~server()
            {
                release();
            }

            bool bind(std::string server_name,
                      std::function<void(std::shared_ptr<session>)> handler_on_connect,
                      std::function<void(std::shared_ptr<session>)> handler_on_disconnect)
            {
                bool result = true;

                if (m_inited == false)
                {
                    result = _create_message_queue(server_name);

                    if (result == false) return result;
                }

                m_inited = true;

                m_client_handler_on_connect = handler_on_connect;
                m_client_handler_on_disconnect = handler_on_disconnect;
                return result;
            }

            void listen()
            {
                m_listen_thread = std::thread{ &server::_listen, this };
            }

            bool release()
            {
                if (m_inited)
                {
                    _mangodetail::interprocess::eipc::protocol::package msg;
                    msg.server_exit.use_this();
                    _send_to_mq(msg);
                    m_listen_thread.join();
                    _message_queue_release();
                    m_inited = false;
                }
                return true;
            }

            inline std::string name()
            {
                return mq_name();
            }

        private:
            bool m_inited;
            std::thread	m_listen_thread;
            std::function<void(std::shared_ptr<session>)> m_client_handler_on_connect;
            std::function<void(std::shared_ptr<session>)> m_client_handler_on_disconnect;

        private:
            void _listen()
            {
                _mangodetail::interprocess::eipc::protocol::package msg;
                boost::ulong_long_type recvd_size = 0;
                unsigned int priority = 0;
                std::map<std::string, std::shared_ptr<session>> session_map;

                while (true)
                {
                    _recive_from_mq(msg, recvd_size, priority);
                    
                    if (recvd_size != sizeof(msg))
                        continue;

                    switch (msg.msg_id)
                    {
                        case _mangodetail::interprocess::eipc::protocol::id::server_exit:
                            return;
                        case _mangodetail::interprocess::eipc::protocol::id::req_connect:
                            {
                                std::shared_ptr<session> new_client = std::make_shared<session>();

                                new_client->_connect(name(),
                                                     msg.req_connect.name,
                                                     msg.req_connect.first_buffer_size,
                                                     msg.req_connect.second_buffer_size);

                                session_map.insert(std::make_pair(new_client->name(), new_client));
                                m_client_handler_on_connect(new_client);
                                break;
                            }
                        case _mangodetail::interprocess::eipc::protocol::id::req_disconnect:
                            {
                                auto iter = session_map.find(msg.req_disconnect.name);

                                if (iter != session_map.end())
                                {
                                    m_client_handler_on_disconnect(iter->second);
                                    session_map.erase(iter);
                                }
                                break;
                            }
                        default:
                            break;
                    }
                }
            }
        };
    }
}
_MANGO_NAMESPACE_END
_MANGO_PACK_POP
