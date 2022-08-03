/**
 * @file viewer_test.cpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-03 22:01:54
 */
#include "debug/viewer.hpp"

#include <glog/logging.h>


int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    auto viewer = slam::Viewer::getInstance();

    viewer.render();
}
