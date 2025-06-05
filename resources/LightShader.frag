#version 330 core
out vec4 FragColor;

uniform vec3 lightSourceCubeColor;

void main()
{
	FragColor = vec4(lightSourceCubeColor, 1.0);
}