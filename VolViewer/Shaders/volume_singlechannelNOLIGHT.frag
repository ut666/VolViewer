uniform int channel;
uniform sampler3D volumeTexture;
uniform sampler2D RGBlookupTexture;
uniform sampler2D RAGABAlookupTexture;

void main()
{	

	vec3 texcoord = gl_TexCoord[0].stp; //+ texture2D(noiseTexture, dir*V/32);

	//get the texture rgb data
	float texValue;
	texValue = texture3D(volumeTexture, texcoord).x;
	
	//========================================
	//GET OUR VOXEL COORDINATE AND RGB VALUE
	//========================================
	//get our coord in screen space (nb) screen space is always -128 -> 128
	//vec3 voxelcoord = 256.0*(vec3(gl_TexCoord[0].s, gl_TexCoord[0].t, gl_TexCoord[0].p))-128.0;

	//==============================
	//APLY OUR TRANSFER FUNCTION
	//==============================
	//make sure we sample from centre of lookup table
	float lookup_offset = (1.0/512.0/2.0);
	texValue += lookup_offset;

	//get lookup table values
	//r,g,b,ra,ga,ba
	vec3 RGBlookupValue, RAGABAlookupValue;
	RGBlookupValue = RAGABAlookupValue = vec3(0.0,0.0,0.0);
	vec4 finalColour = vec4(0.0,0.0,0.0,0.0);

//	if(channel==0)
	{
		RGBlookupValue.x = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).x;
		RAGABAlookupValue.x = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).x;
		finalColour = vec4(RGBlookupValue.x,0.0,0.0, RAGABAlookupValue.x);
	}
/*	if(channel==1)
	{
		RGBlookupValue.y = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).y;
		RAGABAlookupValue.y = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).y;
		finalColour = vec4(0.0,RGBlookupValue.y,0.0, RAGABAlookupValue.y);
	}
	if(channel==2)
	{
		RGBlookupValue.z = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).z;
		RAGABAlookupValue.z = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).z;
		finalColour = vec4(0.0,0.0,RGBlookupValue.z, RAGABAlookupValue.z);
	}
	if(channel==3)
	{
		RGBlookupValue.x = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).x;
		RAGABAlookupValue.x = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).x;
		finalColour = vec4(RGBlookupValue.x,RGBlookupValue.x,RGBlookupValue.x, RAGABAlookupValue.x);
	}*/

	//finalColour.rgb *= finalColour.a;
	if(finalColour.w==0.0) discard;

	//assing the colour to our final colour
	gl_FragColor = finalColour;	
	//gl_FragColor.rgb *= gl_FragColor.a;
}