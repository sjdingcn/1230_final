#version 330 core

layout(location = 0) in vec3 position; // Position of the vertex
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout(location = 5) in vec2 texCoord; // UV texture coordinates
layout(location = 10) in float arrowOffset; // Sideways offset for billboarded normal arrows

out vec3 color; // Computed color for this vertex
//out vec2 texc;
out float transparency;

uniform float surface_width;
uniform float surface_height;
uniform sampler2D wave_surface;
// global data
uniform float k;
uniform float sea_level;
uniform vec3 tone;
uniform float extinc;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
uniform mat4 surface_m;

// Light data
uniform vec3 lightDirection;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 func;
uniform float finegrainess;

//Objects Data
const int MAX_OBJS = 3;
uniform mat4 obj_poses[MAX_OBJS];
uniform mat4 obj_pose1;
uniform mat4 obj_pose2;
uniform mat4 obj_pose3;

// Material data
uniform vec3 diffuse_color;
uniform float transparent;

const float M_PI = 3.1415926535897932384626433832795;
const int supersample = 1;
const float step_fine = 0.025;
const float step_coarse = 0.025;

void main() {
    vec4 position_cameraSpace = m * vec4(position, 1.0);
    vec4 position_worldSpace = inverse(v) * m * vec4(position, 1.0);
    vec4 camera_position_world = inverse(v)*vec4(0,0,0,1);
    //    vec4 position_worldSpace = m * vec4(position, 1.0);
    //    vec4 position_cameraSpace = v * m * vec4(position, 1.0);
    gl_Position = p * position_cameraSpace;

    vec3 offsets[5] = vec3[5](
                       vec3( 0,  0, 0),
                       vec3( 1, -1, 0)/finegrainess,
                       vec3( 1,  1, 0)/finegrainess,
                       vec3(-1,  1, 0)/finegrainess,
                       vec3(-1, -1, 0)/finegrainess);

    color = vec3(0,0,0);
    for(int o=0; o<supersample; o++)
    {

        position_cameraSpace = m * vec4(position+offsets[o], 1.0);
        position_worldSpace = inverse(v) * m * vec4(position+offsets[o], 1.0);

        vec4 vertexToLight = normalize(v * vec4(-lightDirection, 0));
//        vec4 vertexToLight = normalize(position_cameraSpace - v * vec4(lightPosition, 1));
        vec4 eyeDirection = normalize(vec4(0,0,0,1) - position_cameraSpace);
        vec4 eyeWorldDirection = -normalize(inverse(v)*eyeDirection);

        float cosT = dot(eyeDirection, vertexToLight);
        float rayleigh = 3/16/M_PI*(1 + cosT*cosT);

        float ray_y = eyeWorldDirection.y;
        float ray_L = 1;
        float light_y = abs(lightDirection.y);
        float light_L = length(lightDirection);
        float depth = max(sea_level - position_worldSpace.y, 0);
        float S_discrete = 200;

        // a little safety
        if(sea_level < position_worldSpace.y)
        {
            S_discrete = 0;
        }

        vec3 sample_color = vec3(0,0,0);
        vec3 point_color = vec3(0,0,0);
        float attenu = 1;

        float depth_left = depth;
        float stride = step_coarse;
        vec3 point_pos = position_worldSpace.xyz;

        int sample_cnt = 0;
        bool sample_flag = false;

        // for directional light source
        vec3 ilu_ray = -normalize(lightDirection);
        float eye_dis = 0;

        // move away from the camera a little
//        float step_horizontal_distance = stride*length(eyeWorldDirection.xz)+0.001;

        float ilu_dis = depth_left/ilu_ray.y;
        vec3 suf_pos = point_pos + ilu_dis*ilu_ray;

        float dis_sf = distance(suf_pos.xz, camera_position_world.xz);
        if(dis_sf < 13)
        {
            float strides = (13-dis_sf)/(length(eyeWorldDirection.xz)+0.00001);
            strides = min(strides, 20);
            depth_left = depth_left - strides*ray_y;
            point_pos = point_pos + strides*eyeWorldDirection.xyz;
        }

        bool distance_flag = true;
        for (float s=0;s<S_discrete;s++)
        {

            depth_left = depth_left - stride*ray_y;
            point_pos = point_pos + stride*eyeWorldDirection.xyz;

            // for dot light source
//            vec3 ilu_ray = normalize(lightPosition - point_pos);
            float ilu_dis = depth_left/ilu_ray.y;
            vec3 suf_pos = point_pos + ilu_dis*ilu_ray;


            vec4 suf_pos_org = inverse(surface_m)*vec4(suf_pos, 1);
            suf_pos = suf_pos_org.xyz;

            if (suf_pos.x+0.5<0 || suf_pos.x+0.5>1) continue;
            if (suf_pos.z+0.5<0 || suf_pos.z+0.5>1) continue;
            vec4 height = texture(wave_surface, vec2(suf_pos.x+0.5, suf_pos.z+0.5));


            if (depth_left+height.g-height.b<0 || depth_left>10) break;

            eye_dis += stride;
            if (height.r < 0.2 && height.g < 0.2)
//            if ( (suf_pos.z+8)*(suf_pos.z+8)+(suf_pos.x)*(suf_pos.x)>4)
//            if ( sin(suf_pos.z)*sin(suf_pos.z)+sin(suf_pos.x)*sin(suf_pos.x)<1.9)
            {
                stride = step_coarse;
                sample_flag = false;
                continue;
            }
            else
            {
                if(!sample_flag)
                {
                    sample_flag = true;
                    depth_left = depth_left + step_coarse*ray_y;
                    point_pos = point_pos - step_coarse*eyeWorldDirection.xyz;
                    s--;
                }
                stride = step_fine;
            }



            float surface_intensity = pow(clamp((height.r-0.2)*10, 0, 1), 3);
            float absorb = exp(-extinc*ilu_dis-1);
            point_color = absorb*diffuse_color + 1.f/extinc*lightColor*rayleigh*(1-absorb);

            attenu = min(1./(func.x+func.y*eye_dis+func.z*eye_dis*eye_dis), 1.);
            color += point_color*0.2*surface_intensity*attenu;
            sample_cnt++;


        }
        color += sample_color;
//        color += sample_color/sample_cnt*(1+sample_cnt/30);
    }

    color = clamp(color/3, 0.0, 1.0)*0.7;
    transparency = (color.x+color.y+color.z)/3.f;
//    transparency = transparent;
}
