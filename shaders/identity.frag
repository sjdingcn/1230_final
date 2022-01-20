#version 330 core

in vec2 uv;
uniform sampler2D tex;
uniform vec3 lightColor;

out vec4 fragColor;

void main(){
    fragColor = vec4(0);
    fragColor = texture(tex, uv);
//    fragColor = vec4(1,1,1,0.5);
}
