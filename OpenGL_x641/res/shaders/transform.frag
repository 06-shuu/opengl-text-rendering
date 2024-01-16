
#version 330 core
out vec3 frag_color;
in vec2 TextCoord; 
uniform vec4 vertColor;
uniform sampler2D texSampler1;
uniform sampler2D texSampler2;

void main()
{
	 //frag_color = vertColor;
	 vec4 wallColor = texture(texSampler1, TextCoord);
	 vec4 marioColor = texture(texSampler2, TextCoord);
	 frag_color = mix(wallColor.rgb, marioColor.rgb, marioColor.a); //to mix the two textures
};
