attribute vec4 aPosition;
attribute vec4 aNormal;

uniform mat4 uModelMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uNormalMatrix;

varying vec3 vPosition;
varying vec3 vNormal;

void main() {
    vPosition = (uModelMatrix * aPosition).xyz;
    vNormal = normalize((uNormalMatrix * aNormal).xyz);

    gl_Position = uProjectionMatrix * uModelViewMatrix * aPosition;
}