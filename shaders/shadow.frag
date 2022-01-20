#version 330 core

in vec3 color;
in vec2 texc;
in float transparency;
out vec4 fragColor;

uniform int useTexture = 0;
uniform vec2 repeatUV;

void main(){
//    vec3 texColor = texture(tex, texc*repeatUV).rgb;
//    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));
//    fragColor = vec4(color * texColor, 1);
    fragColor = vec4(color, transparency);
}
