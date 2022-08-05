/*
 * test_g2o.cpp
 *
 * Create Date : 2022-08-05 16:27:33
 * Copyright (c) 2019- Yusuke Kitamura <ymyk6602@gmail.com>
 */
#include <iostream>
#include <filesystem>

#include <glog/logging.h>
#include <gperftools/profiler.h>


namespace fs = std::filesystem;

int main(int argc, char ** argv) {
    // logging codes
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    fs::path save_dir("./prof");
    if (!fs::exists(save_dir)) {
        fs::create_directories(save_dir);
    }
    auto save_path = (save_dir / fs::path((fs::path(argv[0])).stem().string() + ".prof")).generic_string();
    ProfilerStart(save_path.c_str());
    std::cout << "Profiling to " << save_path << std::endl;
    // end logging codes



    ProfilerStop();
}
