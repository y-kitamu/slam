#version 440
precision mediump float;
in vec2 v_pos;
uniform sampler2D imageTexture;

out vec4 outColor;


void main() {
    // float pattern = sin(10 * v_pos.y + u_time) * sin(10 * v_pos.x + u_time) * 0.5 + 0.5;
    // vec3 color = mix(colorA, colorB, pattern);
    outColor = texture(imageTexture, v_pos);
    // outColor = vec4(v_pos.xy, 1.0, 1.0);
}
