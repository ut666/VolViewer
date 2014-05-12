
uniform sampler3D volumeTextureR;
uniform sampler3D volumeTextureG;
uniform sampler3D volumeTextureB;
uniform sampler2D RGBlookupTexture;

void main()
{	
	//retrieve our texture coordinate
	vec3 texcoord = gl_TexCoord[0].stp;

	//get the texture rgb data
	float texValueR = texture3D(volumeTextureR, texcoord).x;
	float texValueG = texture3D(volumeTextureG, texcoord).x;
	float texValueB = texture3D(volumeTextureB, texcoord).x;

	//get lookup table values
	//r,g,b,ra,ga,ba
	vec4 RGBlookupValue = vec4(0.0);
	RGBlookupValue.x = texture2D(RGBlookupTexture, vec2(texValueR, 0.0)).x;
	RGBlookupValue.y = texture2D(RGBlookupTexture, vec2(texValueG, 0.0)).y;
	RGBlookupValue.z = texture2D(RGBlookupTexture, vec2(texValueB, 0.0)).z;

	//max	
	RGBlookupValue.w = RGBlookupValue.x;
	RGBlookupValue.w = max(RGBlookupValue.w, RGBlookupValue.y);
	RGBlookupValue.w = max(RGBlookupValue.w, RGBlookupValue.z);

/*	
	//min
	RGBlookupValue.w = RGBlookupValue.x;
	RGBlookupValue.w = min(RGBlookupValue.w, RGBlookupValue.y);
	RGBlookupValue.w = min(RGBlookupValue.w, RGBlookupValue.z);	
*/

/*	
	//average
	//RGBlookupValue.w = (RGBlookupValue.x+RGBlookupValue.y+RGBlookupValue.z)/3.0;
*/
	//assing the colour to our final colour
	gl_FragColor = RGBlookupValue;	
}