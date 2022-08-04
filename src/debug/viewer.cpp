/**
 * @file viewer.cpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-03 20:42:03
 */
#include <debug/viewer.hpp>

#include <future>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <pangolin/display/display.h>
#include <Eigen/Eigen>
#include <memory>
#include <opencv2/opencv.hpp>

#include <extension/imgui_impl_pangolin.h>
#include "data.hpp"
#include "plugin.hpp"
#include "shader.hpp"

namespace {

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;


const std::vector<std::shared_ptr<slam::AbstractShader>> ImageShaders{
    std::make_shared<slam::SimpleImageShader>()};
const std::vector<std::shared_ptr<slam::AbstractShader>> PointCloudShaders{
    std::make_shared<slam::SimplePointCloudShader>()};

const std::vector<std::shared_ptr<slam::AbstractPlugin>> Plugins{std::make_shared<slam::SimplePlugin>()};

}  // namespace


namespace slam {

std::once_flag Viewer::initFlag;
bool Viewer::initialized = false;
std::shared_ptr<Viewer> Viewer::instance = nullptr;


Viewer::Viewer() {
    if (!initialized) {
        initialize();
        initialized = true;
    } else {
        slam_loge("Viewer must be Singleton object. Get instance by getInstance().");
    }
}

void Viewer::initialize() {
    pangolin::CreateWindowAndBind(window_name, WINDOW_WIDTH, WINDOW_HEIGHT);
    width = WINDOW_WIDTH;
    height = WINDOW_HEIGHT;
    {
        // add window resize callback
        auto window = pangolin::GetBoundWindow();
        window->ResizeSignal.connect([this](const pangolin::WindowResizeEvent& ev) {
            this->width = ev.width;
            this->height = ev.height;
        });
    }

    for (auto& shader : ImageShaders) {
        shader->init();
    }
    for (auto& shader : PointCloudShaders) {
        shader->init();
    }

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

    slam_logd("Viewer initialized.");
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

void Viewer::addImage(const cv::Mat& image) { images.push_back(std::make_shared<ImageData>(image)); };

void Viewer::addPointCloud(const std::vector<Eigen::Vector2f>& points) {
    point_clouds.push_back(std::make_shared<PointCloudData>(points));
};


void Viewer::renderImGui() {
    // plugin selector

    // image shader selector

    // point cloud shader selector
}


}  // namespace slam
