uniform sampler2D Texmap0;
uniform sampler2D RGBlookupTexture;
uniform sampler2D RAGABAlookupTexture;

void main()
{
	vec4 texValue0 = texture2D(Texmap0, gl_TexCoord[0].xy);
	 
	//==============================
	//APLY OUR TRANSFER FUNCTION
	//==============================

	//get lookup table values
	//r,g,b,ra,ga,ba
	vec3 RGBlookupValue, RAGABAlookupValue;
	RGBlookupValue.x = (texture2D(RGBlookupTexture, vec2(texValue0.x, 0.5))).x;
	RAGABAlookupValue.x = (texture2D(RAGABAlookupTexture, vec2(texValue0.x, 0.5))).x;
      
	//==============================
	//NOW APLY OUR BLENDING
	//==============================
	vec4 finalColour = vec4(RGBlookupValue.x,RGBlookupValue.x,RGBlookupValue.x, RAGABAlookupValue.x);

	//assing the colour to our final colour
	gl_FragColor = finalColour;	
}
