/**
 * @file imgui_impl_pangolin.cpp
 * @brief
 * @author Yusuke Kitamura <ymyk6602@gmail.com>
 * @date 2022-08-02 17:50:59
 */
#include "imgui_impl_pangolin.h"

#include <chrono>

#include <pangolin/display/display.h>
#include "imgui.h"

#include "debug/debug.hpp"

namespace {
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

double get_time() {
    auto millisec_since_epoch =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    return millisec_since_epoch / 1000.0;
}
}  // namespace


struct ImGui_ImplPangolin_Data {
    int window_width;
    int window_height;
    double time;
};


static ImGui_ImplPangolin_Data* ImGui_ImplPangolin_GetBackendData() {
    return ImGui::GetCurrentContext() ? (ImGui_ImplPangolin_Data*)ImGui::GetIO().BackendPlatformUserData
                                      : NULL;
}


bool ImGui_ImplPangolin_Init(int width, int height) {
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");

    ImGui_ImplPangolin_Data* bd = IM_NEW(ImGui_ImplPangolin_Data)();
    bd->window_width = width;
    bd->window_height = height;
    bd->time = get_time();

    io.BackendPlatformUserData = bd;
    io.BackendPlatformName = "imgui_impl_pangolin";

    pangolin::WindowInterface* window = pangolin::GetBoundWindow();
    window->ResizeSignal.connect([](pangolin::WindowResizeEvent event) {
        ImGui_ImplPangolin_Data* _bd = ImGui_ImplPangolin_GetBackendData();
        _bd->window_width = event.width;
        _bd->window_height = event.height;
    });

    return true;
}


void ImGui_ImplPangolin_Shutdown() {
    ImGui_ImplPangolin_Data* bd = ImGui_ImplPangolin_GetBackendData();
    IM_ASSERT(bd != NULL && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    io.BackendPlatformName = NULL;
    io.BackendPlatformUserData = NULL;
    IM_DELETE(bd);
}


void ImGui_ImplPangolin_NewFrame() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplPangolin_Data* bd = ImGui_ImplPangolin_GetBackendData();
    IM_ASSERT(bd != NULL && "No platform backend to render with, or already shutdown?");

    io.DisplaySize = ImVec2((float)bd->window_width, (float)bd->window_height);
    if (bd->window_width > 0 && bd->window_height > 0) {
        GLint dims[4] = {0};
        glGetIntegerv(GL_VIEWPORT, dims);
        GLint fbWidth = dims[2];
        GLint fbHeight = dims[3];
        // io.DisplayFramebufferScale =
        //     ImVec2((float)bd->window_width / fbWidth, (float)bd->window_height / fbHeight);
    }

    double current_time = get_time();
    float delta = (float)(current_time - bd->time);
    io.DeltaTime = delta > 0.0 ? delta : 1.0f / 60.0f;
    bd->time = current_time;
}
