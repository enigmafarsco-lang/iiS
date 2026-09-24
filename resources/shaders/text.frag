#version 330 core
in vec2 TexCoords;


uniform sampler2D text;
uniform vec3 color;

void main()
{
    vec4 sampled = vec4(1.0,1.0,1.0,texture2D(text,TexCoords).r);
    gl_FragColor = vec4(color.rgb,1.0) * sampled;
}
