#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexcoord;

out vec2 texCoords;
out vec3 worldFragPos;
// out vec4 projectionFragPos; // for reflection and refraction

layout (std140) uniform Proj_View {
    mat4 projection;
    mat4 view;
};
// water 不使用 model 矩阵，因为生成顶点的时候就已经把它变换到 [-0.5, 0.5]^2 的范围
uniform vec2 mapScale;
uniform float height;

void main() {
    texCoords = aTexcoord;
    worldFragPos = vec3(aPos.x * mapScale.x, height, aPos.y * mapScale.y);
    gl_Position = projection * view * vec4(worldFragPos, 1.0);
    // projectionFragPos = gl_Position;
}