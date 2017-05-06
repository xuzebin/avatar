uniform samplerCube uEnvironmentMap;

varying vec3 vPosition;

void main() {
    gl_FragColor = textureCube(uEnvironmentMap, normalize(vPosition));
}

