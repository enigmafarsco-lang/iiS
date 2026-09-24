#version 330
uniform sampler2D texture0;
in vec2 qt_TexCoord0;

void main(void)
{
    gl_FragColor = texture2D(texture0, qt_TexCoord0);
	// = vec4(color.rgb,0.5);
}
