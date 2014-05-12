varying vec3 vNormal;
varying vec3 vVertex;
varying vec3 vEye;

void main(void)
{
  vVertex = gl_Vertex.xyz;
  vNormal = gl_Normal;
  vEye = vec4(0,0,512.0,0);

  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
}
