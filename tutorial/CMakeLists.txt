cmake_minimum_required(VERSION 3.10)

# set the project name
project(hotrodClientTutorial)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(clientTutorial-1 clientTutorial-1.cpp)
target_compile_options(clientTutorial-1 PUBLIC "-DHR_PROTO_EXPORT=")
target_link_libraries(clientTutorial-1 hotrod)
