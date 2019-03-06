#version 330 core

in vec2 texc;
flat in vec3 color;

out vec4 fragColor;

uniform int useTexture = 0;
uniform sampler2D tex;

void main(){
    if (useTexture == 1) {
        fragColor = texture(tex, texc);
    } else {
        fragColor = vec4(color, 1);
    }
}
