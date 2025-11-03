#version 330 core
out vec4 outColor;
void main() {
    vec2 uv = gl_FragCoord.xy / vec2(500.0, 500.0);
    float r = fract(sin(float(gl_PrimitiveID) * 123.9898) * 91111.5453);
    float g = fract(sin(float(gl_PrimitiveID) * 72.233) * 94599.6789);
    float b = fract(sin(float(gl_PrimitiveID) * 45.164) * 84567.4321);
    vec3 baseColor = vec3(r, g, b);
    baseColor *= 0.5;
    outColor = vec4(baseColor, 1.0);
}
