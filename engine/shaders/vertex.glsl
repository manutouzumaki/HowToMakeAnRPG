#version 330 core

layout (location=0) in vec2 aPos;
layout (location=1) in vec4 aColor;
layout (location=2) in vec2 aTexCoord;
layout (location=3) in float aTex;

uniform mat4 uProj;
uniform mat4 uView;

out vec4 fColor;
out vec2 fTexCoord;
out float fTex;

void main()
{
    fColor = aColor;
    fTexCoord = aTexCoord;
    fTex = aTex;
    gl_Position = uProj * uView * vec4(aPos, 0.0f, 1.0f);
}

