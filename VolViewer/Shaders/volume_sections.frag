
uniform int channel;
uniform sampler3D volumeTextureR;
uniform sampler3D volumeTextureG;
uniform sampler3D volumeTextureB;
uniform sampler2D RGBlookupTexture;

void main()
{	

	vec3 texcoord = gl_TexCoord[0].stp;
	
	//get the texture rgb data
	vec3 texValue;
	texValue.x = texture3D(volumeTextureR, texcoord).x;
	texValue.y = texture3D(volumeTextureG, texcoord).x;
	texValue.z = texture3D(volumeTextureB, texcoord).x;

	//==============================
	//APLY OUR TRANSFER FUNCTION
	//==============================
	//make sure we sample from centre of lookup table
	//float lookup_offset = (1.0/256.0/2.0);
	//texValue += lookup_offset;

	//get lookup table values
	//r,g,b,ra,ga,ba
	vec4 finalColour = vec4(0.0);

	//get lookup table values.
	vec3 RGBlookupValue = vec3(0.0);
	
	RGBlookupValue.x = texture2D(RGBlookupTexture, vec2(texValue.x, 0.0)).x;
	RGBlookupValue.y = texture2D(RGBlookupTexture, vec2(texValue.y, 0.0)).y;
	RGBlookupValue.z = texture2D(RGBlookupTexture, vec2(texValue.z, 0.0)).z;

	finalColour = vec4(RGBlookupValue,1.0);
	
	if(RGBlookupValue.x==0.0 && RGBlookupValue.y==0.0 && RGBlookupValue.z==0.0) finalColour.w = 0.0;
	else  finalColour.w = 1.0;

	if(finalColour.w==0.0) discard;
	
	//assing the colour to our final colour
	gl_FragColor = finalColour;	
}