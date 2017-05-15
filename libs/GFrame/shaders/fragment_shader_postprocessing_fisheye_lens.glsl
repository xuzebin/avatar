uniform sampler2D uScreenFrameBuffer;
varying vec2 vTexCoord;

const vec2 resolution = vec2(600, 600);
const float lensSize = 0.3;
vec2 m = vec2(0.5, 0.5);//mouse position

void main(void)
{
    vec2 p = gl_FragCoord.xy / resolution.xy;


    vec2 d = p - m;
    float r = sqrt(dot(d, d)); // distance of pixel from mouse

    vec2 uv;
    vec3 col = vec3(0.0, 0.0, 0.0);
    if (r > lensSize+0.01)
    {
        uv = p;
        col = texture2D(uScreenFrameBuffer, uv).xyz;
    }
    else if (r < lensSize-0.01)
    {
        // Thanks to Paul Bourke for these formulas; see
        // http://paulbourke.net/miscellaneous/lenscorrection/
        // and .../lenscorrection/lens.c
        // Choose one formula to uncomment:
        // SQUAREXY:
        // uv = m + vec2(d.x * abs(d.x), d.y * abs(d.y));
        // SQUARER:
        uv = m + d * r; // a.k.a. m + normalize(d) * r * r
        // SINER:
        // uv = m + normalize(d) * sin(r * 3.14159 * 0.5);
        // ASINR:
        // uv = m + normalize(d) * asin(r) / (3.14159 * 0.5);
        col = texture2D(uScreenFrameBuffer, uv).xyz;
    }
    gl_FragColor = vec4(col, 1.0);
}