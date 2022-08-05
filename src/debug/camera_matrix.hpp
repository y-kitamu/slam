/**
 * @file camera_matrix.hpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-04 15:21:25
 */
#ifndef CAMERA_MATRIX_HPP__
#define CAMERA_MATRIX_HPP__

#include <pangolin/pangolin.h>
#include <Eigen/Eigen>
#include <Eigen/Geometry>

#include <debug/debug.hpp>

namespace {
constexpr float ZOOM_SCALE = 0.05f;
}

namespace slam {


Eigen::Matrix4f zoom(const Eigen::Matrix4f& mat, float dz) {
    Eigen::Matrix4f scale;
    dz *= ZOOM_SCALE;
    scale = Eigen::Scaling(Eigen::Vector4f{1.0f + dz, 1.0f + dz, 1.0f, 1.0f});
    return scale * mat;
}


Eigen::Matrix4f pan(const Eigen::Matrix4f& mat, float dx, float dy, float width, float height) {
    Eigen::Affine3f trans(mat);
    trans(0, 3) += dx / width * 2.0;
    trans(1, 3) += dy / height * 2.0;
    return trans.matrix();
}

Eigen::Matrix4f getPixel2glMat(const cv::Mat& img) {
    float scale_x = img.cols * 0.5;
    float scale_y = img.rows * 0.5;
    Eigen::Translation3f pre((float)-img.cols / 2.0f, (float)-img.rows / 2.0f, 0.0f);
    // Eigen::Translation3f post(0.5f, 0.5f, 0.0f);
    auto rescale = Eigen::Scaling(1.0f / scale_x, -1.0f / scale_y, 1.0f);
    Eigen::Affine3f aff = rescale * pre;
    return aff.matrix();
}


}  // namespace slam


#endif  // CAMERA_MATRIX_HPP__
