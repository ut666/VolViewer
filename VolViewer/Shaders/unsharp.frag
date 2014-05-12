varying vec2 TexCoord;				//current texcoord

//uniform sampler2D depth_texture;
uniform sampler2D scene_texture;

uniform float	texwidth;			//texture_width
uniform float	kernel[9];			// kernel
uniform float	kernel_norm;		// kernel normalisation
uniform float	postprocess2d_amount; //amount of filter to add in
//uniform vec2 camerarange;

/*float readDepth( in vec2 coord ) {
	return (2.0 * camerarange.x) / (camerarange.y + camerarange.x - texture2D( depth_texture, coord ).x * (camerarange.y - camerarange.x));	
}*/

void main()
{
	float stepsize	= 1.0/texwidth;
	
	//image color
	vec4 originalcolor = texture2D(scene_texture,TexCoord);
	
	//apply our kernel
	vec4 finalcolor;
	finalcolor	= texture2D(scene_texture,TexCoord+vec2(-stepsize,stepsize))  * kernel[0];
	finalcolor	+= texture2D(scene_texture,TexCoord+vec2(0, stepsize))  * kernel[1];
	finalcolor	+= texture2D(scene_texture,TexCoord+vec2(stepsize, stepsize))  * kernel[2];
	finalcolor	+= texture2D(scene_texture,TexCoord+vec2(-stepsize,0))  * kernel[3];
	finalcolor	+= texture2D(scene_texture,TexCoord)* kernel[4];
	finalcolor	+= texture2D(scene_texture,TexCoord+vec2(stepsize,0))   * kernel[5];
	finalcolor	+= texture2D(scene_texture,TexCoord+vec2(-stepsize,-stepsize)) * kernel[6];
	finalcolor	+= texture2D(scene_texture,TexCoord+vec2(0,-stepsize))  * kernel[7];
	finalcolor	+= texture2D(scene_texture,TexCoord+vec2(stepsize,-stepsize))  * kernel[8];
	finalcolor /= kernel_norm;

	//convert to luminance to avoid colour shifts on rgb images
	float orig = (originalcolor.x + originalcolor.y + originalcolor.z)/3.0;
	float fin = (finalcolor.x + finalcolor.y + finalcolor.z)/3.0;
	
	//mask
	vec4 maskcolor = vec4(orig-fin);
	
	vec4 mask = postprocess2d_amount*maskcolor;

	//add them 
	finalcolor = originalcolor+mask;

	//set alpha to 1.0
	finalcolor.w = 1.0;
	
	gl_FragColor = finalcolor;
}