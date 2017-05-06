uniform sampler2D uScreenFrameBuffer;
varying vec2 vTexCoord;

uniform float uMouseXOffset;
#define HueLevCount 6
#define SatLevCount 7
#define ValLevCount 4

float edge_thres = 0.2; // 0.2;
float edge_thres2 = 5.0; // 5.0;
float HueLevels[HueLevCount];// = float[] (0.0,140.0,160.0,240.0,240.0,360.0);
float SatLevels[SatLevCount];// = float[] (0.0,0.15,0.3,0.45,0.6,0.8,1.0);
float ValLevels[ValLevCount];// = float[] (0.0,0.3,0.6,1.0);

void initArrays(void) {
    HueLevels[0] = 0.0;
    HueLevels[1] = 140.0;
    HueLevels[2] = 160.0;
    HueLevels[3] = 240.0;
    HueLevels[4] = 240.0;
    HueLevels[5] = 360.0;

    SatLevels[0] = 0.0;
    SatLevels[1] = 0.15;
    SatLevels[2] = 0.3;
    SatLevels[3] = 0.45;
    SatLevels[4] = 0.6;
    SatLevels[5] = 0.8;
    SatLevels[6] = 1.0;

    ValLevels[0] = 0.0;
    ValLevels[1] = 0.3;
    ValLevels[2] = 0.6;
    ValLevels[3] = 1.0;
}

vec3 RGBtoHSV( float r, float g, float b)
{
    float minv, maxv, delta;
    vec3 res;

    minv = min(min(r, g), b);
    maxv = max(max(r, g), b);
    res.z = maxv;            // v

    delta = maxv - minv;

    if( maxv != 0.0 )
        res.y = delta / maxv;      // s
    else {
        // r = g = b = 0      // s = 0, v is undefined
        res.y = 0.0;
        res.x = -1.0;
        return res;
    }

    if( r == maxv )
        res.x = ( g - b ) / delta;      // between yellow & magenta
    else if( g == maxv )
        res.x = 2.0 + ( b - r ) / delta;   // between cyan & yellow
    else
        res.x = 4.0 + ( r - g ) / delta;   // between magenta & cyan

    res.x = res.x * 60.0;            // degrees
    if( res.x < 0.0 )
        res.x = res.x + 360.0;

    return res;
}

vec3 HSVtoRGB(float h, float s, float v )
{
    int i;
    float f, p, q, t;
    vec3 res;

    if( s == 0.0 )
    {
        // achromatic (grey)
        res.x = v;
        res.y = v;
        res.z = v;
        return res;
    }

    h /= 60.0;         // sector 0 to 5
    i = int(floor( h ));
    f = h - float(i);         // factorial part of h
    p = v * ( 1.0 - s );
    q = v * ( 1.0 - s * f );
    t = v * ( 1.0 - s * ( 1.0 - f ) );

    if (i == 0) {
        res.x = v;
        res.y = t;
        res.z = p;
    } else if (i == 1) {
        res.x = q;
        res.y = v;
        res.z = p;
    } else if (i == 2) {
        res.x = p;
        res.y = v;
        res.z = t;
    } else if (i == 3) {
        res.x = p;
        res.y = q;
        res.z = v;
    } else if (i == 4) {
        res.x = t;
        res.y = p;
        res.z = v;
    } else {
        res.x = v;
        res.y = p;
        res.z = q;
    }
    return res;
}

float nearestLevel(float col, int mode)
{
    int levCount;
    if (mode==0) levCount = HueLevCount;
    if (mode==1) levCount = SatLevCount;
    if (mode==2) levCount = ValLevCount;

    for (int i =0; i<levCount-1; i++ ) {
        if (mode==0) {
            if (col >= HueLevels[i] && col <= HueLevels[i+1]) {
                return HueLevels[i+1];
            }
        }
        if (mode==1) {
            if (col >= SatLevels[i] && col <= SatLevels[i+1]) {
                return SatLevels[i+1];
            }
        }
        if (mode==2) {
            if (col >= ValLevels[i] && col <= ValLevels[i+1]) {
                return ValLevels[i+1];
            }
        }
    }
}

// averaged pixel intensity from 3 color channels
float avg_intensity(vec4 pix)
{
    return (pix.r + pix.g + pix.b)/3.;
}

vec4 get_pixel(vec2 coords, float dx, float dy)
{
    return texture2D(uScreenFrameBuffer,coords + vec2(dx, dy));
}

// returns pixel color
float IsEdge(in vec2 coords)
{
    float dxtex = 1.0 / 1024.0;//float(textureSize(uScreenFrameBuffer,0));
    float dytex = 1.0 / 1024.0;//float(textureSize(uScreenFrameBuffer,0));
    float pix[9];
    int k = -1;
    float delta;

    // read neighboring pixel intensities
    for (int i=-1; i<2; i++) {
        for(int j=-1; j<2; j++) {
            k++;
            pix[k] = avg_intensity(get_pixel(coords,float(i)*dxtex,
                                             float(j)*dytex));
        }
    }

    // average color differences around neighboring pixels
    delta = (abs(pix[1]-pix[7])+
             abs(pix[5]-pix[3]) +
             abs(pix[0]-pix[8])+
             abs(pix[2]-pix[6])
             )/4.;

    //return clamp(5.5*delta,0.0,1.0);
    return clamp(edge_thres2*delta,0.0,1.0);
}

void main()
{
    initArrays();
    vec2 uv = vTexCoord;
    vec4 tc = vec4(1.0, 0.0, 0.0, 1.0);
    if (uv.x > (uMouseXOffset + 0.002))
    {
        vec3 colorOrg = texture2D(uScreenFrameBuffer, uv).rgb;
        vec3 vHSV =  RGBtoHSV(colorOrg.r,colorOrg.g,colorOrg.b);
        vHSV.x = nearestLevel(vHSV.x, 0);
        vHSV.y = nearestLevel(vHSV.y, 1);
        vHSV.z = nearestLevel(vHSV.z, 2);
        float edg = IsEdge(uv);
        vec3 vRGB = (edg >= edge_thres)? vec3(0.0,0.0,0.0):HSVtoRGB(vHSV.x,vHSV.y,vHSV.z);
        tc = vec4(vRGB.x,vRGB.y,vRGB.z, 1);  
    }
    else if (uv.x < (uMouseXOffset-0.002))
    {
        tc = texture2D(uScreenFrameBuffer, uv);
    }
    gl_FragColor = tc;
}
