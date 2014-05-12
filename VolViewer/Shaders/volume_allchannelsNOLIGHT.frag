
uniform int channel;
uniform sampler3D volumeTexture;
uniform sampler3D volumeTexture2;
uniform sampler2D RGBlookupTexture;
uniform sampler2D RAGABAlookupTexture;

void main()
{		
	//get the texture rgb data
	float texValue = texture3D(volumeTexture, gl_TexCoord[0].stp).x;

	//get lookup table values
	//r,g,b,ra,ga,ba
	float RGBlookupValue, RAGABAlookupValue;
	RGBlookupValue = RAGABAlookupValue = 0.0;
	vec4 finalColour = vec4(0.0);
	
	switch (channel) 
	{
		case 0:
			RGBlookupValue = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).x;
			RAGABAlookupValue = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).x;
			finalColour = vec4(RGBlookupValue, 0.0, 0.0, RAGABAlookupValue);
		break;
		case 1:
			RGBlookupValue = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).y;
			RAGABAlookupValue = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).y;			
			finalColour = vec4(0.0,RGBlookupValue,0.0, RAGABAlookupValue);
		break;
		case 2:
			RGBlookupValue = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).z;
			RAGABAlookupValue = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).z;
			finalColour = vec4(0.0, 0.0, RGBlookupValue, RAGABAlookupValue);
		break;
		case 3:
			RGBlookupValue = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).x;
			RAGABAlookupValue = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).x;
			finalColour = vec4(RGBlookupValue,RGBlookupValue,RGBlookupValue, RAGABAlookupValue);
		break;
		case 4:
			float texValue2 = texture3D(volumeTexture2, gl_TexCoord[0].stp).x;		
			RGBlookupValue = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).x;
			RAGABAlookupValue = texture2D(RAGABAlookupTexture, vec2(texValue2, 0.5)).x;	 
			finalColour = vec4(RGBlookupValue,RGBlookupValue,RGBlookupValue, RAGABAlookupValue);	
		break;
		default:
		break;
	}
	
	/*
	if(channel==0)
	{
		RGBlookupValue.x = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).x;
		RAGABAlookupValue.x = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).x;

		finalColour = vec4(RGBlookupValue.x, 0.0, 0.0, RAGABAlookupValue.x);
	}
	if(channel==1)
	{
		RGBlookupValue.y = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).y;
		RAGABAlookupValue.y = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).y;
		
		finalColour = vec4(0.0,RGBlookupValue.y,0.0, RAGABAlookupValue.y);
	}
	if(channel==2)
	{
		RGBlookupValue.z = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).z;
		RAGABAlookupValue.z = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).z;

		finalColour = vec4(0.0, 0.0, RGBlookupValue.z, RAGABAlookupValue.z);
	}
	if(channel==3)
	{		
		RGBlookupValue.x = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).x;
		RAGABAlookupValue.x = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).x;

		finalColour = vec4(RGBlookupValue.x,RGBlookupValue.x,RGBlookupValue.x, RAGABAlookupValue.x);
	}
	if(channel==4)
	{
		float texValue2 = texture3D(volumeTexture2, gl_TexCoord[0].stp).x;
		
		RGBlookupValue.x = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).x;
		RAGABAlookupValue.x = texture2D(RAGABAlookupTexture, vec2(texValue2, 0.5)).x;
 
		finalColour = vec4(RGBlookupValue.x,RGBlookupValue.x,RGBlookupValue.x, RAGABAlookupValue.x);	
	}*/
	
	if(finalColour.w==0.0)
	{
		discard;
		return;
	}
	
	//assing the colour to our final colour
	gl_FragColor = finalColour;	
}