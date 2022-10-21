#version 330 core

out vec4 color;
in vec2 Texcoord;

uniform sampler2D ourTexture1;

void main()
{

    color = texture(ourTexture1, Texcoord);
    
}