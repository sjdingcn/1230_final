#version 330 core

// Ouput data
//out float fragmentdepth;
out vec4 fragColor;
void main(){

//    fragmentdepth = gl_FragCoord.z;
    fragColor = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1);
}
