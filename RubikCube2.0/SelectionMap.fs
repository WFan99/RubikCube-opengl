#version 330 core
out vec4 FragColor;

uniform int code;

void main()
{    
    FragColor.r = code/255.0;
}