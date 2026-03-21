#version 330 core

#include "common/common.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 transform;
uniform float time;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    vec3 hsvColor = rgb2hsv(aColor);
    hsvColor.x = fract(hsvColor.x + time * 0.1);
    ourColor = hsv2rgb(hsvColor);
}
