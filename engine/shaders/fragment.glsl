#version 330 core

uniform sampler2D uTextures[8];

in vec4 fColor;
in vec2 fTexCoord;
in float fTex;

out vec4 color;

void main()
{
    if(fTex > 0.0f)
    {
        int texIndex = int(fTex);
        color = fColor * texture(uTextures[texIndex], fTexCoord);
    }
    else
    {
        color = fColor;
    }
    color = fColor;
}
