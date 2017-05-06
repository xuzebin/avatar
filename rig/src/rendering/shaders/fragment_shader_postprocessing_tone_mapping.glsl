uniform sampler2D uScreenFrameBuffer;
varying vec2 vTexCoord;
uniform float uExposure;

void main() {
    const float gamma = 2.2;
    vec3 hdrColor = texture2D(uScreenFrameBuffer, vTexCoord).rgb;
    vec3 mapped = vec3(1.0) - exp(-hdrColor * uExposure);
    mapped = pow(mapped, vec3(gamma));

    gl_FragColor = vec4(mapped, 1.0);
}

