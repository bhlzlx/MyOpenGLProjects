#version 330

uniform sampler2D diffuse_texture;
uniform sampler2D ambient_texture;
uniform sampler2D specular_texture;

struct FragmentIn
{
	vec4	Ld;			// 光源方向（这个可以插值）
	vec2	coord;		// 纹理参数
	vec4	norm;
	vec4	ambient;
	vec4	diffuse;
	vec4	specular;
	float	shiness;	// 固定值
};

in FragmentIn fragmentIn;

out vec4 FragColor;

void main()
{
	vec4 Color = texture2D( ambient_texture, fragmentIn.coord.st ) * fragmentIn.ambient;
	Color += texture2D( diffuse_texture, fragmentIn.coord.st ) * fragmentIn.diffuse;
	float Kspec = max( dot(fragmentIn.Ld, fragmentIn.norm), 0.0 );
	
	Color += texture2D( specular_texture, fragmentIn.coord.st ) * pow( Kspec, fragmentIn.shiness ) * fragmentIn.specular;
	
	FragColor = Color;
}