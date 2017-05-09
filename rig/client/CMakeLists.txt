cmake_minimum_required(VERSION 2.8)

project(AvatarClient)

find_package(OpenCV REQUIRED)

SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

include(../libs/dlib/cmake)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB CLIENT_SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(AvatarClient SHARED ${CLIENT_SRC})

target_link_libraries(AvatarClient dlib::dlib ${OpenCV_LIBS})
