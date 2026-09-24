#version 330 core

//out vec4 fColor;
uniform vec4 color;

void main()
{
	gl_FragColor = vec4(color.rgb,1.0);		
}
