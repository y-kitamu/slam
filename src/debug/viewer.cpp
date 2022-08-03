/**
 * @file viewer.cpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-03 20:42:03
 */
#include "viewer.hpp"

#include <future>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <pangolin/display/display.h>
#include <Eigen/Eigen>
#include <memory>
#include <opencv2/opencv.hpp>

#include "extension/imgui_impl_pangolin.h"

namespace {

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;


class SimpleImageShader : public slam::AbstractShader {
  public:
    SimpleImageShader() = default;
    ~SimpleImageShader() override = default;
    void init() override {}
    void draw() override {}
};


class SimplePointCloudShader : public slam::AbstractShader {
  public:
    SimplePointCloudShader() = default;
    ~SimplePointCloudShader() override = default;
    void init() override {}
    void draw() override {}
};


const std::vector<std::shared_ptr<slam::AbstractShader>> ImageShaders{
    std::make_shared<SimpleImageShader>()};
const std::vector<std::shared_ptr<slam::AbstractShader>> PointCloudShaders{
    std::make_shared<SimplePointCloudShader>()};

class SimplePlugin : public slam::AbstractPlugin {
  public:
    SimplePlugin() {}
    ~SimplePlugin() override {}

    void init() override {
        auto viewer = slam::Viewer::getInstance();
        auto images = viewer.getImages();
        if (images.size() > 0) {
            current_image = images[0];
        }
        auto points = viewer.getPointClouds();
        if (points.size() > 0) {
            current_point_clouds = points[0];
        }
    }

    void draw() override {
        auto viewer = slam::Viewer::getInstance();
        auto image_shader = viewer.getImageShader();
        auto point_cloud_shader = viewer.getPointCloudShader();
    }

  private:
    std::shared_ptr<slam::ImageData> current_image;
    std::shared_ptr<slam::PointCloudData> current_point_clouds;
    Eigen::Matrix3f camera_pose;
};


const std::vector<std::shared_ptr<slam::AbstractPlugin>> Plugins{std::make_shared<SimplePlugin>()};

}  // namespace


namespace slam {

Viewer::Viewer() {
    pangolin::CreateWindowAndBind(window_name, WINDOW_WIDTH, WINDOW_HEIGHT);

    plugin = Plugins[0];
    image_shader = ImageShaders[0];
    point_cloud_shader = PointCloudShaders[0];

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigInputTrickleEventQueue = true;
    ImGui_ImplOpenGL3_Init("#version 440");
    ImGui_ImplPangolin_Init(WINDOW_WIDTH, WINDOW_HEIGHT);
    ImGui::StyleColorsDark();
}

void Viewer::render() {
    const pangolin::View& view = pangolin::Display("viewport").SetBounds(0.0f, 1.0f, 0.0f, 1.0f, 1.0);

    plugin->init();

    glClearColor(0.64f, 0.5f, 0.81f, 0.0f);
    while (!pangolin::ShouldQuit()) {
        auto future = std::async(std::launch::async, [&]() {
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0 / fps)));
        });

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplPangolin_NewFrame();
        ImGui::NewFrame();

        plugin->draw();

        view.Activate();
        renderImGui();

        // render imgui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // swap buffers
        pangolin::FinishFrame();

        future.wait();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplPangolin_Shutdown();
    ImGui::DestroyContext();
}

void Viewer::renderImGui() {
    // plugin selector

    // image shader selector

    // point cloud shader selector
}


}  // namespace slam
