/*
 * test_matrix_transform.cpp
 *
 * Create Date : 2022-08-14 09:26:49
 * Copyright (c) 2019- Yusuke Kitamura <ymyk6602@gmail.com>
 */
#include <filesystem>
#include <iostream>

#include <glog/logging.h>
#include <gperftools/profiler.h>

#include <pcl/common/transforms.h>
#include <pcl/console/parse.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_cloud.h>
#include <pcl/visualization/pcl_visualizer.h>

namespace fs = std::filesystem;


void showHelp(char* program_name) {
    std::cout << std::endl;
    std::cout << "Usage: " << program_name << "  cloud_filename.[pcd|ply]" << std::endl;
    std::cout << "-h : Show this help." << std::endl;
}


int main_impl(int argc, char** argv) {
    if (pcl::console::find_switch(argc, argv, "-h") || pcl::console::find_switch(argc, argv, "--help")) {
        showHelp(argv[0]);
        return 0;
    }

    std::vector<int> filenames;
    bool file_is_pcd = false;

    filenames = pcl::console::parse_file_extension_argument(argc, argv, "ply");
    if (filenames.size() != 1) {
        filenames = pcl::console::parse_file_extension_argument(argc, argv, ".pcd");
        if (filenames.size() != 1) {
            showHelp(argv[0]);
            return -1;
        } else {
            file_is_pcd = true;
        }
    }

    pcl::PointCloud<pcl::PointXYZ>::Ptr source_cloud(new pcl::PointCloud<pcl::PointXYZ>());

    if (file_is_pcd) {
        if (pcl::io::loadPCDFile(argv[filenames[0]], *source_cloud) < 0) {
            std::cerr << "Error loading point cloud " << argv[filenames[0]] << std::endl;
            return -1;
        }
    } else {
        if (pcl::io::loadPLYFile(argv[filenames[0]], *source_cloud) < 0) {
            std::cerr << "Error loading point cloud " << argv[filenames[0]] << std::endl;
            return -1;
        }
    }

    Eigen::Matrix4f transform = Eigen::Matrix4f::Identity();
    float theta = M_PI / 4.0;
    transform(0, 0) = cos(theta);
    transform(0, 1) = -sin(theta);
    transform(1, 0) = sin(theta);
    transform(1, 1) = cos(theta);
    transform(0, 3) = 2.5;

    pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud(new pcl::PointCloud<pcl::PointXYZ>());
    pcl::transformPointCloud(*source_cloud, *transformed_cloud, transform);

    std::cout << *transformed_cloud << std::endl;
    printf(
        "\nPoint cloud colors :  white  = original point cloud\n"
        "                        red  = transformed point cloud\n");

    pcl::visualization::PCLVisualizer viewer("Cloud Viewer");
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> source_cloud_color_handler(
        source_cloud, 255, 255, 255);
    viewer.addPointCloud(source_cloud, source_cloud_color_handler, "original_cloud");

    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> transformed_cloud_color_handler(
        transformed_cloud, 255, 0, 0);
    viewer.addPointCloud(transformed_cloud, transformed_cloud_color_handler, "transformed_cloud");

    viewer.addCoordinateSystem(1.0, "cloud", 0);
    viewer.setBackgroundColor(0.05, 0.05, 0.05, 0);
    viewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 2,
                                            "original_cloud");
    viewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 2,
                                            "transformed_cloud");

    while (!viewer.wasStopped()) {
        viewer.spinOnce();
    }
    return 0;
}


int main(int argc, char** argv) {
    // logging codes
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    fs::path save_dir("./prof");
    if (!fs::exists(save_dir)) {
        fs::create_directories(save_dir);
    }
    auto save_path =
        (save_dir / fs::path((fs::path(argv[0])).stem().string() + ".prof")).generic_string();
    ProfilerStart(save_path.c_str());
    std::cout << "Profiling to " << save_path << std::endl;
    // end logging codes

    // Insert your codes here!!!
    main_impl(argc, argv);

    // finish program
    ProfilerStop();
}
