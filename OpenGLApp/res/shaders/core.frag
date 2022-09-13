#version 330 core

in vec2 TexCoord;
in float Height;
out vec4 color;

// Texture samplers
uniform sampler2D ourTexture1;

void main()
{
    // Linearly interpolate between both textures (second texture is only slightly combined)
    //color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    float h = (Height + 16)/32.0f;	// shift and scale the height in to a grayscale value
    color = vec4(h, h, h, 1.0);
}