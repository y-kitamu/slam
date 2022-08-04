/**
 * @file simple_plugin.hpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-04 09:14:22
 */
#ifndef SIMPLE_PLUGIN_HPP__
#define SIMPLE_PLUGIN_HPP__

#include <debug/viewer.hpp>

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
        auto viewer = slam::Viewer::getInstance();
        auto image_shader = viewer->getImageShader();
        auto point_cloud_shader = viewer->getPointCloudShader();

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

    void setCurrentImage(const std::shared_ptr<slam::ImageData>& image) {
        auto viewer = slam::Viewer::getInstance();
        current_image = image;
        auto image_shader = viewer->getImageShader();
        image_shader->setData(current_image);
    }

  private:
    std::shared_ptr<slam::ImageData> current_image;
    std::vector<std::shared_ptr<slam::PointCloudData>> current_point_clouds;
    Eigen::Matrix4f camera_pose = Eigen::Matrix4f::Identity();
};


}  // namespace slam

#endif  // SIMPLE_PLUGIN_HPP__
