/**
 * @file pangolin_test.cpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-01 08:08:06
 */
// pangolinをincludeする際に必要なmacroを定義
#ifndef HAVE_EIGEN
#define HAVE_EIGEN
#endif
#ifndef HAVE_GLEW
#define HAVE_GLEW
#endif
#include <filesystem>
#include <future>

#include <glog/logging.h>
#include <pangolin/display/display.h>
#include <pangolin/display/view.h>
#include <pangolin/gl/gl.h>
#include <pangolin/gl/gldraw.h>
#include <pangolin/gl/glsl.h>
#include <pangolin/gl/glvbo.h>
#include <pangolin/windowing/window.h>
#include <argparse/argparse.hpp>
#include <opencv2/opencv.hpp>
#include "fmt/format.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include "slam.hpp"

namespace fs = std::filesystem;

// struct Events {
//     pangolin::WindowResizeEvent window_resize;
//     pangolin::KeyboardEvent keyboard;
//     pangolin::MouseEvent mouse;
//     pangolin::MouseMotionEvent mouse_motion;
//     pangolin::SpecialInputEvent special_input;
// };


void sample(const cv::Mat& image) {
    auto shader_dir = fs::path(__FILE__).parent_path() / "shader";
    auto vert_shader = shader_dir / "simple.vert";
    auto frag_shader = shader_dir / "simple.frag";

    pangolin::CreateWindowAndBind("Pango GL Triangle", 500, 500);
    {
        auto window = pangolin::GetBoundWindow();
        window->ResizeSignal.connect([](const pangolin::WindowResizeEvent& ev) {
            logd("window resize: {} {}", ev.width, ev.height);
        });
        window->KeyboardSignal.connect([](const pangolin::KeyboardEvent& ev) {
            auto _ev = ev;
            logd("keyboard: key = {}, pressed = {}, x = {}, y = {}, mask = {}", ev.key, ev.pressed, ev.x,
                 ev.y, _ev.key_modifiers.mask());
        });
        window->MouseSignal.connect([](const pangolin::MouseEvent& ev) {
            auto _ev = ev;
            logd("mouse: button = {}, pressed = {}, x = {}, y = {}, mask = {}", ev.button, ev.pressed,
                 ev.x, ev.y, _ev.key_modifiers.mask());
        });
        window->MouseMotionSignal.connect([](const pangolin::MouseMotionEvent& ev) {
            auto _ev = ev;
            logd("mouse motion: x = {}, y = {}, mask = {}", ev.x, ev.y, _ev.key_modifiers.mask());
        });
        window->SpecialInputSignal.connect([](const pangolin::SpecialInputEvent& ev) {
            auto _ev = ev;
            logd("special input: inputtype = {}, p = ({}, {}, {}, {}), x = {}, y = {}, mask = {}",
                 ev.inType, ev.p[0], ev.p[1], ev.p[2], ev.p[3], ev.x, ev.y, _ev.key_modifiers.mask());
        });
    }

    pangolin::GlSlProgram prog;
    prog.AddShaderFromFile(pangolin::GlSlVertexShader, vert_shader.string());
    prog.AddShaderFromFile(pangolin::GlSlFragmentShader, frag_shader.string());
    prog.Link();

    pangolin::View& image_view = pangolin::Display("viewport").SetBounds(0.2f, 0.8f, 0.2f, 0.8f, 1.0);
    pangolin::View& window_view = pangolin::Display("viewport").SetBounds(0.0f, 1.0f, 0.0f, 1.0f, 1.0);

    pangolin::GlBuffer vbo(pangolin::GlArrayBuffer,
                           std::vector<Eigen::Matrix<float, 5, 1>>{{-0.5f, -0.5f, 0.0f, 0.0f, 0.0f},
                                                                   {0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
                                                                   {-0.5f, 0.5f, 0.0f, 0.0f, 1.0f},
                                                                   {0.5f, 0.5f, 0.0f, 1.0f, 1.0f}});
    vbo.Bind();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, (void*)(sizeof(GL_FLOAT) * 3));

    glBindVertexArray(0);

    pangolin::GlTexture imageTexture(image.cols, image.rows, GL_RGB, false, 0, GL_RGB, GL_UNSIGNED_BYTE);

    // imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    {
        //
        io.ConfigInputTrickleEventQueue = true;
    }
    ImGui_ImplOpenGL3_Init("#version 440");
    ImGui_ImplPangolin_Init(500, 500);
    ImGui::StyleColorsDark();

    glClearColor(0.64f, 0.5f, 0.81f, 0.0f);

    bool show_demo_window = true;
    bool show_sample_window = true;

    while (!pangolin::ShouldQuit()) {
        auto future = std::async(std::launch::async, [&]() {
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0 / 60.0)));
        });

        // Clear the window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // image_view.Activate();

        prog.Bind();
        imageTexture.Upload(image.data, GL_RGB, GL_UNSIGNED_BYTE);

        glBindVertexArray(vao);
        vbo.Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, vbo.num_elements);

        glBindVertexArray(0);
        vbo.Unbind();
        prog.Unbind();

        window_view.Activate();

        // imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplPangolin_NewFrame();
        ImGui::NewFrame();
        {
            if (show_demo_window) {
                ImGui::ShowDemoWindow(&show_demo_window);
            }
            ImGuiContext* g = ImGui::GetCurrentContext();

            ImGui::Begin("Hello, world!", &show_sample_window, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text(fmt::format("FPS : {}", 1.0 / io.DeltaTime).c_str());

            if (g->HoveredWindow && g->HoveredWindow->Name) {
                ImGui::Text(fmt::format("Hovered window : {}", g->HoveredWindow->Name).c_str());
            } else {
                ImGui::Text("Hovered window : None");
            }

            ImGui::Text(fmt::format("Mouse position : ({}, {})", io.MousePos.x, io.MousePos.y).c_str());
            for (int i = 0; i < g->Windows.Size; i++) {
                ImGuiWindow* window = g->Windows[i];
                auto rect = window->OuterRectClipped;
                ImGui::Text(fmt::format("{}, (l, t, r, b) = ({}, {}, {}, {}), contain mouse = {}",
                                        window->Name, rect.Min[0], rect.Min[1], rect.Max[0], rect.Max[1],
                                        rect.Contains(io.MousePos))
                                .c_str());
            }

            ImGui::End();
        }

        // render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        pangolin::FinishFrame();

        future.wait();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplPangolin_Shutdown();
    ImGui::DestroyContext();
}

int main(int argc, char** argv) {
    // google glogの初期化
    google::InitGoogleLogging(argv[0]);
    //クラッシュ時にスタックトレースを吐くうシグナルハンドラを設定
    google::InstallFailureSignalHandler();

    argparse::ArgumentParser parser("Pangolin test");
    parser.add_argument("-i", "--image")
        .help("Input Image")
        .default_value(std::string("/home/kitamura/work/slam/dataset/sample_image/lena.png"));

    try {
        parser.parse_args(argc, argv);  // Example: ./main --color red --color green --color blue
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    auto image_path = fs::path(parser.get<std::string>("--image"));
    std::cout << image_path.generic_string() << std::endl;

    cv::Mat img = cv::imread(image_path.string());

    sample(img);
    return 0;
}
