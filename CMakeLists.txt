cmake_minimum_required(VERSION 2.5)
project(MyMuduo)

# MyMuduo最终编译成so动态库，设置动态库的路径，放在根目录的lib文件夹下
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# 设置调试信息 以及 启动c++11标准   如果cmake构建不了在后面加上 -fPIC
set(CMake_CXX_FLAGS "${CMake_CXX_FLAGS} -g -std=c++11")

# 定义参与编译的源代码文件
aux_source_directory(. SRC_SOURCE)
# 编译生成动态库MyMuduo
add_library(${PROJECT_NAME} SHARED ${SRC_SOURCE})