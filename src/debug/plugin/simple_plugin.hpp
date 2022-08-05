/**
 * @file simple_plugin.hpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-04 09:14:22
 */
#ifndef SIMPLE_PLUGIN_HPP__
#define SIMPLE_PLUGIN_HPP__

#include <debug/viewer.hpp>

#include <imgui.h>
#include <Eigen/Geometry>

#include "../camera_matrix.hpp"
#include "../data.hpp"

#include <debug/debug.hpp>

namespace slam {

class SimplePlugin : public slam::AbstractPlugin {
  public:
    SimplePlugin() {}
    ~SimplePlugin() override {}

    void init() override {
        auto viewer = slam::Viewer::getInstance();
        pangolin::Display(viewport_name).SetBounds(0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

        auto images = viewer->getImages();
        for (auto& image : images) {
            auto d = std::dynamic_pointer_cast<slam::ImageData>(image);
            if (d) {
                setCurrentImage(d);
                break;
            }
        }

        current_point_clouds.clear();
        auto points = viewer->getPointClouds();
        for (auto& point : points) {
            auto d = std::dynamic_pointer_cast<slam::PointCloudData>(point);
            if (d) {
                current_point_clouds.emplace_back(d);
                break;
            }
        }
    }

    void draw() override {
        auto& view = pangolin::Display(viewport_name);
        view.Activate();

        handleEvents();

        auto viewer = slam::Viewer::getInstance();
        auto image_shader = viewer->getImageShader();
        auto point_cloud_shader = viewer->getPointCloudShader();

        Eigen::Affine3f camera_pose(camera_pose_mat);
        Eigen::Affine3f scaled_camera_pose(
            camera_pose_mat * pixel2gl_mat);  // pixel座標からのスケール変換込みのcamera_pose

        if (current_image) {
            image_shader->setMVPMatrix(camera_pose);
            image_shader->draw();
        }

        for (auto& point_cloud : current_point_clouds) {
            point_cloud_shader->setMVPMatrix(scaled_camera_pose);
            point_cloud_shader->setData(point_cloud);
            point_cloud_shader->draw();
        }
    }

  private:
    void drawImGui() {}

    /**
     * @brief Mouse, keyboard input callback
     */
    void handleEvents() {
        auto& view = pangolin::Display(viewport_name);

        ImGuiIO& io = ImGui::GetIO();

        if (!io.WantCaptureMouse) {
            if (std::abs(io.MouseWheel) > 1e-5) {
                camera_pose_mat = slam::zoom(camera_pose_mat, io.MouseWheel);
            }
            if (io.MouseDown[0]) {
                float dx = io.MouseDelta[0];
                float dy = io.MouseDelta[1];
                camera_pose_mat = slam::pan(camera_pose_mat, dx, -dy, view.v.w, view.v.h);
            }
        }

        if (!io.WantCaptureKeyboard) {
        }
    }

    void setCurrentImage(const std::shared_ptr<slam::ImageData>& image) {
        auto viewer = slam::Viewer::getInstance();
        auto& view = pangolin::Display(viewport_name);

        current_image = image;
        auto image_shader = viewer->getImageShader();
        image_shader->setData(current_image);

        camera_pose_mat = Eigen::Matrix4f::Identity();
        pixel2gl_mat = slam::getPixel2glMat(view, current_image->getImage());
    }

  private:
    std::shared_ptr<slam::ImageData> current_image;
    std::vector<std::shared_ptr<slam::PointCloudData>> current_point_clouds;
    //
    std::string viewport_name = "simple_plugin_viewport";
    // geometry params
    Eigen::Matrix4f camera_pose_mat = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f pixel2gl_mat = Eigen::Matrix4f::Identity();
};


}  // namespace slam

#endif  // SIMPLE_PLUGIN_HPP__
