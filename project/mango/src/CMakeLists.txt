
#------------------------------------
#    Config build Behavior
#------------------------------------
#SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")

list(APPEND CMAKE_BUILD_RPATH "./")
list(APPEND CMAKE_INSTALL_RPATH "./")


#------------------------------------
#    Config predefined macros.
#------------------------------------
set(usr_project_export_macro_symbol _${usr_project_majuscule_name}_EXPORTS)
set(usr_project_debug_macro_symbol _${usr_project_majuscule_name}_DEBUG)

if (CMAKE_HOST_WIN32)
    set(usr_host_platform_macro_symbol _WINDOWS)
    add_definitions( -D_CRT_SECURE_NO_WARNINGS )
elseif(CMAKE_HOST_UNIX)
    set(usr_host_platform_macro_symbol _LINUX) 
endif()

add_definitions( -D${usr_project_export_macro_symbol} )  # project export symbol

if(CMAKE_CONFIGURATION_TYPES STREQUAL Debug)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug )
    
endif()


#-------------------------
#    Config basic_config.h
#-------------------------
configure_file("./config/basic_config.h.in" "${usr_project_path}/${usr_project_minuscule_name}/config/basic_config.h")


#-------------------------
#    Config include path and link path
#-------------------------
include_directories(
    "${Boost_INCLUDE_DIR}" "${usr_project_path}/")
link_directories(
    "${Boost_LIBRARY_DIRS}" "${usr_project_path}/bin")


#-------------------------
#    Config build target
#-------------------------
#add_library(kernel MODULE src/kernel.cpp)

file(GLOB_RECURSE libmongo_src ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
message("list " ${CMAKE_CURRENT_SOURCE_DIR})
message("list " ${libmongo_src})
#add_executable(mongo "src/mango.cpp" ${mongo_src})
#add_executable(mongo_test "unit/mango_test.cpp")
#add_library(libmango SHARED "interprocess/lock_free_cyclic_queue.cpp")


#-------------------------
#    Config install
#-------------------------
#set(local_install_inc_path "${usr_lib_path}/${usr_project_minuscule_name}/include/${usr_project_minuscule_name}")
#set(local_install_lib_path "${usr_lib_path}/${usr_project_minuscule_name}/lib")

#[[install(DIRECTORY "./include/Derain/" DESTINATION ${local_install_inc_path})
install(TARGETS kernel DESTINATION ${local_install_lib_path})]]


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