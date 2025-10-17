cmake_minimum_required(VERSION 3.26)

project(uv-slicer VERSION 0.0.1
    DESCRIPTION "emscripten uv slicer api"
    LANGUAGES C CXX)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
set(CMAKE_CXX_STANDARD 20)
set(CPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/cpp)
set(CPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/cpp/include)
set(WEB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/cpp/web)

if(NOT DEFINED LOG_LEVEL)
    set(LOG_LEVEL 0) # 默认启用 INFO 级别
endif()

message(STATUS "build wasm")
file(GLOB WRAPPER_FILES ${CPP_DIR}/*.cpp)

add_executable(${PROJECT_NAME}
    ${WRAPPER_FILES}
    ${WEB_DIR}/main.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE LOG_LEVEL=${LOG_LEVEL})

set(FLAGS "--bind -O3 -msimd128 -DNDEBUG -sENVIRONMENT=web,worker \
    -fexceptions -sNO_DISABLE_EXCEPTION_CATCHING \
    -sMODULARIZE=1 -sEXPORT_ES6=1 -sEXPORT_NAME=createUvSlicerModule \
    -sWASM_BIGINT=1 -sMAXIMUM_MEMORY=2GB -sALLOW_MEMORY_GROWTH=1 -sEXIT_RUNTIME=0 \
    --post-js ${WEB_DIR}/post-script.js --emit-tsd ${PROJECT_NAME}.d.ts")

message(STATUS "Final flags: ${FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")

target_include_directories(${PROJECT_NAME} PRIVATE ${CPP_DIR} ${CPP_INCLUDE_DIR})

# copy output to tango-web
set(DESTINATION_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/wasm) # 目标目录名称

find_program(PYTHON_EXECUTABLE NAMES python3 python)

if(NOT PYTHON_EXECUTABLE)
    message(FATAL_ERROR "Python executable not found. Please ensure Python is installed and available in your PATH.")
endif()

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_BINARY_DIR}/uv-slicer.d.ts"
    "${DESTINATION_DIR}/uv-slicer.d.ts"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_BINARY_DIR}/uv-slicer.js"
    "${DESTINATION_DIR}/uv-slicer.js"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_BINARY_DIR}/uv-slicer.wasm"
    "${DESTINATION_DIR}/uv-slicer.wasm"
    COMMENT "Copying build outputs to the test directory"
)
