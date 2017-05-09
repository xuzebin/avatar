uniform sampler2D uScreenFrameBuffer;
varying vec2 vTexCoord;

uniform float uBlurSize;

void main() {
    float blurSize = uBlurSize;
    vec4 sum = vec4(0.0);
    float gaussianParams[16];
    gaussianParams[0] = 0.000004;
    gaussianParams[1] = 0.000012;
    gaussianParams[2] = 0.00004;
    gaussianParams[3] = 0.00012;
    gaussianParams[4] = 0.000331;
    gaussianParams[5] = 0.000841;
    gaussianParams[6] = 0.001971;
    gaussianParams[7] = 0.004258;
    gaussianParams[8] = 0.008483;
    gaussianParams[9] = 0.015583;
    gaussianParams[10] = 0.026396;
    gaussianParams[11] = 0.04123;
    gaussianParams[12] = 0.059384;
    gaussianParams[13] = 0.07887;
    gaussianParams[14] = 0.096593;
    gaussianParams[15] = 0.109084;


    //= { 0.000004, 0.000012, 0.00004, 0.00012, 0.000331, 0.000841, 0.001971, 0.004258, 0.008483, 0.015583, 0.026396, 0.04123, 0.059384, 0.07887, 0.096593, 0.109084 };

    float k = 16.0;
    for (int i = 0; i < 16; i++) {
        sum += texture2D(uScreenFrameBuffer, vec2(vTexCoord.x, vTexCoord.y - k * blurSize)) * gaussianParams[i];
        k -= 1.0;
    }
    sum += texture2D(uScreenFrameBuffer, vec2(vTexCoord.x, vTexCoord.y)) * 0.113597;

    k = 16.0;
    for (int i = 0; i < 16; i++) {
        sum += texture2D(uScreenFrameBuffer, vec2(vTexCoord.x, vTexCoord.y + k * blurSize)) * gaussianParams[i];
        k -= 1.0;
    }

    gl_FragColor = sum;
}

