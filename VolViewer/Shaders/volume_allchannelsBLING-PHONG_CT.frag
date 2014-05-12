uniform int channel;
uniform sampler3D volumeTexture;
uniform sampler2D RGBlookupTexture;
uniform sampler2D RAGABAlookupTexture;
varying vec3 V;
varying vec3 V1;

//COOK-TORRANCE LIGHTING MODEL
vec3 shading_func(vec3 N, vec3 V, vec3 L)
{
	//==============================================
	//EVALUATE OUR NORMAL BY CENTRAL DIFFERENCE
	//==============================================
	float stepsize	= 1.0/256.0;
	vec3 sample1,sample2,sample3,sample4;
	sample1.x = texture3D(volumeTexture, gl_TexCoord[0].stp-vec3(stepsize,0,0)).x;
	sample2.x = texture3D(volumeTexture, gl_TexCoord[0].stp+vec3(stepsize,0,0)).x;
	sample1.y = texture3D(volumeTexture, gl_TexCoord[0].stp-vec3(0,stepsize,0)).x;
	sample2.y = texture3D(volumeTexture, gl_TexCoord[0].stp+vec3(0,stepsize,0)).x;
	sample1.z = texture3D(volumeTexture, gl_TexCoord[0].stp-vec3(0,0,stepsize)).x;
	sample2.z = texture3D(volumeTexture, gl_TexCoord[0].stp+vec3(0,0,stepsize)).x;

	N = sample2-sample1;
	N = normalize(N);
	//N.rgb = N.rgb*2.0 - 1.0;
	//N = normalize(N);

	vec3 Ka = gl_LightSource[0].ambient; //vec3(0.1, 0.1, 0.1);
	vec3 Kd = gl_LightSource[0].diffuse; //vec3(0.6, 0.6, 0.6);
	vec3 Ks = gl_LightSource[0].specular; //vec3(0.2, 0.2, 0.2);
	float mean = 0.25;
	float scale = 0.05;

	vec3 ambient = Ka;

	vec3 H = normalize(L+V1);
	float n_h = dot(H,N);	
	float n_v = dot(V1,N);	
	float v_h = dot(H,V1);	
	float n_l = dot(L,N);	

	vec3 diffuse = Kd * max(n_l, 0.0);

	float fresnel = pow(1.0+v_h, 1.5);
	float delta = acos(n_h);
	float exponent = -pow((delta/mean),1.5);
	float microfacets = scale * exp(exponent);

	float term1 = (2.0 * n_h * n_v)/n_h;
	float term2 = (2.0 * n_h * n_l)/n_h;
	float selfshadow = min(1.0, min(term1,term2));

	vec3 specular = (Ks*fresnel*microfacets*selfshadow)/n_v;

	return ambient+diffuse+specular;
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
	vec4 finalColour;
	finalColour = vec4(0.0, 0.0, 0.0, 0.0);

	if(channel==0)
	{
		RGBlookupValue.x = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).x;
		RAGABAlookupValue.x = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).x;
		finalColour = vec4(RGBlookupValue.x,0.0,0.0, RAGABAlookupValue.x);
	}
	else if(channel==1)
	{
		RGBlookupValue.y = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).y;
		RAGABAlookupValue.y = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).y;
		finalColour = vec4(0.0,RGBlookupValue.y,0.0, RAGABAlookupValue.y);
	}
	else if(channel==2)
	{
		RGBlookupValue.z = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).z;
		RAGABAlookupValue.z = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).z;
		finalColour = vec4(0.0,0.0,RGBlookupValue.z, RAGABAlookupValue.z);
	}
	else if(channel==3)
	{
		RGBlookupValue.x = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).x;
		RAGABAlookupValue.x = texture2D(RAGABAlookupTexture, vec2(texValue, 0.5)).x;
		finalColour = vec4(RGBlookupValue.x,RGBlookupValue.x,RGBlookupValue.x, RAGABAlookupValue.x);
	}

	vec3 N;
	vec3 L;
	vec3 shading;

	L = normalize(V1 - (gl_TextureMatrix[0]*gl_LightSource[0].position).xyz);	//gl_LightSource[0].position.xyz - V
	
	shading = shading_func(N,V,-L);
	finalColour += vec4(shading,0.0);

	//assing the colour to our final colour
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

	//==============================================
	//EVALUATE OUR NORMAL BY CENTRAL DIFFERENCE
	//==============================================
 /* 	vec4 normal;
	bool degenerate;
//	if(silhouette_enable==true || gooch_enable==true || lighting_enable==true)
//	{
		float stepsize	= 1.0/256.0;
		normal.x = (texture3D(volumeTexture, vec3(gl_TexCoord[0].s-stepsize, gl_TexCoord[0].t, gl_TexCoord[0].p)) - texture3D(volumeTexture, vec3(gl_TexCoord[0].s+stepsize, gl_TexCoord[0].t, gl_TexCoord[0].p))).x;
		normal.y = (texture3D(volumeTexture, vec3(gl_TexCoord[0].s, gl_TexCoord[0].t-stepsize, gl_TexCoord[0].p)) - texture3D(volumeTexture, vec3(gl_TexCoord[0].s, gl_TexCoord[0].t+stepsize, gl_TexCoord[0].p))).x;
		normal.z = (texture3D(volumeTexture, vec3(gl_TexCoord[0].s, gl_TexCoord[0].t, gl_TexCoord[0].p-stepsize)) - texture3D(volumeTexture, vec3(gl_TexCoord[0].s, gl_TexCoord[0].t, gl_TexCoord[0].p+stepsize))).x;
		
		float gradMag = length(normal);

		//We have to scale the gradient between -1 and 1
		//normal.rgb = normal.rgb*2.0 - 1.0;
		degenerate = texValue < .1;
		if(!degenerate)
		{
			normal.rgb = normalize(gl_NormalMatrix * normalize(normal.rgb));
		}
//	}*/
	//=============================
	//APPLY OUR LIGHTING
	//=============================
/*	vec3 L = normalize(gl_LightSource[0].position.xyz - V); 
	vec3 E = normalize(-V); // we are in Eye Coordinates, so EyePos is (0,0,0)
	vec3 R = normalize(-reflect(L, normal)); 

	vec3 E1 = normalize(-V1); // we are in Eye Coordinates, so EyePos is (0,0,0)
	float dotView = dot(E1, normal.xyz);

	//calculate Ambient Term:
	vec4 Iamb = gl_FrontLightProduct[0].ambient;

	//calculate Diffuse Term:
	vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(vec3(normal),L), 0.0);

	// calculate Specular Term:
	vec4 Ispec = max(dot(R,E),0.0);

	finalColour.rgb *=  (Iamb + Idiff + Ispec);
*/
/*	//=============================
	//APPLY SILHOUTTE ENHANCEMENTS
	//=============================
	if(silhouette_enable==true)
	{
		vec3 E1 = normalize(-V1); // we are in Eye Coordinates, so EyePos is (0,0,0)
		float dotView = dot(E1, normal.xyz);

		if(!degenerate) finalColour.a += silhouette_amount*pow(1.0-abs(dotView),2.0);
	}

	//=============================
	//APPLY GOOCH LIGHTING
	//=============================
	if(gooch_enable==true)
	{
		//Calculate the diffuse contribution of the light
		float dotLight = dot(normalize(vec3(-gl_LightSource[0].position)), normal.rgb);

		//Calculate the correct color according to the Gooch shading model.
		//toneLight ranges from -1 to 1
		dotLight = dot(vec3(normal),L);
		
		vec3 warm = vec3(gooch_warmCol[0], gooch_warmCol[1], gooch_warmCol[2]);
		vec3 cool = vec3(gooch_coolCol[0], gooch_coolCol[1], gooch_coolCol[2]);

		finalColour.rgb += gooch_toneCont*(((1.0+dotLight)*.5)*warm + (1.0-((1.0+dotLight)*.5))*cool);
	}
	
	//float boundCont = 0.5;
//	float boundExp = silhouette_amount;
//	finalColour.a += silhouette_amount*pow(gradMag,boundExp);

//	float edgeExp = silhouette_amount;
//	float edgeThresh = 0.1 ;
//	float edgeVal = pow(1.0-abs(dotView),edgeExp);
//	if(edgeVal >= edgeThresh && !degenerate) finalColour.rgb = mix(finalColour.rgb, vec3(0.0,0.0,0.0), pow((edgeVal-edgeThresh)/(1.0-edgeThresh),4.0));
*/
	//=============================
	//APPLY LIGHTING
	//=============================
//	if(lighting_enable==true) 

	//finalColour.xyz = vec3(gl_FrontLightProduct[0].specular);