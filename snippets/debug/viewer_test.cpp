/**
 * @file viewer_test.cpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-03 22:01:54
 */
#include <glog/logging.h>
#include <argparse/argparse.hpp>

#include <slam.hpp>


int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    argparse::ArgumentParser parser("Pangolin test");
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
    cv::Mat img = cv::imread(image_path.string());

    auto viewer = slam::Viewer::getInstance();

    if (false) {
        auto window = pangolin::GetBoundWindow();
        window->ResizeSignal.connect([](const pangolin::WindowResizeEvent& ev) {
            slam_logd("window resize: {} {}", ev.width, ev.height);
        });
        window->KeyboardSignal.connect([](const pangolin::KeyboardEvent& ev) {
            auto _ev = ev;
            slam_logd("keyboard: key = {}, pressed = {}, x = {}, y = {}, mask = {}", ev.key, ev.pressed,
                      ev.x, ev.y, _ev.key_modifiers.mask());
        });
        window->MouseSignal.connect([](const pangolin::MouseEvent& ev) {
            auto _ev = ev;
            slam_logd("mouse: button = {}, pressed = {}, x = {}, y = {}, mask = {}", ev.button,
                      ev.pressed, ev.x, ev.y, _ev.key_modifiers.mask());
        });
        window->MouseMotionSignal.connect([](const pangolin::MouseMotionEvent& ev) {
            auto _ev = ev;
            slam_logd("mouse motion: x = {}, y = {}, mask = {}", ev.x, ev.y, _ev.key_modifiers.mask());
        });
        window->PassiveMouseMotionSignal.connect([](const pangolin::MouseMotionEvent& ev) {
            auto _ev = ev;
            slam_logd("passive mouse motion: x = {}, y = {}, mask = {}", ev.x, ev.y,
                      _ev.key_modifiers.mask());
        });
        window->SpecialInputSignal.connect([](const pangolin::SpecialInputEvent& ev) {
            auto _ev = ev;
            slam_logd("special input: inputtype = {}, p = ({}, {}, {}, {}), x = {}, y = {}, mask = {}",
                      ev.inType, ev.p[0], ev.p[1], ev.p[2], ev.p[3], ev.x, ev.y,
                      _ev.key_modifiers.mask());
        });
    }

    viewer->addImage(img);
    viewer->addPointCloud({
        Eigen::Vector2f{100, 100},
        Eigen::Vector2f{200, 100},
        Eigen::Vector2f{200, 200},
        Eigen::Vector2f{100, 200},
    });
    // viewer->addPointCloud({
    //     Eigen::Vector2f{0.2, 0.2},
    //     Eigen::Vector2f{0.8, 0.8},
    //     Eigen::Vector2f{0.8, 0.2},
    //     Eigen::Vector2f{0.2, 0.8},
    // });
    viewer->render();
}
