/**
 * @file simple_image.hpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-04 09:00:31
 */
#ifndef SIMPLE_IMAGE_HPP__
#define SIMPLE_IMAGE_HPP__

#include <debug/viewer.hpp>

#include <pangolin/gl/gl.h>
#include <pangolin/gl/gldraw.h>
#include <pangolin/gl/glsl.h>
#include <pangolin/gl/glvbo.h>

#include <debug/debug.hpp>
#include "../data.hpp"

namespace {

constexpr char SHADER[] = R"SHADER(
@start vertex

#version 440
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 uMVPMatrix;

out vec2 v_pos;

void main() {
    gl_Position = uMVPMatrix * vec4(aPos, 1.0);
    v_pos = aTexCoord;
}

@start fragment
#version 440
precision mediump float;

in vec2 v_pos;
uniform sampler2D imageTexture;

out vec4 outColor;

void main() {
    outColor = texture(imageTexture, v_pos);
}

)SHADER";

}  // namespace

namespace slam {

class SimpleImageShader : public slam::AbstractShader {
  public:
    SimpleImageShader() = default;
    ~SimpleImageShader() override = default;

    void init() override {
        prog.AddShader(pangolin::GlSlAnnotatedShader, SHADER);
        prog.Link();

        vbo = pangolin::GlBuffer(pangolin::GlArrayBuffer, std::vector<Eigen::Matrix<float, 5, 1>>{
                                                              {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
                                                              {1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
                                                              {-1.0f, 1.0f, 0.0f, 0.0f, 0.0f},
                                                              {1.0f, 1.0f, 0.0f, 1.0f, 0.0f}});
        vbo.Bind();

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5,
                              (void*)(sizeof(GL_FLOAT) * 3));
        glBindVertexArray(0);
    }

    void draw() override {
        if (data) {
            prog.Bind();
            prog.SetUniform("uMVPMatrix", uMVPMatrix.matrix());
            slam_logd("mvmat : {}", uMVPMatrix.matrix());
            auto image = data->getImage();

            imageTexture.Upload(image.data, GL_BGR, GL_UNSIGNED_BYTE);

            glBindVertexArray(vao);
            vbo.Bind();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, vbo.num_elements);
            glBindVertexArray(0);
            vbo.Unbind();
            prog.Unbind();
        }
    }

    void setData(std::shared_ptr<AbstractData> data) override {
        auto image_data = std::dynamic_pointer_cast<ImageData>(data);
        if (image_data) {
            this->data = image_data;
            auto image = image_data->getImage();
            imageTexture =
                pangolin::GlTexture(image.cols, image.rows, GL_RGB, false, 0, GL_BGR, GL_UNSIGNED_BYTE);
            slam_logd("Set Image : {}", image);
        } else {
            slam_loge(
                "SimpleImageShader::setData: invalid data type. Data must be instance of "
                "std::shared_ptr<slam::ImageData>.");
        }
    }

    void setMVPMatrix(const Eigen::Affine3f& matrix) override { uMVPMatrix = matrix; }

  private:
    void drawImGui() {}

  private:
    std::shared_ptr<ImageData> data;
    // shader uniform variables
    Eigen::Affine3f uMVPMatrix;
    // opengl objects
    pangolin::GlSlProgram prog;
    pangolin::GlBuffer vbo;
    pangolin::GlTexture imageTexture;
    GLuint vao;
    //
    bool sampling_linear = false;
};

}  // namespace slam


#endif  // SIMPLE_IMAGE_HPP__
