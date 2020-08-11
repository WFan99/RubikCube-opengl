#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

out vec3 fragPos;
out vec4 shadowPos;
out vec3 normal;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
	fragPos = vec3(model * vec4(aPos, 1.0));
	normal = mat3(transpose(inverse(model))) * aNorm;  
	texCoord = aTex;
	shadowPos = lightSpaceMatrix *  vec4(fragPos, 1.0);
	gl_Position = projection * view * vec4(fragPos, 1.0);
}