///
/// copyright: Copyright (c) 2017—2021 宋豪杰. All rights reserved.
///     email: mirage_project@163.com
/// 
///	@date 2017-02-02 by 宋豪杰
///       创建了此文档 
#pragma once

#include <type_traits>

#define _MANGO_TYPE_NAME_TO_STR(type, ...) #type##__VA_ARGS__

//! @breif 此宏定义用于判断某个类型是否同时满足标准布局（standard_layout）
//!     和 平凡（trivial）。
#define MANGO_VERIFY_PLAN_OLD_DATA(type, ...)                                              \
    static_assert(std::is_standard_layout<type##__VA_ARGS__>::value,                       \
        _MANGO_TYPE_NAME_TO_STR(type, #__VA_ARGS__) " : type must be standard layout!");   \
                                                                                           \
    static_assert(std::is_trivial<type##__VA_ARGS__>::value,                               \
        _MANGO_TYPE_NAME_TO_STR(type, #__VA_ARGS__) " : type must be trivial!");

#define _MANGO_STANDARD_LAYOUT_CALIBRATOR(type, ...)                                       \
    static_assert(std::is_standard_layout<type##__VA_ARGS__>::value,                       \
        _MANGO_TYPE_NAME_TO_STR(type, #__VA_ARGS__) " : type must be standard layout!");   \

#define _MANGO_TRIVIAL_CALIBRATOR(type, ...)                                               \
    static_assert(std::is_trivial<type##__VA_ARGS__>::value,                               \
        _MANGO_TYPE_NAME_TO_STR(type, #__VA_ARGS__) " : type must be trivial!");
