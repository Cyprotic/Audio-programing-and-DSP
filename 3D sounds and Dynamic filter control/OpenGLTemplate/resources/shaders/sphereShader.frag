#version 400 core

uniform int levels;

in vec3 vColour;			// Note: colour is smoothly interpolated (default)
in float fIntensity;
out vec4 vOutputColour;

void main()
{	
	//vOutputColour = vec4(vColour, 1.0);
	vec3 quantisedColour = floor(vColour * levels) / levels;
	vOutputColour = vec4(quantisedColour, 0.5f);
}
