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

#include "custom_format.hpp"
#include "viewer.hpp"

namespace {

namespace fs = std::filesystem;

}  // namespace

const fs::path PROJECT_ROOT = fs::path(__FILE__).parent_path().parent_path();

#define _log(loglevel, fmtstr, ...)                                                             \
    std::cout << fmt::format("[{:%Y-%m-%d %H:%M:%S} {} File: {}, Line: {}] ",                   \
                             fmt::localtime(std::time(nullptr)), loglevel,                      \
                             fs::relative(fs::path(__FILE__), PROJECT_ROOT).string(), __LINE__) \
              << fmt::format(fmtstr __VA_OPT__(, ) __VA_ARGS__) << std::endl;

#ifdef NDEBUG
#define logd(fmtstr, ...)
#else
#define logd(fmtstr, ...) _log("DEBUG", fmtstr __VA_OPT__(, ) __VA_ARGS__)
#endif  // NDEBUG

#define logw(fmtstr, ...) _log("WARNING", fmtstr __VA_OPT__(, ) __VA_ARGS__)
#define loge(fmtstr, ...) _log("ERROR", fmtstr __VA_OPT__(, ) __VA_ARGS__)


#endif  // DEBUG_HPP__
