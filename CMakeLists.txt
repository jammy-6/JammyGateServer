CMAKE_MINIMUM_REQUIRED(VERSION 3.3)
PROJECT(JammyGateServer VERSION 1.0 LANGUAGES CXX)

set(RESOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/resource)
message(STATUS "RESOURCE_DIR = ${RESOURCE_DIR}")

# 配置grpc相关
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/proto)

# 配置项目宏相关
configure_file(Config.h.in Config.h)
include_directories(${CMAKE_BINARY_DIR})
include_directories("/root/.local/include")
# VS sln项目文件分类
if(WIN32)
	source_group("CMake Rules"  REGULAR_EXPRESSION "^$")
    source_group("Generated Files" REGULAR_EXPRESSION "moc_.*|ui_.*|qrc_.*|mocs_.*")
	source_group("Header Files" REGULAR_EXPRESSION ".h$")
	source_group("Source Files" REGULAR_EXPRESSION ".cpp$")
    source_group("Qrc Files"    REGULAR_EXPRESSION ".qrc$")
    source_group("Ui Files"    REGULAR_EXPRESSION ".ui$")
	source_group("zero"			REGULAR_EXPRESSION ".stamp$|.rule$")
endif()

# 配置项目构建类型
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELWITHDEBINFO CACHE STRING "Build type" FORCE)
endif()

# 由于生成的C++文件都是放在编译目录里的（在Shadow Building中有别于源码目录），
# 所以还需要将编译目录包含到Include目录，否则编译器会找不到这些头文件：
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 添加项目头文件路径
set("${PROJECT_NAME}_include_dir" "${CMAKE_CURRENT_SOURCE_DIR}/include") 
include_directories(${${PROJECT_NAME}_include_dir})
file(GLOB_RECURSE "${PROJECT_NAME}_include" "${${PROJECT_NAME}_include_dir}/*.h")
MESSAGE(STATUS "${PROJECT_NAME}_include = ${${PROJECT_NAME}_include}")

# 添加项目源文件路径
set("${PROJECT_NAME}_src_dir" "${CMAKE_CURRENT_SOURCE_DIR}/src") 
file(GLOB_RECURSE "${PROJECT_NAME}_src" "${${PROJECT_NAME}_src_dir}/*.cpp")
MESSAGE(STATUS "${PROJECT_NAME}_src = ${${PROJECT_NAME}_src}")

# 添加proto源文件路径
set("${PROJECT_NAME}_protocc_dir" "${CMAKE_BINARY_IDR}/proto") 
file(GLOB_RECURSE "${PROJECT_NAME}_protocc_src" "${${PROJECT_NAME}_protocc_dir}/*.cc")
MESSAGE(STATUS "${PROJECT_NAME}_protocc_src = ${${PROJECT_NAME}_protocc_src}")
include_directories(${${PROJECT_NAME}_protocc_dir})

# 添加 Hiredis 库路径
include_directories("/repo/hiredis")
# 查找 Hiredis 库
find_library(HIREDIS_LIB hiredis PATHS "/repo/hiredis/build")
MESSAGE(STATUS "find HIREDIS_LIB PATH =  ${HIREDIS_LIB}")
find_package(spdlog REQUIRED)


# 配置rpcservice相关动态库
add_library(
    ${PROJECT_NAME}_rpcservice
    SHARED
    ${${PROJECT_NAME}_src_dir}/StatusGrpcClient.cpp
    ${${PROJECT_NAME}_src_dir}/VerifyGrpcClient.cpp
)

target_link_libraries(
    ${PROJECT_NAME}_rpcservice
        ${PROJECT_NAME}_proto
        spdlog::spdlog
)

# 配置cserver相关动态库
add_library(
    ${PROJECT_NAME}_server
    ${${PROJECT_NAME}_src_dir}/CServer.cpp
    ${${PROJECT_NAME}_src_dir}/HttpConnection.cpp
    ${${PROJECT_NAME}_src_dir}/ConfigMgr.cpp
)

target_link_libraries(
    ${PROJECT_NAME}_server
        ${PROJECT_NAME}_asio
        spdlog::spdlog
)

# 配置mysql相关动态库
add_library(
    ${PROJECT_NAME}_mysql
    SHARED
    ${${PROJECT_NAME}_src_dir}/MysqlDao.cpp
    ${${PROJECT_NAME}_src_dir}/MysqlMgr.cpp
    ${${PROJECT_NAME}_src_dir}/MysqlPool.cpp
)
# 连接mysqlconn8连接库与spdlog
target_link_libraries(
    ${PROJECT_NAME}_mysql 
        pthread 
        mysqlcppconn
        spdlog::spdlog
)

# 配置redis相关动态库
add_library(
    ${PROJECT_NAME}_redis
    SHARED
    ${${PROJECT_NAME}_src_dir}/RedisMgr.cpp
    ${${PROJECT_NAME}_src_dir}/RedisConPool.cpp
)

target_link_libraries(
    ${PROJECT_NAME}_redis
        pthread 
        ${HIREDIS_LIB}
        spdlog::spdlog
        
)

# 配置asio相关动态库
add_library(
    ${PROJECT_NAME}_asio
    SHARED
    ${${PROJECT_NAME}_src_dir}/AsioIOServicePool.cpp
)

target_link_libraries(
    ${PROJECT_NAME}_asio 
        pthread 
        spdlog::spdlog 
        
)

# 生成可执行文件
add_executable(     
    ${PROJECT_NAME}
    ${${PROJECT_NAME}_src_dir}/LogicSystem.cpp
    ${${PROJECT_NAME}_src_dir}/main.cpp
)
# 链接proto和hiredis库mysql库
target_link_libraries(
    ${PROJECT_NAME} 
        ${PROJECT_NAME}_mysql
        ${PROJECT_NAME}_redis
        ${PROJECT_NAME}_server
        ${PROJECT_NAME}_rpcservice
        spdlog::spdlog
) 


