#version 330 

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 incol;
layout (location = 2) in vec3 offset;


uniform mat4 MVP;

out vec3 outcol;
out vec3 pos;

void main()
{
    outcol=incol;
	pos=aPos;

	
    gl_Position = MVP*vec4(aPos.x, aPos.y, aPos.z, 3.0); //mo¿na wykorzystaæ do skalowania
	
}