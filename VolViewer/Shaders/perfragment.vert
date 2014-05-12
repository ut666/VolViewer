varying vec3 normal, lightDir, halfVector;
uniform vec3 surfcol;
	
void main()
{	
	vec4 worldPos;

	normal = normalize(gl_NormalMatrix * gl_Normal);
		
	worldPos = gl_Vertex;
	lightDir = normalize(vec3(gl_LightSource[0].position - worldPos));
	
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);
	
	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = ftransform();
} 

