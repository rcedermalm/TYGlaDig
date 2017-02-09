#version 330 core

//Color: .rgba
//Texture: .stpq
//Vector: .xyzw

in vec3 vColor;
in vec2 vTexCoord;

out vec4 color;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
//uniform vec4 ourColor;
//uniform vec3 lDir;

void main()
{
    color = mix(texture(ourTexture1, vTexCoord), texture(ourTexture2, vTexCoord), 0.2) * vec4(vColor, 1.0f);
}