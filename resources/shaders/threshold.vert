#version 330
layout(location = 0) in vec3 position;


uniform mat4    modelToWorld;
uniform mat4    worldToCamera;
uniform mat4    cameraToView;


void main(void)
{
    gl_Position =  cameraToView * worldToCamera * modelToWorld * vec4(position, 1.0);
}
