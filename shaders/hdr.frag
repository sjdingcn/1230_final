#version 330 core

in vec2 uv;

uniform sampler2D tex;
uniform float exposure;

out vec4 fragColor;

void main(){
    vec4 hdrColor = texture(tex, uv);

    // exposure tone mapping
    //higher exposure gives more details in dark regions
    vec3 mapped = vec3(1.0) - exp(-exposure * hdrColor.rgb);

    fragColor = vec4(mapped, hdrColor.a);
}
