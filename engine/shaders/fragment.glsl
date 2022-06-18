#version 330 core

in vec4 fColor;
in vec2 fTexCoord;
in float fTex;

out vec4 color;

void main()
{
    color = fColor;
}
