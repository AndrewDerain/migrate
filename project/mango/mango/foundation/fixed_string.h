///
/// @file FixedString.h:
/// @version 1.0.0.2 
/// @brief    calss: fixed_string
///        function: shadow
///
#pragma once

#include "../config/config.h"
#include <array>
#include <cstring>
#include <cassert>
#include <type_traits>

_MANGO_NAMESPACE_BEGIN
namespace foundation
{
    template<size_t _FixedSize>
    class fixed_string;
}
_MANGO_NAMESPACE_END

template<size_t _FixedSize>
inline bool
operator==(const mango::foundation::fixed_string<_FixedSize>& _Left, const char* const _Right);

#pragma pack(push, _CRT_PACKING)
_MANGO_NAMESPACE_BEGIN
namespace foundation
{
    /// @brief 以 '\0' 字符结尾的固定长度字符串
    /// @param _FixedSize 字符串长度，包含 '\0' 字符
    template<size_t _FixedSize>
    class fixed_string :
        public std::array<char, _FixedSize>
    {
    private:
        template<size_t _dest_size, size_t _small_size>
        struct _FixedString_assign_from_smaller {
            inline static void assign(fixed_string<_dest_size>* dest,
                fixed_string<_small_size>& source) {
                strcpy(dest->data(), source.data());
            }
        };

        template<size_t _dest_size, size_t _larger_size>
        struct _FixedString_assign_from_larger {
            inline static void assign(fixed_string<_dest_size>* dest,
                fixed_string<_larger_size>& source) {
                size_t source_len = strnlen(source.c_str(), _larger_size) + 1;

                if (source_len <= _dest_size) {
                    memcpy(dest->_Elems, source.data(), source_len);
                }
                else {
                    memcpy(dest->data(), source.data(), _dest_size);
                    dest->_Elems[_dest_size - 1] = '\0';
                }
            }
        };

        template<size_t _FixedSize>
        friend bool ::operator==(const fixed_string<_FixedSize>&, const char* const);

    public:
        inline void assign(const char* str = "") {
            strncpy(this->data(), str, _FixedSize);
            this->_Elems[_FixedSize - 1] = '\0';
        }

        template<size_t _SourceSize>
        inline void assign(fixed_string<_SourceSize>& source_str) {
            std::conditional <
                (_SourceSize <= _FixedSize),
                _FixedString_assign_from_smaller<_FixedSize, _SourceSize>,
                _FixedString_assign_from_larger<_FixedSize, _SourceSize>
            >::type::assign(this, source_str);
        }

        inline const char* c_str() {
            return this->data();
        }

        inline size_t length() const {
            return strlen(this->data());
        }

        inline int compare(const char* str) {
            return strcmp(this->data(), str);
        }

        inline fixed_string<_FixedSize>& operator=(const char* str) {
            assign(str);
            return *this;
        }

    private:
        inline bool _Equal(const char* const right) const {
            return strcmp(this->data(), right) == 0;
        }
    };
    //DERAIN_VERIFY_PLAN_OLD_DATA(FixedString<0>);
    //DERAIN_VERIFY_PLAN_OLD_DATA(FixedString<1>);

} // namesapce foundation
_MANGO_NAMESPACE_END
#pragma pack(pop)

template<size_t _FixedSize>
inline bool
operator==(const mango::foundation::fixed_string<_FixedSize>& _Left, const char* const _Right) {
    return (_Left._Equal(_Right));
}
