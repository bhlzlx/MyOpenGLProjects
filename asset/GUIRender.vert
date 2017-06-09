#version 330

layout (location = 0) in vec2 Vert;
layout (location = 1) in vec2 PrimTexCoord;
layout (location = 2) in vec2 MaskTexCoord;

out vec2 TexCoord0;
out vec2 TexCoord1;

void main()
{
    gl_Position = vec4( Vert, 0.0, 1.0);
    TexCoord0 = PrimTexCoord;
    TexCoord1 = MaskTexCoord;
}