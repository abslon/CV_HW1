#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
	vec3 pos = aPos * 0.05f;
	gl_Position = vec4(pos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}