
uniform int channel;
uniform sampler3D volumeTexture;
uniform sampler2D RGBlookupTexture;
uniform sampler2D RAGABAlookupTexture;
varying vec3 V;
varying vec3 V1;


vec3 eval_normal(void)
{
	vec3 normal;
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

	normal = sample2-sample1;
//	normal.rgb = normal.rgb*2.0 - 1.0;
	normal = normalize(normal);

	return normal;
}
 
//BLING-PHONG LIGHTING MODEL
vec3 shading_func(vec3 N, vec3 VV, vec3 L)
{
	vec3 Ka = gl_LightSource[0].ambient.xyz; //vec3(0.1, 0.1, 0.1);
	vec3 Kd = gl_LightSource[0].diffuse.xyz; //vec3(0.6, 0.6, 0.6);
	vec3 Ks = gl_LightSource[0].specular.xyz; //vec3(0.2, 0.2, 0.2);
	float n = 16.0;

	vec3 H = normalize(L+VV);

	vec3 ambient = Ka ;

	float diffuseLight = max(dot(L,N),0.0);
	vec3 diffuse = Kd * diffuseLight;

	float specularLight = pow(max(dot(H,N),0.0), n);
	if(diffuseLight<=0.0) specularLight = 0.0;
	vec3 specular = Ks * specularLight;

	return ambient+diffuse+specular;
}
vec3 getNormal(vec3 v1, vec3 v2, vec3 v3)
{
	vec3 N, n0, n1;
	
	n0 = v1 - v2;
	n1 = v2 - v3;
	
	N = cross(n0, n1);
	N = normalize(N);
		
	return N;
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
		
	/*float size = (1.0/512.0) * 128.0;	
	float halfang = 60.0*(3.141592654/180.0);
	vec3 direction = gl_LightSource[0].spotDirection;	
	vec3 apex =	vec3(0.0,0.0,-size/2.0);
	vec3 vp = (V1-apex);

	float alpha = acos(dot(normalize(vp), normalize(direction)));
	if(alpha<halfang)
	{
		finalColour.xy *= 1.5;
		finalColour.w *= 0.25*pow(0.01,length(vp));
	}
	else
	{
	
	}*/

	float size = (1.0/512.0) * 64.0;
	
	vec3 pyramidv1 = vec3(-size,size,-size);
	vec3 pyramidv2 = vec3(-size,-size,-size);
	vec3 pyramidv3 = vec3(size,-size,-size);
	vec3 pyramidv4 = vec3(size,size,-size);
	vec3 pyramidappex = vec3(0.0,0.0,-size/2.0);

	vec3 c0 = (pyramidv1+pyramidappex+pyramidv2)/3.0;
	vec3 c1 = (pyramidv2+pyramidappex+pyramidv3)/3.0;
	vec3 c2 = (pyramidappex+pyramidv4+pyramidv3)/3.0;
	vec3 c3 = (pyramidappex+pyramidv1+pyramidv4)/3.0;
	
	vec3 n0 = getNormal(pyramidv1, pyramidappex, pyramidv2);
	vec3 n1 = getNormal(pyramidv2, pyramidappex, pyramidv3);
	vec3 n2 = getNormal(pyramidappex, pyramidv4, pyramidv3);
	vec3 n3 = getNormal(pyramidappex, pyramidv1, pyramidv4);
	
	c0 = gl_ModelViewMatrix*vec4(c0,1.0);
	c1 = gl_ModelViewMatrix*vec4(c1,1.0);
	c2 = gl_ModelViewMatrix*vec4(c2,1.0);
	c3 = gl_ModelViewMatrix*vec4(c3,1.0);
		
	n0 = gl_ModelViewMatrix*vec4(n0,1.0);
	n1 = gl_ModelViewMatrix*vec4(n1,1.0);
	n2 = gl_ModelViewMatrix*vec4(n2,1.0);
	n3 = gl_ModelViewMatrix*vec4(n3,1.0);

	//&& dot(n2, V1-c2)>0.0 && dot(n3, V1-c3)>0.0 //&& dot(n3, V1-c3)>0.0
	if(dot(n0, V1-c0)>0.0 && dot(n2, V1-c2)>0.0)
	{
		//finalColour.xy *= 1.5;
		if(channel!=1) finalColour.w = 0.0;//0.06*length(V1-pyramidappex);
	}
	
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