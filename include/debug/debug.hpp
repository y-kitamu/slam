/**
 * @file debug.hpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-07-31 11:14:26
 */
#ifndef DEBUG_HPP__
#define DEBUG_HPP__

#include <filesystem>

#include <fmt/chrono.h>
#include <fmt/format.h>

namespace {

namespace fs = std::filesystem;

}  // namespace

const fs::path PROJECT_ROOT = fs::path(__FILE__).parent_path().parent_path();

#define _slam_log(loglevel, fmtstr, ...)                                                        \
    std::cout << fmt::format("[{:%Y-%m-%d %H:%M:%S} {} File: {}, Line: {}] ",                   \
                             fmt::localtime(std::time(nullptr)), loglevel,                      \
                             fs::relative(fs::path(__FILE__), PROJECT_ROOT).string(), __LINE__) \
              << fmt::format(fmtstr __VA_OPT__(, ) __VA_ARGS__) << std::endl;

#ifdef NDEBUG
#define logd(fmtstr, ...)
#else
#define slam_logd(fmtstr, ...) _slam_log("DEBUG", fmtstr __VA_OPT__(, ) __VA_ARGS__)
#endif  // NDEBUG

#define slam_logw(fmtstr, ...) _slam_log("WARNING", fmtstr __VA_OPT__(, ) __VA_ARGS__)
#define slam_loge(fmtstr, ...) _slam_log("ERROR", fmtstr __VA_OPT__(, ) __VA_ARGS__)


#include "custom_format.hpp"
#include "viewer.hpp"


#endif  // DEBUG_HPP__
