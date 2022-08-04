/**
 * @file data.hpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-04 10:55:47
 */
#ifndef DATA_HPP__
#define DATA_HPP__

#include <debug/viewer.hpp>


namespace slam {

class PointCloudData : public AbstractData {
  public:
    PointCloudData() = default;
    ~PointCloudData() override = default;

  public:
    PointCloudData(const std::vector<Eigen::Vector3f>& points) : points(points) {}
    PointCloudData(const std::vector<Eigen::Vector2f>& points) {
        for (const auto& p : points) {
            this->points.push_back(Eigen::Vector3f(p[0], p[1], 0.0));
        }
    }
    const Eigen::Vector4f getColor() const {
        return Eigen::Vector4f(color.x(), color.y(), color.z(), 1.0);
    }
    const std::vector<Eigen::Vector3f>& getPoints() const { return points; }

  private:
    Eigen::Vector3f color;
    std::vector<Eigen::Vector3f> points;
};


class ImageData : public AbstractData {
  public:
    ImageData() = default;
    ~ImageData() override = default;

  public:
    ImageData(const cv::Mat& img) : img(img) {}
    const cv::Mat& getImage() const { return img; }

  private:
    cv::Mat img;
    std::vector<std::shared_ptr<PointCloudData>> point_clouds;
};

}  // namespace slam


#endif // DATA_HPP__
