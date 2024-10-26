CMAKE_MINIMUM_REQUIRED(VERSION 3.3)
PROJECT(JammyGateServer VERSION 1.0 LANGUAGES CXX)

set(RESOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/resource)
message(STATUS ${RESOURCE_DIR})
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(Config.h.in Config.h)
# 设置uic文件生成路径
# set(CMAKE_AUTOUIC_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/generatefiles")
# option(AUTOMOC_PATH_PREFIX ON)
# set(CMAKE_AUTOMOC_PATH_PREFIX "-o${CMAKE_BINARY_DIR}/generatefiles")
if(WIN32)
	# 整理自动生成的文件
	source_group("CMake Rules"  REGULAR_EXPRESSION "^$")
    source_group("Generated Files" REGULAR_EXPRESSION "moc_.*|ui_.*|qrc_.*|mocs_.*")
	source_group("Header Files" REGULAR_EXPRESSION ".h$")
	source_group("Source Files" REGULAR_EXPRESSION ".cpp$")
    source_group("Qrc Files"    REGULAR_EXPRESSION ".qrc$")
    source_group("Ui Files"    REGULAR_EXPRESSION ".ui$")
	source_group("zero"			REGULAR_EXPRESSION ".stamp$|.rule$")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELWITHDEBINFO CACHE STRING "Build type" FORCE)
endif()
set(CMAKE_PREFIX_PATH "/usr/local/lib/cmake")
# message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}/generatefiles")

# 由于生成的C++文件都是放在编译目录里的（在Shadow Building中有别于源码目录），
# 所以还需要将编译目录包含到Include目录，否则编译器会找不到这些头文件：
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# # 查找Boost库
# find_package(Boost REQUIRED COMPONENTS system filesystem asio)

# # # 包含Boost头文件路径
# include_directories(${Boost_INCLUDE_DIRS})

include_directories(${CMAKE_BINARY_DIR})

# 添加项目头文件路径
set("${PROJECT_NAME}_include_dir" "${CMAKE_CURRENT_SOURCE_DIR}/include") 
include_directories(${${PROJECT_NAME}_include_dir})
file(GLOB_RECURSE "${PROJECT_NAME}_include" "${${PROJECT_NAME}_include_dir}/*.h")

# 添加项目源文件路径
set("${PROJECT_NAME}_src_dir" "${CMAKE_CURRENT_SOURCE_DIR}/src") 
file(GLOB_RECURSE "${PROJECT_NAME}_src" "${${PROJECT_NAME}_src_dir}/*.cpp")

# 生成可执行文件
add_executable(     
    ${PROJECT_NAME}
    ${${PROJECT_NAME}_src}
    ${${PROJECT_NAME}_include}
)

# 配置链接库
# 链接Boost库
#  target_link_libraries(${PROJECT_NAME} pthread ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} pthread )