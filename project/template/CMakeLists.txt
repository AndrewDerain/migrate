#------------------------------------
#     Config CMake Minimum Version
#------------------------------------
cmake_minimum_required(VERSION 3.16)


#------------------------------------
#    Config project name.
#------------------------------------
project(
	"DerainFramework"
	VERSION 0.1.0.0 
	DESCRIPTION "Provide a series of basic tools." )
	
set(usr_project_minuscule_name "derain_kernel")
set(usr_project_majuscule_name "DERAIN_KERNEL")


#------------------------------------
#    Config c++ standard
#------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


#------------------------------------
#    Config Path.
#------------------------------------
set(usr_project_path "${PROJECT_SOURCE_DIR}")
set(usr_lib_path     "${usr_project_path}/../../lib")
set(usr_third_path   "${usr_project_path}/../../third")


#------------------------------------
#    Config build Behavior
#------------------------------------
SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")

list(APPEND CMAKE_BUILD_RPATH "./")
list(APPEND CMAKE_INSTALL_RPATH "./")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG  "${usr_project_path}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG  "${usr_project_path}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG  "${usr_project_path}/bin")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE  "${usr_project_path}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE  "${usr_project_path}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE  "${usr_project_path}/bin")


#------------------------------------
#    Config predefined macros.
#------------------------------------
set(usr_project_export_macro_symbol _${usr_project_majuscule_name}_EXPORTS)
set(usr_project_debug_macro_symbol _${usr_project_majuscule_name}_DEBUG)

if (CMAKE_HOST_WIN32)
	#add_definitions( -D_WINDOWS ) 
	add_definitions( -D_CRT_SECURE_NO_WARNINGS )
elseif(CMAKE_HOST_UNIX)
	#add_definitions( -D_LINUX )
endif()

add_definitions( -D${usr_project_export_macro_symbol} )  # project export symbol

if(CMAKE_CONFIGURATION_TYPES STREQUAL Debug)
	set(CMAKE_BUILD_TYPE Debug)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug )
    
endif()


#-------------------------
#    Config boost
#-------------------------
set(usr_boost_version 1.72.0) 

if(CMAKE_HOST_WIN32)
	set(usr_boost_path "${usr_third_path}/boost/1_72_0/include/boost-1_72")
elseif(CMAKE_HOST_UNIX)
	set(usr_boost_path "${usr_third_path}/boost/1_72_0/include/")
endif()

set(Boost_USE_STATIC_LIBS       OFF)  # ON: 查找 boost 的静态库           OFF: 不查找 boost 的静态库          
set(Boost_USE_DEBUG_LIBS         ON)  # ON: 查找 Debug 版本的 boost 库;   OFF: 不查找 Debug 版本的 boost 库   
set(Boost_USE_RELEASE_LIBS       ON)  # ON: 查找 Release 版本的 boost 库; OFF: 不查找 Release 版本的 boost 库 
set(Boost_USE_MULTITHREADED      ON)  # ON: 查找多线程库                  OFF: 不查找多线程库                 
set(Boost_USE_STATIC_RUNTIME    OFF)  # ON: 静态链接到 C++ 运行库                                            
set(BOOST_NO_SYSTEM_PATHS        ON)  # ON: 不使用 系统默认的 boost 库地址
set(BOOSTROOT ${usr_boost_path})
add_definitions( -DBOOST_ALL_DYN_LINK )

if(CMAKE_HOST_WIN32)
	find_package(Boost ${usr_boost_version} COMPONENTS)
elseif(CMAKE_HOST_UNIX)
	find_package(Boost ${usr_boost_version} COMPONENTS chrono thread filesystem system)
endif()

if(Boost_FOUND)
	message("    | Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}")
	message("    | Boost_LIBRARY_DIRS = ${Boost_LIBRARY_DIRS}")
else()
	message("    | msg boost not find!")
endif()


#-------------------------
#    Config KernelConfig.h
#-------------------------
configure_file("./src/KernelConfig.h.in" "${usr_project_path}/include/Derain/Basic/KernelConfig.h")


#-------------------------
#    Config include path and link path
#-------------------------
include_directories(
	"${Boost_INCLUDE_DIR}" "${usr_project_path}/include/")
link_directories(
	"${Boost_LIBRARY_DIRS}" "${usr_project_path}/bin")


#-------------------------
#    Config build target
#-------------------------
add_library(kernel MODULE src/kernel.cpp)

file(GLOB kernel_test_src 
	"./include/Derain/*"
	"./include/Derain/Basic/*"
	"./include/Derain/Foundation/*"
	"./include/Derain/Utility/*")
add_executable(kernel_test src/kernel_test.cpp ${kernel_test_src})
add_executable(unit unit/unit.cpp)


#-------------------------
#    Config install
#-------------------------
set(local_install_inc_path "${usr_lib_path}/Derain/Framework/include/Derain")
set(local_install_lib_path "${usr_lib_path}/Derain/Framework/lib")

install(DIRECTORY "./include/Derain/" DESTINATION ${local_install_inc_path})
install(TARGETS kernel DESTINATION ${local_install_lib_path})


#------------------------------------
#    Print configs
#------------------------------------
message("    | CMAKE_SYSTEM_NAME        = ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")  
message("    | CMAKE_BUILD_TOOL         = ${CMAKE_BUILD_TOOL}")
message("    | CMAKE_CXX_FLAGS          = ${CMAKE_CXX_FLAGS}")
message("    | CMAKE_CXX_FLAGS_DEBUG    = ${CMAKE_CXX_FLAGS_DEBUG}")
message("    | CMAKE_CXX_FLAGS_RELEASE  = ${CMAKE_CXX_FLAGS_RELEASE}")
message("    | CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
message("    | PRECOMPILE_HEADERS       = ${PRECOMPILE_HEADERS}")
message("    | CMAKE_BUILD_TYPE         = ${CMAKE_BUILD_TYPE}")
message("")