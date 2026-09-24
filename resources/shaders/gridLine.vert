#version 330 core
layout(location = 0) in vec2 position;

uniform mat4    modelToWorld;
uniform mat4    worldToCamera;
uniform mat4    cameraToView;

//uniform float   scale;


void main()
{
    vec4 VP = vec4(position.xy,0.0,1.0);
    gl_Position =  cameraToView * worldToCamera * modelToWorld * vec4(VP.xy,0.0, 1.0);

}

