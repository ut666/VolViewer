
uniform sampler3D volumeTextureR;
uniform sampler3D volumeTextureG;
uniform sampler3D volumeTextureB;
uniform sampler2D RGBlookupTexture;

void main() 
{	
	//Calculate vertex's texture coordinate
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;	
	
	gl_ClipVertex = gl_ModelViewMatrix*gl_Vertex;
 
 	//Calculate vertex's final, transformed position.
	gl_Position = ftransform();
}
