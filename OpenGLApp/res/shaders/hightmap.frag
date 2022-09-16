#version 330 core

in float Height;
out vec4 color;



void main()
{
    
    float h = (Height + 16)/32.0f;	// shift and scale the height in to a grayscale value
    color = vec4(h, h, h, 1.0);
}