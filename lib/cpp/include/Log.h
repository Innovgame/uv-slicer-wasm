// log.h
#pragma once

#include <cstring> // for strrchr
#include <format>  // C++20 <format>
#include <iostream>

// 提取文件名（兼容 Windows 和 Linux 路径分隔符）
#define __FILENAME__                                                                                                   \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1                                                               \
                            : (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__))

// 默认日志级别（0: NONE, 1: ERROR, 2: INFO, 3: DEBUG）
#ifndef LOG_LEVEL
#define LOG_LEVEL 3 // 默认启用所有日志
#endif

// 日志级别宏定义
#if LOG_LEVEL >= 3
#define LOG_DEBUG(fmt, ...)                                                                                            \
    std::cout << std::format("[DEBUG] {}:{} | " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__) << std::endl
#else
#define LOG_DEBUG(fmt, ...)
#endif

#if LOG_LEVEL >= 2
#define LOG_INFO(fmt, ...) std::cout << std::format("[INFO]  " fmt, ##__VA_ARGS__) << std::endl
#else
#define LOG_INFO(fmt, ...)
#endif

#if LOG_LEVEL >= 1
#define LOG_ERROR(fmt, ...)                                                                                            \
    std::cerr << std::format("[ERROR] {}:{} | " fmt, __FILENAME__, __LINE__, ##__VA_ARGS__) << std::endl
#else
#define LOG_ERROR(fmt, ...)
#endif