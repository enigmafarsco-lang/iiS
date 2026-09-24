#version 330
uniform vec4    color;
out vec4 FragColor;
void main(void)
{
    FragColor =  vec4(color.rgb,color.a);
}
