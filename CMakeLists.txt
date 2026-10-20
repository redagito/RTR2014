cmake_minimum_required(VERSION 3.28)

project(RTR2014 LANGUAGES CXX)

enable_testing()

# Found here so that the tests can use the json types as well.
find_package(jsoncpp CONFIG REQUIRED)

add_subdirectory(RTR2014)
add_subdirectory(App)
add_subdirectory(Test)