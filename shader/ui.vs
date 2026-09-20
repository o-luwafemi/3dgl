#version 330 core

layout (location = 0) in vec2 aPos;

uniform mat4 uProjection;
uniform vec2 uPos;
uniform vec2 uSize;

void main()
{
    vec2 pixelPos = uPos + aPos * uSize;
    gl_Position = uProjection * vec4(pixelPos, 0.0, 1.0);
}