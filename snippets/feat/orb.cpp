/**
 * @file orb.cpp
 * @brief opencvのorb extractorのテスト
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-01 15:14:16
 */
#include <filesystem>

#include <argparse/argparse.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/opencv.hpp>

#include <slam.hpp>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    argparse::ArgumentParser parser("ORB extractor test");
    parser.add_argument("-i", "--image")
        .help("Input Image")
        .default_value(std::string("/home/kitamura/work/slam/dataset/sample_image/lena.png"));

    try {
        parser.parse_args(argc, argv);  // Example: ./main --color red --color green --color blue
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    auto image_path = fs::path(parser.get<std::string>("--image"));
    std::cout << image_path.generic_string() << std::endl;

    if (!fs::exists(image_path)) {
        std::cout << "File does not exists" << std::endl;
        return 0;
    }

    cv::Mat img = cv::imread(image_path.string());

    cv::Ptr<cv::Feature2D> feat_extractor = cv::ORB::create();
    std::vector<cv::KeyPoint> keypoints;

    feat_extractor->detect(img, keypoints);

    slam_logd("Number of key points : {}", keypoints.size());

    auto viewer = slam::Viewer::getInstance();
    viewer->addImage(img);
    viewer->addPointCloud(keypoints);
    viewer->render();
}
