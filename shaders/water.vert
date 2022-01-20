#version 330 core

layout(location = 0) in vec3 position; // Position of the vertex
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout(location = 5) in vec2 texCoord; // UV texture coordinates
layout(location = 10) in float arrowOffset; // Sideways offset for billboarded normal arrows

out vec3 color; // Computed color for this vertex
out vec2 texc;
out float transparency;

// global data
uniform float ka;
uniform float kd;
uniform float ks;
uniform float sea_level;
uniform float extinc;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

// Light data
const int MAX_LIGHTS = 3;
uniform int lightTypes[MAX_LIGHTS];         // 0 for point, 1 for directional
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
//uniform vec3 lightAttenuations[MAX_LIGHTS]; // Constant, linear, and quadratic term
uniform vec3 lightColors[MAX_LIGHTS];

// Material data
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform vec3 transparent;
uniform float shininess;
uniform vec2 repeatUV;

uniform float r0;
uniform float eta;

void main() {
    texc = texCoord * repeatUV;

    vec4 position_cameraSpace = v * m * vec4(position, 1.0);
    vec4 normal_cameraSpace = vec4(normalize(mat3(transpose(inverse(v * m))) * normal), 0);

    vec4 position_worldSpace = m * vec4(position, 1.0);
    vec4 normal_worldSpace = vec4(normalize(mat3(transpose(inverse(m))) * normal), 0);

    vec4 camera_position = inverse(v)*vec4(0,0,0,1);
    // TODO: fill the rest in
    float F = r0 + (1-r0)*pow(1-dot(normal_cameraSpace, -normalize(position_cameraSpace)), 5);
    if (camera_position.y<sea_level) F = r0 + (1-r0)*pow(1-dot(-normal_cameraSpace, -normalize(position_cameraSpace)), 5);

    gl_Position = p * position_cameraSpace;
    float a= 0.2, d = 0.8, s = 0.5;

    float depth = max(sea_level - position_worldSpace.y, 0);
    float absorb = exp(-extinc*depth*4);

    color = ambient_color.xyz*a; // Add ambient component

    for (int i = 0; i < MAX_LIGHTS; i++) {
        vec4 vertexToLight = vec4(0);
        // Point Light
        if (lightTypes[i] == 0) {
            vertexToLight = normalize(v * vec4(lightPositions[i], 1) - position_cameraSpace);
        } else if (lightTypes[i] == 1) {
            // Dir Light
            vertexToLight = normalize(v * vec4(-lightDirections[i], 0));
        }

        // Add diffuse component
        float diffuseIntensity = max(0.0, dot(vertexToLight, normal_cameraSpace));
        if (camera_position.y<sea_level) diffuseIntensity = max(0.0, dot(vertexToLight, -normal_cameraSpace));
        color += max(vec3(0), lightColors[i] * diffuse_color * diffuseIntensity)*d;

        // Add specular component
        if (camera_position.y<sea_level)
        {
//            vec4 lightRefraction = normalize(refract(-vertexToLight, normal_cameraSpace, 1.f/eta));
//            vec4 eyeDirection = normalize(vec4(0,0,0,1) - position_cameraSpace);
//            float cos = max(0.0, dot(eyeDirection, lightRefraction)-0.7);
//            float specIntensity = pow(cos, shininess);
//            color += max (vec3(0), lightColors[i] * specular_color * specIntensity)*s;
            float specIntensity = max(-position.y-0.25, 0.f)*0.6;
            color += max (vec3(0), lightColors[i] * specular_color * specIntensity)*s;
        }
//        else
//        {
//            vec4 lightReflection = normalize(-reflect(vertexToLight, normal_cameraSpace));
//            vec4 eyeDirection = normalize(vec4(0,0,0,1) - position_cameraSpace);
//            float cos = max(0.0, dot(eyeDirection, lightReflection)-0.9);
//            float specIntensity = pow(cos, shininess);
//            color += max (vec3(0), lightColors[i] * specular_color * specIntensity)*s;
//        }

    }

//    color = clamp(color, 0.0, 1.0);
    transparency = (transparent[0]+transparent[1]+transparent[2])/3.0f;
    if (camera_position.y<sea_level)
    {
//        color = clamp(color, 0.0, 1.0);
        color = clamp(mix(color, vec3(0.8,0.8,0.8), F), 0.0, 1.0);
    }
    else
    {
//        color = clamp(color, 0.0, 1.0);
//        transparency = (color.x+color.y+color.z)/3.f;
        color = clamp(mix(color, vec3(0.2,0.5,0.9), F), 0.0, 1.0);
        transparency = clamp(1-F, 0.0, 1.0);
    }


}
