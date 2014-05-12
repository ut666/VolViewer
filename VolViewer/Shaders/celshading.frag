varying vec3 vNormal;
varying vec3 vVertex;
varying vec3 vEye;

#define shininess 20.0

void main (void)
{
	vec4 color0 = vec4(0.8, 0.8, 0.8, 1.0);    // Material Color: 0.8, 0.0, 0.0, 1.0
	vec4 color1 = vec4(0.0, 0.0, 0.0, 1.0);    // Silhouette Color: black
	vec4 color2 = vec4(0.4, 0.4, 0.4, 1.0);    // Specular Color: 0.8, 0.0, 0.0, 1.0

	vNormal = vNormal;

	// Lighting
	vec3 eyePos =  vEye.xyz;
	vec3 lightPos = vec3(gl_LightSource[0].position);

	vec3 EyeVert = normalize(eyePos - vVertex);
	vec3 LightVert = normalize(lightPos - vVertex);
	vec3 EyeLight = normalize(LightVert+EyeVert);
	vec3 Normal = normalize(gl_NormalMatrix * vNormal);

	// Simple Silhouette
	float sil = max(dot(Normal,EyeVert), 0.0);
	
	if (sil < 0.5) gl_FragColor = color1;
	else 
	{

		gl_FragColor = color0 ; //* sil;

		// Specular part
		float spec = pow(max(dot(Normal,EyeLight),0.0), shininess);

		if (spec < 0.25) gl_FragColor *= 0.8;
		else gl_FragColor = color2; 

		//gl_FragColor += color2*smoothstep(0.1,0.4,spec);
		gl_FragColor += color2*step(0.4,spec);

		gl_FragColor += spec;  // advanced 3D Toon

		// Diffuse part
		float diffuse = max(dot(Normal,LightVert),0.0);
		if (diffuse < 0.5) gl_FragColor *=0.8;

		gl_FragColor.w = 1.0;
	
	}
}