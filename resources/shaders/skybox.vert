#version 330 core

layout (location = 0) in vec3 aPosition;

out vec3 localPos;

uniform mat4 projection;
uniform mat4 view;

void main() {
    localPos = aPosition;
    mat4 rotOnlyView = mat4(mat3(view)); // strip translation, keep rotation
    vec4 clipPos = projection * rotOnlyView * vec4(localPos, 1.0);
    gl_Position = clipPos.xyww; // force depth to the far plane
}
