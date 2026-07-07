#version 330 core

layout (location = 0) in vec3 aPosition;

out vec3 localPos;

uniform mat4 captureProjection;
uniform mat4 captureView;

void main() {
    localPos = aPosition;
    gl_Position = captureProjection * captureView * vec4(localPos, 1.0);
}
