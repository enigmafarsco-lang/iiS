#version 330
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 mask;

uniform mat4    modelToWorld;
uniform mat4    worldToCamera;
uniform mat4    cameraToView;


void main(void)
{
	vec4 VP = vec4(mask.x + ((mask.y / 2.0)* position.x) , position.y , 0.0 ,1.0);//vec4(position.xy,0.0,1.0);
    gl_Position =  cameraToView * worldToCamera * modelToWorld * VP;
}
