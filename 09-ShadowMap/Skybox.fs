#version 330 core

in vec3 fragPos;
in vec2 texCoord;

out vec4 FragColor;
uniform sampler2D skybox;

void main()
{
	FragColor = texture(skybox,texCoord);
}