uniform sampler2D uScreenFrameBuffer;
varying vec2 vTexCoord;

void main() {
    vec4 texColor = texture2D(uScreenFrameBuffer, vTexCoord);
    float gray = (texColor.x + texColor.y + texColor.z) / 3.0;
    gl_FragColor = vec4(gray, gray, gray, 1.0);
}

