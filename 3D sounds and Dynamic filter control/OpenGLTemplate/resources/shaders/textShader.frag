#version 400 core

in vec2 vTexCoord;
out vec4 vOutputColour;

uniform sampler2D sampler0;
uniform vec4 vColour;

void main()
{
	vec4 vTexColour = texture(sampler0, vTexCoord);	// Get the texel colour from the image
	vOutputColour = vec4(vTexColour.r) * vColour;			// The texel colour is a grayscale value -- apply to RGBA and combine with vColor
}