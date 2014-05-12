void main() 
{
//
//	vec4 pos = gl_ModelViewMatrix*gl_Vertex;
	
	//Calculate vertex's texture coordinate
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;	
	
//	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
 
 	//Calculate vertex's final, transformed position.
	gl_Position = ftransform();
}