#version 330 core

layout(location = 0) in vec2 position; // Position of the vertex
layout(location = 2) in vec3 vcolor;   // Vertex color

out vec4 vColor;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

void main() {
    vColor = vec4(vcolor, 1.0);
    gl_Position = p * v * m * vec4(position, 0.0, 1.0);
}
