
uniform int channel;
uniform sampler3D volumeTexture;
uniform sampler3D volumeTexture2;
uniform sampler2D RGBlookupTexture;
uniform sampler2D RAGABAlookupTexture;

varying vec3 V;
varying vec3 V1;
varying vec3 L;

//Lambertian Lighting with approximated directional derivatives
vec3 shading_func(vec3 N, vec3 V, vec3 L, vec3 texValue)
{	
	vec3 Ka = gl_LightSource[0].ambient.xyz; //vec3(0.1, 0.1, 0.1);
	vec3 Kd = gl_LightSource[0].diffuse.xyz; //vec3(0.6, 0.6, 0.6);
	
	vec3 ambient = Ka;

	vec3 sampleL = vec3(texture3D(volumeTexture, gl_TexCoord[0].xyz+(L.xyz*0.005)));
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
	//if(texValue==0.0) discard;

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
	else if(channel==4)
	{
		float texValue2 = texture3D(volumeTexture2, gl_TexCoord[0].stp).x;
		
		RGBlookupValue.x = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).x;
		RAGABAlookupValue.x = texture2D(RAGABAlookupTexture, vec2(texValue2, 0.5)).x;

		finalColour = vec4(RGBlookupValue.x,RGBlookupValue.x,RGBlookupValue.x, RAGABAlookupValue.x);	
	}
	vec3 N;
	vec3 shading = shading_func(N,V1,L,finalColour.xyz);
	finalColour += vec4(shading,0.0);

	//assing the colour to our final colour
	if(finalColour.w==0.0) discard;

	gl_FragColor = finalColour;	
}