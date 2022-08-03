/**
 * @file viewer.hpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-03 20:37:55
 */
#ifndef VIEWER_HPP__
#define VIEWER_HPP__

#include <memory>
#include <string>

#include <pangolin/display/display.h>
#include <pangolin/display/view.h>
#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>


namespace slam {

class AbstractShader {
  public:
    AbstractShader() = default;
    virtual ~AbstractShader() = default;
    virtual void init() = 0;
    virtual void draw() = 0;
};


class AbstractPlugin {
  public:
    AbstractPlugin() = default;
    virtual ~AbstractPlugin() = default;
    virtual void init() = 0;
    virtual void draw() = 0;
};


class PointCloudData {
  public:
    PointCloudData(const std::vector<Eigen::Vector3f>& points) : points(points) {}
    PointCloudData(const std::vector<Eigen::Vector2f>& points) {
        for (const auto& p : points) {
            this->points.push_back(Eigen::Vector3f(p[0], p[1], 0.0));
        }
    }

  private:
    Eigen::Vector3f color;
    std::vector<Eigen::Vector3f> points;
    std::shared_ptr<AbstractShader> shader;
};


class ImageData {
  public:
    ImageData(const cv::Mat& img) : img(img) {}

  private:
    cv::Mat img;
    std::shared_ptr<AbstractShader> shader;
    std::vector<std::shared_ptr<PointCloudData>> point_clouds;
};


class Viewer {
  public:
    static Viewer& getInstance() {
        static Viewer instance;
        return instance;
    }

    ~Viewer() {}

    void addImage(const cv::Mat& image) { images.push_back(std::make_shared<ImageData>(image)); };

    template <typename T>
    void addPointCloud(std::vector<Eigen::Vector2<T>> points) {
        point_clouds.push_back(std::make_shared<PointCloudData>(points));
    };

    void render();

    const std::vector<std::shared_ptr<ImageData>>& getImages() const { return images; }
    const std::vector<std::shared_ptr<PointCloudData>>& getPointClouds() const { return point_clouds; }
    const std::shared_ptr<AbstractShader>& getImageShader() const { return image_shader; }
    const std::shared_ptr<AbstractShader>& getPointCloudShader() const { return point_cloud_shader; }

  private:
    Viewer();
    void renderImGui();

  private:
    const std::string window_name = "Debug Viewer";
    int fps = 60;
    std::vector<std::shared_ptr<ImageData>> images;
    std::vector<std::shared_ptr<PointCloudData>> point_clouds;
    std::shared_ptr<AbstractPlugin> plugin;
    std::shared_ptr<AbstractShader> image_shader;
    std::shared_ptr<AbstractShader> point_cloud_shader;
};

}  // namespace slam


#endif  // VIEWER_HPP__
