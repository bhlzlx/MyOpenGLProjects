#version 330

uniform sampler2D diffuse_texture;
uniform sampler2D ambient_texture;
uniform sampler2D specular_texture;
uniform sampler2D normal_texture;

struct FragmentIn
{
	vec4	Ld;			// 光源方向（这个可以插值）
	vec4	Eyed;		//
	vec2	coord;		// 纹理参数
	vec4	norm;
	vec4    tangent;
	vec4	ambient;
	vec4	diffuse;
	vec4	specular;
	float	shiness;	// 固定值
};

in FragmentIn fragmentIn;

out vec4 FragColor;

vec4 GetNormalMapNormal()
{
	vec4 Normal = normalize(fragmentIn.norm);
    vec4 Tangent = normalize(fragmentIn.tangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent.xyz, Normal.xyz);
    vec3 BumpMapNormal = texture(normal_texture, fragmentIn.coord.st ).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent.xyz, Bitangent.xyz, Normal.xyz);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return vec4(NewNormal, 0.0); 
}

void main()
{
	vec4 Color = texture2D( ambient_texture, fragmentIn.coord.st ) * fragmentIn.ambient;

	vec4 finalNormal = GetNormalMapNormal();
	float diff = max( dot( fragmentIn.Ld, finalNormal ) , 0.0);

	Color += texture2D( diffuse_texture, fragmentIn.coord.st ) * diff;

	vec4 refl = normalize( reflect(fragmentIn.Ld, finalNormal) );

	float Kspec = max( dot(refl, fragmentIn.Eyed), 0.0 );
	
	Color += texture2D( specular_texture, fragmentIn.coord.st ) * pow( Kspec, fragmentIn.shiness ) * fragmentIn.specular;
	
	FragColor = Color;
}