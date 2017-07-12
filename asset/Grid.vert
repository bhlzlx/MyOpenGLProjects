#version 330

layout (location = 0) in vec3 pos;
layout (std140) uniform RenderParam{
	mat4 proj;		// MVP矩阵(Model矩阵就不需要了，因为不会改变坐标架的位置的)
	mat4 view;
	vec4 color;		// 坐标架的颜色
};

struct FragmentIn
{
	vec4 color;
};

out FragmentIn fragmentIn;

void main()
{
	fragmentIn.color = color;	
    gl_Position = proj * view * vec4( pos, 1.0);
}