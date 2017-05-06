uniform sampler2D uScreenFrameBuffer;
varying vec2 vTexCoord;

void main() {
    gl_FragColor = texture2D(uScreenFrameBuffer, vTexCoord);
}

