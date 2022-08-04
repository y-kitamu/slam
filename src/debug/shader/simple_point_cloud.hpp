/**
 * @file simple_point_cloud.hpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-04 09:11:58
 */
#ifndef SIMPLE_POINT_CLOUD_HPP__
#define SIMPLE_POINT_CLOUD_HPP__

#include <debug/debug.hpp>

#include "../data.hpp"


namespace slam {

class SimplePointCloudShader : public slam::AbstractShader {
  public:
    ~SimplePointCloudShader() override = default;

    SimplePointCloudShader() {}

    void init() override {}

    void draw() override {
        if (data) {
        }
    }

    void setData(std::shared_ptr<slam::AbstractData> data) override {
        auto point_cloud_data = std::dynamic_pointer_cast<slam::PointCloudData>(data);
        if (point_cloud_data) {
            this->data = point_cloud_data;
        } else {
            slam_loge(
                "SimplePointCloudShader::setData: invalid data type. Data must be instance of "
                "std::shared_ptr<slam::PointCloudData>.");
        }
    }

    void setMVPMatrix(const Eigen::Affine3f& mat) override {}

  private:
    std::shared_ptr<slam::PointCloudData> data;
};
}  // namespace slam


#endif // SIMPLE_POINT_CLOUD_HPP__
