#version 330

uniform sampler2D diffuse_texture;
uniform sampler2D ambient_texture;
uniform sampler2D specular_texture;

in vec2 coordOut;
in vec3 diffuse_color_out;

out vec4 FragColor;

void main()
{
    FragColor = texture2D(diffuse_texture, coordOut.st );
	FragColor = FragColor * vec4(diffuse_color_out, 1.0);
}