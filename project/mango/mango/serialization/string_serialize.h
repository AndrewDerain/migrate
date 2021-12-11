#pragma once

#include "../config/config.h"

#include <string>

_MANGO_NAMESPACE_BEGIN

template<typename _SubClass>
class serializable_to_string
{
    std::string to_string() const {
        return static_cast<_SubClass*>(this)->to_string();
    }
    void from_string(std::string) {
        return static_cast<_SubClass*>(this)->from_string();
    }
};

_MANGO_NAMESPACE_END
