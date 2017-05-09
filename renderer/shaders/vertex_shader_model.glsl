attribute vec4 aPosition;
attribute vec2 aTexCoord;
attribute vec4 aNormal;
attribute vec4 aBinormal;
attribute vec4 aTangent;

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uNormalMatrix;

varying vec2 vTexCoord;
varying vec3 vPosition;

varying mat3 vTBNMatrix;//for normal map

void main() {
    vTexCoord = aTexCoord;
    vec4 vPos4 = uModelViewMatrix * aPosition;
    vPosition = vPos4.xyz;
    
    vTBNMatrix = mat3(normalize((uNormalMatrix * aTangent).xyz),
                      normalize((uNormalMatrix * aBinormal).xyz),
                      normalize((uNormalMatrix * aNormal).xyz));
    
    gl_Position = uProjectionMatrix * vPos4;
}