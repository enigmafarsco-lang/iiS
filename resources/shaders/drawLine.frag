#version 330

uniform vec4 color;


void main()
{

        //gl_FragColor = color/*vec4(1.0,1.0,1.0,1.0)*/;
		gl_FragColor = vec4(color.rgb, 0.5);
}
