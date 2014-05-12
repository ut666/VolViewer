varying vec2 TexCoord;

void main(void)
{
   TexCoord			= vec2(gl_MultiTexCoord0);
   gl_TexCoord[0]	= gl_MultiTexCoord0;
   gl_Position		= gl_ModelViewProjectionMatrix * gl_Vertex;
}
