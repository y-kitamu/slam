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
        handleEvents();

        auto viewer = slam::Viewer::getInstance();
        auto image_shader = viewer->getImageShader();
        auto point_cloud_shader = viewer->getPointCloudShader();

        Eigen::Affine3f camera_pose(camera_pose_mat);

        if (current_image) {
            image_shader->setMVPMatrix(camera_pose);
            image_shader->draw();
        }

        for (auto& point_cloud : current_point_clouds) {
            point_cloud_shader->setMVPMatrix(camera_pose);
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
        auto viewer = Viewer::getInstance();
        ImGuiIO& io = ImGui::GetIO();

        if (!io.WantCaptureMouse) {
            if (std::abs(io.MouseWheel) > 1e-5) {
                camera_pose_mat = slam::zoom(camera_pose_mat, io.MouseWheel);
            }
            if (io.MouseDown[0]) {
                if (start_mouse_pos.x() < 0 && start_mouse_pos.y() < 0) {
                    start_mouse_pos = Eigen::Vector2f(io.MousePos.x, io.MousePos.y);
                } else {
                    float dx = io.MousePos.x - start_mouse_pos.x();
                    float dy = io.MousePos.y - start_mouse_pos.y();
                    camera_pose_mat =
                        slam::pan(camera_pose_mat, dx, -dy, viewer->getWidth(), viewer->getHeight());
                }
            } else {
                start_mouse_pos = Eigen::Vector2f(-1, -1);
            }
        }

        if (!io.WantCaptureKeyboard) {
        }
    }

    void setCurrentImage(const std::shared_ptr<slam::ImageData>& image) {
        auto viewer = slam::Viewer::getInstance();
        current_image = image;
        auto image_shader = viewer->getImageShader();
        image_shader->setData(current_image);
    }

  private:
    std::shared_ptr<slam::ImageData> current_image;
    std::vector<std::shared_ptr<slam::PointCloudData>> current_point_clouds;
    // geometry params
    Eigen::Matrix4f camera_pose_mat = Eigen::Matrix4f::Identity();
    Eigen::Vector2f start_mouse_pos;
};


}  // namespace slam

#endif  // SIMPLE_PLUGIN_HPP__
