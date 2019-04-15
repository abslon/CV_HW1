#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
// layout (location = 1) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
// out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	float size = 0.02f;

	FragPos = vec3(model * vec4(aPos, 1.0)) * size;
    Normal = mat3(transpose(inverse(model))) * aNormal;  
	gl_Position =  proj * view * vec4(FragPos, 1.0);
	// TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}