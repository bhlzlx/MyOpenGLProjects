#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 coord;

layout (std140) uniform RenderParam{
	mat4 proj;		// MVP矩阵
	mat4 view;
	mat4 model;		
	vec4 Lp;		// 点光源位置
	vec4 Lc;		// 点光源颜色
	vec4 Eyep;		// 观察点位置（摄像机位置）
	vec4 Ka;		// 环境光掩码
	vec4 Kd;		// 散射光掩码
	vec4 Ks;		// 镜面光掩码
	float shiness;	// 高光系数
};

struct FragmentIn
{
	vec4	Ld;			// 光源方向（这个可以插值）
	vec4	Eyed;		//
	vec2	coord;		// 纹理参数
	vec4	norm;
	vec4	ambient;
	vec4	diffuse;
	vec4	specular;
	float	shiness;	// 固定值
};

out FragmentIn fragmentIn;

void main()
{
    fragmentIn.coord = coord;
	fragmentIn.shiness = shiness;
	fragmentIn.norm = normalize( model * vec4(norm,0.0) ); // 变换后的法线
	
	vec4 modelPos = model * vec4( pos, 1.0);
	fragmentIn.Eyed = normalize(Eyep - modelPos);	
	fragmentIn.Ld = normalize( modelPos-Lp );
	
	float Kdiff = max( dot( -fragmentIn.Ld, fragmentIn.norm ) , 0.0); // 散射光的强度

	fragmentIn.diffuse = Kd * Kdiff;
	fragmentIn.ambient = Ka;
	fragmentIn.specular = Ks;
	
    gl_Position = proj * view * modelPos;
}