cmake_minimum_required(VERSION 3.10)
project(HelloMongoose C)
set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME}
    src/main.c
    third-party/mongoose/mongoose.c
)
target_include_directories(${PROJECT_NAME} PRIVATE third-party/mongoose)
target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
target_compile_definitions(${PROJECT_NAME} PRIVATE MG_ENABLE_HTTP=1)

