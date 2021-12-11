#pragma once

#include "client.h"
#include "session.h"


_MANGO_PACK_PUSH
_MANGO_NAMESPACE_BEGIN
namespace interprocess
{
    namespace eipc
    {
        class server :
            protected _mangodetail::interprocess::eipc::basic_eipc
        {
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
