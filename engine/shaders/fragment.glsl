#version 330 core

uniform sampler2D texture1;

in vec4 fColor;
in vec2 fTexCoord;
in float fTex;

out vec4 color;

void main()
{
    if(fTex > 0.0f)
    {
        color = texture(texture1, fTexCoord) * fColor;
    }
    else
    {
        color = fColor;
    }
}
