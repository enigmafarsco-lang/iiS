#version 330 core
layout(location = 0) in vec2 position;

uniform mat4    modelToWorld;
uniform mat4    worldToCamera;
uniform mat4    cameraToView;
uniform mat4    secondCamera;
uniform float 	Time;

out float power;
out float alphaPower;

void main()
{
    vec4 VP = vec4(position.xy,0.0,1.0);
    gl_Position =  cameraToView * worldToCamera * modelToWorld * vec4(VP.xy,0.0, 1.0);

	alphaPower = 1.0;
	power = ((gl_Position.y + 1.0) / 2.0);
	if (power > 1.0)
	{
		power = 1.0;
	}
	if(power < 0)
	{
		power = 0.0;
		alphaPower = 1.0;
	}
    
	power = 1 - power;
	power =  power *(240.0/360.0);
	gl_Position =  secondCamera * worldToCamera * modelToWorld * vec4(position.x,Time,0.0, 1.0);
}

