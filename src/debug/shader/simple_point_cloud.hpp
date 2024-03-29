/**
 * @file simple_point_cloud.hpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-04 09:11:58
 */
#ifndef SIMPLE_POINT_CLOUD_HPP__
#define SIMPLE_POINT_CLOUD_HPP__

#include <debug/debug.hpp>

#include <pangolin/gl/gl.h>
#include <pangolin/gl/gldraw.h>
#include <pangolin/gl/glsl.h>
#include <pangolin/gl/glvbo.h>

#include "../data.hpp"

namespace {

constexpr char SIMPLE_POINT_CLOUD_SHADER[] = R"SHADER(
@start vertex
#version 440

layout(location = 0) in vec3 aPos;

uniform mat4 uMVPMatrix;

void main() {
    gl_Position = uMVPMatrix * vec4(aPos, 1.0);
}

@start fragment
#version 440
precision mediump float;

uniform vec4 uColor;

out vec4 outColor;

void main() {
    outColor = uColor;
}
)SHADER";
}

namespace slam {

class SimplePointCloudShader : public slam::AbstractShader {
  public:
    ~SimplePointCloudShader() override = default;

    SimplePointCloudShader() {}

    void init() override {
        prog.AddShader(pangolin::GlSlAnnotatedShader, SIMPLE_POINT_CLOUD_SHADER);
        prog.Link();

        glGenVertexArrays(1, &vao);
    }

    void draw() override {
        if (data) {
            // slam_logd("render point cloud");
            prog.Bind();
            prog.SetUniform("uMVPMatrix", uMVPMatrix.matrix());
            prog.SetUniform("uColor", data->getColor());
            glPointSize(uPointSize);

            glBindVertexArray(vao);
            vbo.Bind();
            glDrawArrays(GL_POINTS, 0, vbo.num_elements);
            glBindVertexArray(0);
            vbo.Unbind();

            prog.Unbind();
        }
    }

    void setData(std::shared_ptr<slam::AbstractData> data) override {
        auto point_cloud_data = std::dynamic_pointer_cast<slam::PointCloudData>(data);
        if (point_cloud_data) {
            this->data = point_cloud_data;
            vbo = pangolin::GlBuffer(pangolin::GlArrayBuffer, this->data->getPoints());
            vbo.Bind();

            glBindVertexArray(vao);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, 0);
            glBindVertexArray(0);
            vbo.Unbind();
        } else {
            slam_loge(
                "SimplePointCloudShader::setData: invalid data type. Data must be instance of "
                "std::shared_ptr<slam::PointCloudData>.");
        }
    }

    void setMVPMatrix(const Eigen::Affine3f& mat) override { uMVPMatrix = mat; }

  private:
    std::shared_ptr<slam::PointCloudData> data;
    // shader uniform variables
    Eigen::Affine3f uMVPMatrix;
    // opengl objects
    pangolin::GlSlProgram prog;
    pangolin::GlBuffer vbo;
    GLuint vao;
    //
    float uPointSize = 10.0f;
};
}  // namespace slam


#endif // SIMPLE_POINT_CLOUD_HPP__
