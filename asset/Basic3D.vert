#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 coord;

layout (std140) uniform RenderParam{
	mat4 model;		// MVP矩阵
	mat4 view;
	mat4 proj;
	vec3 Lp;		// 点光源位置
	vec3 Lc;		// 点光源颜色
	vec3 Ka;		// 环境光掩码
	vec3 Kd;		// 散射光掩码
	vec3 Ks;		// 散射光掩码
	float shiness;	// 高光系数
};

struct 

out vec2 coordOut;
out vec3 diffuse_color_out;

void main()
{
    coordOut = coord;
	diffuse_color_out = diffuse_color;
    gl_Position = proj * view * model * vec4( pos.x, pos.y, pos.z, 1.0);
}