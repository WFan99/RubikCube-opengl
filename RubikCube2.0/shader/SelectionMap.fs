#version 330 core
out vec4 FragColor;

uniform int code;

void main()
{    
    FragColor = vec4(code/255.0,0,0,0);
}