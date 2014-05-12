varying vec3 V;
varying vec3 V1;
varying vec3 L;

void main() 
{	
 	V =  vec3(gl_ModelViewMatrix * gl_Vertex);
	V1 = vec3(ftransform());
	L = V1-(gl_TextureMatrix[0]*gl_LightSource[0].position).xyz;
    L = normalize(L);

	//Calculate vertex's texture coordinate
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;	
	gl_ClipVertex = gl_ModelViewMatrix*gl_Vertex;
 
 	//Calculate vertex's final, transformed position.
	gl_Position = ftransform();
}