uniform float maxdimension;
uniform float depthcue;
uniform int channel;
uniform vec3 eye;
uniform sampler3D volumeTexture;
uniform sampler3D volumeTexture2;
uniform sampler2D RGBlookupTexture;
uniform sampler2D RAGABAlookupTexture;

varying vec3 V;
varying vec3 V1;
varying vec3 L;
varying vec3 viewVec;
varying vec3 texCoord;
varying vec3 eye;

vec3 eval_normal(void)
{
	vec3 normal;
	//==============================================
	//EVALUATE OUR NORMAL BY CENTRAL DIFFERENCE
	//==============================================
	float stepsize	= 1.0/(maxdimension/2.0);
	vec3 sample1,sample2,sample3,sample4;
	sample1.x = texture3D(volumeTexture, gl_TexCoord[0].stp-vec3(stepsize,0,0)).x;
	sample2.x = texture3D(volumeTexture, gl_TexCoord[0].stp+vec3(stepsize,0,0)).x;
	sample1.y = texture3D(volumeTexture, gl_TexCoord[0].stp-vec3(0,stepsize,0)).x;
	sample2.y = texture3D(volumeTexture, gl_TexCoord[0].stp+vec3(0,stepsize,0)).x;
	sample1.z = texture3D(volumeTexture, gl_TexCoord[0].stp-vec3(0,0,stepsize)).x;
	sample2.z = texture3D(volumeTexture, gl_TexCoord[0].stp+vec3(0,0,stepsize)).x;

	normal = sample2-sample1;
	//normal = normalize(normal);
	return normal;
}
 
//BLING-PHONG LIGHTING MODEL
vec3 shading_func(vec3 N, vec3 VV, vec3 L)
{
	N = normalize(N);

	vec3 Ka = gl_LightSource[0].ambient.xyz; //vec3(0.1, 0.1, 0.1);
	vec3 Kd = gl_LightSource[0].diffuse.xyz; //vec3(0.6, 0.6, 0.6);
	vec3 Ks = gl_LightSource[0].specular.xyz; //vec3(0.2, 0.2, 0.2);
	float n = 8.0;

	vec3 H = normalize(L+VV);

	vec3 ambient = Ka ;

	float diffuseLight = max(dot(L,N),0.0);
	vec3 diffuse = Kd * diffuseLight;

	float specularLight = pow(max(dot(H,N),0.0), n);
	if(diffuseLight<=0.0) specularLight = 0.0;
	vec3 specular = Ks * specularLight;

	return ambient+diffuse+specular;
}

void main()
{	
	vec3 texcoord = gl_TexCoord[0].stp; //+ texture2D(noiseTexture, dir*V/32);

	//get the texture rgb data
	float texValue;
	texValue = texture3D(volumeTexture, texcoord).x;
	if(texValue==0.0) 
	{
		discard;
		return;
	}

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
	if(channel==1)
	{
		RGBlookupValue.y = texture2D(RGBlookupTexture, vec2(texValue, 0.0)).y;
		RAGABAlookupValue.y = texture2D(RAGABAlookupTexture, vec2(texValue, 0.0)).y;

		finalColour = vec4(0.0,RGBlookupValue.y,0.0, RAGABAlookupValue.y);
	}
	if(channel==2)
	{
		RGBlookupValue.z = texture2D(RGBlookupTexture, vec2(texValue, 0.0)).z;
		RAGABAlookupValue.z = texture2D(RAGABAlookupTexture, vec2(texValue, 0.0)).z;

		finalColour = vec4(0.0,0.0,RGBlookupValue.z, RAGABAlookupValue.z);
	}
	if(channel==3)
	{
		RGBlookupValue.x = texture2D(RGBlookupTexture, vec2(texValue, 0.0)).x;
		RAGABAlookupValue.x = texture2D(RAGABAlookupTexture, vec2(texValue, 0.0)).x;
		
		finalColour = vec4(RGBlookupValue.x,RGBlookupValue.x,RGBlookupValue.x, RAGABAlookupValue.x);
	}
	if(channel==4)
	{
		float texValue2 = texture3D(volumeTexture2, gl_TexCoord[0].stp).x;
		
		RGBlookupValue.x = texture2D(RGBlookupTexture, vec2(texValue, 0.5)).x;
		RAGABAlookupValue.x = texture2D(RAGABAlookupTexture, vec2(texValue2, 0.5)).x;

		finalColour = vec4(RGBlookupValue.x,RGBlookupValue.x,RGBlookupValue.x, RAGABAlookupValue.x);	
	}
	
	vec3 N = eval_normal();

	vec3 shading = shading_func(N,V1,L);

	/*float dotView = dot(N, (gl_TextureMatrix[0]*vec4(0,0,-1,0)).xyz);
	
	vec4 silhouette = vec4(0.0);

	bool degenerate = false;
	float length = sqrt(N.x*N.x+N.y*N.y+N.z*N.z);//length(N);
	if(length<0.1) degenerate = true;

	if(!degenerate)
	{
		silhouette =  vec4(4.0*pow(1.0-abs(dotView),4.0));
		finalColour.xyz -= silhouette.xyz;
		//if(silhouette.w>alphathresh) finalColour.w = 1.0;
	}*/

	finalColour += vec4(shading,0.0);
	//finalColour += vec4(shading,0.0);
	//if(finalColour.w==0.0) discard;

	//========
	//depth shading cue
	//========
	vec3 distvec2 = viewVec.xyz-texCoord.xyz;
	float l = length(distvec2.xyz);
	float l_f = length(vec3(0,0,0.9)-viewVec.xyz);
	float l_b = length(vec3(0,0,-1.0)-viewVec.xyz);
	float s = depthcue;
	float f = (l-l_f)/(l_b-l_f);
	f = f*s;
	finalColour.xyz = (f)*gl_LightSource[1].ambient.xyz + (1.0-f)*finalColour.xyz;
	
	//float leng = length(N);
	//if(leng<depthcue) finalColour.w /= 4.0;
	//finalColour.w *= leng;
	
//	finalColour.w = (1.0-f)*gl_LightSource[1].ambient.x + (f)*finalColour.w;

	//assing the colour to our final colour
	gl_FragColor = finalColour;	
}