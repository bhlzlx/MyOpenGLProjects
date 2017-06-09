#version 330

out vec4 FragColor;

in vec2 TexCoord0;
in vec2 TexCoord1;

uniform sampler2D PrimTexSampler;
uniform sampler2D MaskTexSampler;
uniform vec4      Color;
uniform float     Gray;

void main()
{
    vec4 mask_color = texture2D(MaskTexSampler, TexCoord1.st );
    vec4 tex_color = texture2D(PrimTexSampler, TexCoord0.st);
    tex_color.a = tex_color.a * mask_color.r;
    tex_color = tex_color * Color;
    
    float grayv = tex_color.r * .299 + tex_color.g * .587 + tex_color.b * .114;
    vec4 gray_color = vec4( grayv, grayv, grayv, tex_color.a );
    tex_color = mix( tex_color, gray_color, Gray );
	
    FragColor = tex_color;
}