#version 330
layout(location = 0) in vec4 position;


uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

out vec2 qt_TexCoord0;

void main(void)
{
    gl_Position =  cameraToView * worldToCamera * modelToWorld * vec4(position.xy,0.0, 1.0);
    qt_TexCoord0 = position.zw;
}

