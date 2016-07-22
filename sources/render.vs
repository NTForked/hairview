#version 330

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vTangent;

out vec3 fPosCamera;
out vec3 fTanCamera;
out vec3 fLightPosCamera;

out vec4 fPosLight;

uniform mat4 uMVPMat;
uniform mat4 uMVMat;
uniform vec3 uLightPos;
uniform mat4 uDepthMVP;

void main(void) {
    gl_Position = uMVPMat * vec4(vPosition, 1.0);

    fPosCamera = (uMVMat * vec4(vPosition, 1.0)).xyz;
    fTanCamera = (transpose(inverse(uMVMat)) * vec4(vTangent, 1.0)).xyz;
    fLightPosCamera = (uMVMat * vec4(uLightPos, 1.0)).xyz;

    fPosLight = uDepthMVP * vec4(vPosition, 1.0);
}
