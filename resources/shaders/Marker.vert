#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 center;

uniform mat4    modelToWorld;
uniform mat4    worldToCamera;
uniform mat4    cameraToView;
uniform float   xscale;
uniform float	yscale;

void main()
{
    vec4 VP = vec4(center.x ,center.y,0.0,1.0) + (xscale) * vec4(position.x,0.0,0.0,1.0) + (yscale) * vec4(0.0,position.y,0.0,1.0);
	gl_Position =  cameraToView * worldToCamera * modelToWorld * vec4(VP.xyz, 1.0);	
}

