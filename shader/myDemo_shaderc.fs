#version 330 core

#include "common/common.h"

out vec4 FragColor;

in vec3 ourColor;

uniform float time;

void main()
{
    vec3 hsvColor = rgb2hsv(ourColor);
    hsvColor.x = fract(hsvColor.x + time * 0.05);
    vec3 finalColor = hsv2rgb(hsvColor);
    FragColor = vec4(finalColor, 1.0);
}
