#version 330 core

in vec2 uv;

uniform sampler2D tex;
uniform float unit;

out vec4 fragColor;

const int K_SIZE = 25;
void main(){
    fragColor = vec4(0);

    float kernel[25] = float[25](
        1,4,7,4,1,
        4,16,26,16,4,
        7,26,41,26,7,
        4,16,26,16,4,
        1,4,7,4,1
    );

    vec4 cur_pix = texture(tex, uv);
    if(cur_pix.r>0)
    {
        vec2 sample_uv;
        for (int i = -2; i <= 2; i++)
            for (int j = -2; j <= 2; j++)
            {
                vec2 sample_uv = vec2(uv.x + unit*i, uv.y + unit*j);
                fragColor += kernel[(i+2)*5+j+2] * texture(tex, sample_uv);
            }
        fragColor = clamp(fragColor/273.f, 0.f, 1.f);
        fragColor.a = (fragColor.r+fragColor.g+fragColor.b)/3.f*0.7;
    }
    else
    {
        fragColor = vec4(0,0,0,0);
    }
    fragColor = vec4(0,0,0,0);
//    fragColor = texture(tex, uv);
//    fragColor = vec4(1,1,1,0.5);
}
