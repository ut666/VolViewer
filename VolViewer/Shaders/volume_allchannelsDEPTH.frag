
uniform int channel;
uniform sampler3D volumeTexture;
uniform sampler3D volumeTexture2;
uniform sampler2D RGBlookupTexture;
uniform sampler2D RAGABAlookupTexture;
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;

uniform float kernel[9];
uniform float kernel_norm;
uniform float fbosize;
uniform float depthoffset;
uniform float depththickness;
uniform int depthpeal;

#define s2(a, b)				temp = a; a = min(a, b); b = max(temp, b);
#define mn3(a, b, c)			s2(a, b); s2(a, c);
#define mx3(a, b, c)			s2(b, c); s2(a, c);

#define mnmx3(a, b, c)			mx3(a, b, c); s2(a, b);                                   // 3 exchanges
#define mnmx4(a, b, c, d)		s2(a, b); s2(c, d); s2(a, c); s2(b, d);                   // 4 exchanges
#define mnmx5(a, b, c, d, e)	s2(a, b); s2(c, d); mn3(a, c, e); mx3(b, d, e);           // 6 exchanges
#define mnmx6(a, b, c, d, e, f) s2(a, d); s2(b, e); s2(c, f); mn3(a, b, c); mx3(d, e, f); // 7 exchanges

// dot product (3D) which allows vector operations in arguments
#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)
#define norm(v)    sqrt(dot(v,v))  // norm = length of vector
#define d(u,v)     norm(u-v)       // distance = norm of difference

varying vec3 V;
varying vec3 V1;
varying vec3 L;

vec3 eval_normal(void)
{
	vec3 normal;
	//==============================================
	//EVALUATE OUR NORMAL BY CENTRAL DIFFERENCE
	//==============================================
	float stepsize	= 1.0/fbosize;
	vec3 sample1,sample2,sample3,sample4;
	sample1.x = texture3D(volumeTexture, gl_TexCoord[0].stp-vec3(stepsize,0,0)).x;
	sample2.x = texture3D(volumeTexture, gl_TexCoord[0].stp+vec3(stepsize,0,0)).x;
	sample1.y = texture3D(volumeTexture, gl_TexCoord[0].stp-vec3(0,stepsize,0)).x;
	sample2.y = texture3D(volumeTexture, gl_TexCoord[0].stp+vec3(0,stepsize,0)).x;
	sample1.z = texture3D(volumeTexture, gl_TexCoord[0].stp-vec3(0,0,stepsize)).x;
	sample2.z = texture3D(volumeTexture, gl_TexCoord[0].stp+vec3(0,0,stepsize)).x;

	normal = sample2-sample1;
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
float LinearizeDepth(float zoverw)
{		
	float near = gl_ProjectionMatrix[2].z; // camera z near	
	float far = gl_ProjectionMatrix[3].z; // camera z far		

	return (-zoverw-near)/(far-near); // will map near..far to 0..1
//	return (2.0 * n) / (f + n - zoverw * (f - n));	
}
float convertZ(float depthBufferValue )
{
	float near = -1024.0; // camera z near	
	float far = 1024.0; // camera z far		

	float clipZ = ( depthBufferValue - 0.5 ) * 2.0;
	return -(2.0 * far * near) / ( clipZ * ( far - near ) - ( far + near ));
}
float readDepth(vec2 coord ) {
	float near = gl_ProjectionMatrix[2].z; // camera z near	
	float far = gl_ProjectionMatrix[3].z; // camera z far		
	return (2.0 *  near) / (far +  near - texture2D( depthTexture, coord ).x * (far - near));	
}
float medianDepth(vec2 texcood)
{
	float stepsize = 1.0/fbosize;
	stepsize *= 2.0;
	vec2 Tinvsize = vec2(stepsize, stepsize);
	vec3 v[6];

	vec2 texcord = vec2(texcood.x, texcood.y);
	vec4 colorOrg = texture2D( depthTexture, texcord);

	v[0] = texture2D(depthTexture, texcord.xy + vec2(-1.0, -1.0) * Tinvsize).xyz;
	v[1] = texture2D(depthTexture, texcord.xy + vec2( 0.0, -1.0) * Tinvsize).xyz;
	v[2] = texture2D(depthTexture, texcord.xy + vec2(+1.0, -1.0) * Tinvsize).xyz;
	v[3] = texture2D(depthTexture, texcord.xy + vec2(-1.0,  0.0) * Tinvsize).xyz;
	v[4] = texture2D(depthTexture, texcord.xy + vec2( 0.0,  0.0) * Tinvsize).xyz;
	v[5] = texture2D(depthTexture, texcord.xy + vec2(+1.0,  0.0) * Tinvsize).xyz;

	// Starting with a subset of size 6, remove the min and max each time
	vec3 temp;
	mnmx6(v[0], v[1], v[2], v[3], v[4], v[5]);
	v[5] = texture2D(depthTexture, texcood.xy + vec2(-1.0, +1.0) * Tinvsize).xyz;
	mnmx5(v[1], v[2], v[3], v[4], v[5]);
	v[5] = texture2D(depthTexture, texcood.xy + vec2( 0.0, +1.0) * Tinvsize).xyz;
	mnmx4(v[2], v[3], v[4], v[5]);
	v[5] = texture2D(depthTexture, texcood.xy + vec2(+1.0, +1.0) * Tinvsize).xyz;
	mnmx3(v[3], v[4], v[5]);

	vec4 filtered = vec4(v[4].x, v[4].y, v[4].z, 1.0);
	return filtered.x;
}
float smoothDepth(vec2 texcoord)
{
	float stepsize = 1.0/fbosize;
	vec4 filtered;
	filtered	= texture2D(depthTexture,  texcoord	+vec2(-stepsize,stepsize))  * kernel[0];
	filtered	+= texture2D(depthTexture, texcoord	+vec2(0, stepsize))  		* kernel[1];
	filtered	+= texture2D(depthTexture, texcoord	+vec2(stepsize, stepsize))  * kernel[2];
	filtered	+= texture2D(depthTexture, texcoord	+vec2(-stepsize,0))  		* kernel[3];
	filtered	+= texture2D(depthTexture, texcoord)							* kernel[4];
	filtered	+= texture2D(depthTexture, texcoord	+vec2(stepsize,0))   		* kernel[5];
	filtered	+= texture2D(depthTexture, texcoord	+vec2(-stepsize,-stepsize)) * kernel[6];
	filtered	+= texture2D(depthTexture, texcoord	+vec2(0,-stepsize))  		* kernel[7];
	filtered	+= texture2D(depthTexture, texcoord	+vec2(stepsize,-stepsize))  * kernel[8];
	filtered /= kernel_norm;
	return filtered.x;
}
float mylength(vec3 v)
{
	float l = v.x*v.x+v.y*v.y+v.z*v.z;
	l = sqrt(l);
	return l;
}
float dist_Point_to_Segment( vec3 point, vec3 segV0, vec3 segV1)
{
    vec3 v = segV1 - segV0;
    vec3 w = point - segV0;

    float c1 = dot(w,v);
    if ( c1 <= 0.0 ) return d(point, segV0);

    float c2 = dot(v,v);
    if ( c2 <= c1 ) return d(point, segV1);

    float b = c1 / c2;
    vec3 Pb = segV0 + b * v;

    return d(point, Pb);
}
void main()
{		
	//get the texture rgb data
	float texValue = texture3D(volumeTexture, gl_TexCoord[0].stp).x;
	//if(texValue==0.0) discard;
	//float lookup_offset = (1.0/1024.0/2.0);
	//texValue += lookup_offset;

	//get lookup table values
	//r,g,b,ra,ga,ba
	vec3 RGBlookupValue, RAGABAlookupValue;
	RGBlookupValue = RAGABAlookupValue = vec3(0.0);
	vec4 finalColour = vec4(0.0);
	
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
	}
	
	if(finalColour.w==0.0)
	{
		discard;
		return;
	}
	
	if(depthpeal==0)
	{
		finalColour.xyz = normalize(eval_normal());
	}
	else
	{
		float volumeres = fbosize;
		vec2 tc = vec2(gl_FragCoord)/vec2(volumeres, volumeres);
		float offset = depthoffset/20.0;
		float slice_size = depththickness/20.0;

		float depthtex = texture2D(depthTexture, tc).x;
	//	float depthtex = smoothDepth(tc);
	//	float depthtex = medianDepth(tc);

		vec3 dir = texture2D(normalTexture, tc).xyz;
		//gl_TextureMatrix //gl_ModelViewMatrix
		//dir = (gl_ModelViewMatrix*vec4(dir, 1.0)).xyz;
		//dir = vec3(0,0,-1);
		
		vec3 vd0 = vec3(gl_FragCoord.x, gl_FragCoord.y, depthtex+offset);

		vec3 vd1dir = (slice_size)*dir;
		vec3 vd1 = vd0+vd1dir;

		vec3 vd2 = vec3(gl_FragCoord.x, gl_FragCoord.y, gl_FragCoord.z);

		
		//same as taking z as we are z aligned with out view rays
		//but without differentiating front and back of reference point
		/*vec3 dvec0 = vd0-vd1;
		vec3 dvec1 = vd0-vd2;
		float lv0 = mylength(dvec0);
		float lv1 = mylength(dvec1);*/
		//if(lv1<lv0)
		
		
		//if(dist_Point_to_Segment(vd2, vd1, vd0)==0.0)
		//if(vd2.x<vd1.x && vd2.y<vd1.y && vd2.z<vd1.z &&
		//   vd2.x>vd0.x && vd2.y>vd0.y && vd2.z>vd0.z)
		if(vd2.z<vd1.z && vd2.z>vd0.z )
		{
			/*vec3 N = eval_normal();
			float ld = length(N);
			N = normalize(N);
			vec3 shading = shading_func(N,V1,L);
			finalColour.xyz += shading*0.5;*/
		}
		else
		{
			discard;
			return;
		}
	}
	//finalColour.w = 255;
	//assing the colour to our final colour
	gl_FragColor = finalColour;	
}