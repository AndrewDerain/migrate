//!
//! @file    property.h:
//! @version 1.0.0.2
//! @brief   属性 
//!
#pragma once

#include "../config/config.h"

#include <cassert>
#include <functional>

_MANGO_PACK_PUSH
_MANGO_NAMESPACE_BEGIN

namespace foundation
{
    template<typename _ValueType>
    class property;
}

namespace detail
{
    namespace foundation
    {
        template<typename _ValueType>
        struct _Property_operator_ptr_get_ref
        {
            static inline const _ValueType& Get(const mango::foundation::property<_ValueType>& _Class) {
                return _Class.Getter();
            }
        };

        template<typename _ValueType>
        struct _Property_operator_ptr_get_ptr
        {
            static inline const _ValueType* const Get(const mango::foundation::property<_ValueType>& _Class) {
                return &_Class.Getter();
            }
        };
    }
}

namespace foundation
{
    /// @brief 可读可写属性，为类提供属性访问，支持在子类中重载
    /// @tparam _ValueType 属性包装的类型，可以是基础类型，也可以是自定义类型
    /// @note 参考 /unit/PropertyDemo.h
    template<typename _ValueType>
    class property
    {
    public:
        using MyType = property<_ValueType>;
        using ValueType = _ValueType;
        using OperatorPtrRtnType = typename std::conditional<std::is_fundamental_v<_ValueType>, _ValueType&, const _ValueType* const>::type;

        using GetterType = std::function<const ValueType& ()>;
        using SetterType = std::function<void(const ValueType&)>;

        friend class mango::detail::foundation::_Property_operator_ptr_get_ref<ValueType>;
        friend class mango::detail::foundation::_Property_operator_ptr_get_ptr<ValueType>;

    public:
        property() = default;

        inline property(ValueType& value_to_bind) {
            _Bind_value(value_to_bind);
        }

        inline property(GetterType& getter, SetterType& setter) {
            _Bind_operator(getter, setter);
        }

        inline property(GetterType&& getter, SetterType&& setter) {
            _Bind_operator(getter, setter);
        }

        inline MyType& operator=(const MyType& right) {
            Setter(right.Getter());
            //Setter = right.Setter;
            //Getter = right.Getter;
            return *this;
        }

        inline MyType& operator=(const MyType&& right) {
            //Setter(right.Getter());
            Setter = std::move(right.Setter);
            Getter = std::move(right.Getter);
            return *this;
        }

        inline MyType& operator=(const ValueType& right) {
            assert(Getter);
            Setter(right);
            return *this;
        }

        inline MyType& operator=(const ValueType&& right) {
            Setter(right);
            return *this;
        }

        inline operator const ValueType& () const { return Getter(); }

        inline OperatorPtrRtnType operator->() {
            return std::conditional<
                std::is_fundamental_v<ValueType>,
                mango::detail::foundation::_Property_operator_ptr_get_ref<ValueType>,
                mango::detail::foundation::_Property_operator_ptr_get_ptr<ValueType>>::type::Get(*this);
        }

    private:
        GetterType Getter;
        SetterType Setter;

        /// @brief 第一种绑定，直接绑定宿主的成员变量
        ///        宿主在值发生变化时不需要任何操作
        /// @param value_to_bind 
        inline void _Bind_value(ValueType& value_to_bind) {
            Getter = [&]()->auto { return value_to_bind; };
            Setter = [&](const ValueType& val) { value_to_bind = val; };
        }

        /// @brief 第二种绑定，直接绑定宿主的 get/set 函数
        ///        宿主在值获取或发生变化时可以进行操作
        /// @param value_to_bind 
        inline void _Bind_operator(GetterType& getter, SetterType& setter) {
            Getter = getter;
            Setter = setter;
        }
    };


    /// @brief 只读属性，为类提供属性访问，支持在子类中重载
    /// @tparam _ValueType 属性包装的类型，可以是基础类型，也可以是自定义类型
    /// @note 参考 /unit/PropertyDemo.h
    template<typename _ValueType>
    class readonly_property
    {
        using MyType = readonly_property<_ValueType>;
        using ValueType = _ValueType;
        using OperatorPtrRtnType = typename std::conditional<std::is_fundamental_v<_ValueType>, _ValueType&, const _ValueType* const>::type;

        using GetterType = std::function<const ValueType& ()>;

        friend class mango::detail::foundation::_Property_operator_ptr_get_ref<ValueType>;
        friend class mango::detail::foundation::_Property_operator_ptr_get_ptr<ValueType>;

    public:
        readonly_property() = default;
        readonly_property(MyType&) = delete;

        MyType& operator=(MyType& right) = delete;
        MyType& operator=(MyType&& right) = delete;
        MyType& operator=(ValueType& right) = delete;
        MyType& operator=(ValueType&& right) = delete;

        inline readonly_property(MyType&& prop) {
            Getter = std::move(prop.Getter);
        }

        inline readonly_property(ValueType& value_to_bind) {
            _Bind_value(value_to_bind);
        }

        inline readonly_property(GetterType& getter) {
            _Bind_operator(getter);
        }

        inline readonly_property(GetterType&& getter) {
            _Bind_operator(getter);
        }

        inline operator const ValueType& () const {
            return Getter();
        }

        inline OperatorPtrRtnType operator->() {
            return std::conditional<
                std::is_fundamental_v<ValueType>,
                mango::detail::foundation::_Property_operator_ptr_get_ref<ValueType>,
                mango::detail::foundation::_Property_operator_ptr_get_ptr<ValueType>>::type::Get(*this);
        }

    private:
        GetterType Getter;

        /// @brief 第一种绑定，直接绑定宿主的成员变量
        ///        宿主在值发生变化时不需要任何操作
        /// @param value_to_bind 
        inline void _Bind_value(ValueType& value_to_bind) {
            Getter = [&]()->ValueType& { return value_to_bind; };
        }

        /// @brief 第二种绑定，直接绑定宿主的 get 函数
        ///        宿主在值获取时可以进行操作
        /// @param value_to_bind 
        inline void _Bind_operator(GetterType& getter) {
            Getter = getter;
        }
    };

#define _setter(variableType) [&](const variableType& value)->void
#define _getter(variableType) [&]()->variableType&

#define setter _setter
#define getter _getter
}
_MANGO_NAMESPACE_END
_MANGO_PACK_POP
