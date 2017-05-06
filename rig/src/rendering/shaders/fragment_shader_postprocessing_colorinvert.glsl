uniform sampler2D uScreenFrameBuffer;
varying vec2 vTexCoord;

void main() {
    gl_FragColor = vec4(1.0 - texture2D(uScreenFrameBuffer, vTexCoord).xyz, 1.0);
}

