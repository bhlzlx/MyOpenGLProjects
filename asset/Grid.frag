#version 330

struct FragmentIn
{
	vec4 color;
};

in FragmentIn fragmentIn;

out vec4 FragColor;

void main()
{
	FragColor = fragmentIn.color;
}