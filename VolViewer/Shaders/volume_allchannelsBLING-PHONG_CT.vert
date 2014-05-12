varying vec3 V;
varying vec3 V1;

void main() 
{	
   V =  vec3(gl_ModelViewMatrix * gl_Vertex);
   V1 = vec3(ftransform());

	//Calculate vertex's texture coordinate
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;	
	
	gl_ClipVertex = gl_ModelViewMatrix*gl_Vertex;
 
 	//Calculate vertex's final, transformed position.
	gl_Position = ftransform();
}