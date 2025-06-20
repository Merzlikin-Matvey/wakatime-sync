cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

project(WakatimeSync C)
set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME}
    src/main.c
    third-party/mongoose/mongoose.c
)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "Please run cmake with -DCMAKE_TOOLCHAIN_FILE=...")
endif()

find_package(CURL REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE third-party/mongoose)

target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

target_compile_definitions(${PROJECT_NAME} PRIVATE MG_ENABLE_HTTP=1)