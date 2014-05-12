varying vec3 V;
varying vec3 V1;
varying vec3 dir;
varying vec3 I;

void main() 
{	
   V = vec3(gl_ModelViewMatrix * gl_Vertex);
   V1 = vec3(gl_Vertex);
   I = V;
   
   dir = vec3(gl_ModelViewMatrix*vec4(0,0,-1,1));
   
   //Calculate vertex's texture coordinate
   gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;	
   
   gl_ClipVertex = gl_ModelViewMatrix*gl_Vertex;
   
   //Calculate vertex's final, transformed position.
   gl_Position = ftransform();
}
