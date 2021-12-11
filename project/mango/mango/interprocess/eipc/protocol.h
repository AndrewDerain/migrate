#pragma once

#include "../../utility/fixed_string.h"
#include "basic_type.h"

#include <string>


_MANGO_PACK_PUSH
_MANGO_DETAIL_NAMESPACE_BEGIN
namespace interprocess
{
    namespace eipc
    {
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
    } // namespace eipc
} // namespace interprocess
_MANGO_DETAIL_NAMESPACE_END
_MANGO_PACK_POP
