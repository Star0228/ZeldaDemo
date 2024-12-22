#version 330 core

layout(location = 0) in vec3 aPos; // 顶点位置
layout(location = 1) in vec3 aColor; // 顶点颜色

out vec3 ourColor; // 传递给片段着色器的颜色

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}