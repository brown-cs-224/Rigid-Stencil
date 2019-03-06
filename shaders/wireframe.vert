#version 330 core

layout(location = 0) in vec2 position; // Position of the vertex

out vec4 color;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

uniform vec3 wireframeColor = vec3(1.0, 0, 0);

void main() {
    color = vec4(wireframeColor, 1.0);
    gl_Position = p * v * m * vec4(position, 0.0, 1.0);
}
