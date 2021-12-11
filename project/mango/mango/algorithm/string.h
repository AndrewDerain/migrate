#pragma once

#include "../config/config.h"
#include <cctype>
#include <string>
#include <vector>

_MANGO_NAMESPACE_BEGIN
namespace algorithm
{
    /// @brief 去除字符串两端的空白字符
    inline void trim(std::string& str)
    {
        if (str.empty()) return;

        str.erase(0, str.find_first_not_of(" "));
        str.erase(str.find_last_not_of(" ") + 1);
    }

    /// @brief 去除字符串中所有空白字符
    inline void trim_all(std::string& str)
    {
        std::string result;
        if (!str.empty())
            for (auto& c : str)
                if (!std::isspace(c))
                    result += c;
        str = result;
    }

    /// @note 当字符串为空时，也会返回一个空字符串 
    void split(std::string& s, std::string& delim, std::vector<std::string>& ret)
    {
        size_t last = 0;
        size_t index = s.find_first_of(delim, last);
        while (index != std::string::npos)
        {
            ret.push_back(s.substr(last, index - last));
            last = index + 1;
            index = s.find_first_of(delim, last);
        }
        if (index - last > 0)
        {
            ret.push_back(s.substr(last, index - last));
        }
    }
}
_MANGO_NAMESPACE_END
