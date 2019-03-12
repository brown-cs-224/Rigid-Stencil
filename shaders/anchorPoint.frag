#version 330 core

in vec4 fColor;
in vec2 fPoint;
in float fRadius;

out vec4 fragColor;

void main(){
    if (length(gl_FragCoord.xy - fPoint) > fRadius) {
        discard;
    }
    fragColor = fColor;
}
