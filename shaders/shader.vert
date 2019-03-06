#version 330 core

layout(location = 0) in vec2 position; // Position of the vertex
layout(location = 1) in vec2 texCoord; // UV texture coordinates

out vec2 texc;
flat out vec3 color;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

// Convert from HSL to RGB
// source: http://www.geekymonkey.com/Programming/CSharp/RGB2HSL_HSL2RGB.htm
vec3 HSLtoRGB(float h, float s, float l) {
    float v;
    float r, g, b;
    v = (l <= 0.5) ? (l * (1.0 + s)) : (l + s - l * s);

    if (v > 0) {
        float m;
        float sv;
        int sextant;
        float fract, vsf, mid1, mid2;

        m = l + l - v;
        sv = (v - m) / v;
        h *= 6.0;
        sextant = int(h);
        fract = h - sextant;
        vsf = v * sv * fract;
        mid1 = m + vsf;
        mid2 = v - vsf;
        if (sextant == 0) {
            r = v;
            g = mid1;
            b = m;
        } else if (sextant == 1) {
            r = mid2;
            g = v;
            b = m;
        } else if (sextant == 2) {
            r = m;
            g = v;
            b = mid1;
        } else if (sextant == 3) {
            r = m;
            g = mid2;
            b = v;
        } else if (sextant == 4) {
            r = mid1;
            g = m;
            b = v;
        } else if (sextant == 5) {
            r = v;
            g = m;
            b = mid2;
        }
    }

    return vec3(r, g, b);
}

vec3 pickRainbowColor(float x) {
    return HSLtoRGB(x, 0.5, 0.5);
}

float hash(float n) { return fract(sin(n)*753.5453123); }


void main() {
    texc = texCoord;

    color = pickRainbowColor(hash(gl_VertexID));

    gl_Position = p * v * m * vec4(position, 0.0, 1.0);
}
