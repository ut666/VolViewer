
uniform int channel;
uniform sampler3D volumeTexture;
uniform sampler2D RGBlookupTexture;
uniform sampler2D RAGABAlookupTexture;

varying vec3 V;
varying vec3 V1;

//Lambertian Lighting with approximated directional derivatives
vec3 shading_func(vec3 N, vec3 V, vec3 L)
{
	//get the texture rgb data
	float texValue;
	texValue = texture3D(volumeTexture, gl_TexCoord[0].stp).x;
	//make sure we sample from centre of lookup table
	float lookup_offset = (1.0/256.0/2.0);
	texValue += lookup_offset;

	vec3 Ka = gl_LightSource[0].ambient.xyz; //vec3(0.1, 0.1, 0.1);
	vec3 Kd = gl_LightSource[0].diffuse.xyz; //vec3(0.6, 0.6, 0.6);
	vec3 Ks = gl_LightSource[0].specular.xyz; //vec3(0.2, 0.2, 0.2);
	float n = 32.0;
	
	vec3 ambient = Ka;

	vec3 sampleL = vec3(texture3D(volumeTexture, gl_TexCoord[0].xyz+0.02*L.xyz));
	vec3 n_l = (sampleL-vec3(texValue));
	float diffuseLight = max(n_l.x,0.0);

	vec3 diffuse = Kd * n_l;

	return ambient+diffuse;
}

void main()
{	
	vec3 texcoord = gl_TexCoord[0].stp; //+ texture2D(noiseTexture, dir*V/32);

	//get the texture rgb data
	float texValue;
	texValue = texture3D(volumeTexture, texcoord).x;

	//==============================
	//APLY OUR TRANSFER FUNCTION
	//==============================
	//make sure we sample from centre of lookup table
	float lookup_offset = (1.0/256.0/2.0);
	texValue += lookup_offset;

	//get lookup table values
	//r,g,b,ra,ga,ba
	vec3 RGBlookupValue, RAGABAlookupValue;
	RGBlookupValue = RAGABAlookupValue = vec3(0.0);
	vec4 finalColour = vec4(0.0,0.0,0.0,0.0);

	if(channel==0)
	{
		RGBlookupValue.x = texture2D(RGBlookupTexture, vec2(texValue, 0.0)).x;
		RAGABAlookupValue.x = texture2D(RAGABAlookupTexture, vec2(texValue, 0.0)).x;
		finalColour = vec4(RGBlookupValue.x,0.0,0.0, RAGABAlookupValue.x);
	}
	else if(channel==1)
	{
		RGBlookupValue.y = texture2D(RGBlookupTexture, vec2(texValue, 0.0)).y;
		RAGABAlookupValue.y = texture2D(RAGABAlookupTexture, vec2(texValue, 0.0)).y;
		finalColour = vec4(0.0,RGBlookupValue.y,0.0, RAGABAlookupValue.y);
	}
	else if(channel==2)
	{
		RGBlookupValue.z = texture2D(RGBlookupTexture, vec2(texValue, 0.0)).z;
		RAGABAlookupValue.z = texture2D(RAGABAlookupTexture, vec2(texValue, 0.0)).z;
		finalColour = vec4(0.0,0.0,RGBlookupValue.z, RAGABAlookupValue.z);
	}
	else if(channel==3)
	{
		RGBlookupValue.x = texture2D(RGBlookupTexture, vec2(texValue, 0.0)).x;
		RAGABAlookupValue.x = texture2D(RAGABAlookupTexture, vec2(texValue, 0.0)).x;
		finalColour = vec4(RGBlookupValue.x,RGBlookupValue.x,RGBlookupValue.x, RAGABAlookupValue.x);
	}

	vec3 N;
	vec3 L;
	vec3 shading;

	L = normalize(gl_LightSource[0].position.xyz);	
	shading = shading_func(N,V,-L);
	finalColour += vec4(shading,0.0);

	//assing the colour to our final colour
	if(finalColour.a==0.0) finalColour = vec4(0.0);
	gl_FragColor = finalColour;	
}

/*	//Lambertian Lighting with approximated directional derivatives
	vec3 L = normalize(gl_LightSource[0].position.xyz - V);	
	vec3 sampleL = texture3D(volumeTexture, vec3(texcoord+0.01*L)).x;
	vec3 n_l = (sampleL-texValue);
	vec3 diffuse_contrib = vec3(1,1,1);
	vec3 diffuse = diffuse_contrib * n_l;
	finalColour.xyz += diffuse;
*/
