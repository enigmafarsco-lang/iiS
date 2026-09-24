#version 400 core

uniform vec4 color;
out vec4 FragColor;

void main()
{

       FragColor = vec4(color.rgb, 0.5);

}
