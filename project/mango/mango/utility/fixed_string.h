#pragma once

#include "../foundation/fixed_string.h"

_MANGO_NAMESPACE_BEGIN
namespace utility
{
    /// fixed_string 是对普通字符数组包装的模板类，使得普通字符数组能够拥有 std::string 的一些能力（更方便安全的赋值，获取长度）；
    /// 这里借助 shadow 模板函数，使得代码可以在不损性能的情况下，让普通的 c 风格 字符串数组 content 被包装成了 fixed_string 类型
    /// 从而可以更方便的进行字符串操作。
    template<typename _Ty, size_t _Size>
    constexpr inline mango::foundation::fixed_string<_Size>& shadow(_Ty(&content)[_Size])
    {
        return *static_cast<mango::foundation::fixed_string<_Size>*>(static_cast<void*>(content));
    }
}
_MANGO_NAMESPACE_END
