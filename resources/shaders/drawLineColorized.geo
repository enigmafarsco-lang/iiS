#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float	lineWidth;		// the thickness of the line in pixels
uniform vec2	windowScale;	// the size of the viewport in pixels

in float power[];
in float alphaPower[];

out float fPower;
out float fAlphaPower;

vec2 screen_space(vec4 vertex)
{
	return vec2( vertex.xy / vertex.w ) * windowScale;
}

void main()
{	
	// get the two vertices passed to the shader:
	vec2 p0 = screen_space(gl_in[0].gl_Position);	// start of current segment
	vec2 p1 = screen_space(gl_in[1].gl_Position);	// end of current segment
	
	// perform naive culling
	vec2 area = windowScale * 1.2;
	if( p0.x < -area.x || p0.x > area.x ) return;
	if( p0.y < -area.y || p0.y > area.y ) return;
	if( p1.x < -area.x || p1.x > area.x ) return;
	if( p1.y < -area.y || p1.y > area.y ) return;
	
	// determine the direction of the current segment
	vec2 v0 = normalize(p1-p0);
	
	// determine the normal of the current segment
	vec2 n0 = vec2(-v0.y, v0.x);
	
	// generate the triangle strip
	
	float half_lineWidth = lineWidth / 2.0;
	
	fPower = power[0];
	fAlphaPower = alphaPower[0];
	gl_Position = vec4( (p0 + half_lineWidth * n0) / windowScale, 0.0, 1.0 );
	EmitVertex();
	
	fPower = power[0];
	fAlphaPower = alphaPower[0];
	gl_Position = vec4( (p0 - half_lineWidth * n0) / windowScale, 0.0, 1.0 );
	EmitVertex();
	
	fPower = power[1];
	fAlphaPower = alphaPower[1];
	gl_Position = vec4( (p1 + half_lineWidth * n0) / windowScale, 0.0, 1.0 );
	EmitVertex();
	
	fPower = power[1];
	fAlphaPower = alphaPower[1];
	gl_Position = vec4( (p1 - half_lineWidth * n0) / windowScale, 0.0, 1.0 );
	EmitVertex();
	
	EndPrimitive();
	
}