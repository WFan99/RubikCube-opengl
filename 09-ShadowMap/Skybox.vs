#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec3 fragPos;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	fragPos = vec3(model * (aPos, 1.0));
	texCoord = aTex;
	gl_Position  = (projection * view * vec4(aPos, 1.0)).xyww;
	
}