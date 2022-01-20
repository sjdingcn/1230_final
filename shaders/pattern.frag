#version 330 core

in vec2 uv;
uniform sampler2D tex;

out vec4 fragColor;

uniform vec3 lightColor;

void main(){
    float unit = 0.01;
    vec4 tex_color = vec4(0);
    float kernel[25] = float[25](
        1,4,7,4,1,
        4,16,26,16,4,
        7,26,41,26,7,
        4,16,26,16,4,
        1,4,7,4,1
    );

    vec4 cur_pix = texture(tex, uv);
    vec2 sample_uv;
    for (int i = -2; i <= 2; i++)
        for (int j = -2; j <= 2; j++)
        {
            vec2 sample_uv = vec2(uv.x + unit*i, uv.y + unit*j);
            tex_color += kernel[(i+2)*5+j+2] * texture(tex, sample_uv);
        }
    tex_color = clamp(tex_color/273.f, 0.f, 1.f);
    tex_color.a = tex_color.r*0.5;


    fragColor = vec4(lightColor*tex_color.a, tex_color.a);
//    fragColor = vec4(1,1,1,0.5);
}
