#version 330 core

in vec2 fragTexCoord;
uniform sampler2D texSampler;

out vec4 FragColor;

void main()
{
	FragColor = texture(texSampler, fragTexCoord);
}

