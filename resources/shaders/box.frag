#version 330
uniform vec4    boxColor;
out vec4 FragColor;
void main(void)
{
    FragColor =  vec4(boxColor.rgb,1.0);
}
