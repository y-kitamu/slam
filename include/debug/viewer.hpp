/**
 * @file viewer.hpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-03 20:37:55
 */
#ifndef VIEWER_HPP__
#define VIEWER_HPP__

#ifndef HAVE_EIGEN
#define HAVE_EIGEN
#endif
#ifndef HAVE_GLEW
#define HAVE_GLEW
#endif

#include <memory>
#include <string>

#include <pangolin/display/display.h>
#include <pangolin/display/view.h>
#include <Eigen/Eigen>
#include <Eigen/Geometry>
#include <opencv2/opencv.hpp>


namespace slam {

class AbstractData {
  public:
    AbstractData() = default;
    virtual ~AbstractData() = default;
};

class AbstractShader {
  public:
    AbstractShader() = default;
    virtual ~AbstractShader() = default;
    /**
     * @brief Shaderの初期化 (compileなど)を行う関数。Window作成時に(Viewerから)呼び出す
     */
    virtual void init() = 0;
    /**
     * @brief Rendering loop内で呼び出される関数。Pluginから呼び出す
     */
    virtual void draw() = 0;
    /**
     * @brief Shaderを使用するために必要なデータを渡す関数
     * @param data Shaderに渡すデータ。継承先でそれぞれのデータクラスにdynamic castされる
     */
    virtual void setData(std::shared_ptr<AbstractData> data) = 0;
    virtual void setMVPMatrix(const Eigen::Affine3f& mat) = 0;
};


class AbstractPlugin {
  public:
    AbstractPlugin() = default;
    virtual ~AbstractPlugin() = default;
    /**
     * @brief Rendering loopの直前に呼び出される関数
     */
    virtual void init() = 0;
    /**
     * @brief Rendering loop内で呼び出される関数。Viewerから呼び出す
     */
    virtual void draw() = 0;
};


class Viewer {
  public:
    static std::shared_ptr<Viewer> getInstance() {
        std::call_once(initFlag, create);
        assert(instance);
        return instance;
    }

    Viewer();

    void addImage(const cv::Mat& image);

    template <typename T>
    void addPointCloud(const std::vector<T>& points);

    void render();

    const std::vector<std::shared_ptr<AbstractData>>& getImages() const { return images; }
    const std::vector<std::shared_ptr<AbstractData>>& getPointClouds() const { return point_clouds; }
    const std::shared_ptr<AbstractShader>& getImageShader() const { return image_shader; }
    const std::shared_ptr<AbstractShader>& getPointCloudShader() const { return point_cloud_shader; }

    const int getWidth() const { return width; }
    const int getHeight() const { return height; }

  private:
    void initialize();
    void renderImGui();

    static void create() { instance = std::make_shared<Viewer>(); }

    static void destroy() {
        instance.reset();
        instance = nullptr;
    }

  private:
    // window parameter
    const std::string window_name = "Debug Viewer";
    int fps = 60;
    int width, height;
    //
    std::vector<std::shared_ptr<AbstractData>> images;
    std::vector<std::shared_ptr<AbstractData>> point_clouds;
    std::shared_ptr<AbstractShader> image_shader;
    std::shared_ptr<AbstractShader> point_cloud_shader;
    std::shared_ptr<AbstractPlugin> plugin;
    // For singleton pattern
    static std::once_flag initFlag;
    static bool initialized;
    static std::shared_ptr<Viewer> instance;
};

}  // namespace slam


#endif  // VIEWER_HPP__
