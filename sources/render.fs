#version 330

#define MODEL_KAJIYA_KAY 1
#define MODEL_MARSCHNER  2

in vec3 fPosCamera;
in vec3 fTanCamera;
in vec3 fLightPosCamera;
in vec4 fPosLight;

out vec4 outColor;

uniform int  uRefModel;
uniform vec3 uDiffuseColor;
uniform vec3 uSpecularColor;

vec3 marschner(vec3 V, vec3 L, vec3 T) {
    return vec3(1, 1, 0);
}

vec3 kajiyaKay(vec3 V, vec3 L, vec3 T) {
    vec3 H = normalize(V + L);

    float cosLT = dot(L, T);
    float sinLT = sqrt(max(0.0, 1.0 - cosLT * cosLT));
    float diffuse = sinLT;

    float cosHT = dot(H, T);
    float sinHT = sqrt(max(0.0, 1.0 - cosHT * cosHT));
    float specular = pow(sinHT, 16.0);

    return diffuse * uDiffuseColor + specular * uSpecularColor;
}

void main(void) {
    vec3 V = normalize(-fPosCamera);
    vec3 L = normalize(fLightPosCamera - fPosCamera);
    vec3 T = normalize(fTanCamera);
    if (uRefModel == MODEL_KAJIYA_KAY) {
        outColor = vec4(kajiyaKay(V, L, T), 1.0);
    } else if (uRefModel == MODEL_MARSCHNER) {
        outColor = vec4(marschner(V, L, T), 1.0);
    }
}
