#pragma once

#include "../config/config.h"
#include "../algorithm/double.h"
#include "../foundation/enum.h"

#include <string>

_MANGO_WARNING_DISABLE
_MANGO_PACK_PUSH
_MANGO_NAMESPACE_BEGIN
namespace math
{
    /// 区间属性，开区间，闭区间
    struct interval_variety
    {
        enum value_type
        {
            open, //< 开区间
            close //< 闭区间
        };

        enum_basic_operators(interval_variety, value_type, value, value_type::open);

        std::string to_string()
        {
            switch (value)
            {
                enum_to_str_switch_case(open);
                enum_to_str_switch_case(close);
                enum_to_str_switch_default(open);
            }
        }

        void from_string(std::string input)
        {
            switch (value)
            {
                enum_from_str_case(input, value, open);
                enum_from_str_case(input, value, close);
                enum_from_str_default(input, value, open);
            }
        }
    };


    /// 有界性
    struct boundness
    {
        enum value_type
        {
            bounded,  //< 有界
            unbounded //< 无界
        };

        enum_basic_operators(boundness, value_type, value, value_type::bounded);

        std::string to_string()
        {
            switch (value)
            {
                enum_to_str_switch_case(bounded);
                enum_to_str_switch_case(unbounded);
                enum_to_str_switch_default(bounded);
            }
        }
    };

    

    /// @brief  区间
    /// @tparam _Ty 区间的数值类型
    template<typename _Ty>
    class interval
    {
    public:
        /// 区间属性，开区间，闭区间
        using variety = interval_variety;

    public:
        variety left_variety;  //< Open: 表示为左开区间
        variety right_variety; //< Open: 表示为右开区间

        boundness left_bound;  //< Bounded: 表示左边有界
        boundness right_bound; //< Bounded: 表示右边有界

        double left_value;   //< 区间左值
        double right_value;  //< 区间右值

    public:
        inline void assign() {
            left_value = _Ty();
            right_value = _Ty();

            left_variety = variety::open;
            right_variety = variety::open;

            left_bound = boundness::unbounded;
            right_bound = boundness::unbounded;
        }

        inline void assign(variety _Left_variety, boundness _Left_bound, double _Left_value,
                           variety _Right_variety, boundness _Right_bound, double _Right_value)
        {
            left_value = _Left_value;     right_value = _Right_value;
            left_bound = _Left_bound;     right_bound = _Right_bound;
            left_variety = _Left_variety; right_variety = _Right_variety;
        }

        //! @brief 判断 value 是否在此区间内
        inline bool contains(_Ty _Value) const
        {
            bool left_condi = false, right_condi = false;

            // 区间左边有界
            if (left_bound == bounded)
            {
                if (left_variety == variety::open)
                    left_condi = left_value < _Value;
                else
                    left_condi = left_value <= _Value;
            }
            // 区间左边无界
            else left_condi = true;

            // 区间右边有界
            if (right_bound == bounded)
            {
                if (right_variety == variety::open)
                    right_condi = right_value > _Value;
                else
                    right_condi = right_value >= _Value;
            }
            else right_condi = true;

            return left_condi && right_condi;
        }
    };


    /// @brief 区间
    template<>
    class interval<double>
    {
    public:
        using variety = interval_variety;

    public:
        variety left_variety;  //< Open: 表示为左开区间
        variety right_variety; //< Open: 表示为右开区间

        boundness left_bound;  //< Bounded: 表示左边有界
        boundness right_bound; //< Bounded: 表示右边有界

        double left_value;   //< 区间左值
        double right_value;  //< 区间右值

    public:
        inline void assign()
        {
            left_value = std::numeric_limits<double>::min();
            right_value = std::numeric_limits<double>::max();

            left_variety = variety::open;
            right_variety = variety::open;

            left_bound = boundness::unbounded;
            right_bound = boundness::unbounded;
        }

        inline void assign(variety _Left_variety, boundness _Left_bound, double _Left_value,
                           variety _Right_variety, boundness _Right_bound, double _Right_value)
        {
            left_value = _Left_value;     right_value = _Right_value;
            left_bound = _Left_bound;     right_bound = _Right_bound;
            left_variety = _Left_variety; right_variety = _Right_variety;
        }

        //! @brief 判断 value 是否在此区间内
        inline bool contains(double _Value) const
        {
            bool left_condi = false, right_condi = false;

            // 区间左边有界
            if (left_bound == boundness::bounded)
            {
                if (left_variety == variety::open)
                    left_condi = left_value < _Value;
                else
                    left_condi = algorithm::less_equal(left_value, _Value);
            }
            // 区间左边无界
            else left_condi = true;

            // 区间右边有界
            if (right_bound == boundness::bounded)
            {
                if (right_variety == variety::open)
                    right_condi = right_value > _Value;
                else
                    right_condi = algorithm::greater_equal(right_value, _Value);
            }
            else right_condi = true;

            return left_condi && right_condi;
        }
    };
}
_MANGO_NAMESPACE_END
_MANGO_PACK_POP
