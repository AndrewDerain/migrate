#pragma once

#include "../config/config.h"
#include <cmath>
#include <limits>

_MANGO_NAMESPACE_BEGIN
namespace algorithm
{
    /// @brief 判断两个 double 类型是否相等
    static inline bool equal(const double left, const double right) {
        return std::fabs(left - right) <=
            std::numeric_limits<double>::epsilon();
    }

    /// @brief 判断两个 double 类型的大小
    /// @detail 如果 left 的值小于等于 right 的值返回true，否则返回 false
    static inline bool less_equal(const double left, const double right) {
        return (left < right || equal(left, right));
    }

    /// @brief 判断两个 double 类型的大小
    /// @detail 如果 left 的值大于等于 right 的值返回true，否则返回 false
    static inline bool greater_equal(const double left, const double right) {
        return (left > right || equal(left, right));
    }
}
_MANGO_NAMESPACE_END
