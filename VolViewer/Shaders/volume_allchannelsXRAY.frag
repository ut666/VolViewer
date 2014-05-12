
uniform int channel;
uniform sampler3D volumeTexture;
uniform sampler2D RGBlookupTexture;
uniform sampler2D RAGABAlookupTexture;
varying vec3 dir;
varying vec3 V;
varying vec3 V1;
varying vec3 I;

//BLING-PHONG LIGHTING MODEL
vec3 getnormal()
{
	//==============================================
	//EVALUATE OUR NORMAL BY CENTRAL DIFFERENCE
	//==============================================
	float stepsize	= 1.0/512.0;
	vec3 sample1,sample2,sample3,sample4;	
	sample1.x = texture3D(volumeTexture, gl_TexCoord[0].stp-vec3(stepsize,0,0)).x;
	sample2.x = texture3D(volumeTexture, gl_TexCoord[0].stp+vec3(stepsize,0,0)).x;
	sample1.y = texture3D(volumeTexture, gl_TexCoord[0].stp-vec3(0,stepsize,0)).x;
	sample2.y = texture3D(volumeTexture, gl_TexCoord[0].stp+vec3(0,stepsize,0)).x;
	sample1.z = texture3D(volumeTexture, gl_TexCoord[0].stp-vec3(0,0,stepsize)).x;
	sample2.z = texture3D(volumeTexture, gl_TexCoord[0].stp+vec3(0,0,stepsize)).x;

	vec3 N = sample2-sample1;
	N = normalize(N);
	
	//N.rgb = N.rgb*2.0 - 1.0;
	N = normalize(N);
	return N;
}

void main()
{	

	vec3 texcoord = gl_TexCoord[0].stp; //+ texture2D(noiseTexture, dir*V/32);

	//get the texture rgb data
	float texValue;
	texValue = texture3D(volumeTexture, texcoord).x;
	if(texValue==0.0) discard;

	//==============================
	//APLY OUR TRANSFER FUNCTION
	//==============================
	//make sure we sample from centre of lookup table
	//float lookup_offset = (1.0/256.0/2.0);
	//texValue += lookup_offset;

	//get lookup table values
	//r,g,b,ra,ga,ba
	vec3 RGBlookupValue, RAGABAlookupValue;
	RGBlookupValue = RAGABAlookupValue = vec3(0.0);
	vec4 finalColour = vec4(0.0);

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

	vec3 normal = getnormal();
	
	float opac = dot(normalize(gl_NormalMatrix*normal), normalize(gl_TextureMatrixInverse[0]*vec4(texcoord,1.0)));
	opac = abs(opac);
	
	opac = 1.0 - pow(opac, 0.1);
	finalColour.a *= opac;

	if(finalColour.w==0.0) discard;
	
	//assing the colour to our final colour
	gl_FragColor =finalColour;	
}