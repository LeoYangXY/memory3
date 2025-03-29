# CMakeList.txt: memory3 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
project(memory3 LANGUAGES CXX)

# 设置可执行文件输出到 ../out/build/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../out/build)


# 将源代码添加到此项目的可执行文件。
add_executable (memory3  
memory3.cpp
onethreadtest.cpp
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET memory3 PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
