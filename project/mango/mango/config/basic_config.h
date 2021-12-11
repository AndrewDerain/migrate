///
/// @file    basic_config.h:
/// @version 1.0.0.0
/// @brief   定义了项目基本的宏定义
///          mango 0.1.0.0
///          以速度为第一要义，为金融量化交易，高频交易定制的 C++ 组件库.
#pragma once

// ---------------
//   unify packing
// ---------------
#define _MANGO_PACKING 8
#define _MANGO_PACK_PUSH __pragma(pack(push, _MANGO_PACKING))
#define _MANGO_PACK_POP  __pragma(pack(pop))


// ---------------
//   project root namespace definition
// ---------------
#define _MANGO_NAMESPACE_BEGIN \
	namespace mango {

#define _MANGO_NAMESPACE_END   \
	}
 
 #define _MANGO_DETAIL_NAMESPACE_BEGIN \
	namespace _mangodetail {

#define _MANGO_DETAIL_NAMESPACE_END   \
	}

// ---------------
//   warning
// ---------------
#define _MANGO_WARNING_DISABLE __pragma(warning(disable: 26812))


// ---------------
//   system config
// ---------------
#ifdef WIN32
#    ifdef _MANGO_EXPORTS
#        define _MANGO_API __declspec(dllexport)
#    else
#        define _MANGO_API __declspec(dllimport)
#    endif
#endif

#ifdef _LINUX
#    ifdef _MANGO_EXPORTS
#        define _MANGO_API __attribute__((visibility ("default")))
#    else
#        define _MANGO_API
#    endif
#endif


// ---------------
//   debug macro
// ---------------
#ifdef _DEBUG
#    define _MANGO_DEBUG
#endif
