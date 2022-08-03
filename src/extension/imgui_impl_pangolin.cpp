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
#include "imgui_internal.h"


constexpr float MOUSE_WHEEL_SCALE = 1.0f;

static double get_time() {
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::system_clock;
    auto millisec_since_epoch =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    return millisec_since_epoch / 1000.0;
}


struct ImGui_ImplPangolin_Data {
    int window_width;
    int window_height;
    double time;
};


static ImGui_ImplPangolin_Data* getBackendData() {
    return ImGui::GetCurrentContext() ? (ImGui_ImplPangolin_Data*)ImGui::GetIO().BackendPlatformUserData
                                      : NULL;
}


static ImGuiKey keycodeToImGuiKey(int keycode) {
    using namespace pangolin;
    switch (keycode) {
        case PANGO_SPECIAL + PANGO_KEY_F1:
            return ImGuiKey_F1;
        case PANGO_SPECIAL + PANGO_KEY_F2:
            return ImGuiKey_F2;
        case PANGO_SPECIAL + PANGO_KEY_F3:
            return ImGuiKey_F3;
        case PANGO_SPECIAL + PANGO_KEY_F4:
            return ImGuiKey_F4;
        case PANGO_SPECIAL + PANGO_KEY_F5:
            return ImGuiKey_F5;
        case PANGO_SPECIAL + PANGO_KEY_F6:
            return ImGuiKey_F6;
        case PANGO_SPECIAL + PANGO_KEY_F7:
            return ImGuiKey_F7;
        case PANGO_SPECIAL + PANGO_KEY_F8:
            return ImGuiKey_F8;
        case PANGO_SPECIAL + PANGO_KEY_F9:
            return ImGuiKey_F9;
        case PANGO_SPECIAL + PANGO_KEY_F10:
            return ImGuiKey_F10;
        case PANGO_SPECIAL + PANGO_KEY_F11:
            return ImGuiKey_F11;
        case PANGO_SPECIAL + PANGO_KEY_F12:
            return ImGuiKey_F12;
        case PANGO_SPECIAL + PANGO_KEY_LEFT:
            return ImGuiKey_LeftArrow;
        case PANGO_SPECIAL + PANGO_KEY_UP:
            return ImGuiKey_UpArrow;
        case PANGO_SPECIAL + PANGO_KEY_RIGHT:
            return ImGuiKey_RightArrow;
        case PANGO_SPECIAL + PANGO_KEY_DOWN:
            return ImGuiKey_DownArrow;
        case PANGO_SPECIAL + PANGO_KEY_HOME:
            return ImGuiKey_Home;
        case PANGO_SPECIAL + PANGO_KEY_END:
            return ImGuiKey_End;
        case PANGO_SPECIAL + PANGO_KEY_INSERT:
            return ImGuiKey_Insert;
        case PANGO_KEY_TAB:
            return ImGuiKey_Tab;
        case PANGO_KEY_ESCAPE:
            return ImGuiKey_Escape;
        case (int)'\'':
            return ImGuiKey_Apostrophe;
        case (int)',':
            return ImGuiKey_Comma;
        case (int)'-':
            return ImGuiKey_Minus;
        case (int)'.':
            return ImGuiKey_Period;
        case (int)'/':
            return ImGuiKey_Slash;
        case (int)';':
            return ImGuiKey_Semicolon;
        case (int)'=':
            return ImGuiKey_Equal;
        case (int)'[':
            return ImGuiKey_LeftBracket;
        case (int)'\\':
            return ImGuiKey_Backslash;
        case (int)']':
            return ImGuiKey_RightBracket;
        case (int)'`':
            return ImGuiKey_GraveAccent;
        case (int)' ':
            return ImGuiKey_Space;
        case (int)'\n':
            return ImGuiKey_Enter;
        case (int)'\b':
            return ImGuiKey_Backspace;
        case 0x7f:
            return ImGuiKey_Delete;
        case (int)'a':
        case (int)'A':
            return ImGuiKey_A;
        case (int)'b':
        case (int)'B':
            return ImGuiKey_B;
        case (int)'c':
        case (int)'C':
            return ImGuiKey_C;
        case (int)'d':
        case (int)'D':
            return ImGuiKey_D;
        case (int)'e':
        case (int)'E':
            return ImGuiKey_E;
        case (int)'f':
        case (int)'F':
            return ImGuiKey_F;
        case (int)'g':
        case (int)'G':
            return ImGuiKey_G;
        case (int)'h':
        case (int)'H':
            return ImGuiKey_H;
        case (int)'i':
        case (int)'I':
            return ImGuiKey_I;
        case (int)'j':
        case (int)'J':
            return ImGuiKey_J;
        case (int)'k':
        case (int)'K':
            return ImGuiKey_K;
        case (int)'l':
        case (int)'L':
            return ImGuiKey_L;
        case (int)'m':
        case (int)'M':
            return ImGuiKey_M;
        case (int)'n':
        case (int)'N':
            return ImGuiKey_N;
        case (int)'o':
        case (int)'O':
            return ImGuiKey_O;
        case (int)'p':
        case (int)'P':
            return ImGuiKey_P;
        case (int)'q':
        case (int)'Q':
            return ImGuiKey_Q;
        case (int)'r':
        case (int)'R':
            return ImGuiKey_R;
        case (int)'s':
        case (int)'S':
            return ImGuiKey_S;
        case (int)'t':
        case (int)'T':
            return ImGuiKey_T;
        case (int)'u':
        case (int)'U':
            return ImGuiKey_U;
        case (int)'v':
        case (int)'V':
            return ImGuiKey_V;
        case (int)'w':
        case (int)'W':
            return ImGuiKey_W;
        case (int)'x':
        case (int)'X':
            return ImGuiKey_X;
        case (int)'y':
        case (int)'Y':
            return ImGuiKey_Y;
        case (int)'z':
        case (int)'Z':
            return ImGuiKey_Z;
        case (int)'0':
            return ImGuiKey_0;
        case (int)'1':
            return ImGuiKey_1;
        case (int)'2':
            return ImGuiKey_2;
        case (int)'3':
            return ImGuiKey_3;
        case (int)'4':
            return ImGuiKey_4;
        case (int)'5':
            return ImGuiKey_5;
        case (int)'6':
            return ImGuiKey_6;
        case (int)'7':
            return ImGuiKey_7;
        case (int)'8':
            return ImGuiKey_8;
        case (int)'9':
            return ImGuiKey_9;
    }
    return ImGuiKey_None;
}

static void updateKeyModifiers(int pangoModMask, bool pressed) {
    ImGuiIO& io = ImGui::GetIO();
    if (pressed) {
        io.AddKeyEvent(ImGuiKey_ModCtrl, pangoModMask & pangolin::KeyModifierCtrl);
        io.AddKeyEvent(ImGuiKey_ModShift, pangoModMask & pangolin::KeyModifierShift);
        io.AddKeyEvent(ImGuiKey_ModAlt, pangoModMask & pangolin::KeyModifierAlt);
    } else {
        io.AddKeyEvent(ImGuiKey_ModCtrl, false);
        io.AddKeyEvent(ImGuiKey_ModShift, false);
        io.AddKeyEvent(ImGuiKey_ModAlt, false);
    }
}

static void handleKeyboadEvent(const pangolin::KeyboardEvent& event) {
    ImGuiIO& io = ImGui::GetIO();

    auto key = keycodeToImGuiKey(event.key);
    io.AddKeyEvent(key, event.pressed);
    // modifier keys
    auto _ev = event;
    updateKeyModifiers(_ev.key_modifiers.mask(), event.pressed);
}

static void handleMouseEvent(const pangolin::MouseEvent& event) {
    ImGuiIO& io = ImGui::GetIO();

    io.AddMousePosEvent(event.x, event.y);
    switch (event.button) {
        case 0:
            io.AddMouseButtonEvent(ImGuiMouseButton_Left, event.pressed);
            break;
        case 1:
            io.AddMouseButtonEvent(ImGuiMouseButton_Right, event.pressed);
            break;
        case 2:
            io.AddMouseButtonEvent(ImGuiMouseButton_Middle, event.pressed);
            break;
    }
}


static void handleMouseMotionEvent(const pangolin::MouseMotionEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent((float)event.x, (float)event.y);
}

static void handleSpecialInputEvent(const pangolin::SpecialInputEvent& event) {}


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
        ImGui_ImplPangolin_Data* _bd = getBackendData();
        _bd->window_width = event.width;
        _bd->window_height = event.height;
    });

    window->KeyboardSignal.connect(handleKeyboadEvent);
    window->MouseSignal.connect(handleMouseEvent);
    window->MouseMotionSignal.connect(handleMouseMotionEvent);
    window->SpecialInputSignal.connect(handleSpecialInputEvent);

    return true;
}


void ImGui_ImplPangolin_Shutdown() {
    ImGui_ImplPangolin_Data* bd = getBackendData();
    IM_ASSERT(bd != NULL && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    io.BackendPlatformName = NULL;
    io.BackendPlatformUserData = NULL;
    IM_DELETE(bd);
}


void ImGui_ImplPangolin_NewFrame() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplPangolin_Data* bd = getBackendData();
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
