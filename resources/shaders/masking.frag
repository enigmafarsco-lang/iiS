#version 330
uniform vec4    boxColor;

void main(void)
{
    gl_FragColor =  vec4(boxColor.rgb,0.4);
}
