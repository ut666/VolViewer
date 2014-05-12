// Application to vertex shader
varying vec3 N;
varying vec3 I;
varying vec4 Cs;

void main()
{
	vec4 P = gl_ModelViewMatrix * gl_Vertex;
	
	I  = gl_Vertex;
	
	N  = gl_NormalMatrix * gl_Normal;
	Cs = gl_Color;
	
	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;	
} 
