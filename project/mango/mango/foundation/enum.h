///
/// copyright: Copyright (c) 2015—2021 宋豪杰. All rights reserved.
///     email: mirage_project@163.com
///
/// @file enum.h: 
/// @version 3.0.0.0
/// @brief 定义了结构体封装枚举的辅助宏定义
///
///	@date 2019-03-05 by 宋豪杰
///       创建了此文档
///	@date 2020-01-04 by 宋豪杰
///       重新修改实现模式和命名，加入 init_value 用于初始化枚举的初始值
///	@date 2020-04-21 by 宋豪杰
///       去除了默认的构造函数定义，以 assign 替代之。
///       以此来支持 trival 和 standard_layout, 同时避免了不必要的构造
///       函数和析构函数的开销

#pragma once

/// @note 请统一将 enum_name 命名为 value_type, enum_value 命名为 value.
#define enum_basic_operators(wrapper_name, enum_name, enum_value, init_value)                                   \
    private:                                                                                                    \
        enum_name enum_value;                                                                                   \
                                                                                                                \
    public:                                                                                                     \
        inline void assign()                         { enum_value = init_value; }                               \
        inline void assign(const wrapper_name& val)  { this->enum_value = val.enum_value; }                     \
        inline void assign(const enum_name val)      { this->enum_value = val; }                                \
                                                                                                                \
        inline void operator= (const wrapper_name& val)        { this->enum_value = val.enum_value; }           \
        inline bool operator==(const wrapper_name& val)  const { return (this->enum_value == val.enum_value); } \
        inline bool operator< (const wrapper_name& val)  const { return (this->enum_value < val.enum_value);  } \
                                                                                                                \
        inline void operator= (const enum_name val)            { this->enum_value = val; }                      \
        inline bool operator==(const enum_name val)      const { return (this->enum_value == val);}             \
        inline bool operator< (const enum_name val)      const { return (this->enum_value < val); }             \
                                                                                                                \
        inline operator enum_name() const { return this->enum_value;}


#define enum_to_str_switch_case(element) case element: return #element;
#define enum_to_str_switch_default(element) default: return #element;

#define enum_from_str_case(input, enum_value, element) if(input.find(#element) != std::string::npos){ enum_value = element; return;}
#define enum_from_str_default(input, enum_value, element) enum_value = element;
