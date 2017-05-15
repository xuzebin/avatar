uniform samplerCube uEnvironmentMap;
uniform mat4 uNormalMatrix;

varying vec3 vPosition;
varying vec3 vNormal;

mat3 linearTranspose(mat4 m4) {
    return mat3(
        m4[0][0], m4[1][0], m4[2][0],
        m4[0][1], m4[1][1], m4[2][1],
        m4[0][2], m4[1][2], m4[2][2]
    );
}

void main() {
    vec3 reflectVector = reflect(normalize(vPosition), vNormal);
    reflectVector = linearTranspose(uNormalMatrix) * reflectVector;
    
    gl_FragColor = textureCube(uEnvironmentMap, reflectVector);
}

