uniform int channel;
uniform sampler2D Texmap0;
uniform sampler2D Texmap1;
uniform sampler2D RGBlookupTexture;
uniform sampler2D RAGABAlookupTexture;

void main()
{
	vec4 texValue0 = texture2D(Texmap0, gl_TexCoord[0].xy);
	vec4 texValue1 = texture2D(Texmap1, gl_TexCoord[0].xy);
   	vec4 result = mix(texValue0, texValue1, gl_TexCoord[0].z);
	 
	//==============================
	//APLY OUR TRANSFER FUNCTION
	//==============================

	//get lookup table values
	//r,g,b,ra,ga,ba
	vec3 RGBlookupValue, RAGABAlookupValue;
	RGBlookupValue.x = (texture2D(RGBlookupTexture, vec2(result.x, 0.0))).x;
	RAGABAlookupValue.x = (texture2D(RAGABAlookupTexture, vec2(result.x, 0.0))).x;
	RGBlookupValue.y = (texture2D(RGBlookupTexture, vec2(result.y, 0.0))).x;
	RAGABAlookupValue.y = (texture2D(RAGABAlookupTexture, vec2(result.y, 0.0))).x;
	RGBlookupValue.z = (texture2D(RGBlookupTexture, vec2(result.z, 0.0))).x;
	RAGABAlookupValue.z = (texture2D(RAGABAlookupTexture, vec2(result.z, 0.0))).x;

	//==============================
	//NOW APLY OUR BLENDING
	//==============================
	vec4 finalColour=vec4(0.0); // = vec4(RGBlookupValue.z,RGBlookupValue.z,RGBlookupValue.z, RAGABAlookupValue.z);

	if(channel==0)
	{
		finalColour = vec4(RGBlookupValue.x,0.0,0.0, RAGABAlookupValue.x);
	}
	else if(channel==1)
	{
		finalColour = vec4(0.0,RGBlookupValue.y,0.0, RAGABAlookupValue.y);
	}
	else if(channel==2)
	{
		finalColour = vec4(0.0,0.0,RGBlookupValue.z, RAGABAlookupValue.z);
	}
	else if(channel==3)
	{
		finalColour = vec4(RGBlookupValue.x,RGBlookupValue.x,RGBlookupValue.x, RAGABAlookupValue.x);
	}

	//assing the colour to our final colour
	gl_FragColor = finalColour;	
}
