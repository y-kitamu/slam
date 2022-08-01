#version 440
layout(location = 0) in vec3 aPos;  // coordinate in pixel space.
layout(location = 1) in vec2 aTexCoord;

out vec2 v_pos;

void main() {
    gl_Position = vec4(aPos, 1.0);
    // v_pos = vec2(0.5, 0.5);
    v_pos = aTexCoord;
}
