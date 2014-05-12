/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.translate.xt
 */
#include "VolumeRender.h"

VolumeRender::VolumeRender()
{
	cursor3D_screenspace = Vector(-128,-128,-128); 
	cursor3D_texturespace = Vector(0,0,0);
	editor = NULL;
	volshader_id=SHADER_NOLIGHT;
	fbo_depth=0;
	curveXang=curveYang=0.0;
	curve_coords=true;
	detphpeal=false;
	gldrawbuffer="GL_BACK";
}
VolumeRender::~VolumeRender()
{
	editor = NULL;
	if(texNames1) glDeleteTextures(volobj->texdepth,texNames1);
	if(texNames2) glDeleteTextures(volobj->texheight,texNames2);
	if(texNames3) glDeleteTextures(volobj->texwidth,texNames3);
	if(texNames1!=NULL) delete[] texNames1;
	if(texNames2!=NULL) delete[] texNames2;
	if(texNames3!=NULL) delete[] texNames3;

	QThreadPool* threadpool = new QThreadPool(NULL);
	threadpool->globalInstance()->waitForDone();
}
//=================================================================================================================================
void VolumeRender::initializeLeft_DrawBufferGL()
{
	if(gldrawbuffer=="GL_FRONT")
	{
		glDrawBuffer(GL_FRONT_LEFT);
	}
	else if(gldrawbuffer=="GL_BACK")
	{
		glDrawBuffer(GL_BACK_LEFT);
	}
	else if(gldrawbuffer=="GL_FRONT_AND_BACK")
	{
		printf("WARNING: no support for GL_FRONT_AND_BACK in Stereo mode\n");
	}
}
void VolumeRender::initializeRight_DrawBufferGL()
{
	if(gldrawbuffer=="GL_FRONT")
	{
		glDrawBuffer(GL_FRONT_RIGHT);
	}
	else if(gldrawbuffer=="GL_BACK")
	{
		glDrawBuffer(GL_BACK_RIGHT);
	}
	else if(gldrawbuffer=="GL_FRONT_AND_BACK")
	{
		printf("WARNING: no support for GL_FRONT_AND_BACK in Stereo mode\n");
	}
}
void VolumeRender::initialize_DrawBufferGL()
{
	if(gldrawbuffer=="GL_FRONT")
	{
		glDrawBuffer(GL_FRONT);
	}
	else if(gldrawbuffer=="GL_BACK")
	{
		glDrawBuffer(GL_BACK);
	}
	else if(gldrawbuffer=="GL_FRONT_AND_BACK")
	{
		glDrawBuffer(GL_FRONT_AND_BACK);
	}
}
void VolumeRender::initializeGL()
{
	//initialise our variavles
	initvariables();	

	initialize_DrawBufferGL();

	//now lets initialise opengl states
	//===================================
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);  // we prefer good perspective correction
	glShadeModel(GL_SMOOTH);                           // and we prefer goraud-shading
//	glEnable (GL_LINE_SMOOTH);							// Enable Antialiased lines
//	glEnable (GL_POINT_SMOOTH);							// Enable Antialiased lines

	glGenTextures( 1, &SLICEX ); 
	glGenTextures( 1, &SLICEY ); 
	glGenTextures( 1, &SLICEZ ); 

	glGenTextures( 1, &volobj->TEXTURE3D_RED ); 
	glGenTextures( 1, &volobj->TEXTURE3D_GREEN ); 
	glGenTextures( 1, &volobj->TEXTURE3D_BLUE ); 
	glGenTextures( 1, &TEXTURE2D_RGB ); 
	glGenTextures( 1, &TEXTURE2D_RAGABA ); 

	glGenTextures( 1, &TEXTURE2D_FBO_FB ); 
	glGenTextures( 1, &TEXTURE2D_FBO_DP ); 
	glGenTextures( 1, &TEXTURE2D_LODFBO_FB ); 
	glGenTextures( 1, &TEXTURE2D_LODFBO_DP ); 

	glGenTextures( 1, &TEXTURE2D_FBO_DEPTH); 
	glGenTextures( 1, &TEXTURE2D_FBO_COL); 

	//check those nasty opengl extensions...
	//and setup extensions for shader stuff
	glextensions.load_GLextension();
	vol_shadersinglepassobj.glext = vol_rgbsectionshaderobj.glext = vol_sectionshaderobj.glext = vol2dmulti_shaderobj.glext = vol2d_shaderobj.glext = lodfbo_object.glext = meshlist.glextensions = fbo_object.glext = postrendering_shaderobj.glext = mesh_shaderobj.glext = vol_shaderobj.glext = &glextensions;

	if(glextensions.fbo_support)
	{
		init_FBO();
	}

	//Turn on z-bruffer test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//turn on some blending
	glEnable(GL_BLEND);
	blend_mode(bldmode);

	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(1.0, 1.0);

	// Set up lighting & shading,
	// ---------------------------------------
	// OpenGL Settings

	// Light model parameters:
	// -------------------------------------------
	// ---------------------------------------
	// OpenGL Settings
  
/*	GLfloat light0_pos[] = {0.0, 128.0, 0.0, 1.0};
	GLfloat light1_pos[] = {0.0, -128.0, 0.0, 1.0};*/
	GLfloat ambientLight[] = { 0.0, 0.0, 0.0, 1.0f };
	GLfloat diffuseLight[] = { 0.5, 0.5, 0.5, 1.0f };
	GLfloat specularLight[] = { 0.25, 0.25, 0.25, 1.0f };
	
/*	GLfloat light0_dir[] = {0.0, -1.0, 0.0, 0.0};
	GLfloat light1_dir[] = {0.0, 1.0, 0.0, 0.0};

	// Assign created components to GL_LIGHT0
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
*/

/*
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight);

    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 20.0);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20.0);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.5);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_dir);
//	aim_light();*/

	//float fullamb[4] = {0.25, 0.25, 0.25, 1.0};

	//light_pos0 = Vector(0,256,0,0);
	//light_dir0 = -light_pos0;

	light_pos0 = Vector(0, 256, 256,0);	
	light_dir0 = -light_pos0;
	light_dir0.normalize();

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	glDisable(GL_LIGHT7);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, &light_pos0.x);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, &light_dir0.x);
    
	Vector light_pos1 = Vector(0, 256, -256, 0);	
	Vector light_dir1 = -light_pos1;
	light_dir1.normalize();

	Vector depthcol = Vector(0,0,0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, &depthcol.x);

/*	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT1, GL_POSITION, &light_pos1.x);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, &light_dir1.x);

    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 15.0);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.5);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.5);
	
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 15.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.5);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.5);
*/
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);

    glDisable(GL_LIGHTING);

	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
	//glCullFace(GL_BACK);

	//set line and point sizes
	glLineWidth(2.0);
	glPointSize(10.0);

//	glPolygonOffset(1, 0.0001f);
//	glEnable(GL_POLYGON_OFFSET_FILL);

	if(glextensions.shader_support)
	{
		//Volume shader
		GLcharARB *VertexShaderSource, *FragmentShaderSource;

		//Mesh shader
		Vector white = Vector(1,1,1);

		mesh_shaderobj.read_shaderSRC(appPath+"/Shaders/perfragment", &VertexShaderSource, &FragmentShaderSource);
		mesh_shaderobj.install_shader(VertexShaderSource, FragmentShaderSource);
		mesh_shaderobj.enable();
		mesh_shaderobj.sendUniform3fv("surfcol",1, &white.x);
		mesh_shaderobj.disable();

/*		
		Vector volrenderview.camerarange = Vector(1,5,1);
		Vector screensize = Vector(512, 512,0);
		
		postrendering_shaderobj.read_shaderSRC(appPath+"/Shaders/ssao", &VertexShaderSource, &FragmentShaderSource);
		postrendering_shaderobj.install_shader(VertexShaderSource, FragmentShaderSource);
		
		postrendering_shaderobj.enable();
		postrendering_shaderobj.sendUniform1i("depth_texture", 0);
		postrendering_shaderobj.sendUniform1i("scene_texture", 1);
		postrendering_shaderobj.sendUniform2fv("volrenderview.camerarange",1, &volrenderview.camerarange.x);
		postrendering_shaderobj.sendUniform2fv("screensize",1, &screensize.x);
		postrendering_shaderobj.disable();
*/
		//Post-process framebuffer shader
		postrendering_shaderobj.read_shaderSRC(appPath+"/Shaders/unsharp", &VertexShaderSource, &FragmentShaderSource);
		postrendering_shaderobj.install_shader(VertexShaderSource, FragmentShaderSource);

		float kernel[9];
		kernel[0] = 1.0;		kernel[1] = 	2.0;	kernel[2] = 1.0; 
		kernel[3] = 2.0;		kernel[4] = 	4.0;	kernel[5] = 2.0; 
		kernel[6] = 1.0;		kernel[7] = 	2.0;	kernel[8] = 1.0; 
//		kernel[0] = -1.0;		kernel[1] = 	-2.0;	kernel[2] = -1.0; 
//		kernel[3] = 0.0;		kernel[4] = 	0.0;	kernel[5] = 0.0; 
//		kernel[6] = 1.0;		kernel[7] = 	2.0;	kernel[8] = 1.0; 
		postprocess2d_amount = 1.0;
		//Vector camerarange = Vector(1,5,1);

		postrendering_shaderobj.enable();
		//postrendering_shaderobj.sendUniform1i("depth_texture", 0);
		postrendering_shaderobj.sendUniform1i("scene_texture", 0);
		if(fbo_lod==1) postrendering_shaderobj.sendUniform1f("texwidth", render_size);
		else postrendering_shaderobj.sendUniform1f("texwidth", FBO_SIZE);
		postrendering_shaderobj.sendUniform1f("postprocess2d_amount", postprocess2d_amount);
		postrendering_shaderobj.sendUniform1f("kernel_norm", 16.0);
		postrendering_shaderobj.sendUniform1fv("kernel", 9, kernel);
		//postrendering_shaderobj.sendUniform2fv("camerarange",1, &camerarange.x);
		postrendering_shaderobj.disable();

		vol2d_shaderobj.read_shaderSRC(appPath+"/Shaders/vol2d", &VertexShaderSource, &FragmentShaderSource);
		vol2d_shaderobj.install_shader(VertexShaderSource, FragmentShaderSource);
		vol2d_shaderobj.enable();
		vol2d_shaderobj.sendUniform1i("Texmap0", 0);
		vol2d_shaderobj.sendUniform1i("RGBlookupTexture", 2);
		vol2d_shaderobj.sendUniform1i("RAGABAlookupTexture", 3);
		vol2d_shaderobj.disable();

		vol2dmulti_shaderobj.read_shaderSRC(appPath+"/Shaders/vol2dtrilin", &VertexShaderSource, &FragmentShaderSource);
		vol2dmulti_shaderobj.install_shader(VertexShaderSource, FragmentShaderSource);
		vol2dmulti_shaderobj.enable();
		vol2dmulti_shaderobj.sendUniform1i("Texmap0", 0);
		vol2dmulti_shaderobj.sendUniform1i("Texmap1", 1);
		vol2dmulti_shaderobj.sendUniform1i("RGBlookupTexture", 2);
		vol2dmulti_shaderobj.sendUniform1i("RAGABAlookupTexture", 3);
		vol2dmulti_shaderobj.disable();

		vol_sectionshaderobj.read_shaderSRC(appPath+"/Shaders/volume_sections", &VertexShaderSource, &FragmentShaderSource);
		vol_sectionshaderobj.install_shader(VertexShaderSource, FragmentShaderSource);
		vol_sectionshaderobj.enable();
		vol_sectionshaderobj.sendUniform1i("volumeTextureR", 0);
		vol_sectionshaderobj.sendUniform1i("volumeTextureG", 1);
		vol_sectionshaderobj.sendUniform1i("volumeTextureB", 2);
		vol_sectionshaderobj.sendUniform1i("RGBlookupTexture", 3);
		vol_sectionshaderobj.disable();

		vol_rgbsectionshaderobj.read_shaderSRC(appPath+"/Shaders/volume_allchannelsNOLIGHT", &VertexShaderSource, &FragmentShaderSource);
		vol_rgbsectionshaderobj.install_shader(VertexShaderSource, FragmentShaderSource);		
		vol_rgbsectionshaderobj.enable();
		vol_rgbsectionshaderobj.sendUniform1i("volumeTexture", 0);
		vol_rgbsectionshaderobj.sendUniform1i("RGBlookupTexture", 1);
		vol_rgbsectionshaderobj.sendUniform1i("RAGABAlookupTexture", 2);
		vol_rgbsectionshaderobj.disable();

		vol_shadersinglepassobj.read_shaderSRC(appPath+"/Shaders/volume_allchannelsSINGPASS_NOLIGHT", &VertexShaderSource, &FragmentShaderSource);
		vol_shadersinglepassobj.install_shader(VertexShaderSource, FragmentShaderSource);
		vol_shadersinglepassobj.enable();
		vol_shadersinglepassobj.sendUniform1i("volumeTextureR", 0);
		vol_shadersinglepassobj.sendUniform1i("volumeTextureG", 1);
		vol_shadersinglepassobj.sendUniform1i("volumeTextureB", 2);
		vol_shadersinglepassobj.sendUniform1i("RGBlookupTexture", 3);
		vol_shadersinglepassobj.disable();

		volshader_event(volshader_id);
	}
}
void VolumeRender::resizeGL( int w, int h )
{
    glViewport( 0, 0, (GLint)w, (GLint)h );
	glGetIntegerv(GL_VIEWPORT, nonfbo_viewport);

	glMatrixMode( GL_PROJECTION );
    glLoadIdentity();     

	if(h==0) h=1;

	volrenderview.camera.width = w;
	volrenderview.camera.height = h;

	if(volrenderview.projection_toggle==PERSP_PROJECTION)
		gluPerspective(volrenderview.camera.fov, (float)volrenderview.camera.height/(float)volrenderview.camera.width, volrenderview.camera.znear, volrenderview.camera.zfar);	
	else
		glOrtho(-volobj->maxres+volrenderview.zoom, volobj->maxres+volrenderview.zoom, -volobj->maxres+volrenderview.zoom, volobj->maxres+volrenderview.zoom, -1024, 1024);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

 	//update bounds for volrenderview.arcball transform
	volrenderview.arcball.setBounds((GLfloat)w, (GLfloat)h);
	volrenderview.winWidth = w;
    volrenderview.winHeight = h;

	for(int i=0; i<6; i++)	volrenderview.cutting_planes.planes[i].arcball.setBounds((GLfloat)w, (GLfloat)h);
//	if(volrenderview.winWidth>volrenderview.winHeight) winMaxDim = volrenderview.winWidth;
//	else winMaxDim = volrenderview.winHeight;

	build_histogram();
}
void VolumeRender::displayGL(int render)
{
	/*if(offscreen_toggle==0)
	{
  		glClearColor( background_colour.red()/255.0, background_colour.green()/255.0, background_colour.blue()/255.0, 1.0f );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	}*/

	if(stereo_toggle)
	{
		render_stereo(render);
	}
	else
	{
		//glDrawBuffer(GL_FRONT_AND_BACK);
		//glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		//glDrawBuffer(GL_BACK);

		if(!volobj->texture3d)
		{
			LOD_FBO_SIZE = FBO_SIZE = 1024;
			init_FBO();
			postprocess2d_shader();
		}

		render_scene(render);
	}

	//glFinish();
}
//=================================================================================================================================
void VolumeRender::auto_zoom(void)
{
/*	float maxres =  max(volobj->texwidth*(volobj->xsize), volobj->texheight*(volobj->ysize));
	float offset = ((volrenderview.winWidth/volrenderview.winHeight)*maxres)/2.0f;

	maxres = max(maxres, volobj->texdepth*(volobj->zsize));
	maxres += offset;	
	maxres /= 2.0;	

	volrenderview.zoom = 0;
*/
//	glScaled((double)volobj->maxres/(double)volobj->texwidth, (double)volobj->maxres/(double)volobj->texheight, (double)volobj->maxres/(double)volobj->texdepth);
//	glScaled(1.0/((double)volobj->xsize/(double)volobj->maxsize), 1.0/((double)volobj->ysize/(double)volobj->maxsize), 1.0/((double)volobj->zsize/(double)volobj->maxsize));

}
void VolumeRender::initvariables()
{
	//timer.init();
	cam_step_size = 5;

	depthdata = 0;
	pealdepthdata = 0;
	coldata =  0;

	detphpeal  = false;
	depthoffset = 0.0;
	depththickness = 0.0;
	olddepthoffset = 0.0;
	olddepththickness = 0.0;

	vbo_array = 0;
	vbo_indices = 0;
	vbo_indices_size = 0;
	vbo_vert_size = 0;
	vertexVBO_ID = -1;
	indiciesVBO_ID = -1;

	splane[0] = 1.f/255.f;
	splane[1] = 0.f;
	splane[2] = 0.f;
	splane[3] = .5f;

	rplane[0] = 0.f;
	rplane[1] = 1.f/255.f;
	rplane[2] = 0.f;
	rplane[3] = .5f;

	tplane[0] = 0.f;
	tplane[1] = 0.f;
	tplane[2] = 1.f/255.f;
	tplane[3] = .5f;

	max_slices = 1;
	fillSlices = 1;

	LOD_FBO_SIZE = FBO_SIZE = 1024;
	fbo_lod = 0;
	PostProcess_toggle = 1;

	//define a cube by 8 vertices
	volrenderview.bb_v1 = Vector(-1.0,  1.0,  1.0);
	volrenderview.bb_v2 = Vector( 1.0,  1.0,  1.0);	
	volrenderview.bb_v3 = Vector(-1.0, -1.0,  1.0);
	volrenderview.bb_v4 = Vector( 1.0, -1.0,  1.0);
	volrenderview.bb_v5 = Vector(-1.0,  1.0, -1.0);
	volrenderview.bb_v6 = Vector( 1.0,  1.0, -1.0);
	volrenderview.bb_v7 = Vector(-1.0, -1.0, -1.0);
	volrenderview.bb_v8 = Vector( 1.0, -1.0, -1.0);

	Vector bb_size = Vector (256.0,256.0,256.0);
	bb_size /= 2.0;

	volrenderview.bb_v1=volrenderview.bb_v1*bb_size;
	volrenderview.bb_v2=volrenderview.bb_v2*bb_size;
	volrenderview.bb_v3=volrenderview.bb_v3*bb_size;
	volrenderview.bb_v4=volrenderview.bb_v4*bb_size;
	volrenderview.bb_v5=volrenderview.bb_v5*bb_size;
	volrenderview.bb_v6=volrenderview.bb_v6*bb_size;
	volrenderview.bb_v7=volrenderview.bb_v7*bb_size;
	volrenderview.bb_v8=volrenderview.bb_v8*bb_size;

	//0 for perspective, 1 for ortho
	//volrenderview.projection_toggle = ORTHO_PROJECTION;

	lights_toggle = 0;
	measure_toggle = 1;
	offscreen_toggle = 0;

	volrenderview.redclip_toggle = volrenderview.blueclip_toggle = volrenderview.greenclip_toggle = isoclip_toggle = 1;

	volrenderview.cutting_planes.init(256.0);

	cursor3D = "(0,0,0)";

	bldmode = 1;
	loaded = 0;
	
	for(int i=0; i<256; i++)
	{
		rgb_tfunc[3*i+0] = i;
		rgb_tfunc[3*i+1] = i;
		rgb_tfunc[3*i+2] = i;
		ragaba_tfunc[3*i+0] = i;
		ragaba_tfunc[3*i+1] = i;
		ragaba_tfunc[3*i+2] = i;

		backup_rgb_tfunc[3*i+0] = i;
		backup_rgb_tfunc[3*i+1] = i;
		backup_rgb_tfunc[3*i+2] = i;
		backup_ragaba_tfunc[3*i+0] = i;
		backup_ragaba_tfunc[3*i+1] = i;
		backup_ragaba_tfunc[3*i+2] = i;
	}

	//thresholding
	volrenderview.luminance.reset();
	volrenderview.alpha.reset();

	volrenderview.redluminance.reset();
	volrenderview.redalpha.reset();
	volrenderview.greenluminance.reset();
	volrenderview.greenalpha.reset();
	volrenderview.blueluminance.reset();
	volrenderview.bluealpha.reset();

	volshader_id = SHADER_NOLIGHT;

	lighting_enable = 0;
	light_radius = 512.0;
	light_altitude = 0.0;
	light_azimuth  = 0.0;

	silhouette_enable = 1;
	silhouette_amount = 0.5;

	depthcue = 0.0;

	gooch_enable = 0;
	gooch_toneCont = 0.5;
	gooch_warmCol[0] = 0.6;
	gooch_warmCol[1] = 0.6;
	gooch_warmCol[2] = 0.0;
	gooch_coolCol[0] = 0.0;
	gooch_coolCol[1] = 0.0;
	gooch_coolCol[2] = 0.6;

	goochalpha = 1.0;
	goochdegen = 0.0;

	r_channel = 1.0;
	g_channel = 1.0;
	b_channel= 1.0;
	isosurface = 1.0;

	numbslices = 512;
	slices = 512;
	sliceslod = 128;
	SlicesmoveLOD_toggle = 1;

	texNames1 = texNames2 = texNames3 = 0;
	for(int i=0; i<6; i++) volrenderview.plane_rendermode[i] = 0;

	updatehisto=0;

	goochwarm = Vector(1.0,1.0,0.0);
	goochcool = Vector(0.0,0.0,1.0);
	goochcontrib = 1.0;
	goochexp = 4.0;

	current_measure_index = -1;
	current_measure_vertindex = -1;
	current_measure_edgeindex = -1;
	current_measure_faceindex = -1;

	current_measure_drawmode = MEASURE_DRAWMODE_NONE;
	current_measure_editmode = MEASURE_EDIT_NONE;
	current_measure_viewmode = MEASURE_VIEW_NONE;

	measure_magnetline_vert = -1;
	measure_magnetline_vert_old = -1;

	measure_editmode = 0;
	measure_modify=0;
	measure_mode = 0;
	measure_dragmode = 0;

	measure_vertlabel = 0;
	measure_edgelabel = 0;
	measure_facelabel = 0;
	measure_arrowlabel = 0;
	measure_labelslabel = 0;
	measure_listslabel = 0;
	measure_tris = 1;
	measure_facehover = false;
	measure_face_invert = false;
	measure_section_toggle = false;
	measure_updateview_toggle = true;
	measure_section_thickn = 10;
	measure_section_trans = 0;

	measure_magnetline_destvert = -1;
	measure_magnetline_helper = -1;
	measure_current_facemode=0;

	measure_selection_window = 0.01;
	measure_arrowhead_size = 1.5;
	measure_active_line_size = 20.0;
	measure_active_point_size = 20.0;
	measure_line_size = 6.0;
	measure_point_size = 12.0;
	measure_active_face_col = Vector(0.55, 0.35, 0.35);
	measure_active_edge_col = Vector(0.75, 0.55, 0.55);
	measure_active_vert_col = Vector(0.95, 0.75, 0.75);
	measure_face_col = Vector(0.35, 0.35, 0.35);
	measure_edge_col = Vector(0.55, 0.55, 0.55);
	measure_vert_col = Vector(0.75, 0.75, 0.75);
	measure_selected_col = Vector(1.0, 1.0, 0.0);
	measure_magnet_col = Vector(1.0, 0.0, 0.0);

	crop_inv=false;
	crop_shape_selected=-1;

	Vector rgbcol, hsvcol, comphsv;
	int numbcols=100;

	for(int i=0; i<numbcols; i++)
	{
		hsvcol.x = randomFloat(0.0, 360.0);//360.0f*((float)i/(float)numbcols);
		hsvcol.y = randomFloat(0.5, 1.0);
		hsvcol.z = randomFloat(0.5, 1.0);

		//printf("hsvcol: %f, %f, %f\n", hsvcol.x, hsvcol.y, hsvcol.z);

		comphsv = hsvcol;
		//if(i%2==0) CompHSV(hsvcol, &comphsv);
		HSVtoRGB(comphsv, &rgbcol);

		//printf("rgbcol: %f, %f, %f\n", rgbcol.x, rgbcol.y, rgbcol.z);
//		measure_colours.push_back(rgbcol);
	}
}

void VolumeRender::init_FBO(void)
{
		//glDeleteTextures(1, TEXTURE2D_FBO_FB);
		glBindTexture (GL_TEXTURE_2D, TEXTURE2D_FBO_FB); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, FBO_SIZE, FBO_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		//glDeleteTextures(1, TEXTURE2D_FBO_DP);
		glBindTexture (GL_TEXTURE_2D, TEXTURE2D_FBO_DP); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, FBO_SIZE, FBO_SIZE, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		fbo_object.initialize(FBO_SIZE, FBO_SIZE, GL_RGBA, TEXTURE2D_FBO_FB, TEXTURE2D_FBO_DP);
		fbo_object.stop();

		glBindTexture (GL_TEXTURE_2D, TEXTURE2D_LODFBO_FB); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, LOD_FBO_SIZE, LOD_FBO_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glBindTexture (GL_TEXTURE_2D, TEXTURE2D_LODFBO_DP); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, LOD_FBO_SIZE, LOD_FBO_SIZE, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		lodfbo_object.initialize(LOD_FBO_SIZE, LOD_FBO_SIZE, GL_RGBA, TEXTURE2D_LODFBO_FB, TEXTURE2D_LODFBO_DP);
		lodfbo_object.stop();

		if(depthdata) delete[] depthdata;
		depthdata = new unsigned int[FBO_SIZE*FBO_SIZE];

		if(pealdepthdata) delete[] pealdepthdata;
		pealdepthdata = new unsigned int[FBO_SIZE*FBO_SIZE];

		if(coldata) delete[] coldata;
		coldata = new unsigned char[4*FBO_SIZE*FBO_SIZE];
}

void VolumeRender::init_3dTexture_bricks(int subdivs)
{
	printf("init_3dTexture_bricks()\n");

	int brick_subdivs=BRICKS_SUBDIV;
	int number_of_bricks = (int)pow((float)BRICKS_SUBDIV,3);
	PlaneWidgetInCube plane;

	//work out x,y,z chunks we can divide data into
	float brick_xres = volobj->texwidth/(float)brick_subdivs;
	float brick_yres = volobj->texheight/(float)brick_subdivs;
	float brick_zres = volobj->texdepth/(float)brick_subdivs;

	if(glextensions.pbo_support)
	{
		printf("Initialising Pixel Buffer Object...\n");

		int datasize = brick_xres*brick_yres*brick_zres;

		glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);
		glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
		glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
		glextensions.glTexImage3D(GL_TEXTURE_3D, 0,  GL_LUMINANCE,
			brick_xres, brick_yres, brick_zres,
			0,
			GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);

		glextensions.glGenBuffersARB(1, &TEXTURE3D_PBO);
        glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, TEXTURE3D_PBO);
        glextensions.glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, datasize, 0, GL_STREAM_DRAW_ARB);
        glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	}

	printf("Number of bricks: %d\n", number_of_bricks);
	printf("Brick Resolutions: %d x %d x %d\n", (int)brick_xres, (int)brick_yres, (int)brick_zres);

	//allocate some memory for our brick data
	brick_textures.resize(number_of_bricks);
	brick_centres.resize(number_of_bricks);
	empty_brick.resize(number_of_bricks);

	float vert_res = (volobj->maxres/BRICKS_SUBDIV);

	for(int n=0; n<number_of_bricks; n++)
	{
		empty_brick[n] = 0;	
	}

	//now build up the brick textures
	float x_iter, y_iter, z_iter;
	int helper=0;
	int index_3D=0;
	int brick_index_3D=0;
	float r,g,b,a;
	int isempty=0;
	int empty_bricks;
	empty_bricks=0;
	float maxsize;

	printf("Building Bricks \n");

	QStringList strlst = volio->load_subimage_strngList();
	QThreadPool* threadpool = new QThreadPool(NULL);
	threadpool->setMaxThreadCount(2);
	for(int i=0; i<brick_subdivs; i++)
	{
		x_iter = brick_xres*i;

		for(int j=0; j<brick_subdivs; j++)
		{
			y_iter = brick_yres*j;
	
			for(int k=0; k<brick_subdivs; k++)
			{
				z_iter = brick_zres*k;

				brick_textures[helper].resize((brick_xres)*(brick_yres)*(brick_zres)*1);
		
				isempty = 1;

				//empty_brick[helper] = 
				//volio->load_subimage(strlst, x_iter, y_iter, z_iter, brick_xres, brick_yres, brick_zres, &brick_textures[helper][0]);

				QtThreadedBrickLoader* threadloader = new QtThreadedBrickLoader(helper, volio, strlst, x_iter, y_iter, z_iter, brick_xres, brick_yres, brick_zres, &brick_textures[helper], &empty_brick);
				threadpool->globalInstance()->start(threadloader); //QThread::TimeCriticalPriority
				//empty_brick[helper] = false;

				brick_centres[helper] = Vector((x_iter+x_iter+brick_xres)/2.0, (y_iter+y_iter+brick_yres)/2.0, (z_iter+z_iter+brick_zres)/2.0, helper);
				
				brick_centres[helper].x /= (float)volobj->texwidth;
				brick_centres[helper].y /= (float)volobj->texheight;
				brick_centres[helper].z /= (float)volobj->texdepth;

				brick_centres[helper].x -= 0.5;
				brick_centres[helper].y -= 0.5;
				brick_centres[helper].z -= 0.5;

				brick_centres[helper].x *= (float)volobj->texwidth;
				brick_centres[helper].y *= (float)volobj->texheight;
				brick_centres[helper].z *= (float)volobj->texdepth;

				maxsize =  max((float)volobj->xsize, (float)volobj->ysize);
				maxsize =  max((float)maxsize, (float)volobj->zsize);

				brick_centres[helper].x *= (volobj->xsize/maxsize);
				brick_centres[helper].y *= (volobj->ysize/maxsize);
				brick_centres[helper].z *= (volobj->zsize/maxsize);
				
				helper++;
			}
		}
	}

	printf("Bricks: %d / %d\n", number_of_bricks-empty_bricks, number_of_bricks);
	//sort(brick_centres.begin(), brick_centres.end());
	loaded=1;
}
//=================================================================================================================================
void VolumeRender::silhouettes_shader(void)
{
	if(glextensions.shader_support==1 && volshader_id!=SHADER_NOLIGHT)
	{
	/*	vol_shaderobj.enable();
		vol_shaderobj.sendUniform1i("silhouette_amount", silhouette_amount);
		vol_shaderobj.disable();*/
	}
}
void VolumeRender::postprocess2d_shader(void)
{
	if(glextensions.shader_support==1)
	{
		postrendering_shaderobj.enable();
		postrendering_shaderobj.sendUniform1f("postprocess2d_amount", postprocess2d_amount);
		if(fbo_lod==1) postrendering_shaderobj.sendUniform1f("texwidth", LOD_FBO_SIZE);
		else postrendering_shaderobj.sendUniform1f("texwidth", FBO_SIZE);
		postrendering_shaderobj.disable();
	}
}
void VolumeRender::silhouette_shader(void)
{
	if(glextensions.shader_support==1 && volshader_id!=SHADER_NOLIGHT)
	{
	/*	vol_shaderobj.enable();
		vol_shaderobj.sendUniform1f("silhouette_amount", silhouette_amount);
		vol_shaderobj.disable();*/
	}
}
void VolumeRender::volshader_update_eye(void)
{
	/*if(volshader_id==SHADER_PHOTOREAL)
	{
		vol_shaderobj.enable();
		vol_shaderobj.sendUniform3fv("eye",1, &volrenderview.eye.x);
		vol_shaderobj.disable();
	}
	else if(volshader_id==SHADER_NONPHOTOREAL)
	{
		vol_shaderobj.enable();
		vol_shaderobj.sendUniform3fv("eye",1, &volrenderview.eye.x);
		vol_shaderobj.disable();
	}*/
}
void VolumeRender::volshader_event(int id)
{
	volshader_id = id;

//	printf("ID: %d\n", id);

	printf("volshader_event\n");

	GLcharARB *VertexShaderSource, *FragmentShaderSource;
	
	//"Multi Channel NOLIGHT", 
	//"Multi Channel LAMBERTIAN", 
	//"Multi Channel BLING-PHONG", 
	//"Multi Channel GOOCH", 
	
	//"Single Channel NOLIGHT", 
	//"Single Channel FULL", 
	//"Multi Channel NOLIGHT", 
	//"Multi Channel LAMBERTIAN", 
	//"Multi Channel BLING-PHONG", 
	//"Multi Channel COOK-TORRANCE", 
	//"Multi Channel GOOCH", 
	//"Multi Channel BLING-PHONG 2SP", 
	
	if(id==SHADER_NOLIGHT)
	{
		vol_shaderobj.read_shaderSRC(appPath+"/Shaders/volume_allchannelsNOLIGHT", &VertexShaderSource, &FragmentShaderSource);
		vol_shaderobj.install_shader(VertexShaderSource, FragmentShaderSource);
		
		vol_shaderobj.enable();
		vol_shaderobj.sendUniform1i("volumeTexture", 0);
		vol_shaderobj.sendUniform1i("RGBlookupTexture", 1);
		vol_shaderobj.sendUniform1i("RAGABAlookupTexture", 2);
		vol_shaderobj.disable();

		//volshader_id =2;
	}
	else if(id==SHADER_LAMBERT)
	{
		vol_shaderobj.read_shaderSRC(appPath+"/Shaders/volume_allchannelsLAMBERTIAN", &VertexShaderSource, &FragmentShaderSource);
		vol_shaderobj.install_shader(VertexShaderSource, FragmentShaderSource);
		
		vol_shaderobj.enable();
		vol_shaderobj.sendUniform1i("volumeTexture", 0);
		vol_shaderobj.sendUniform1i("RGBlookupTexture", 1);
		vol_shaderobj.sendUniform1i("RAGABAlookupTexture", 2);
		vol_shaderobj.disable();
		//volshader_id =3;
	}
	else if(id==SHADER_PHOTOREAL)
	{
		vol_shaderobj.read_shaderSRC(appPath+"/Shaders/volume_allchannelsBLING-PHONG", &VertexShaderSource, &FragmentShaderSource);
		vol_shaderobj.install_shader(VertexShaderSource, FragmentShaderSource);
		
		vol_shaderobj.enable();
		vol_shaderobj.sendUniform1i("volumeTexture", 0);
		vol_shaderobj.sendUniform1i("RGBlookupTexture", 1);
		vol_shaderobj.sendUniform1i("RAGABAlookupTexture", 2);
		vol_shaderobj.sendUniform1f("maxdimension", volobj->maxres*2.0); 
		vol_shaderobj.sendUniform1f("depthcue", depthcue);
		//vol_shaderobj.sendUniform3fv("eye",1, &volrenderview.eye.x);
		vol_shaderobj.disable();
		
		//volshader_id = 4;
	}
	else if(id==SHADER_NONPHOTOREAL)
	{		
		//
		vol_shaderobj.read_shaderSRC(appPath+"/Shaders/volume_allchannelsGOOCH", &VertexShaderSource, &FragmentShaderSource);
		vol_shaderobj.install_shader(VertexShaderSource, FragmentShaderSource);

		vol_shaderobj.enable();
		vol_shaderobj.sendUniform1i("volumeTexture", 0);
		vol_shaderobj.sendUniform1i("RGBlookupTexture", 1);
		vol_shaderobj.sendUniform1i("RAGABAlookupTexture", 2);
		vol_shaderobj.sendUniform1f("silhCont", goochcontrib);
		vol_shaderobj.sendUniform1f("silhExp", goochexp);
		vol_shaderobj.sendUniform3fv("gooch_warmCol",1, &goochwarm.x);
		vol_shaderobj.sendUniform3fv("gooch_coolCol",1, &goochcool.x);
		vol_shaderobj.sendUniform1f("alphathresh", goochalpha);
		vol_shaderobj.sendUniform1f("degenthresh", goochdegen);
		vol_shaderobj.sendUniform1f("maxdimension", volobj->maxres*2.0); 
		vol_shaderobj.sendUniform1f("depthcue", depthcue);
		//vol_shaderobj.sendUniform3fv("eye",1, &volrenderview.eye.x);
		vol_shaderobj.disable();
		//volshader_id = 6;
	}
	else if(id==SHADER_SEM)
	{
		vol_shaderobj.read_shaderSRC(appPath+"/Shaders/volume_allchannelsSEM", &VertexShaderSource, &FragmentShaderSource);
		vol_shaderobj.install_shader(VertexShaderSource, FragmentShaderSource);

		vol_shaderobj.enable();
		vol_shaderobj.sendUniform1i("volumeTexture", 0);
		vol_shaderobj.sendUniform1i("RGBlookupTexture", 1);
		vol_shaderobj.sendUniform1i("RAGABAlookupTexture", 2);
		vol_shaderobj.sendUniform1f("silhCont", goochcontrib);
		vol_shaderobj.sendUniform1f("silhExp", goochexp);
		vol_shaderobj.sendUniform1f("maxdimension", volobj->maxres*2.0); 
		vol_shaderobj.sendUniform1f("depthcue", depthcue);
		vol_shaderobj.disable();
		//volshader_id = 7;
	}
	else if(id==SHADER_TOON)
	{
		vol_shaderobj.read_shaderSRC(appPath+"/Shaders/volume_allchannelsTOON", &VertexShaderSource, &FragmentShaderSource);
		vol_shaderobj.install_shader(VertexShaderSource, FragmentShaderSource);

		vol_shaderobj.enable();
		vol_shaderobj.sendUniform1i("volumeTexture", 0);
		vol_shaderobj.sendUniform1i("RGBlookupTexture", 1);
		vol_shaderobj.sendUniform1i("RAGABAlookupTexture", 2);
		vol_shaderobj.disable();
		//volshader_id = 8;
	}
	else if(id==SHADER_DEPTH)
	{
		//printf("SHADER DEPTH\n");
		QString sn = appPath+"/Shaders/volume_allchannelsDEPTH";
		printf("loading: %s\n", sn.toLatin1().data());
		vol_shaderobj.read_shaderSRC(appPath+"/Shaders/volume_allchannelsDEPTH", &VertexShaderSource, &FragmentShaderSource);
		vol_shaderobj.install_shader(VertexShaderSource, FragmentShaderSource);
		
		float kernel[9];
		kernel[0] = 1.0;		kernel[1] = 	2.0;	kernel[2] = 1.0; 
		kernel[3] = 2.0;		kernel[4] = 	4.0;	kernel[5] = 2.0; 
		kernel[6] = 1.0;		kernel[7] = 	2.0;	kernel[8] = 1.0; 
		
		detphpeal = true;

		vol_shaderobj.enable();
		vol_shaderobj.sendUniform1i("volumeTexture", 0);
		vol_shaderobj.sendUniform1i("RGBlookupTexture", 1);
		vol_shaderobj.sendUniform1i("RAGABAlookupTexture", 2);
		vol_shaderobj.sendUniform1i("depthTexture", 3);
		vol_shaderobj.sendUniform1i("normalTexture", 4);
		vol_shaderobj.sendUniform1f("fbosize", 128.0);	
		vol_shaderobj.sendUniform1f("depthoffset", depthoffset);	
		vol_shaderobj.sendUniform1f("depththickness", depththickness);
		vol_shaderobj.sendUniform1f("depthpeal", detphpeal);
 		//vol_shaderobj.sendUniform1f("kernel_norm", 16.0);
		//vol_shaderobj.sendUniform1fv("kernel", 9, kernel);
		vol_shaderobj.disable();
	}

}
void VolumeRender::shaderDepthCueEvent(Vector col, float contrib)
{
	depthcue = contrib;
	glLightfv(GL_LIGHT1, GL_AMBIENT, &col.x);

	if(volshader_id==SHADER_PHOTOREAL)
	{
		vol_shaderobj.enable();
		vol_shaderobj.sendUniform1i("volumeTexture", 0);
		vol_shaderobj.sendUniform1i("RGBlookupTexture", 1);
		vol_shaderobj.sendUniform1i("RAGABAlookupTexture", 2);
		vol_shaderobj.sendUniform1f("maxdimension", volobj->maxres*2.0); 
		vol_shaderobj.sendUniform1f("depthcue", depthcue);
		vol_shaderobj.disable();
	}
	else if(volshader_id==SHADER_NONPHOTOREAL)
	{
		vol_shaderobj.enable();
		vol_shaderobj.sendUniform1i("volumeTexture", 0);
		vol_shaderobj.sendUniform1i("RGBlookupTexture", 1);
		vol_shaderobj.sendUniform1i("RAGABAlookupTexture", 2);
		vol_shaderobj.sendUniform1f("silhCont", goochcontrib);
		vol_shaderobj.sendUniform1f("silhExp", goochexp);
		vol_shaderobj.sendUniform1f("alphathresh", goochalpha);
		vol_shaderobj.sendUniform1f("degenthresh", goochdegen);
		vol_shaderobj.sendUniform3fv("gooch_warmCol",1, &goochwarm.x);
		vol_shaderobj.sendUniform3fv("gooch_coolCol",1, &goochcool.x);
		vol_shaderobj.sendUniform1f("maxdimension", volobj->maxres*2.0); 
		vol_shaderobj.sendUniform1f("depthcue", depthcue);
		vol_shaderobj.disable();
	}
	else if(volshader_id==SHADER_SEM)
	{
		vol_shaderobj.enable();
		vol_shaderobj.sendUniform1i("volumeTexture", 0);
		vol_shaderobj.sendUniform1i("RGBlookupTexture", 1);
		vol_shaderobj.sendUniform1i("RAGABAlookupTexture", 2);
		vol_shaderobj.sendUniform1f("silhCont", goochcontrib);
		vol_shaderobj.sendUniform1f("silhExp", goochexp);
		vol_shaderobj.sendUniform1f("maxdimension", volobj->maxres*2.0); 
		vol_shaderobj.sendUniform1f("depthcue", depthcue);
		vol_shaderobj.disable();
	}
}

void VolumeRender::shaderGoochEvent(Vector warm, Vector cool, int contrib, int expon, int alpha, int degen)
{
//	printf("%d\n",volshader_id);

	if(volshader_id==SHADER_NONPHOTOREAL)
	{
		goochwarm = warm;
		goochcool = cool;
		goochcontrib = contrib/2.0;
		goochexp = expon;
		goochalpha = (float)alpha/10.0;
		goochdegen = (float)degen/100.0;
		
		vol_shaderobj.enable();
		vol_shaderobj.sendUniform1i("volumeTexture", 0);
		vol_shaderobj.sendUniform1i("RGBlookupTexture", 1);
		vol_shaderobj.sendUniform1i("RAGABAlookupTexture", 2);
		vol_shaderobj.sendUniform1f("silhCont", goochcontrib);
		vol_shaderobj.sendUniform1f("silhExp", goochexp);
		vol_shaderobj.sendUniform1f("alphathresh", goochalpha);
		vol_shaderobj.sendUniform1f("degenthresh", goochdegen);
		vol_shaderobj.sendUniform3fv("gooch_warmCol",1, &goochwarm.x);
		vol_shaderobj.sendUniform3fv("gooch_coolCol",1, &goochcool.x);
		vol_shaderobj.sendUniform1f("maxdimension", volobj->maxres*2.0); 
		vol_shaderobj.sendUniform1f("depthcue", depthcue);
		vol_shaderobj.disable();
	}
	else if(volshader_id==SHADER_SEM)
	{
		goochcontrib = contrib;
		goochexp = expon;

		vol_shaderobj.enable();
		vol_shaderobj.sendUniform1i("volumeTexture", 0);
		vol_shaderobj.sendUniform1i("RGBlookupTexture", 1);
		vol_shaderobj.sendUniform1i("RAGABAlookupTexture", 2);
		vol_shaderobj.sendUniform1f("silhCont", goochcontrib);
		vol_shaderobj.sendUniform1f("silhExp", goochexp);
		vol_shaderobj.disable();
	}

}
//=================================================================================================================================
void VolumeRender::aim_light(void)
{
	glPushMatrix();

//	glTranslatef(tx, ty, 0.0);
//	glMultMatrixf(volrenderview.arcball.Transform.M);		

//	glLightfv(GL_LIGHT1, GL_POSITION, &light_pos0.x);
//	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, &light_dir0.x);

	if(volrenderview.projection_toggle==PERSP_PROJECTION)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMultMatrixf(volrenderview.arcball.Transform.M);		
		glTranslatef(-volrenderview.eye.x, -volrenderview.eye.y, -volrenderview.eye.z);
	}

	light_pos0 = Vector(0,0,light_radius);
	light_pos0.rotate3D(Vector(1,0,0),(3.14159265358979323846/180.0)*light_altitude);
	light_pos0.rotate3D(Vector(0,1,0),(3.14159265358979323846/180.0)*light_azimuth);
	light_pos0.w = 0.0;

//	light_pos0.x=cosf((3.14159265358979323846/180.0)*light_azimuth)*sinf((3.14159265358979323846/180.0)*light_altitude);
//	light_pos0.y=sinf((3.14159265358979323846/180.0)*light_azimuth)*sinf((3.14159265358979323846/180.0)*light_altitude);
//	light_pos0.z=cosf((3.14159265358979323846/180.0)*light_altitude);
//	light_pos0.w = 1.0;

/*	light_pos0.x+=cosf((3.14159265358979323846/180.0)*light_azimuth)*sinf((3.14159265358979323846/180.0)*light_altitude);
	light_pos0.y+=sinf((3.14159265358979323846/180.0)*light_azimuth)*sinf((3.14159265358979323846/180.0)*light_altitude);
	light_pos0.z+=cosf((3.14159265358979323846/180.0)*light_altitude);
	light_pos0.w = 1.0;
*/
	//printf("light_pos0: %f, %f, %f\n", light_pos0.x, light_pos0.y, light_pos0.z);
	//light_pos0+=0.01;
	//light_pos0.round(2);
	//printf("rounded light_pos: %f, %f, %f\n", light_pos0.x, light_pos0.y, light_pos0.z);

//	light_pos0 *= light_radius;

	light_dir0 = Vector(0,0,0)-light_pos0;
	light_dir0.normalize();

	//printf("light_dir0: %f, %f, %f\n", light_dir0.x, light_dir0.y, light_dir0.z);

	glLightfv(GL_LIGHT0, GL_POSITION, &light_pos0.x);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, &light_dir0.x);

	/*light_pos1.x=  light_radius*cos((3.141592654/180.0)*(180.0-light_azimuth))*cos((3.141592654/180.0)*(180.0-light_altitude));
	light_pos1.y=  -light_radius*sin((3.141592654/180.0)*(180.0-light_azimuth))*cos((3.141592654/180.0)*(180.0-light_altitude));
	light_pos1.z=  (light_radius*sin((3.141592654/180.0)*(180.0-light_altitude)));
	light_pos1.w = 1.0;

	light_dir1 = -light_pos1;
	light_dir1.normalize();
	
	glLightfv(GL_LIGHT1, GL_POSITION, &light_pos1.x);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, &light_dir1.x);*/
/*	
	light_pos1.x=  light_radius*cos((3.141592654/180.0)*(180.0-light_azimuth))*cos((3.141592654/180.0)*(180.0-light_altitude));
	light_pos1.y=  -light_radius*sin((3.141592654/180.0)*(180.0-light_azimuth))*cos((3.141592654/180.0)*(180.0-light_altitude));
	light_pos1.z=  (light_radius*sin((3.141592654/180.0)*(180.0-light_altitude)));

	light_dir1 = -light_pos1;
	light_dir1.normalize();

	glLightfv(GL_LIGHT1, GL_POSITION, &light_pos1.x);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, &light_dir1.x);
*/
	glPopMatrix();
}
void VolumeRender::light_colours(Vector amb, Vector diff, Vector spec)
{
	glLightfv(GL_LIGHT0, GL_AMBIENT, &amb.x);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, &diff.x);
	glLightfv(GL_LIGHT0, GL_SPECULAR, &spec.x);
}
void VolumeRender::update_FBOsize(int size)
{
	fbo_lod=size;
	if(fbo_lod==1) render_size = LOD_FBO_SIZE;
	else render_size = FBO_SIZE;

	if(depthdata) delete[] depthdata;
	depthdata = new unsigned int[FBO_SIZE*FBO_SIZE];

	if(pealdepthdata) delete[] pealdepthdata;
	pealdepthdata = new unsigned int[FBO_SIZE*FBO_SIZE];

	if(coldata) delete[] coldata;
	coldata = new unsigned char[4*FBO_SIZE*FBO_SIZE];

	postrendering_shaderobj.enable();
	postrendering_shaderobj.sendUniform1f("texwidth", render_size);
	postrendering_shaderobj.disable();

//	fbo_object.stop();
//	lodfbo_object.stop();
}
void VolumeRender::check_extensions(void)
{
/*
    #ifdef WIN32

	glTexImage3D = NULL;
	glBlendColorEXT = NULL;
	//glCompressedTexImage3DARB =NULL;
	//glCompressedTexImageARB =NULL;
	glBlendEquationEXT =NULL;
  
	// 3D TEXTURING EXTENSION
	// --------------------------------------------
	// OpenGL 1.3+ GL_TEXTURE_3D became standard.
	// OpenGL 1.2 used GL_TEXTURE_3D_EXT.
	// --------------------------------------------
	if(glTexImage3D==NULL)
	{
		glTexImage3D = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");
		if (NULL == glTexImage3D)
		{
//			QMessageBox::warning(this, "GL Extensions", "No glTexImage3D/glTexImage3DEXT!");
			system("pause");
			exit(1);
		} 
	}

	if(glBlendColorEXT==NULL)
	{
		// --------------------------------------------
		// BLEND COLOR EXTENSION
		// --------------------------------------------
		glBlendColorEXT = (PFNGLBLENDCOLORPROC)wglGetProcAddress("glBlendColor");
	
		if(glBlendColorEXT==NULL)
		{
			glBlendColorEXT = (PFNGLBLENDCOLOREXTPROC)wglGetProcAddress("glBlendColorEXT");
			if(glBlendColorEXT==NULL)
			{
//				QMessageBox::warning(this, "GL Extensions", "No glBlendColor/glBlendColorEXT!");
				system("pause");
				exit(1);
			}
		}  
	}

	glBlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC)wglGetProcAddress("glBlendEquationEXT");
	if ( glBlendEquationEXT == NULL)
	{
//		QMessageBox::warning(this, "GL Extensions", "No glBlendEquationEXT!");
		system("pause");
		exit(1);
	} 
	
	#else
  
	if(glBlendColorEXT==NULL)
	{
		// --------------------------------------------
		// BLEND COLOR EXTENSION
		// --------------------------------------------
		//glBlendColorEXT = (PFNGLBLENDCOLORPROC)glXGetProcAddress("glBlendColor");
		if (NULL == glBlendColorEXT)
		{
		  	glBlendColorEXT = (PFNGLBLENDCOLOREXTPROC)glXGetProcAddress("glBlendColorEXT");
			if (NULL == glBlendColorEXT)
			{
//				QMessageBox::warning(this, "GL Extensions", "No glBlendColor/glBlendColorEXT!");
				system("pause");
				exit(1);
			}
		}  
	}

	if (!strstr((char*)glGetString(GL_EXTENSIONS), "GL_EXT_texture_filter_anisotropic"))
	{
//				QMessageBox::warning(this, "GL Extensions", "GL_EXT_texture_filter_anisotropic");
				system("pause");
				exit(1);
	}       
    #endif*/
}
void VolumeRender::blend_mode(int blendmode)
{	
	bldmode = blendmode;

//	printf("blendmode: %d\n", blendmode);

//	glEnable(GL_BLEND);
	
	switch (blendmode) 
	{
		case 1:	//BLEND
    	//#ifdef WIN32

			//enable front to back...
			//glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
	
			// enable back-to-front compositing GL_SRC_ALPHA
			glextensions.glBlendEquationEXT(GL_FUNC_ADD_EXT);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			background_colour=old_background_colour;
			//glextensions.glBlendEquationEXT(GL_FUNC_SUBTRACT_EXT);
			//glextensions.glBlendEquationEXT(GL_FUNC_ADD_EXT);
			//glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);

			background_colour=old_background_colour;

			//old_background_colour=background_colour;
      	//#endif

			break;
		case 2: //MAXIMUMINTENSITY:
    	//#ifdef WIN32
			/*glEnable(GL_BLEND);
			glextensions.glBlendEquationEXT(GL_FUNC_ADD_EXT);
			glextensions.glBlendFuncSeparateEXT(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	*/
			//glDisable(GL_DEPTH_TEST);
//			glextensions.glBlendFuncSeparateEXT(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
//			glextensions.glBlendFuncSeparateEXT(GL_DST_ALPHA, GL_ZERO,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
					
			// enable maximum intensity projection
			glextensions.glBlendEquationEXT(GL_MAX_EXT);
			glBlendFunc(GL_SRC_COLOR,GL_DST_COLOR);  
			background_colour=Qt::black;
			
	//	#endif
			break;
		case 3: //MINIMUMINTENSITY:
    	//#ifdef WIN32
			// enable minimum intensity projection
			glextensions.glBlendEquationEXT(GL_MIN_EXT);
			glBlendFunc(GL_SRC_COLOR,GL_DST_COLOR); 
			background_colour=Qt::black;
	//    #endif
		case 4: //MINIMUMINTENSITY:
    	//#ifdef WIN32
			//attenuate...
			printf("%d\n", (int)numbslices);
			float attenuation =  0.01;
			glextensions.glBlendEquationEXT(GL_FUNC_ADD_EXT);
			glBlendFunc(GL_CONSTANT_ALPHA_EXT, GL_ONE);
			glextensions.glBlendColorEXT(1.0f, 1.0f, 1.0f, attenuation);
			background_colour=Qt::black;
	//#endif
			break;
	}

	Vector rgb, hsv;
	rgb.x = background_colour.red()/255.0;
	rgb.y = background_colour.green()/255.0;
	rgb.z = background_colour.blue()/255.0;
	RGBtoHSV(rgb, &hsv);

	if(hsv.z>0.5)
	{
		background_fontcolour = QColor(0.0,0.0,0.0);
	}
	else
	{
		background_fontcolour = QColor(255.0,255.0,255.0);
	}
}

void VolumeRender::build_volhistogram(void)
{
	int r,g,b;
	for (int iii=0; iii<256*3; iii++)
	{
		volobj->histogram[iii]=0;
	}

	for(int i=0; i<volobj->texdepth*volobj->texheight*volobj->texwidth; i++)
	{
		r = volobj->texture3d[3*i+0];
		g = volobj->texture3d[3*i+1];
		b = volobj->texture3d[3*i+2];

		volobj->histogram[3*r+0]++;
		volobj->histogram[3*g+1]++;
		volobj->histogram[3*b+2]++;
	}
}
void VolumeRender::build_histogram(void)
{
	//printf("build_histogram\n");
	if(!volobj->texture3d) return;

	int r0,g0,b0;
	r0=g0=b0=0 ;
	int r,g,b,ra,ba,ga, hist_r, hist_g, hist_b;
	r=g=b=ra=ba=ga=hist_r=hist_g=hist_b=0;

	int* newhist =  new int[256*3];
	for (int iii=0; iii<256*3; iii++)
	{
		newhist[iii]=0.0;
	}

	if(updatehisto==1)
	{
		for(int ii=0; ii<256; ii++)
		{
			if(editor->tranfer_function_tab->hist->fe_seleted==1)
			{
				hist_r = rgb_tfunc[3*ii];	
				hist_g = rgb_tfunc[3*ii+1];
				hist_b = rgb_tfunc[3*ii+2];
			}
			if(editor->tranfer_function_tab->hist->fe_seleted==3)
			{
				hist_r = rgb_tfunc[3*ii];
			}
			else if(editor->tranfer_function_tab->hist->fe_seleted==4)
			{
				hist_g = rgb_tfunc[3*ii+1];
			}
			else if(editor->tranfer_function_tab->hist->fe_seleted==5)
			{
				hist_b = rgb_tfunc[3*ii+2];
			}
			else if(editor->tranfer_function_tab->hist->fe_seleted==2)
			{
				hist_r = ragaba_tfunc[3*ii];
				hist_g = ragaba_tfunc[3*ii+1];
				hist_b = ragaba_tfunc[3*ii+2];
			}
			else if(editor->tranfer_function_tab->hist->fe_seleted==6)
			{
				hist_r = ragaba_tfunc[3*ii];
			}
			else if(editor->tranfer_function_tab->hist->fe_seleted==7)
			{
				hist_g = ragaba_tfunc[3*ii+1];
			}
			else if(editor->tranfer_function_tab->hist->fe_seleted==8)
			{
				hist_b = ragaba_tfunc[3*ii+2];
			}

			newhist[3*hist_r] += volobj->histogram[3*ii];
			newhist[3*hist_g+1] += volobj->histogram[3*ii+1];
			newhist[3*hist_b+2] += volobj->histogram[3*ii+2];
		}
	}
	else
	{
		for(int ii=0; ii<256*3; ii++) newhist[ii] = volobj->histogram[ii];
	}

	for(int i=0; i<256; i++)
	{
		hist_r = 3*i;
		hist_g = 3*i+1;
		hist_b = 3*i+2;

		r = g =	b = ra = ga = ba = i;

		if(editor->tranfer_function_tab->hist->fe_seleted==1)
		{
			editor->tranfer_function_tab->hist->data[r] = newhist[hist_r]+newhist[hist_g]+newhist[hist_b];
		}
		else if(editor->tranfer_function_tab->hist->fe_seleted==2)
		{
			editor->tranfer_function_tab->hist->data[ra] = newhist[hist_r]+newhist[hist_g]+newhist[hist_b];
		}
		else if(editor->tranfer_function_tab->hist->fe_seleted==3)
		{
			editor->tranfer_function_tab->hist->data[r] = newhist[hist_r];
		}
		else if(editor->tranfer_function_tab->hist->fe_seleted==4)
		{
			editor->tranfer_function_tab->hist->data[g] = newhist[hist_g];
		}
		else if(editor->tranfer_function_tab->hist->fe_seleted==5)
		{
			editor->tranfer_function_tab->hist->data[b] = newhist[hist_b];
		}
		else if(editor->tranfer_function_tab->hist->fe_seleted==6)
		{
			editor->tranfer_function_tab->hist->data[ra] = newhist[hist_r];
		}
		else if(editor->tranfer_function_tab->hist->fe_seleted==7)
		{
			editor->tranfer_function_tab->hist->data[ga] = newhist[hist_g];
		}
		else if(editor->tranfer_function_tab->hist->fe_seleted==8)
		{
			editor->tranfer_function_tab->hist->data[ba] = newhist[hist_b];
		}
	}

	delete[] newhist;

	editor->rebuild_tabs();
}
//=================================================================================================================================
void VolumeRender::select_channel(int channel)
{

	for(int i=0; i<256; i++)
	{
		if(channel==0)
		{
			backup_rgb_tfunc[3*i+0] = rgb_tfunc[3*i+0];
			backup_rgb_tfunc[3*i+1] = rgb_tfunc[3*i+1];
			backup_rgb_tfunc[3*i+2] = rgb_tfunc[3*i+2];

			backup_ragaba_tfunc[3*i+0] = ragaba_tfunc[3*i+0];
			backup_ragaba_tfunc[3*i+1] = ragaba_tfunc[3*i+1];
			backup_ragaba_tfunc[3*i+2] = ragaba_tfunc[3*i+2];
		}
		if(channel==1)
		{
			rgb_tfunc[3*i+0] = backup_rgb_tfunc[3*i+0];
			rgb_tfunc[3*i+1] = 0;
			rgb_tfunc[3*i+2] = 0;

			ragaba_tfunc[3*i+0] = backup_ragaba_tfunc[3*i+0];
			ragaba_tfunc[3*i+1] = 0;
			ragaba_tfunc[3*i+2] = 0;
		}
		else if(channel==2)
		{
			rgb_tfunc[3*i+1] = backup_rgb_tfunc[3*i+1];
			rgb_tfunc[3*i+0] = 0;
			rgb_tfunc[3*i+2] = 0;

			ragaba_tfunc[3*i+1] = backup_ragaba_tfunc[3*i+1];
			ragaba_tfunc[3*i+0] = 0;
			ragaba_tfunc[3*i+2] = 0;
		}
		else if(channel==3)
		{
			rgb_tfunc[3*i+2] = backup_rgb_tfunc[3*i+2];
			rgb_tfunc[3*i+1] = 0;
			rgb_tfunc[3*i+0] = 0;

			ragaba_tfunc[3*i+2] = backup_ragaba_tfunc[3*i+2];
			ragaba_tfunc[3*i+1] = 0;
			ragaba_tfunc[3*i+0] = 0;
		}
		else if(channel==4)
		{
			rgb_tfunc[3*i+0] = backup_rgb_tfunc[3*i+0];
			rgb_tfunc[3*i+1] = backup_rgb_tfunc[3*i+1];
			rgb_tfunc[3*i+2] = backup_rgb_tfunc[3*i+2];

			ragaba_tfunc[3*i+0] = backup_ragaba_tfunc[3*i+0];
			ragaba_tfunc[3*i+1] = backup_ragaba_tfunc[3*i+1];
			ragaba_tfunc[3*i+2] = backup_ragaba_tfunc[3*i+2];
		}

	}

	if(channel!=0) load_1DLookupTables(true);
}
void VolumeRender::sort_bricks(void)
{
	vector< Vector > lengths;
	lengths.resize(brick_centresrot.size());

	Vector eye = Vector(0,0,0.0);
	eye = eye.mult_matrix(volrenderview.arcball.Transform.M);
	
	//apply our volrenderview.camera rotation to our brick centres
	for(int n=0; n<brick_centres.size(); n++)
	{
		brick_centresrot[n] = brick_centres[n].mult_matrix(volrenderview.arcball.Transform.M);
		brick_centresrot[n].w = brick_centres[n].w;
	}
	int i;
	for(i=0; i<brick_centresrot.size(); i++)
	{	
		lengths[i].x = 0.0;
		lengths[i].y = 0.0;
		lengths[i].z = -brick_centresrot[i].z;//-(brick_centresrot[i]-eye).length(); //-brick_centresrot[i].z; // -(brick_centres[i]-eye).length(); //
		lengths[i].w = i;
	}

	sort(lengths.begin(), lengths.end());

	for(i=0; i<brick_centresrot.size(); i++)
	{
		sorted_brick_indices[i] = lengths[i].w;
	}
}
void VolumeRender::setViewfromEuler(float angleX, float angleY, float angleZ)
{
	//tx=ty=0;
	/*Matrix3fT rotation_mat;
	Quaternion quatX, quatY, quatZ;

	quatX.from_angle(Vector(1,0,0), angleX);
	quatY.from_angle(Vector(1,0,0), angleY);
	quatZ.from_angle(Vector(1,0,0), angleZ);

	Quaternion result;
	result = quatX*quatY*quatZ;
	result.to_matrix(&rotation_mat);

	for(int i=0; i<9; i++) volrenderview.arcball.ThisRot.M[i] = rotation_mat.M[i];

	Matrix4fSetRotationFromMatrix3f(&volrenderview.arcball.Transform, &rotation_mat);	// Set Our Final Transform's Rotation From This One

	Vector trans = Vector(volrenderview.translate.x, volrenderview.translate.y, volrenderview.translate.z);
	trans = trans.mult_matrix9(rotation_mat.M);
	
	volrenderview.translate.x = trans.x;
	volrenderview.translate.y = trans.y;
	volrenderview.translate.z = trans.z;*/

	if(volrenderview.projection_toggle==PERSP_PROJECTION) return;
	
	Matrix4x4 rot;
	int i;
	rot.find_rotation_from_euler(angleX, angleY, angleZ);
	for(i=0; i<16; i++) volrenderview.arcball.Transform.M[i] = rot.m[i];
	
	volrenderview.arcball.ThisRot.M[0] = rot.m[0];
	volrenderview.arcball.ThisRot.M[1] = rot.m[1];
	volrenderview.arcball.ThisRot.M[2] = rot.m[2];
	volrenderview.arcball.ThisRot.M[3] = rot.m[4];
	volrenderview.arcball.ThisRot.M[4] = rot.m[5];
	volrenderview.arcball.ThisRot.M[5] = rot.m[6];
	volrenderview.arcball.ThisRot.M[6] = rot.m[8];
	volrenderview.arcball.ThisRot.M[7] = rot.m[9];
	volrenderview.arcball.ThisRot.M[8] = rot.m[10];
	
	//volrenderview.arcball.setIdentity();
}

void VolumeRender::setViewfromEuler2(float angleX, float angleY, float angleZ)
{
/*	Matrix3fT rotation_mat;
	Quaternion quatX, quatY, quatZ;

	quatX.from_angle(Vector(1,0,0), angleX);
	quatY.from_angle(Vector(1,0,0), angleY);
	quatZ.from_angle(Vector(1,0,0), angleZ);

	Quaternion result;
	result = quatX*quatY*quatZ;
	result.to_matrix(&rotation_mat);

	//for(int i=0; i<9; i++) volrenderview.arcball.ThisRot.M[i] = rotation_mat.M[i];

	Matrix3fMulMatrix3f(&rotation_mat, &volrenderview.arcball.ThisRot);						// Accumulate Last Rotation Into This One
	Matrix4fSetRotationFromMatrix3f(&volrenderview.arcball.Transform, &rotation_mat);	// Set Our Final Transform's Rotation From This One

	Vector trans = Vector(volrenderview.translate.x, volrenderview.translate.y, volrenderview.translate.z);
	trans = trans.mult_matrix9(rotation_mat.M);
	
	volrenderview.translate.x = trans.x;
	volrenderview.translate.y = trans.y;
	volrenderview.translate.z = trans.z;*/
	
	if(volrenderview.projection_toggle==PERSP_PROJECTION) return;


	Matrix4x4 rot;
	rot.find_rotation_from_euler(angleX, angleY, angleZ);

    Matrix3fT Rot3x; //safe not to initialize
	Rot3x.M[0] = rot.m[0];
	Rot3x.M[1] = rot.m[1];
	Rot3x.M[2] = rot.m[2];
	Rot3x.M[3] = rot.m[4];
	Rot3x.M[4] = rot.m[5];
	Rot3x.M[5] = rot.m[6];
	Rot3x.M[6] = rot.m[8];
	Rot3x.M[7] = rot.m[9];
	Rot3x.M[8] = rot.m[10];

	Matrix3fMulMatrix3f(&Rot3x, &volrenderview.arcball.ThisRot);					// Accumulate Last Rotation Into This One
	//Matrix3fSetIdentity(&Rot3x);
	Matrix4fSetRotationFromMatrix3f(&volrenderview.arcball.Transform, &Rot3x);	// Set Our Final Transform's Rotation From This One
}
//=================================================================================================================================
void VolumeRender::compute_boundingbox(void)
{
//	printf("compute_boundingbox()\n");

	//compute our bounding box and clipping plane dimensions
	volobj->compute_boundingbox();

	//volrenderview.cutting_planes.init(volobj->maxres*volobj->xsize/volobj->maxsize, volobj->maxres*volobj->ysize/volobj->maxsize, volobj->maxres*volobj->zsize/volobj->maxsize);
	//printf("bb scale: %f, %f, %f\n", (volobj->xsize/volobj->maxsize), (volobj->ysize/volobj->maxsize), (volobj->zsize/volobj->maxsize));

	Vector size = Vector(volobj->boundingboxSize.x*(volobj->xsize/volobj->maxsize), volobj->boundingboxSize.y*(volobj->ysize/volobj->maxsize), volobj->boundingboxSize.z*(volobj->zsize/volobj->maxsize));
	Vector c = Vector(volobj->boundingboxCentre.x*(volobj->xsize/volobj->maxsize), volobj->boundingboxCentre.y*(volobj->ysize/volobj->maxsize), volobj->boundingboxCentre.z*(volobj->zsize/volobj->maxsize));
	volrenderview.cutting_planes.init(c, size);

	volrenderview.bb_v1 = Vector(-1.0,  1.0,  1.0);
	volrenderview.bb_v2 = Vector( 1.0,  1.0,  1.0);
	volrenderview.bb_v3 = Vector(-1.0, -1.0,  1.0);
	volrenderview.bb_v4 = Vector( 1.0, -1.0,  1.0);
	volrenderview.bb_v5 = Vector(-1.0,  1.0, -1.0);
	volrenderview.bb_v6 = Vector( 1.0,  1.0, -1.0);
	volrenderview.bb_v7 = Vector(-1.0, -1.0, -1.0);
	volrenderview.bb_v8 = Vector( 1.0, -1.0, -1.0);

	Vector bb_size = Vector(volobj->boundingboxSize.x*volobj->xsize/volobj->maxsize, volobj->boundingboxSize.y*volobj->ysize/volobj->maxsize, volobj->boundingboxSize.z*volobj->zsize/volobj->maxsize);
	bb_size /= 2.0;

	volrenderview.bb_v1=volrenderview.bb_v1*bb_size;
	volrenderview.bb_v2=volrenderview.bb_v2*bb_size;
	volrenderview.bb_v3=volrenderview.bb_v3*bb_size;
	volrenderview.bb_v4=volrenderview.bb_v4*bb_size;
	volrenderview.bb_v5=volrenderview.bb_v5*bb_size;
	volrenderview.bb_v6=volrenderview.bb_v6*bb_size;
	volrenderview.bb_v7=volrenderview.bb_v7*bb_size;
	volrenderview.bb_v8=volrenderview.bb_v8*bb_size;

	volrenderview.bb_v1 += c;
	volrenderview.bb_v2 += c;
	volrenderview.bb_v3 += c;
	volrenderview.bb_v4 += c;
	volrenderview.bb_v5 += c;
	volrenderview.bb_v6 += c;
	volrenderview.bb_v7 += c;
	volrenderview.bb_v8 += c;

//	printf("compute_boundingbox(): DONE\n");
}
void VolumeRender::compute_boundingboxDS(Vector min, Vector max)
{
//	printf("DS compute_boundingbox()\n");

	Vector c = (max+min)/2.0f;
	Vector size = max-min;
	volrenderview.cutting_planes.init(c, size);
	volrenderview.bb_v1 = Vector(min.x, max.y, max.z);
	volrenderview.bb_v2 = Vector(max.x, max.y, max.z);
	volrenderview.bb_v3 = Vector(min.x, min.y, max.z);
	volrenderview.bb_v4 = Vector(max.x, min.y, max.z);
	volrenderview.bb_v5 = Vector(min.x, max.y, min.z);
	volrenderview.bb_v6 = Vector(max.x, max.y, min.z);
	volrenderview.bb_v7 = Vector(min.x, min.y, min.z);
	volrenderview.bb_v8 = Vector(max.x, min.y, min.z);
}
void VolumeRender::load_TextureMap(void)
{
	if(volobj->texture3d==NULL) return;

	if(volume_render_mode!=4)
	{
		if (volume_render_mode==1 || volume_render_mode==5)
		{
			//printf("Loading 2D Textures\n");
			load_2DTextureMaps();
		}
		else if(volume_render_mode==2 || volume_render_mode==3) 
		{
			//printf("Loading 3D Texture\n");
			load_3DTextureMap();		
		}
	}
	else if(volume_render_mode==4)
	{	
		//printf("timelapsefolder: %d\n",timelapsefolder);
		BRICKS_SUBDIV=2;
		init_3dTexture_bricks(0);
	}

/*	
	volrenderview.cutting_planes.v1 -= c;
	volrenderview.cutting_planes.v2 -= c;
	volrenderview.cutting_planes.v3 -= c;
	volrenderview.cutting_planes.v4 -= c;
	volrenderview.cutting_planes.v5 -= c;
	volrenderview.cutting_planes.v6 -= c;
	volrenderview.cutting_planes.v7 -= c;
	volrenderview.cutting_planes.v8 -= c;
*/

//	compute_boundingbox();
	load_1DLookupTables(true);

	if(volume_render_mode!=4) 
	{
		build_volhistogram();
		build_histogram();
	}
//	printf("finished\n");
}
void VolumeRender::load_2DTextureMaps(void)
{
//	if(volobj->width>256) return;
//	if(volobj->height>256) return;
//	if(volobj->depth>256) return;
	
	fillSlices = volobj->texdepth;//1 << (int)ceil(log(volobj->texdepth)/log(2));

	if(texNames1)
	{
		glDeleteTextures(volobj->texdepth,texNames1);
		delete[] texNames1;

	}
	texNames1 = new GLuint[fillSlices];

	if(texNames2)
	{
		glDeleteTextures(volobj->texheight,texNames2);
		delete[] texNames2;
	}

	texNames2 = new GLuint[volobj->texheight];
	
	if(texNames3)
	{
		glDeleteTextures(volobj->texwidth,texNames3);
		delete[] texNames3;
	}
	texNames3 = new GLuint[volobj->texwidth];

	init_FBO();

	GLubyte *volume1 = new GLubyte[volobj->texdepth*volobj->texheight*volobj->texwidth];
	GLubyte *volume2 = new GLubyte[volobj->texheight*fillSlices*volobj->texwidth];
	GLubyte *volume3 = new GLubyte[volobj->texwidth*volobj->texheight*fillSlices];

	int index_3D = 0;
	GLubyte rpixel = 0;
	GLubyte gpixel = 0;
	GLubyte bpixel = 0;
	GLubyte apixel = 0;

	maxX = (volobj->texwidth)/2.0;
	minX = -1*maxX;
	maxY = (volobj->texheight)/2.0;
	minY = -1*maxY;
	maxZ = (volobj->texdepth)/2.0;
	minZ = -1*maxZ;

	int progress_iter_steps = volobj->texdepth/20;
	int progress_iter=0;
	int prog;
	printf("Creating 2D Texture maps... \n");
		
	for (int z=0;z<volobj->texdepth;z++)
	{
/*		if(progress_iter==z)
		{
			prog = ( (float)(z+1)/(float)(volobj->texdepth) )*100.0f;
			printf("\r\r Progress = %.2f", prog);
			fflush(stdout);

			progress_iter += progress_iter_steps;
		}
*/
		for (int y=0;y<volobj->texheight;y++)
		{
			for (int x=0;x<volobj->texwidth;x++)
			{
				index_3D = 3*(((((z)*volobj->texheight)+(y))*volobj->texwidth)+(x));
				
				rpixel = gpixel = bpixel = (GLubyte) volobj->texture3d[index_3D+0];
				//apixel = (GLubyte) volobj->texture3d[3*index_3D+3];
				
				int index1 = z*(volobj->texheight*volobj->texwidth)+(y*volobj->texwidth)+x;
				int index2 = y*(fillSlices*volobj->texwidth)+(z*volobj->texwidth)+x;
				int index3 = x*(fillSlices*volobj->texheight)+(y*fillSlices)+z;

				volume1[index1] = rpixel;
				/*volume1[3*index1+1] = gpixel;
				volume1[3*index1+2] = bpixel;*/

				volume2[index2] = gpixel;
				/*volume2[3*index2+1] = gpixel;
				volume2[3*index2+2] = bpixel;*/

				volume3[index3] = bpixel;
				/*volume3[3*index3+1] = gpixel;
				volume3[3*index3+2] = bpixel;*/
			}
		}
	}
	printf("\n");
              
	glGenTextures(volobj->texdepth,texNames1);
	glGenTextures(volobj->texheight,texNames2);
	glGenTextures(volobj->texwidth,texNames3);
	
	//reset texture matrix
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	//disable auto texture coords
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	int i;
	progress_iter_steps = volobj->texwidth/20;
	progress_iter=0;
	
	printf("Loading XY 2D Texture maps... \n");
	for (i=0;i<volobj->texwidth;i++)
	{
/*		if(progress_iter==i)
		{
			prog = ( (float)(i+1)/(float)(volobj->texwidth) )*100.0f;
			printf("\r\r Progress = %.2f", prog);
			fflush(stdout);

			progress_iter += progress_iter_steps;
		}
*/
		// Create MipMapped Texture
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // ( NEW )
		gluBuild2DMipmaps(GL_TEXTURE_2D, 1, fillSlices, volobj->texheight, GL_LUMINANCE, GL_UNSIGNED_BYTE, volume3+(i*(fillSlices*volobj->texheight))); // ( NEW )
		//if (maximumAnisotropy != 0.0f) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnisotropy);

		//setup textur env 0 
		glBindTexture(GL_TEXTURE_2D,texNames3[i]);
		glTexImage2D(
				   GL_TEXTURE_2D,    // target
				   0,                // level
				   GL_LUMINANCE, // internal
				   fillSlices,             // width
				   volobj->texheight,             // height
				   0,                // border
				   GL_LUMINANCE, // format
				   GL_UNSIGNED_BYTE, // type
				   volume3+(i*(fillSlices*volobj->texheight)));
	}
	printf("\n");

	delete[] volume3;
	volume3=NULL;

	progress_iter_steps = volobj->texheight/20;
	progress_iter=0;

	printf("Loading XZ 2D Texture maps... \n");
	for (i=0;i<volobj->texheight;i++)
	{
/*		if(progress_iter==i)
		{
			prog = ( (float)(i+1)/(float)(volobj->texheight) )*100.0f;
			printf("\r\r Progress = %.2f", prog);
			fflush(stdout);
			progress_iter += progress_iter_steps;
		}
*/
		// Create MipMapped Texture
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // ( NEW )
		//if (maximumAnisotropy != 0.0f) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnisotropy);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 1, volobj->texwidth, volobj->texdepth, GL_LUMINANCE, GL_UNSIGNED_BYTE, volume2+(i*(volobj->texwidth*volobj->texdepth))); // ( NEW )
		
		//setup textur env 0 
		glBindTexture(GL_TEXTURE_2D,texNames2[i]);
		glTexImage2D(
				   GL_TEXTURE_2D,    // target
				   0,                // level
				   GL_LUMINANCE, // internal
				   volobj->texwidth,             // width
				   volobj->texdepth,             // height
				   0,                // border
				   GL_LUMINANCE, // format
				   GL_UNSIGNED_BYTE, // type
				   volume2+(i*(volobj->texwidth*volobj->texdepth)));

	}
	printf("\n");

	delete[] volume2;
	volume2=NULL;

	progress_iter_steps = volobj->texdepth/20;
	progress_iter=0;

	printf("Loading YZ 2D Texture maps... \n");
	for (i=0;i<volobj->texdepth;i++)
	{
/*		if(progress_iter==i)
		{
			prog = ( (float)(i+1)/(float)(volobj->texdepth) )*100.0f;
			printf("\r\r Progress = %.2f", prog);
			fflush(stdout);

			progress_iter += progress_iter_steps;
		}*/
		// Create MipMapped Texture
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // ( NEW )
		//if (maximumAnisotropy != 0.0f) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnisotropy);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 1, volobj->texwidth, volobj->texheight, GL_LUMINANCE, GL_UNSIGNED_BYTE, volume1+(i*volobj->texwidth*volobj->texheight)); // ( NEW )

		//setup textur env 0 
		glBindTexture(GL_TEXTURE_2D,texNames1[i]);
		glTexImage2D(
				   GL_TEXTURE_2D,		 // target
				   0,					// level
				   GL_LUMINANCE,					// internal
				   volobj->texwidth,         // width
				   volobj->texheight,             // height
				   0,                // border
				   GL_LUMINANCE, // format
				   GL_UNSIGNED_BYTE, // type
				   volume1+(i*volobj->texwidth*volobj->texheight));

	}
	
	int resX = volobj->width;
	int resY = volobj->height;
	int resZ = volobj->depth;
	volrenderview.cutting_planes.init(volobj->maxres*1.1,volobj->maxres*1.1,volobj->maxres*1.1);

	volrenderview.bb_v1 = Vector(-1.0,  1.0,  1.0);
	volrenderview.bb_v2 = Vector( 1.0,  1.0,  1.0);	
	volrenderview.bb_v3 = Vector(-1.0, -1.0,  1.0);
	volrenderview.bb_v4 = Vector( 1.0, -1.0,  1.0);
	volrenderview.bb_v5 = Vector(-1.0,  1.0, -1.0);
	volrenderview.bb_v6 = Vector( 1.0,  1.0, -1.0);
	volrenderview.bb_v7 = Vector(-1.0, -1.0, -1.0);
	volrenderview.bb_v8 = Vector( 1.0, -1.0, -1.0);

	Vector bb_size = Vector (volobj->maxres,volobj->maxres,volobj->maxres);
	bb_size /= 2.0;

	volrenderview.bb_v1=volrenderview.bb_v1*bb_size;
	volrenderview.bb_v2=volrenderview.bb_v2*bb_size;
	volrenderview.bb_v3=volrenderview.bb_v3*bb_size;
	volrenderview.bb_v4=volrenderview.bb_v4*bb_size;
	volrenderview.bb_v5=volrenderview.bb_v5*bb_size;
	volrenderview.bb_v6=volrenderview.bb_v6*bb_size;
	volrenderview.bb_v7=volrenderview.bb_v7*bb_size;
	volrenderview.bb_v8=volrenderview.bb_v8*bb_size;

	printf("\n");
	delete[] volume1;
	volume1=NULL;
}
void VolumeRender::load_3DTextureMap(void)
{
	/*
	if(texNames1) glDeleteTextures(volobj->texdepth,texNames1);
	if(texNames2) glDeleteTextures(volobj->texheight,texNames2);
	if(texNames3) glDeleteTextures(volobj->texwidth,texNames3);
	*/

	//printf("LOADING: Sending data to GPU\n");

    GLfloat borderColor[4] =  {0.0, 0.0, 0.0, 0.0};
	//printf("Loading 3D Texture maps... \n");

/*	glextensions.glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE8, volobj->texwidth, volobj->texheight, volobj->texdepth, 0, GL_RGB, GL_UNSIGNED_BYTE, &volobj->texture3d[0]);

	if(glGetError() == GLU_OUT_OF_MEMORY)
	{
		printf("GL out of memory");
		return;
	}

	GLint width, height, depth;
	glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_HEIGHT, &height);
	glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_DEPTH, &depth);

	if (width == 0 || height == 0 || depth == 0)
	{
		printf("%d x %d x % d is too large!\n", volobj->texwidth, volobj->texheight, volobj->texdepth);
		return;
	}
	else
	{
		printf("%d x %d x % d is OK!\n", volobj->texwidth, volobj->texheight, volobj->texdepth);
	} 
*/

	//----------------------------------------
	//NON-PBO TRANSFER
	//----------------------------------------

	int size = volobj->texwidth*volobj->texheight*volobj->texdepth;
	unsigned char* texture;
	texture = new unsigned char[size];

	for(int i=0; i<size; i++)
		texture[i] = volobj->texture3d[3*i];

	float border_colour[4] = {0.0, 0.0, 0.0, 0.0};

	//================
	//get red channel
	//================
	//upload red channel 
	glDeleteTextures(1, &volobj->TEXTURE3D_RED);
	glDeleteTextures(1, &volobj->TEXTURE3D_GREEN);
	glDeleteTextures(1, &volobj->TEXTURE3D_BLUE);

	int w, h, d;

	if(volobj->has_red || volobj->is_greyscale)
	{
		glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);

		//avoid boundary problems 
		glTexParameterfv(GL_TEXTURE_3D,GL_TEXTURE_BORDER_COLOR,border_colour);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP); //GL_MIRRORED_REPEAT_ARB GL_MIRRORED_REPEAT_IBM GL_CLAMP GL_REPEAT
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP); 
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP); 

		//set up some mipmaping
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//GL_LINEAR GL_NEAREST
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glPixelStorei(GL_UNPACK_SKIP_PIXELS, 3);

		w = volobj->texwidth;
		h = volobj->texheight;
		d = volobj->texdepth;
		glextensions.glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE8, w, h, d, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL); 
		glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &w); 
		if (w==0)
		{
			QMessageBox* mb = new QMessageBox( "WARNING",
					"Texture Memory Limitation \nThe volume could not be loaded to the GPU. Try reducing the max texture size in the settings (SPANNER ICON).",QMessageBox::Warning,
					QMessageBox::Ok | QMessageBox::Default,0,0 );

			mb->exec();
			printf("ERROR: GPU Texture upload failed\n");
			return;
		}

		//upload the data
		glextensions.glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE8, //numbslices  GL_LUMINANCE16 GL_LUMINANCE GL_COMPRESSED_LUMINANCE GL_COMPRESSED_LUMINANCE_ARB 
				volobj->texwidth, volobj->texheight, volobj->texdepth,
				0,
				GL_LUMINANCE, GL_UNSIGNED_BYTE, &texture[0]);
	}

	if(volobj->has_green)
	{
		//================
		//get green channel
		//================
		for(int i=0; i<size; i++)
			texture[i] = volobj->texture3d[3*i+1];

		//upload green channel
		glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_GREEN);

		//avoid boundary problems 
		glTexParameterfv(GL_TEXTURE_3D,GL_TEXTURE_BORDER_COLOR,border_colour);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP); //GL_MIRRORED_REPEAT_ARB GL_MIRRORED_REPEAT_IBM GL_CLAMP GL_REPEAT
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP); 
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP); 

		//set up some mipmaping
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//GL_LINEAR GL_NEAREST
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glPixelStorei(GL_UNPACK_SKIP_PIXELS, 3);

		w = volobj->texwidth;
		h = volobj->texheight;
		d = volobj->texdepth;
		glextensions.glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE8, w, h, d, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL); 
		glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &w); 
		if (w==0)
		{ 
			QMessageBox* mb = new QMessageBox( "WARNING",
					"Texture Memory Limitation \nThe volume could not be loaded to the GPU. Try reducing the max texture size in the settings (SPANNER ICON).",QMessageBox::Warning,
					QMessageBox::Ok | QMessageBox::Default,0,0 );
	
			printf("ERROR: GPU Texture upload failed\n");
			return;
		}

		//upload the data
		glextensions.glTexImage3D(GL_TEXTURE_3D, 0,  GL_LUMINANCE8, //GL_COMPRESSED_LUMINANCE
				volobj->texwidth, volobj->texheight, volobj->texdepth,
				0,
				GL_LUMINANCE, GL_UNSIGNED_BYTE, &texture[0]);
	}

	if(volobj->has_blue)
	{
		//================
		//get blue channel
		//================
		for(int i=0; i<size; i++)
			texture[i] = volobj->texture3d[3*i+2];

		//upload blue channel
		glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_BLUE);

		//avoid boundary problems 
		glTexParameterfv(GL_TEXTURE_3D,GL_TEXTURE_BORDER_COLOR,border_colour);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP); //GL_MIRRORED_REPEAT_ARB GL_MIRRORED_REPEAT_IBM GL_CLAMP GL_REPEAT
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP); 
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP); 

		//set up some mipmaping
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//GL_LINEAR GL_NEAREST
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glPixelStorei(GL_UNPACK_SKIP_PIXELS, 3);

		w = volobj->texwidth;
		h = volobj->texheight;
		d = volobj->texdepth;
		glextensions.glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE8, w, h, d, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL); 
		glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &w); 
		if (w==0)
		{ 
			QMessageBox* mb = new QMessageBox( "WARNING",
					"Texture Memory Limitation \nThe volume could not be loaded to the GPU. Try reducing the max texture size in the settings (SPANNER ICON).",QMessageBox::Warning,
					QMessageBox::Ok | QMessageBox::Default,0,0 );

			printf("ERROR: GPU Texture upload failed\n");
			return;
		}

		//upload the data
		glextensions.glTexImage3D(GL_TEXTURE_3D, 0,  GL_LUMINANCE8, //GL_COMPRESSED_LUMINANCE
				volobj->texwidth, volobj->texheight, volobj->texdepth,
				0,
				GL_LUMINANCE, GL_UNSIGNED_BYTE, &texture[0]);
	}

	delete texture;

	//----------------------------------------
	//PBO TRANSFER
	//----------------------------------------
/*	
	int datasize = volobj->texwidth*volobj->texheight*volobj->texdepth;

	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
	glextensions.glTexImage3D(GL_TEXTURE_3D, 0,  GL_LUMINANCE,
		volobj->texwidth, volobj->texheight, volobj->texdepth,
		0,
		GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_GREEN);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
	glextensions.glTexImage3D(GL_TEXTURE_3D, 0,  GL_LUMINANCE,
		volobj->texwidth, volobj->texheight, volobj->texdepth,
		0,
		GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_BLUE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
	glextensions.glTexImage3D(GL_TEXTURE_3D, 0,  GL_LUMINANCE,
		volobj->texwidth, volobj->texheight, volobj->texdepth,
		0,
		GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);

	glextensions.glGenBuffersARB(1, &TEXTURE3D_PBO);
    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, TEXTURE3D_PBO);
    glextensions.glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, datasize, 0, GL_STREAM_DRAW_ARB);
    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	GLubyte* ptr;

	//================
	//get red channel
	//================
    // start to modify pixel values ///////////////////
    // bind PBO to update pixel values
    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, TEXTURE3D_PBO);

    // map the buffer object into client's memory
    // Note that glMapBufferARB() causes sync issue.
    // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
    // for GPU to finish its job. To avoid waiting (stall), you can call
    // first glBufferDataARB() with NULL pointer before glMapBufferARB().
    // If you do that, the previous data in PBO will be discarded and
    // glMapBufferARB() returns a new allocated pointer immediately
    // even if GPU is still working with the previous data.
    glextensions.glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, datasize, 0, GL_STREAM_DRAW_ARB);
    ptr = (GLubyte*)glextensions.glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
	
	if(ptr)
    {
        // update data directly on the mapped buffer
        //updatePixels(ptr, DATA_SIZE);
        for(int i=0; i<datasize; i++) ptr[i] = volobj->texture3d[3*i+0];
		glextensions.glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer
    }

    // bind the texture and PBO
    glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);
    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, TEXTURE3D_PBO);

    // copy pixels from PBO to texture object
    // Use offset instead of pointer.
    glextensions.glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, volobj->texwidth, volobj->texheight, volobj->texdepth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);
    //glextensions.glTexSubImage3D(GL_TEXTURE_3D, 0, 0,0,0, volobj->texwidth, volobj->texheight, volobj->texdepth, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);

	//print_glError();

    // it is good idea to release PBOs with ID 0 after use.
    // Once bound with 0, all pixel operations behave normal ways.
    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	//================
	//get green channel
	//================`

    // start to modify pixel values ///////////////////
    // bind PBO to update pixel values
    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, TEXTURE3D_PBO);

    // map the buffer object into client's memory
    // Note that glMapBufferARB() causes sync issue.
    // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
    // for GPU to finish its job. To avoid waiting (stall), you can call
    // first glBufferDataARB() with NULL pointer before glMapBufferARB().
    // If you do that, the previous data in PBO will be discarded and
    // glMapBufferARB() returns a new allocated pointer immediately
    // even if GPU is still working with the previous data.
    glextensions.glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, datasize, 0, GL_STREAM_DRAW_ARB);
    ptr = (GLubyte*)glextensions.glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
	
	if(ptr)
    {
        // update data directly on the mapped buffer
        //updatePixels(ptr, DATA_SIZE);
        for(int i=0; i<datasize; i++) ptr[i] = volobj->texture3d[3*i+1];
		glextensions.glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer
    }

    // bind the texture and PBO
    glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);
    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, volobj->TEXTURE3D_GREEN);

    // copy pixels from PBO to texture object
    // Use offset instead of pointer.
    glextensions.glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, volobj->texwidth, volobj->texheight, volobj->texdepth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);
    //glextensions.glTexSubImage3D(GL_TEXTURE_3D, 0, 0,0,0, volobj->texwidth, volobj->texheight, volobj->texdepth, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);

	//print_glError();

    // it is good idea to release PBOs with ID 0 after use.
    // Once bound with 0, all pixel operations behave normal ways.
    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);


	//================
	//get blue channel
	//================

    // start to modify pixel values ///////////////////
    // bind PBO to update pixel values
    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, TEXTURE3D_PBO);

    // map the buffer object into client's memory
    // Note that glMapBufferARB() causes sync issue.
    // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
    // for GPU to finish its job. To avoid waiting (stall), you can call
    // first glBufferDataARB() with NULL pointer before glMapBufferARB().
    // If you do that, the previous data in PBO will be discarded and
    // glMapBufferARB() returns a new allocated pointer immediately
    // even if GPU is still working with the previous data.
    glextensions.glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, datasize, 0, GL_STREAM_DRAW_ARB);
    ptr = (GLubyte*)glextensions.glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
	
	if(ptr)
    {
        // update data directly on the mapped buffer
        //updatePixels(ptr, DATA_SIZE);
        for(int i=0; i<datasize; i++) ptr[i] = volobj->texture3d[3*i+2];
		glextensions.glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer
    }

    // bind the texture and PBO
    glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_BLUE);
    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, TEXTURE3D_PBO);

    // copy pixels from PBO to texture object
    // Use offset instead of pointer.
    glextensions.glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, volobj->texwidth, volobj->texheight, volobj->texdepth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);
    //glextensions.glTexSubImage3D(GL_TEXTURE_3D, 0, 0,0,0, volobj->texwidth, volobj->texheight, volobj->texdepth, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);

	//print_glError();

    // it is good idea to release PBOs with ID 0 after use.
    // Once bound with 0, all pixel operations behave normal ways.
    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
*/
	//printf("LOADING: Finshed sending data to GPU\n");

	//printf("LOADING: RAM to GPU: %d msec.\n", t.elapsed());
}
void VolumeRender::load_1DLookupTables(bool contrib_correct)
{
	//if no 3d texture return
	if(volobj->texture3d==NULL) return;

	unsigned char col_func[256*3];
	unsigned char alpha_func[256*3];
	double contrib = (double)numbslices/(double)volobj->texwidth;
	contrib  = 1.0/contrib;

//	if(contrib<1.0) contrib = 1.0;
	//printf("contrib: %f\n", contrib);
	//printf("numbslices: %d\n", numbslices);

//	printf("Loading 1D Lookup Tables\n");
	
	double r,g,b;
	double r0,g0,b0;


	for(int i=0; i<256; i++)
	{
		r0 = ragaba_tfunc[3*i+0]/255.0;
		g0 = ragaba_tfunc[3*i+1]/255.0;
		b0 = ragaba_tfunc[3*i+2]/255.0;

		if(contrib_correct)
		{
			r = 1.0-(pow((double)(1.0-r0), contrib));
			g = 1.0-(pow((double)(1.0-g0), contrib));
			b = 1.0-(pow((double)(1.0-b0), contrib));

			r*=255.0;
			g*=255.0;
			b*=255.0;

			if(r>255.0) r=255.0;
			if(g>255.0) g=255.0;
			if(b>255.0) b=255.0;
			if(r<0.0) r=0.0;
			if(g<0.0) g=0.0;
			if(b<0.0) b=0.0;

			alpha_func[3*i+0] = floor(r + 0.5);
			alpha_func[3*i+1] = floor(g + 0.5);
			alpha_func[3*i+2] = floor(b + 0.5);
		}
		else
		{
			alpha_func[3*i+0] = r0*255.0;
			alpha_func[3*i+1] = g0*255.0;
			alpha_func[3*i+2] = b0*255.0;
		}

		/*
		//COLOUR
		r0 = rgb_tfunc[3*i+0]/255.0;
		g0 = rgb_tfunc[3*i+1]/255.0;
		b0 = rgb_tfunc[3*i+2]/255.0;

		r = 1.0-(pow((double)(1.0-r0), contrib));
		g = 1.0-(pow((double)(1.0-g0), contrib));
		b = 1.0-(pow((double)(1.0-b0), contrib));
		
		r*=255.0;
		g*=255.0;
		b*=255.0;

		if(r>255.0) r=255.0;
		if(g>255.0) g=255.0;
		if(b>255.0) b=255.0;
		if(r<0.0) r=0.0;
		if(g<0.0) g=0.0;
		if(b<0.0) b=0.0;

		col_func[3*i+0] = floor(r + 0.5);
		col_func[3*i+1] = floor(g + 0.5);
		col_func[3*i+2] = floor(b + 0.5);*/
	}
	
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &rgb_tfunc[0]);	
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &alpha_func[0]);	

}
void VolumeRender::load_1DLookupTablesBricks(int brickres, int brickslices)
{
	//if no 3d texture return
	if(volobj->texture3d==NULL) return;

	unsigned char alpha_func[256*3];
	float contrib = 1.0/((float)brickslices/(float)brickres);
	
//	printf("contrib: %f\n", contrib);
//	printf("Loading 1D Lookup Tables\n");
	
	float r,g,b;
	float r0,g0,b0;

	for(int i=0; i<256; i++)
	{
		r0 = ragaba_tfunc[3*i+0];
		g0 = ragaba_tfunc[3*i+1];
		b0 = ragaba_tfunc[3*i+2];

		r = 1.0-(pow((float)(1.0-r0/255.0), contrib));
		g = 1.0-(pow((float)(1.0-g0/255.0), contrib));
		b = 1.0-(pow((float)(1.0-b0/255.0), contrib));
		
		r*=255;
		g*=255;
		b*=255;

		if(r>255) r=255;
		if(g>255) g=255;
		if(b>255) b=255;
		if(r<0) r=0;
		if(g<0) g=0;
		if(b<0) b=0;

		alpha_func[3*i+0] = r;
		alpha_func[3*i+1] = g;
		alpha_func[3*i+2] = b;
	}

	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &rgb_tfunc[0]);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &alpha_func[0]);
}
void VolumeRender::load_3dTexture_brick(int brickindex)
{
	if(loaded==0) return;
	if(brick_textures[brickindex].empty()) return;

//	printf("load_3dTexture_brick()\n");
	
	int brick_subdivs=BRICKS_SUBDIV;

	float i_offset = volobj->texwidth/(float)brick_subdivs;
	float j_offset = volobj->texheight/(float)brick_subdivs;
	float k_offset = volobj->texdepth/(float)brick_subdivs;

	int datasize = i_offset*j_offset*k_offset;

	GLubyte* ptr;

    // start to modify pixel values ///////////////////
    // bind PBO to update pixel values
    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, TEXTURE3D_PBO);

    // map the buffer object into client's memory
    // Note that glMapBufferARB() causes sync issue.
    // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
    // for GPU to finish its job. To avoid waiting (stall), you can call
    // first glBufferDataARB() with NULL pointer before glMapBufferARB().
    // If you do that, the previous data in PBO will be discarded and
    // glMapBufferARB() returns a new allocated pointer immediately
    // even if GPU is still working with the previous data.
    glextensions.glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, datasize, 0, GL_STREAM_DRAW_ARB);
    ptr = (GLubyte*)glextensions.glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
	
	if(ptr)
    {
        // update data directly on the mapped buffer
        //updatePixels(ptr, DATA_SIZE);
        for(int i=0; i<datasize; i++) ptr[i] = brick_textures[brickindex][i];
		glextensions.glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer
    }

    // bind the texture and PBO
    glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);
    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, TEXTURE3D_PBO);

    // copy pixels from PBO to texture object
    // Use offset instead of pointer.
    glextensions.glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, i_offset, j_offset, k_offset, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);
    //glextensions.glTexSubImage3D(GL_TEXTURE_3D, 0, 0,0,0, i_offset,j_offset,k_offset, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);

	//print_glError();

    // it is good idea to release PBOs with ID 0 after use.
    // Once bound with 0, all pixel operations behave normal ways.
    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	/*int datasize = i_offset*j_offset*k_offset;

	//before upload
	glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, volobj->TEXTURE3D_RED);
	glextensions.glBufferSubDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0, datasize, &brick_textures[brickindex][0]);

	//perform upload
	glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, volobj->TEXTURE3D_RED);
	glextensions.glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, i_offset, j_offset, k_offset, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);

    glextensions.glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);*/

/*
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);

	//avoid boundary problems 
//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB);
//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_ARB);
//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT_ARB);

	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//set up some mipmaping
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering

	glextensions.glTexImage3D(GL_TEXTURE_3D, 0,  GL_LUMINANCE,
		i_offset, j_offset, k_offset,
		0,
		GL_LUMINANCE, GL_UNSIGNED_BYTE, &brick_textures[brickindex][0]);
*/
}

//=================================================================================================================================
void VolumeRender::render_setup_fbo(void)
{
	//------------------------------------------------
	//Setup our FBO object
	//------------------------------------------------
	if(glextensions.fbo_support)
	{
		if(fbo_lod==1)
		{
			render_size = LOD_FBO_SIZE;
			lodfbo_object.bind(lodfbo_object.frameBufferIndex);
		}
		else
		{
			render_size = FBO_SIZE;
			fbo_object.bind(fbo_object.frameBufferIndex);
		}

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		
		glViewport( 0, 0, (GLint)volrenderview.winWidth, (GLint)volrenderview.winWidth);
		glGetIntegerv(GL_VIEWPORT, volrenderview.viewport);	

		glViewport( 0, 0, (GLint)render_size, (GLint)render_size );
		
		if(stereo_toggle==0)
		{
			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();     
	
			float maxres =  max(volobj->texwidth*(volobj->xsize), volobj->texheight*(volobj->ysize));
			float offset = ((volrenderview.winWidth/volrenderview.winHeight)*maxres)/2.0f;

			maxres = max(maxres, volobj->texdepth*(volobj->zsize));
			maxres += offset;	
			maxres /= 2.0;	
	
			if(volrenderview.projection_toggle==PERSP_PROJECTION)
				gluPerspective(volrenderview.camera.fov, (float)volrenderview.winWidth/(float)volrenderview.winWidth, volrenderview.camera.znear, volrenderview.camera.zfar);	
			else if(volrenderview.projection_toggle==ORTHO_PROJECTION)
				glOrtho(-maxres-volrenderview.zoom, maxres+volrenderview.zoom, -maxres-volrenderview.zoom, maxres+volrenderview.zoom, -1024.0, 1024.0);

			//printf("maxres: %f\n", maxres);
			//printf("zoom: %f\n", volrenderview.zoom);
			//printf("ortho: %f to %f\n", -maxres-volrenderview.zoom, maxres+volrenderview.zoom);
		}

		glGetDoublev (GL_PROJECTION_MATRIX, volrenderview.projmatrix);
	}
}
void VolumeRender::render_display_fbo(void)
{
	if(offscreen_toggle==0)
	{
		if(glextensions.fbo_support)
		{
			//------------------------------------------------
			//Close the FBO
			//------------------------------------------------

			int quad_size = render_size/2.0;
			if(fbo_lod==1) lodfbo_object.stop();		
			else fbo_object.stop();	

			//------------------------------------------------
			//Render the texture mapped quad of our scene
			//------------------------------------------------
			glViewport( 0, 0, (GLint)volrenderview.winWidth, (GLint)volrenderview.winHeight );
			glGetIntegerv(GL_VIEWPORT, nonfbo_viewport);	

			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			glClearColor( background_colour.red()/255.0, background_colour.green()/255.0, background_colour.blue()/255.0, 1.0f );

			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();     
			glOrtho(-volrenderview.winWidth, volrenderview.winWidth, -volrenderview.winHeight, volrenderview.winHeight, -1024, 1024);
			glGetDoublev (GL_PROJECTION_MATRIX, nonfbo_projmatrix);
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glGetDoublev (GL_MODELVIEW_MATRIX, nonfbo_mvmatrix);
			
			if(glextensions.shader_support && PostProcess_toggle) postrendering_shaderobj.enable();
			
			glextensions.glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, TEXTURE2D_FBO_DP);
			if(fbo_lod==1) glBindTexture(GL_TEXTURE_2D, TEXTURE2D_LODFBO_FB);
			else  glBindTexture(GL_TEXTURE_2D, TEXTURE2D_FBO_FB);
			glEnable(GL_TEXTURE_2D);

			//if(fbo_depth==1) glBindTexture(GL_TEXTURE_2D, TEXTURE2D_FBO_DP);

			//glextensions.glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, TEXTURE2D_FBO_DP);
			//glEnable(GL_TEXTURE_2D);
			//glextensions.glActiveTexture(GL_TEXTURE1);
			//glBindTexture(GL_TEXTURE_2D, TEXTURE2D_FBO_FB);
			//glEnable(GL_TEXTURE_2D);
	
  			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_QUADS);
					glTexCoord2f(0+0.01, 0+0.01); glVertex3f(-volrenderview.winWidth, -volrenderview.winWidth, 0.0f);
					glTexCoord2f(1-0.01, 0+0.01); glVertex3f( volrenderview.winWidth, -volrenderview.winWidth, 0.0f);
					glTexCoord2f(1-0.01, 1-0.01); glVertex3f( volrenderview.winWidth,  volrenderview.winWidth, 0.0f);
					glTexCoord2f(0+0.01, 1-0.01); glVertex3f(-volrenderview.winWidth,  volrenderview.winWidth, 0.0f);

			glEnd();

			glDisable(GL_TEXTURE_2D);

			if(glextensions.shader_support && PostProcess_toggle) postrendering_shaderobj.disable();

			glextensions.glActiveTexture(GL_TEXTURE0);
			glDisable(GL_TEXTURE_2D);
			//glextensions.glActiveTexture(GL_TEXTURE1);
			//glDisable(GL_TEXTURE_2D);
			glextensions.glActiveTexture(GL_TEXTURE0);
		}
	}
}
void VolumeRender::render_scene(int render)
{
	//------------------------------------------------
	//setup our our rendering fbo
	//------------------------------------------------
	render_setup_fbo();

	//------------------------------------------------
	//Render the non-volume objects
	//------------------------------------------------
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(-volrenderview.eye.x, -volrenderview.eye.y, -volrenderview.eye.z);

	//if(projection_toggle==0) glTranslatef(0.0, 0.0, -(volrenderview.zoom+285.0f));

	glPushMatrix();

		aim_light();
		if(lights_toggle) render_light(Vector(light_pos0.x, light_pos0.y, light_pos0.z), Vector(light_dir0.x, light_dir0.y, light_dir0.z));

		glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);
			
		glPushMatrix();
		
			if(volrenderview.projection_toggle==ORTHO_PROJECTION) glTranslatef(-volrenderview.eye.x, -volrenderview.eye.y, -volrenderview.eye.z);
			glMultMatrixf(volrenderview.arcball.Transform.M);
			glGetDoublev (GL_MODELVIEW_MATRIX, volrenderview.mvmatrix);
			if(volrenderview.projection_toggle==PERSP_PROJECTION) glTranslatef(-volrenderview.eye.x, -volrenderview.eye.y, -volrenderview.eye.z);

			/*
			glLineWidth(5.0);
			glBegin(GL_LINES);
			glColor3f(1,0,0);
			glVertex3f(volrenderview.at.x-0.5,volrenderview.at.y,volrenderview.at.z);
			glVertex3f(volrenderview.at.x+0.5,volrenderview.at.y,volrenderview.at.z);
			glColor3f(0,1,0);
			glVertex3f(volrenderview.at.x,volrenderview.at.y-0.5,volrenderview.at.z);
			glVertex3f(volrenderview.at.x,volrenderview.at.y+0.5,volrenderview.at.z);
			glColor3f(0,0,1);
			glVertex3f(volrenderview.at.x,volrenderview.at.y,volrenderview.at.z-0.5);
			glVertex3f(volrenderview.at.x,volrenderview.at.y,volrenderview.at.z+0.5);
			glEnd();
			glLineWidth(1.0);
			*/

			float scene_size = (float)LOD_FBO_SIZE;
			if(fbo_lod) scene_size = (float)LOD_FBO_SIZE/4.0f;
			float linesize = scene_size*(1.0/64.0);
			float pointsize = scene_size*(1.0/64.0);
			float bg_oversize = 2.0;
			Vector col = Vector(background_fontcolour.red()/255.0, background_fontcolour.blue()/255.0, background_fontcolour.green()/255.0);
			Vector eye = volrenderview.eye;		
			eye = eye.mult_matrix((float*)&texrot_mat.m[0]);

			volrenderview.cutting_planes.disable();
			volrenderview.cutting_planes.display(linesize, pointsize, bg_oversize, col, eye);
			volrenderview.cutting_planes.enable();
			
			//tell gl about our clipping planes
			volrenderview.cutting_planes.setup();			
			volrenderview.cutting_planes.disable();

			//draw our slices planes
			if(slice_draw_mode) render_slices(slice_draw_mode);

			//draw our bounding box
			if(BB_toggle) render_boundingbox();

			if(measure_toggle) render_measurements();
			
			render_cropshape();

			//draw our cursor
			if(cursor_toggle) render_3DCursor();

			//draw our isosurfaces
			if(isosurface_toggle) render_meshes();

			//render our clipping plane texture mapped slices
			render_3dTextureSlice();

			//render_3dTextureMultiPass_cpu();
			/*
			glColor3f(0.5,0.5,0.5);
			glBegin(GL_QUADS);
				glVertex3f(-100, -100,  100);
				glVertex3f( 100, -100,  100);
				glVertex3f( 100, -100, -100);
				glVertex3f(-100, -100, -100);
			glEnd();
			*/
		glPopMatrix();

	glPopMatrix();

	/*
	//Debug center of object + centre of rotation
	glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPointSize(10.0);
		glBegin(GL_POINTS);
			glColor3f(1,1,0);
			glVertex3f(0.0, 0.0, 0.0);
			glColor3f(1,0,1);
			glVertex3f(-volrenderview.translate.x, -volrenderview.translate.y, -volrenderview.translate.z);
			glColor3f(0,0,1);
			glVertex3f(volrenderview.translate.x, volrenderview.translate.y, volrenderview.translate.z);
		glEnd();
	glPopMatrix();
	*/

	//------------------------------------------------
	//Render the Volume
	//------------------------------------------------
	//render our volume, 2d or 3d textures
	if(volume_toggle)
	{
		volrenderview.cutting_planes.enable();
		glEnable(GL_LIGHTING);
		glEnable(GL_BLEND);

		blend_mode(bldmode);

		if(volume_render_mode==1) render_2dTexture();
		else if(volume_render_mode==2)
		{
			if(volrenderview.projection_toggle==ORTHO_PROJECTION) render_3dTextureMultiPass();
			else render_3dTextureMultiPass_cpu();
		}
		else if(volume_render_mode==3) render_3dTexture();
		else if(volume_render_mode==4) render_3dTexture_bricks(0);
		else if(volume_render_mode==5) render_2dMultiTextureMultiPass();
			
		glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);
		volrenderview.cutting_planes.disable();
	}

	//------------------------------------------------
	//display the fbo as a textured quad
	//------------------------------------------------
	render_display_fbo();

	//display our world axis in non-fbo space
	if(axis_toggle) render_axis();
}
//=================================================================================================================================
void VolumeRender::rebuild_texcoords(void)
{
	texrot_mat.m[0] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[0];
	texrot_mat.m[1] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[4];
	texrot_mat.m[2] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[8];
	texrot_mat.m[3] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[12];
	texrot_mat.m[4] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[1];
	texrot_mat.m[5] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[5];
	texrot_mat.m[6] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[9];
	texrot_mat.m[7] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[13];
	texrot_mat.m[8] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[2];
	texrot_mat.m[9] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[6];
	texrot_mat.m[10] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[10];
	texrot_mat.m[11] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[14];
	texrot_mat.m[12] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[3];
	texrot_mat.m[13] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[7];
	texrot_mat.m[14] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[11];
	texrot_mat.m[15] = volrenderview.cutting_planes.planes[0].arcball.Transform.M[15];

	//enable texture matrix 
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	//apply our volrenderview.camera rotation to 3d texture
	//we could do this on each texcoord,
	//but this is more efficient
	glTranslatef((float)volobj->texwidth/(float)volobj->maxres*volobj->xsize,(float)volobj->texheight/(float)volobj->maxres*volobj->ysize,(float)volobj->texdepth/(float)volobj->maxres*volobj->zsize);
	glMultMatrixf(texrot_mat.m);		
	glTranslatef(-(float)volobj->texwidth/(float)volobj->maxres*volobj->xsize,-(float)volobj->texheight/(float)volobj->maxres*volobj->ysize,-(float)volobj->texdepth/(float)volobj->maxres*volobj->zsize);

	//switch back to modelview matrix
	glMatrixMode(GL_MODELVIEW);
}
void VolumeRender::render_3dTexture_bricks(int subdivs)
{
	//if we dont have any 3d texture return
	if(volobj->texture3d==NULL) return;

	//if we dont have any brick centres or cutting planes, ie: no bricks created return
	if(brick_centres.size()==0) return;

	//some variable helpers for our rendering routine
	Vector brick_centre;
	Vector v1, v2, v3, v4;
	Vector t1, t2, t3, t4;
	float s, offset;
	int brick_index = 0;

	//number of brick subdivisions
	int brick_subdivs=BRICKS_SUBDIV;

	//allocate some storage for our transformed 
	//brick centres
	brick_centresrot.clear();
	brick_centresrot.resize(brick_centres.size());

	//allocate some storage for our sorted brick indices
	sorted_brick_indices.clear();
	sorted_brick_indices.resize(brick_centres.size());

	float vert_res = (volobj->maxres/BRICKS_SUBDIV);
	float vert_resX = (volobj->texwidth/BRICKS_SUBDIV);
	float vert_resY = (volobj->texheight/BRICKS_SUBDIV);
	float vert_resZ = (volobj->texdepth/BRICKS_SUBDIV);

	float maxsize =  max((float)volobj->xsize, (float)volobj->ysize);
	maxsize =  max((float)maxsize, (float)volobj->zsize);

	//geometry for our view-aligned slices
	CVector m_pVertices[8];
	m_pVertices[0] = CVector(-1.0,-1.0,-1.0, 1.0,  0.0, 0.0, 0.0);
	m_pVertices[1] = CVector( 1.0,-1.0,-1.0, 1.0,  1.0, 0.0, 0.0);
	m_pVertices[2] = CVector( 1.0, 1.0,-1.0, 1.0,  1.0, 1.0, 0.0);
	m_pVertices[3] = CVector(-1.0, 1.0,-1.0, 1.0,  0.0, 1.0, 0.0);															
	m_pVertices[4] = CVector(-1.0,-1.0, 1.0, 1.0,  0.0, 0.0, 1.0);
	m_pVertices[5] = CVector( 1.0,-1.0, 1.0, 1.0,  1.0, 0.0, 1.0);
	m_pVertices[6] = CVector( 1.0, 1.0, 1.0, 1.0,  1.0, 1.0, 1.0);
	m_pVertices[7] = CVector(-1.0, 1.0, 1.0, 1.0,  0.0, 1.0, 1.0);
		
	Vector m_pEdges[12];
	m_pEdges[0]  = Vector(0,1);
	m_pEdges[1]  = Vector(1,2);
	m_pEdges[2]  = Vector(2,3);
	m_pEdges[3]  = Vector(3,0);
	m_pEdges[4]  = Vector(0,4);
	m_pEdges[5]  = Vector(1,5);
	m_pEdges[6]  = Vector(2,6);
	m_pEdges[7]  = Vector(3,7);
	m_pEdges[8]  = Vector(4,5);
	m_pEdges[9]  = Vector(5,6);
	m_pEdges[10] = Vector(6,7);
	m_pEdges[11] = Vector(7,4);

	const int m_pEdgeList[8][12] = {
		{ 0,1,5,6,   4,8,11,9,  3,7,2,10 }, // v0 is front
		{ 0,4,3,11,  1,2,6,7,   5,9,8,10 }, // v1 is front
		{ 1,5,0,8,   2,3,7,4,   6,10,9,11}, // v2 is front
		{ 7,11,10,8, 2,6,1,9,   3,0,4,5  }, // v3 is front
		{ 8,5,9,1,   11,10,7,6, 4,3,0,2  }, // v4 is front
		{ 9,6,10,2,  8,11,4,7,  5,0,1,3  }, // v5 is front
		{ 9,8,5,4,   6,1,2,0,   10,7,11,3}, // v6 is front
		{ 10,9,6,5,  7,2,3,1,   11,4,8,0 }  // v7 is front
	}; 


	//sort brick centres by distance to viewer
	sort_bricks();

	glPointSize(4.0);
	glLineWidth(2.0);	
	glDisable(GL_LIGHTING);

	//tranverse each brick
	for(int i=0; i<brick_centres.size(); i++)
	{
		//get our sorted brick index
		brick_index =  sorted_brick_indices[i];

		//get our current brick centre
		brick_centre = brick_centresrot[brick_index];

		//if(empty_brick[brick_centre.w]==0)
		{
			//glPushMatrix();

				//reset our modelview matrix
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

				//apply volrenderview.camera and volrenderview.zoom transformations
				glTranslatef(volrenderview.at.x, volrenderview.at.y, volrenderview.at.z);
				glTranslatef(brick_centre.x, brick_centre.y, brick_centre.z);
				glMultMatrixf(volrenderview.arcball.Transform.M);
				
				glScalef(vert_resX/2.0, vert_resY/2.0, vert_resZ/2.0);
				glScalef((volobj->xsize/maxsize), (volobj->ysize/maxsize), (volobj->zsize/maxsize));

				float pMatrix[16];
				glGetFloatv(GL_MODELVIEW_MATRIX,pMatrix);
				CVector viewVec(-pMatrix[2],-pMatrix[6],-pMatrix[10],0.0);

				double dMaxDist = viewVec * m_pVertices[0];
				double dMinDist = dMaxDist;
				int nMaxIdx = 0;
				for(int i = 1; i < 8; ++i) 
				{
					double dist = viewVec * m_pVertices[i];
					if ( dist > dMaxDist) 
					{
						dMaxDist = dist;
						nMaxIdx = i;
					}
					if ( dist < dMinDist) 
					{
						dMinDist = dist;
					}
				}

				//printf("dMinDist %f, dMaxDist %f \n", dMinDist, dMaxDist);
				CVector vecStart[12];
				CVector vecDir[12];
				CVector texStart[12];
				CVector texDir[12];

				float lambda[12];
				float lambda_inc[12];
				double denom;

				int m_nNumSlices = (float)vert_res/2.0;

				float EPSILON = 0.000001;
				dMinDist += EPSILON;
				dMaxDist -= EPSILON;
				double dPlaneDist    =  dMinDist;   
				double dPlaneDistInc =  (dMaxDist-dMinDist)/double(m_nNumSlices);

				for(int ii = 0; ii < 12; ii++) 
				{
					vecStart[ii] = m_pVertices[(int)m_pEdges[m_pEdgeList[nMaxIdx][ii]].x];
					vecDir[ii]   = m_pVertices[(int)m_pEdges[m_pEdgeList[nMaxIdx][ii]].y] - m_pVertices[(int)m_pEdges[m_pEdgeList[nMaxIdx][ii]].x];

					denom = vecDir[ii] *viewVec;

					if (1.0 + denom != 1.0) 
					{
						lambda_inc[ii] =  dPlaneDistInc/denom;
						lambda[ii]     = (dPlaneDist - vecStart[ii] * viewVec)/denom;
					} 
					else
					{
						lambda[ii]     = -1.0;
						lambda_inc[ii] =  0.0;	
					}
				}
				
				volrenderview.cutting_planes.disable();

				/*glColor3f(1.0, 1.0, 1.0);
				glBegin(GL_LINES);
				for(int ii = 0; ii < 12; ii++) 
				{
					glVertex3dv(&m_pVertices[(int)m_pEdges[m_pEdgeList[nMaxIdx][ii]].x][0]);
					glVertex3dv(&m_pVertices[(int)m_pEdges[m_pEdgeList[nMaxIdx][ii]].y][0]);
				}
				glEnd();*/

				volrenderview.cutting_planes.enable();
				
			//glPopMatrix();

			glPushMatrix();
				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();

				glMatrixMode(GL_MODELVIEW);
				//glLoadIdentity();
				
				//upload to gfx current 3d texture brick
				load_3dTexture_brick(brick_centresrot[brick_index].w);
				
				//load_1DLookupTablesBricks(brick_index, m_nNumSlices);

				//enable and attach texture units
				glextensions.glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);
				glEnable(GL_TEXTURE_3D);
				glextensions.glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_GREEN);
				glEnable(GL_TEXTURE_3D);
				glextensions.glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_BLUE);
				glEnable(GL_TEXTURE_3D);
				glextensions.glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
				glEnable(GL_TEXTURE_2D);
				glextensions.glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
				glEnable(GL_TEXTURE_2D);
				glextensions.glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, TEXTURE2D_JITTER);
				glEnable(GL_TEXTURE_2D);

				//enable our GLSL shaders if we support them
				if(volume_render_mode!=1 && glextensions.shader_support==1) 
						vol_shaderobj.enable();

				//glEnable(GL_LIGHTING);
				glEnable(GL_TEXTURE_3D);
				glEnable(GL_BLEND);
				blend_mode(bldmode);

				vol_shaderobj.sendUniform1i("channel", 3);
				vol_shaderobj.sendUniform1i("volumeTexture", 0);
					
				CVector intersection[6];
				float lmb[12];

				for(int n = m_nNumSlices-1; n >= 0; --n) 
				{
					for(int e = 0; e < 12; e++) 
					{
						lmb[e] = lambda[e] + n*lambda_inc[e];
					}

					if      ((lmb[0] >= 0.0) && (lmb[0] < 1.0)) intersection[0] = vecStart[0] + lmb[0] * vecDir[0];
					else if ((lmb[1] >= 0.0) && (lmb[1] < 1.0)) intersection[0] = vecStart[1] + lmb[1] * vecDir[1];
					else if ((lmb[3] >= 0.0) && (lmb[3] < 1.0)) intersection[0] = vecStart[3] + lmb[3] * vecDir[3];
					else continue;
					
					if	    ((lmb[2] >= 0.0) && (lmb[2] < 1.0)) intersection[1] = vecStart[2] + lmb[2] * vecDir[2];
					else if ((lmb[0] >= 0.0) && (lmb[0] < 1.0)) intersection[1] = vecStart[0] + lmb[0] * vecDir[0];
					else if ((lmb[1] >= 0.0) && (lmb[1] < 1.0)) intersection[1] = vecStart[1] + lmb[1] * vecDir[1];
					else intersection[1] = vecStart[3] + lmb[3] * vecDir[3];

					if      ((lmb[4] >= 0.0) && (lmb[4] < 1.0)) intersection[2] = vecStart[4] + lmb[4] * vecDir[4];
					else if ((lmb[5] >= 0.0) && (lmb[5] < 1.0)) intersection[2] = vecStart[5] + lmb[5] * vecDir[5];
					else intersection[2] = vecStart[7] + lmb[7] * vecDir[7];
					
					if	    ((lmb[6] >= 0.0) && (lmb[6] < 1.0)) intersection[3] = vecStart[6] + lmb[6] * vecDir[6];
					else if ((lmb[4] >= 0.0) && (lmb[4] < 1.0)) intersection[3] = vecStart[4] + lmb[4] * vecDir[4];
					else if ((lmb[5] >= 0.0) && (lmb[5] < 1.0)) intersection[3] = vecStart[5] + lmb[5] * vecDir[5];
					else intersection[3] = vecStart[7] + lmb[7] * vecDir[7];

					if	    ((lmb[8] >= 0.0) && (lmb[8] < 1.0)) intersection[4] = vecStart[8] + lmb[8] * vecDir[8] ;
					else if ((lmb[9] >= 0.0) && (lmb[9] < 1.0)) intersection[4] = vecStart[9] + lmb[9] * vecDir[9] ;
					else intersection[4] = vecStart[11]+ lmb[11]* vecDir[11];
					
					if	    ((lmb[10]>= 0.0) && (lmb[10]< 1.0)) intersection[5] = vecStart[10]+ lmb[10]* vecDir[10];
					else if ((lmb[8] >= 0.0) && (lmb[8] < 1.0)) intersection[5] = vecStart[8] + lmb[8] * vecDir[8] ;
					else if ((lmb[9] >= 0.0) && (lmb[9] < 1.0)) intersection[5] = vecStart[9] + lmb[9] * vecDir[9] ;
					else intersection[5] = vecStart[11]+ lmb[11]* vecDir[11];
				
					//float c = 1.0f-float(n)/float(m_nNumSlices);
					//glColor3f(c,c,c);
					glColor3f(1.0,1.0,1.0);
					glBegin(GL_TRIANGLE_FAN);
					for(int ii = 0; ii < 6; ++ii)
					{
						intersection[ii].glVertex(false,true);
					}
					glEnd();
				}				

				glextensions.glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_3D);
				glextensions.glActiveTexture(GL_TEXTURE1);
				glDisable(GL_TEXTURE_3D);
				glextensions.glActiveTexture(GL_TEXTURE2);
				glDisable(GL_TEXTURE_3D);
				glextensions.glActiveTexture(GL_TEXTURE3);
				glDisable(GL_TEXTURE_2D);
				glextensions.glActiveTexture(GL_TEXTURE4);
				glDisable(GL_TEXTURE_2D);
				glextensions.glActiveTexture(GL_TEXTURE5);
				glDisable(GL_TEXTURE_2D);

				//reset default to texture unit 0
				glextensions.glActiveTexture(GL_TEXTURE0);

				glDisable(GL_TEXTURE_3D);
				glDisable(GL_BLEND);
				glDisable(GL_LIGHTING);

				//disable our GLSL shaders if we support them
				if(volume_render_mode!=1 && glextensions.shader_support==1) 
						vol_shaderobj.disable();

			glPopMatrix();
		}
	}
}
void VolumeRender::render_3dTexture(void)
{
	if(volobj->texture3d==NULL) return;
	int i;

	rebuildwithscaling_texcoords();

	int resX = volobj->texwidth;
	int resY = volobj->texheight;
	int resZ = volobj->texdepth;

	float maxres = 0.0;
	maxres = max((float)maxres, (float)resX);
	maxres = max((float)maxres, (float)resY);
	maxres = max((float)maxres, (float)resZ);

	resX = resY = resZ = maxres;

	if(glextensions.multitexture_support && glextensions.shader_support)
	{
		vol_shadersinglepassobj.enable();

		//enable 3 texture units and bind our 3D volume, and the 2 lookup tables
		glextensions.glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);
		glEnable(GL_TEXTURE_3D);
		glextensions.glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_GREEN);
		glEnable(GL_TEXTURE_3D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_BLUE);
		glEnable(GL_TEXTURE_3D);
		glextensions.glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
		glEnable(GL_TEXTURE_2D);
	}
	
	v1 = Vector(-resX, -resY, 0);
	v2 = Vector( resX, -resY, 0);	
	v3 = Vector( resX,  resY, 0);
	v4 = Vector(-resX,  resY, 0);
	t1 = Vector(-0.5,-0.5, 0);
	t2 = Vector(1.5,-0.5, 0);
	t3 = Vector(1.5,1.5, 0);
	t4 = Vector(-0.5,1.5, 0);

	glColor3f(1.0,1.0,1.0);
	for(i = -numbslices/2; i <numbslices+numbslices/2; i++)
	{
		t1.z = t2.z = t3.z = t4.z = (float)i/(float)numbslices;
		v1.z = v2.z = v3.z = v4.z = -(resZ/2.0) + ((float)i/(float)numbslices)*resZ;

		glBegin(GL_QUADS);
			glTexCoord3fv(&t1.x);
			glVertex3fv(&v1.x);
			glTexCoord3fv(&t2.x);
			glVertex3fv(&v2.x);
			glTexCoord3fv(&t3.x);
			glVertex3fv(&v3.x);
			glTexCoord3fv(&t4.x);
			glVertex3fv(&v4.x);
		glEnd();
	}

	/*Vector v1, v2, v3, v4;
	Vector t1, t2, t3, t4;
	float s, offset;

	int temp;
	temp = slices;
	slices *= 2;
	slices = maxres;

	glColor3f(1.0,1.0,1.0);
	//glBindTexture(GL_TEXTURE_3D, 0);
	glEnable(GL_TEXTURE_3D);
	glBegin(GL_QUADS);
	for(i = -slices/2; i <slices+slices/2; i++)
	{
		s = (float)i/(float)slices;
		offset = -128.0 + ((float)i/(float)slices)*256.0;

		v1 = Vector(-256.f+tx, -256.f+ty, offset);
		v2 = Vector( 256.f+tx, -256.f+ty, offset);	
		v3 = Vector( 256.f+tx,  256.f+ty, offset);
		v4 = Vector(-256.f+tx,  256.f+ty, offset);
		
		t1 = Vector(-0.5,-0.5, s);
		t2 = Vector(1.5,-0.5, s);
		t3 = Vector(1.5,1.5, s);
		t4 = Vector(-0.5,1.5, s);

		glTexCoord3fv(&t1.x);
		glVertex3fv(&v1.x);
		glTexCoord3fv(&t2.x);
		glVertex3fv(&v2.x);
		glTexCoord3fv(&t3.x);
		glVertex3fv(&v3.x);
		glTexCoord3fv(&t4.x);
		glVertex3fv(&v4.x);
	}
	glEnd();
	glDisable(GL_TEXTURE_3D);
	
	slices = temp;*/
	
	if(glextensions.multitexture_support && glextensions.shader_support)
	{
		//disable shader object
		vol_shadersinglepassobj.disable();

		//disable 3 texture units
		glextensions.glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_3D);
		glextensions.glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_3D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_3D);
		glextensions.glActiveTexture(GL_TEXTURE3);
		glDisable(GL_TEXTURE_2D);
		
		//reset default to texture unit 0
		glextensions.glActiveTexture(GL_TEXTURE0);
	}
}
void VolumeRender::init_VBO(void)
{
	/*
	if(numbslices==0) return;

	vbo_vert_size = numbslices*2*4; //- to + numbslices (2) and 4 verts/tex per slice
	vbo_indices_size = numbslices*2*6; //- to + numbslices (2) and 2 triangles with 3 vertices

	if(vbo_array) delete[] vbo_array;
	vbo_array = new vbo_vert[vbo_vert_size]; 
	
	if(vbo_indices) delete[] vbo_indices;
	vbo_indices = new unsigned short[vbo_indices_size];
	
	if(vertexVBO_ID!=-1) glextensions.glDeleteBuffersARB(1, &vertexVBO_ID);
	if(indiciesVBO_ID!=-1) glextensions.glDeleteBuffersARB(1, &indiciesVBO_ID);
	
	Vector vbo_v1 = Vector(-volobj->maxres, -volobj->maxres, 0);
	Vector vbo_v2 = Vector( volobj->maxres, -volobj->maxres, 0);	
	Vector vbo_v3 = Vector( volobj->maxres,  volobj->maxres, 0);
	Vector vbo_v4 = Vector(-volobj->maxres,  volobj->maxres, 0);
	Vector vbo_t1 = Vector(-0.5,-0.5, 0);
	Vector vbo_t2 = Vector(1.5,-0.5, 0);
	Vector vbo_t3 = Vector(1.5,1.5, 0);
	Vector vbo_t4 = Vector(-0.5,1.5, 0);
	
	int index;
	
	for(int i = -numbslices/2; i <numbslices+numbslices/2; i++)
	//for(i = numbslices+numbslices/2; i >=-numbslices/2; i--)
	{
		vbo_t1.z = vbo_t2.z = vbo_t3.z = vbo_t4.z = (float)i/(float)numbslices;
		vbo_v1.z = vbo_v2.z = vbo_v3.z = vbo_v4.z = -((float)volobj->maxres/2.0) + ((float)i/(float)numbslices)*(float)volobj->maxres;

		index = numbslices/2+i;

		//GEOMETRY
		vbo_array[4*index+0].x = vbo_v1.x;
		vbo_array[4*index+0].y = vbo_v1.y;
		vbo_array[4*index+0].z = vbo_v1.z;
		vbo_array[4*index+0].translate.x = vbo_t1.x;
		vbo_array[4*index+0].translate.y = vbo_t1.y;
		vbo_array[4*index+0].translate.z = vbo_t1.z;
		
		vbo_array[4*index+1].x = vbo_v2.x;
		vbo_array[4*index+1].y = vbo_v2.y;
		vbo_array[4*index+1].z = vbo_v2.z;
		vbo_array[4*index+1].translate.x = vbo_t2.x;
		vbo_array[4*index+1].translate.y = vbo_t2.y;
		vbo_array[4*index+1].translate.z = vbo_t2.z;

		vbo_array[4*index+2].x = vbo_v3.x;
		vbo_array[4*index+2].y = vbo_v3.y;
		vbo_array[4*index+2].z = vbo_v3.z;
		vbo_array[4*index+2].translate.x = vbo_t3.x;
		vbo_array[4*index+2].translate.y = vbo_t3.y;
		vbo_array[4*index+2].translate.z = vbo_t3.z;

		vbo_array[4*index+3].x = vbo_v4.x;
		vbo_array[4*index+3].y = vbo_v4.y;
		vbo_array[4*index+3].z = vbo_v4.z;
		vbo_array[4*index+3].translate.x = vbo_t4.x;
		vbo_array[4*index+3].translate.y = vbo_t4.y;
		vbo_array[4*index+3].translate.z = vbo_t4.z;
		
		//TRIANGLE 1
		vbo_indices[6*index+0]  = 4*index+0;
		vbo_indices[6*index+1]  = 4*index+1;
		vbo_indices[6*index+2]  = 4*index+2;

		//TRIANGLE 2
		vbo_indices[6*index+3]  = 4*index+0;
		vbo_indices[6*index+4]  = 4*index+2;
		vbo_indices[6*index+5]  = 4*index+3;
	}

	glextensions.glGenBuffersARB(1, &vertexVBO_ID);
	glextensions.glBindBufferARB(GL_ARRAY_BUFFER, vertexVBO_ID);
	glextensions.glBufferDataARB(GL_ARRAY_BUFFER, sizeof(vbo_vert)*vbo_vert_size, &vbo_array[0].x, GL_STATIC_DRAW);

	glextensions.glGenBuffersARB(1, &indiciesVBO_ID);
	glextensions.glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, indiciesVBO_ID);
	glextensions.glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*vbo_indices_size, vbo_indices, GL_STATIC_DRAW);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glextensions.glBindBufferARB(GL_ARRAY_BUFFER, 0);
	
	//delete[] vbo_array;
	//delete[] vbo_indices;

	*/
}
void VolumeRender::render_VBO(void)
{
	if(vertexVBO_ID==-1) return;
	
	//DISPLAY 
	glextensions.glBindBufferARB(GL_ARRAY_BUFFER, vertexVBO_ID);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(vbo_vert), BUFFER_OFFSET(0));   //The starting point of the VBO, for the vertices
	glextensions.glActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(3, GL_FLOAT, sizeof(vbo_vert), BUFFER_OFFSET(12));   //The starting point of texcoords, 12 bytes away / 4*8 (32) bits / floats
 
	glextensions.glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, indiciesVBO_ID);
	glDrawElements(GL_TRIANGLES, vbo_indices_size, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));   //The starting point of the IBO

	//0 and 3 are the first and last vertices
	//glextensions.glDrawRangeElementsEXT(GL_TRIANGLES, 0, vbo_indices_size, vbo_indices_size, GL_UNSIGNED_INT, BUFFER_OFFSET(0));   //The starting point of the IBO
	//glDrawRangeElements may or may not give a performance advantage over glDrawElements

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glextensions.glBindBufferARB(GL_ARRAY_BUFFER, 0);
	glextensions.glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void VolumeRender::render_VBO_start(void)
{
	if(vertexVBO_ID==-1) return;
	
	//DISPLAY 
	glextensions.glBindBufferARB(GL_ARRAY_BUFFER, vertexVBO_ID);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(vbo_vert), BUFFER_OFFSET(0));   //The starting point of the VBO, for the vertices
	glextensions.glActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(3, GL_FLOAT, sizeof(vbo_vert), BUFFER_OFFSET(12));   //The starting point of texcoords, 12 bytes away / 4*8 (32) bits / floats
 
	glextensions.glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, indiciesVBO_ID);
}
void VolumeRender::render_VBO_end(void)
{
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glextensions.glBindBufferARB(GL_ARRAY_BUFFER, 0);
	glextensions.glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void VolumeRender::render_VBO_range(int start, int end)
{
	glextensions.glDrawRangeElementsEXT(GL_TRIANGLES, start, end, vbo_indices_size, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));   //The starting point of the IBO
}
void VolumeRender::rebuildwithscaling_texcoords(void)
{
	Vector res = Vector(volobj->maxres, volobj->maxres, volobj->maxres);
	Vector  texture_translate = -volrenderview.eye.mult_matrix(&volrenderview.arcball.Transform.M[0]);
	texture_translate = texture_translate/res;

	/*Vector texture_translate = (volobj->maxres/2.0)+volrenderview.eye;
	texture_translate = texture_translate.mult_matrix(&volrenderview.arcball.Transform.M[0]);
	texture_translate = (volobj->maxres/2.0) - texture_translate;
	texture_translate = texture_translate/res;*/

	texrot_mat.m[0] = volrenderview.arcball.Transform.M[0];
	texrot_mat.m[1] = volrenderview.arcball.Transform.M[4];
	texrot_mat.m[2] = volrenderview.arcball.Transform.M[8];
	texrot_mat.m[3] = volrenderview.arcball.Transform.M[12];
	texrot_mat.m[4] = volrenderview.arcball.Transform.M[1];
	texrot_mat.m[5] = volrenderview.arcball.Transform.M[5];
	texrot_mat.m[6] = volrenderview.arcball.Transform.M[9];
	texrot_mat.m[7] = volrenderview.arcball.Transform.M[13];
	texrot_mat.m[8] = volrenderview.arcball.Transform.M[2];
	texrot_mat.m[9] = volrenderview.arcball.Transform.M[6];
	texrot_mat.m[10] = volrenderview.arcball.Transform.M[10];
	texrot_mat.m[11] = volrenderview.arcball.Transform.M[14];
	texrot_mat.m[12] = volrenderview.arcball.Transform.M[3];
	texrot_mat.m[13] = volrenderview.arcball.Transform.M[7];
	texrot_mat.m[14] = volrenderview.arcball.Transform.M[11];
	texrot_mat.m[15] = volrenderview.arcball.Transform.M[15];
	
	//enable texture matrix 
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glTranslated(0.5,0.5,0.5);
	glScaled((double)volobj->maxres/(double)volobj->texwidth, (double)volobj->maxres/(double)volobj->texheight, (double)volobj->maxres/(double)volobj->texdepth);
	glScaled(1.0/((double)volobj->xsize/(double)volobj->maxsize), 1.0/((double)volobj->ysize/(double)volobj->maxsize), 1.0/((double)volobj->zsize/(double)volobj->maxsize));
	glMultMatrixf(texrot_mat.m);
	glTranslated(-0.5,-0.5,-0.5);
	//glTranslated(-texture_translate.x, -texture_translate.y, -texture_translate.z);

//	glTranslated(0.5,0.5,0.5);
//	glScaled((double)volobj->maxres/(double)volobj->texwidth, (double)volobj->maxres/(double)volobj->texheight, (double)volobj->maxres/(double)volobj->texdepth);
//	glScaled(1.0/((double)volobj->xsize/(double)volobj->maxsize), 1.0/((double)volobj->ysize/(double)volobj->maxsize), 1.0/((double)volobj->zsize/(double)volobj->maxsize));
//	glTranslated(-0.5,-0.5,-0.5);
	

	//switch back to modelview matrix
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
}
void VolumeRender::render_3dTextureMultiPass_cpu(void)
{
	//compute geometry 
	volrenderview.cutting_planes.eval_viewaligned_slices(numbslices, volrenderview.arcball.Transform.M, volrenderview.eye, volrenderview.at);

glPushMatrix();
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMultMatrixf(volrenderview.arcball.Transform.M);
	glTranslatef(-volrenderview.eye.x, -volrenderview.eye.y, -volrenderview.eye.z);
	
	float maxsize =  max((float)volobj->xsize, (float)volobj->ysize);
	maxsize =  max((float)maxsize, (float)volobj->zsize);
//	Vector res = Vector(1,1,1);

	Vector res = Vector(volobj->texwidth*volobj->xsize/volobj->maxsize, volobj->texheight*volobj->ysize/volobj->maxsize, volobj->texdepth*volobj->zsize/volobj->maxsize);
//	Vector texres = Vector(1.0/((double)volobj->xsize/(double)volobj->maxsize), 1.0/((double)volobj->ysize/(double)volobj->maxsize), 1.0/((double)volobj->zsize/(double)volobj->maxsize));
	//Vector res = Vector(volobj->texwidth*(volobj->xsize/maxsize), volobj->texheight*(volobj->ysize/maxsize), volobj->texdepth*(volobj->zsize/maxsize));
	//Vector res = Vector(volobj->maxres, volobj->maxres, volobj->maxres);
	//Vector res = Vector(volobj->boundingboxSize.x*volobj->xsize/volobj->maxsize, volobj->boundingboxSize.y*volobj->ysize/volobj->maxsize, volobj->boundingboxSize.z*volobj->zsize/volobj->maxsize);

//	glScaled((double)volobj->maxres/(double)volobj->texwidth, (double)volobj->maxres/(double)volobj->texheight, (double)volobj->maxres/(double)volobj->texdepth);
//	glScaled(1.0/((double)volobj->xsize/(double)volobj->maxsize), 1.0/((double)volobj->ysize/(double)volobj->maxsize), 1.0/((double)volobj->zsize/(double)volobj->maxsize));

	if(volobj->is_greyscale)
	{
		if(volrenderview.redclip_toggle==0 || volrenderview.greenclip_toggle==0 || volrenderview.blueclip_toggle==0) volrenderview.cutting_planes.disable();
		else volrenderview.cutting_planes.enable();

		vol_shaderobj.enable();
		vol_shaderobj.sendUniform1i("channel", 3);
		vol_shaderobj.sendUniform1i("volumeTexture", 0);
				
		//enable 3 texture units and bind our 3D volume, and the 2 lookup tables
		glextensions.glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);

		//enable 3 texture units and bind our 3D volume, and the 2 lookup tables
		glextensions.glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);

		Vector v1, t1;
		glColor3f(1,1,1);
		for(int i=0; i<volrenderview.cutting_planes.view_algined_slices.size(); i++)
		{
			//float c = 1.0f-float(i)/float(numbslices);
			//glColor3f(c,c,c);
			glBegin(GL_TRIANGLE_FAN);
				for(int j=0; j<volrenderview.cutting_planes.view_algined_slices[i].size(); j++)
				{
					v1 = volrenderview.cutting_planes.view_algined_slices[i][j];
					v1 = v1*res;
					t1 = ((v1)+(res/2.0f));
					t1 = t1/res;

					glTexCoord3fv(&t1.x);
					glVertex3fv(&v1.x);
				}
			glEnd();
		}
	
		//disable shader object
		vol_shaderobj.disable();

		//reset default to texture unit 0
		glextensions.glActiveTexture(GL_TEXTURE0);
	}
	else
	{
		vol_shaderobj.enable();

		//enable 3 texture units and bind our 3D volume, and the 2 lookup tables
		glextensions.glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);

		if(detphpeal)
		{
			glextensions.glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, TEXTURE2D_FBO_DEPTH);
			glextensions.glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, TEXTURE2D_FBO_COL);
		}
		
		volrenderview.cutting_planes.enable();
	
		for(int i=0; i<volrenderview.cutting_planes.view_algined_slices.size(); i++)
		{
			if(r_channel)
			{
				if(volrenderview.redclip_toggle==0) volrenderview.cutting_planes.disable();

				vol_shaderobj.sendUniform1i("channel", 0);				
				//vol_shaderobj.sendUniform1i("volumeTexture", 0);
				glextensions.glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);

				glBegin(GL_TRIANGLE_FAN);
					for(int j=0; j<volrenderview.cutting_planes.view_algined_slices[i].size(); j++)
					{
						v1 = volrenderview.cutting_planes.view_algined_slices[i][j];
						v1 = v1*res;
						t1 = ((v1)+(res/2.0f));
						t1 = t1/res;

						glTexCoord3fv(&t1.x);
						glVertex3fv(&v1.x);
					}
				glEnd();

				if(volrenderview.redclip_toggle==0) volrenderview.cutting_planes.enable();
			}
			if(g_channel)
			{
				if(volrenderview.greenclip_toggle==0) volrenderview.cutting_planes.disable();

				vol_shaderobj.sendUniform1i("channel", 1);			
				//vol_shaderobj.sendUniform1i("volumeTexture", 0);
				glextensions.glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_GREEN);

				glBegin(GL_TRIANGLE_FAN);
					for(int j=0; j<volrenderview.cutting_planes.view_algined_slices[i].size(); j++)
					{
						v1 = volrenderview.cutting_planes.view_algined_slices[i][j];
						v1 = v1*res;
						t1 = ((v1)+(res/2.0f));
						t1 = t1/res;

						glTexCoord3fv(&t1.x);
						glVertex3fv(&v1.x);
					}
				glEnd();
					
				if(volrenderview.greenclip_toggle==0) volrenderview.cutting_planes.enable();
			}
			if(b_channel)
			{
				if(volrenderview.blueclip_toggle==0) volrenderview.cutting_planes.disable();

				vol_shaderobj.sendUniform1i("channel", 2);				
				//vol_shaderobj.sendUniform1i("volumeTexture", 0);
				glextensions.glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_BLUE);

				glBegin(GL_TRIANGLE_FAN);
					for(int j=0; j<volrenderview.cutting_planes.view_algined_slices[i].size(); j++)
					{
						v1 = volrenderview.cutting_planes.view_algined_slices[i][j];
						v1 = v1*res;
						t1 = ((v1)+(res/2.0f));
						t1 = t1/res;

						glTexCoord3fv(&t1.x);
						glVertex3fv(&v1.x);
					}
				glEnd();

				if(volrenderview.blueclip_toggle==0) volrenderview.cutting_planes.enable();
			}
		}

		volrenderview.cutting_planes.disable();

		//disable shader object
		vol_shaderobj.disable();

		//reset default to texture unit 0
		glextensions.glActiveTexture(GL_TEXTURE0);

	}

glPopMatrix();
}
void VolumeRender::render_3dTextureMultiPass(void)
{
	glPushMatrix();
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();

		if(volrenderview.projection_toggle==ORTHO_PROJECTION) glTranslatef(-volrenderview.eye.x, -volrenderview.eye.y, -volrenderview.eye.z);
		//if(projection_toggle==0) glTranslatef(0.0, 0.0, -(volrenderview.zoom+285.0f));
		//glTranslatef(volrenderview.eye.x, volrenderview.eye.y, volrenderview.eye.z);

		/*Vector trans = volrenderview.eye;
		trans = trans.mult_matrix(&texrot_mat.m[0]);
		glTranslatef(-trans.x, -trans.y, -trans.z);*/

		//Vector trans = Vector(0,0,0);
		/*trans -= volrenderview.at;

		trans = trans.mult_matrix(&volrenderview.arcball.Transform.M[0]);
		glTranslatef(trans.x, trans.y, trans.z);*/

		//Vector translate = ((float)volobj->maxres/2.0)+volrenderview.eye;
		//Vector trans = ((float)volobj->maxres/2.0)-translate;
		/*
		Matrix4f_t rotmat;
		rotmat = Matrix4fInverse(&volrenderview.arcball.Transform);

		Vector trans = volrenderview.eye;
		trans = trans.mult_matrix(rotmat.M);

		glTranslatef(trans.x, trans.y, trans.z);
		*/
		/*
		Vector translate = ((float)volobj->maxres/2.0)+volrenderview.eye;
		translate = translate.mult_matrix(&volrenderview.arcball.Transform.M[0]);
		Vector trans = ((float)volobj->maxres/2.0)-translate;
		Vector res = Vector(volobj->maxres, volobj->maxres, volobj->maxres);
		Vector texture_translate = trans/res;

		glTranslatef(texture_translate.x, texture_translate.y, texture_translate.z);
		*/

		//printf("volobj->maxres: %d\n", volobj->maxres);

		float worldbb = 10.0;
		v1 = Vector(-volobj->maxres*worldbb, -volobj->maxres*worldbb, 0);
		v2 = Vector( volobj->maxres*worldbb, -volobj->maxres*worldbb, 0);	
		v3 = Vector( volobj->maxres*worldbb,  volobj->maxres*worldbb, 0);
		v4 = Vector(-volobj->maxres*worldbb,  volobj->maxres*worldbb, 0);
		t1 = Vector(0.0-worldbb, 0.0-worldbb, 0);
		t2 = Vector(1.0+worldbb, 0.0-worldbb, 0);
		t3 = Vector(1.0+worldbb, 1.0+worldbb, 0);
		t4 = Vector(0.0-worldbb, 1.0+worldbb, 0);

		if(volobj->texture3d==NULL)
		{
			return;
		}

		int i;

		rebuildwithscaling_texcoords();
		
		glColor3f(1.0,1.0,1.0);

		if(volobj->is_greyscale)
		{
				//printf("singlechannel\n");
				if(volrenderview.redclip_toggle==0 || volrenderview.greenclip_toggle==0 || volrenderview.blueclip_toggle==0) volrenderview.cutting_planes.disable();
				else volrenderview.cutting_planes.enable();

				//if(glextensions.multitexture_support && glextensions.shader_support)
				{
					vol_shaderobj.enable();

					vol_shaderobj.sendUniform1i("channel", 3);
					//vol_shaderobj.sendUniform1i("volumeTexture", 0);
				
					//enable 3 texture units and bind our 3D volume, and the 2 lookup tables
					glextensions.glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);

					//enable 3 texture units and bind our 3D volume, and the 2 lookup tables
					glextensions.glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
					glextensions.glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);

				}
			
				glColor3f(1.0,1.0,1.0);

				//render_VBO();

				/*
				render_VBO_start();
				int index;
				for(i = -numbslices/2; i <numbslices+numbslices/2; i++)
				{
					index = numbslices/2+i;
					render_VBO_range(6*index+0, 6*index+5);
				}
				render_VBO_end();*/

				float offset = -((float)volobj->maxres/2.0);
				float interp;
				float slice_iter = ((float)numbslices*worldbb);

				glBegin(GL_TRIANGLES);
				for(i = -slice_iter*worldbb; i <numbslices+slice_iter*worldbb; i++)
				{
					interp = (float)i/(float)numbslices;
					t1.z = t2.z = t3.z = t4.z = interp;
					v1.z = v2.z = v3.z = v4.z = offset + interp*(float)volobj->maxres;

					glTexCoord3fv(&t1.x);
					glVertex3fv(&v1.x);
					glTexCoord3fv(&t2.x);
					glVertex3fv(&v2.x);
					glTexCoord3fv(&t3.x);
					glVertex3fv(&v3.x);

					glTexCoord3fv(&t1.x);
					glVertex3fv(&v1.x);
					glTexCoord3fv(&t3.x);
					glVertex3fv(&v3.x);
					glTexCoord3fv(&t4.x);
					glVertex3fv(&v4.x);
				}
				glEnd();
			
			

				//if(glextensions.multitexture_support && glextensions.shader_support)
				{
					//disable shader object
					vol_shaderobj.disable();

					//reset default to texture unit 0
					glextensions.glActiveTexture(GL_TEXTURE0);
				}

		}
		else
		{
				//if(glextensions.multitexture_support && glextensions.shader_support)
				{
					vol_shaderobj.enable();

					//enable 3 texture units and bind our 3D volume, and the 2 lookup tables
					glextensions.glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
					glextensions.glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);

					if(detphpeal)
					{
						glextensions.glActiveTexture(GL_TEXTURE3);
						glBindTexture(GL_TEXTURE_2D, TEXTURE2D_FBO_DEPTH);
						glextensions.glActiveTexture(GL_TEXTURE4);
						glBindTexture(GL_TEXTURE_2D, TEXTURE2D_FBO_COL);
					}
				}

				/*	int index;
					for(i = -numbslices/2; i <numbslices+numbslices/2; i++)
					{
						index = numbslices/2+i;

						render_VBO_range(6*index, 6*index+5);
					}*/

			
				volrenderview.cutting_planes.enable();
				//render_VBO();

				float offset = -((float)volobj->maxres/2.0);
				float interp;

				//glBegin(GL_QUADS);
				for(i = -numbslices/2; i <numbslices+numbslices/2; i++)
	//			for(i = numbslices+numbslices/2; i >=-numbslices/2; i--)
				{
					interp = (float)i/(float)numbslices;
					t1.z = t2.z = t3.z = t4.z = interp;
					v1.z = v2.z = v3.z = v4.z = offset + interp*(float)volobj->maxres;

					if(r_channel)
					{
						if(volrenderview.redclip_toggle==0) volrenderview.cutting_planes.disable();

						vol_shaderobj.sendUniform1i("channel", 0);				
						//vol_shaderobj.sendUniform1i("volumeTexture", 0);
						glextensions.glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);

						glBegin(GL_TRIANGLES);
							glTexCoord3fv(&t1.x);
							glVertex3fv(&v1.x);
							glTexCoord3fv(&t2.x);
							glVertex3fv(&v2.x);
							glTexCoord3fv(&t3.x);
							glVertex3fv(&v3.x);

							glTexCoord3fv(&t1.x);
							glVertex3fv(&v1.x);
							glTexCoord3fv(&t3.x);
							glVertex3fv(&v3.x);
							glTexCoord3fv(&t4.x);
							glVertex3fv(&v4.x);
						glEnd();

						if(volrenderview.redclip_toggle==0) volrenderview.cutting_planes.enable();
					}
					if(g_channel)
					{
						if(volrenderview.greenclip_toggle==0) volrenderview.cutting_planes.disable();

						vol_shaderobj.sendUniform1i("channel", 1);			
						//vol_shaderobj.sendUniform1i("volumeTexture", 0);
						glextensions.glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_GREEN);

						glBegin(GL_TRIANGLES);
							glTexCoord3fv(&t1.x);
							glVertex3fv(&v1.x);
							glTexCoord3fv(&t2.x);
							glVertex3fv(&v2.x);
							glTexCoord3fv(&t3.x);
							glVertex3fv(&v3.x);

							glTexCoord3fv(&t1.x);
							glVertex3fv(&v1.x);
							glTexCoord3fv(&t3.x);
							glVertex3fv(&v3.x);
							glTexCoord3fv(&t4.x);
							glVertex3fv(&v4.x);
						glEnd();
					
						if(volrenderview.greenclip_toggle==0) volrenderview.cutting_planes.enable();
					}
					if(b_channel)
					{
						if(volrenderview.blueclip_toggle==0) volrenderview.cutting_planes.disable();

						vol_shaderobj.sendUniform1i("channel", 2);				
						//vol_shaderobj.sendUniform1i("volumeTexture", 0);
						glextensions.glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_BLUE);

						glBegin(GL_TRIANGLES);
							glTexCoord3fv(&t1.x);
							glVertex3fv(&v1.x);
							glTexCoord3fv(&t2.x);
							glVertex3fv(&v2.x);
							glTexCoord3fv(&t3.x);
							glVertex3fv(&v3.x);

							glTexCoord3fv(&t1.x);
							glVertex3fv(&v1.x);
							glTexCoord3fv(&t3.x);
							glVertex3fv(&v3.x);
							glTexCoord3fv(&t4.x);
							glVertex3fv(&v4.x);
						glEnd();

						if(volrenderview.blueclip_toggle==0) volrenderview.cutting_planes.enable();
					}
				}

				volrenderview.cutting_planes.disable();

	//			if(glextensions.multitexture_support && glextensions.shader_support)
				{
					//disable shader object
					vol_shaderobj.disable();

					//reset default to texture unit 0
					glextensions.glActiveTexture(GL_TEXTURE0);
				}

		}

	//	volrenderview.cutting_planes.disable();
	glPopMatrix();

}
void VolumeRender::render_3dTextureSlicePlane_i_bricks(int i, GLenum surface,int mode, int text)
{
	if(volrenderview.cutting_planes.plane_clippoints[i].empty()) return;
/*
	vol_rgbsectionshaderobj.enable();	
	vol_rgbsectionshaderobj.sendUniform1i("channel", mode);	
	vol_rgbsectionshaderobj.sendUniform1i("volumeTexture", text);

	for(int j=0; j<6; j++)
	{
		if(j!=i) volrenderview.cutting_planes.enable(j);
	}

	float maxsize =  max((float)volobj->xsize, (float)volobj->ysize);
	maxsize =  max((float)maxsize, (float)volobj->zsize);
	Vector res = Vector(volobj->texwidth*(volobj->xsize/maxsize), volobj->texheight*(volobj->ysize/maxsize), volobj->texdepth*(volobj->zsize/maxsize));

	float vert_res = (volobj->maxres/BRICKS_SUBDIV);
	float vert_resX = (volobj->texwidth/BRICKS_SUBDIV);
	float vert_resY = (volobj->texheight/BRICKS_SUBDIV);
	float vert_resZ = (volobj->texdepth/BRICKS_SUBDIV);
	
	Vector brickres = Vector(vert_resX, vert_resY, vert_resZ);
	Vector brickreshalf = brickres/2.0f;
	int brick_index;
	Vector brick_centre;
	Vector brick_start;

	for(int ii=0; ii<brick_centres.size(); ii++)
	{
		//get our sorted brick index
		brick_index =  sorted_brick_indices[ii];
		
		if(brick_index==4)
		{

		//get our current brick centre
		brick_centre = brick_centresrot[brick_index];

		brick_start = brick_centres[brick_index]-(brickres/2.0f);
		//printf("brick_start: %f, %f, %f\n", brick_start.x, brick_start.y, brick_start.z);

		//glScalef(vert_resX/2.0, vert_resY/2.0, vert_resZ/2.0);
		//glScalef((volobj->xsize/maxsize), (volobj->ysize/maxsize), (volobj->zsize/maxsize));

		load_3dTexture_brick(brick_centresrot[brick_index].w);

		//enable and attach texture units
		glextensions.glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);
		glEnable(GL_TEXTURE_3D);
		glextensions.glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_GREEN);
		glEnable(GL_TEXTURE_3D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_BLUE);
		glEnable(GL_TEXTURE_3D);
		glextensions.glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_JITTER);
		glEnable(GL_TEXTURE_2D);
		
		glColor3f(1,1,1);
		glBegin(surface);
		for(int j=0; j<volrenderview.cutting_planes.plane_clippoints[i].size(); j++)
		{
			v1 = volrenderview.cutting_planes.plane_clippoints[i][j]-(0.2f*volrenderview.cutting_planes.planes[i].normal);
			//t1 = (v1-brick_centre)/brickres;
			//t1 = v1/res;
			t1 =  (v1 - brick_start)/brickres;
			glTexCoord3fv(&t1.x);
			glVertex3fv(&v1.x);
		}
		glEnd();
		glBegin(surface);
		for(int j=0; j<volrenderview.cutting_planes.plane_clippoints[i].size(); j++)
		{
			v1 = volrenderview.cutting_planes.plane_clippoints[i][j]+(0.2f*volrenderview.cutting_planes.planes[i].normal);
			//t1 = (v1-brick_centre)/brickres;
			//t1 = v1/res;
			t1 =  (v1 - brick_start)/brickres;
			//printf("t1: %f, %f, %f\n", t1.x, t1.y, t1.z);

			glTexCoord3fv(&t1.x);
			glVertex3fv(&v1.x);
		}
		glEnd();

		//disable texture units
		glextensions.glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_3D);
		glextensions.glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_3D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_3D);
		glextensions.glActiveTexture(GL_TEXTURE3);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE4);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE5);
		glDisable(GL_TEXTURE_2D);

		//reset default to texture unit 0
		glextensions.glActiveTexture(GL_TEXTURE0);

		}
	}

	volrenderview.cutting_planes.disable();
	vol_rgbsectionshaderobj.disable();*/	
/*
	//if we dont have any 3d texture return
	if(volobj->texture3d==NULL) return;

	//if we dont have any brick centres or cutting planes, ie: no bricks created return
	if(brick_centres.size()==0) return;

	//some variable helpers for our rendering routine
	Vector brick_centre;
	Vector v1, v2, v3, v4;
	Vector t1, t2, t3, t4;
	float s, offset;
	int brick_index = 0;

	//number of brick subdivisions
	int brick_subdivs=BRICKS_SUBDIV;

	//allocate some storage for our transformed 
	//brick centres
	brick_centresrot.clear();
	brick_centresrot.resize(brick_centres.size());

	//allocate some storage for our sorted brick indices
	sorted_brick_indices.clear();
	sorted_brick_indices.resize(brick_centres.size());

	float vert_res = (volobj->maxres/BRICKS_SUBDIV);
	float vert_resX = (volobj->texwidth/BRICKS_SUBDIV);
	float vert_resY = (volobj->texheight/BRICKS_SUBDIV);
	float vert_resZ = (volobj->texdepth/BRICKS_SUBDIV);

	float maxsize =  max((float)volobj->xsize, (float)volobj->ysize);
	maxsize =  max((float)maxsize, (float)volobj->zsize);

	//geometry for our view-aligned slices
	CVector m_pVertices[8];
	m_pVertices[0] = CVector(-1.0,-1.0,-1.0, 1.0,  0.0, 0.0, 0.0);
	m_pVertices[1] = CVector( 1.0,-1.0,-1.0, 1.0,  1.0, 0.0, 0.0);
	m_pVertices[2] = CVector( 1.0, 1.0,-1.0, 1.0,  1.0, 1.0, 0.0);
	m_pVertices[3] = CVector(-1.0, 1.0,-1.0, 1.0,  0.0, 1.0, 0.0);															
	m_pVertices[4] = CVector(-1.0,-1.0, 1.0, 1.0,  0.0, 0.0, 1.0);
	m_pVertices[5] = CVector( 1.0,-1.0, 1.0, 1.0,  1.0, 0.0, 1.0);
	m_pVertices[6] = CVector( 1.0, 1.0, 1.0, 1.0,  1.0, 1.0, 1.0);
	m_pVertices[7] = CVector(-1.0, 1.0, 1.0, 1.0,  0.0, 1.0, 1.0);
		
	Vector m_pEdges[12];
	m_pEdges[0]  = Vector(0,1);
	m_pEdges[1]  = Vector(1,2);
	m_pEdges[2]  = Vector(2,3);
	m_pEdges[3]  = Vector(3,0);
	m_pEdges[4]  = Vector(0,4);
	m_pEdges[5]  = Vector(1,5);
	m_pEdges[6]  = Vector(2,6);
	m_pEdges[7]  = Vector(3,7);
	m_pEdges[8]  = Vector(4,5);
	m_pEdges[9]  = Vector(5,6);
	m_pEdges[10] = Vector(6,7);
	m_pEdges[11] = Vector(7,4);

	const int m_pEdgeList[8][12] = {
		{ 0,1,5,6,   4,8,11,9,  3,7,2,10 }, // v0 is front
		{ 0,4,3,11,  1,2,6,7,   5,9,8,10 }, // v1 is front
		{ 1,5,0,8,   2,3,7,4,   6,10,9,11}, // v2 is front
		{ 7,11,10,8, 2,6,1,9,   3,0,4,5  }, // v3 is front
		{ 8,5,9,1,   11,10,7,6, 4,3,0,2  }, // v4 is front
		{ 9,6,10,2,  8,11,4,7,  5,0,1,3  }, // v5 is front
		{ 9,8,5,4,   6,1,2,0,   10,7,11,3}, // v6 is front
		{ 10,9,6,5,  7,2,3,1,   11,4,8,0 }  // v7 is front
	}; 

	//sort brick centres by distance to viewer
	sort_bricks();

	glPointSize(4.0);
	glLineWidth(2.0);	
	glDisable(GL_LIGHTING);

	//printf("vert_res: %f\n", vert_res);

	//tranverse each brick
	for(int ij=0; ij<brick_centres.size(); ij++)
	{
		//get our sorted brick index
		brick_index =  sorted_brick_indices[ij];

		//get our current brick centre
		brick_centre = brick_centresrot[brick_index];

		CVector viewVec = CVector(volrenderview.cutting_planes.planes[i].normal.x, volrenderview.cutting_planes.planes[i].normal.y, volrenderview.cutting_planes.planes[i].normal.z);
		Vector c1 = volrenderview.cutting_planes.planes[i].v1 + volrenderview.cutting_planes.planes[i].v2 + volrenderview.cutting_planes.planes[i].v3 + volrenderview.cutting_planes.planes[i].v4;
		c1 /= 4.0;
		Vector c2 = volrenderview.cutting_planes.planes[i]._v1 + volrenderview.cutting_planes.planes[i]._v2 + volrenderview.cutting_planes.planes[i]._v3 + volrenderview.cutting_planes.planes[i]._v4;
		c2 /= 4.0;
		Vector c5 = brick_centres[brick_index];

		Vector c3 = c2 - c1;
		Vector c4 = c2 - c5;

		//float n =  volrenderview.cutting_planes.planes[i].normal.dot(c3);
		float n = c3.length();
		float n2 = c4.length();

		glBegin(GL_POINTS);
			glColor3f(1,0,0);
			glVertex3fv(&c2.x);
			glColor3f(0,1,0);
			glVertex3fv(&c5.x);
		glEnd();

		printf("brick: %d, dist: %f\n", ij, n2);
		//if(empty_brick[brick_centre.w]==0)
		if(n2>0.0 && n2<vert_res)
		{
			glBegin(GL_POINTS);
				glColor3f(1,1,0);
				glVertex3fv(&c5.x);
			glEnd();

			//n *= (float)brick_subdivs;
			glPushMatrix();

				//reset our modelview matrix
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

				//apply volrenderview.camera and transformations
				glTranslatef(tx, ty, 0.0);
				glTranslatef(brick_centre.x, brick_centre.y, brick_centre.z);
				glMultMatrixf(volrenderview.arcball.Transform.M);
				
				glScalef(vert_resX/2.0, vert_resY/2.0, vert_resZ/2.0);
				glScalef((volobj->xsize/maxsize), (volobj->ysize/maxsize), (volobj->zsize/maxsize));

				double dMaxDist = viewVec * m_pVertices[0];
				double dMinDist = dMaxDist;
				int nMaxIdx = 0;
				for(int ij = 1; ij < 8; ++ij) 
				{
					double dist = viewVec * m_pVertices[ij];
					if ( dist > dMaxDist) 
					{
						dMaxDist = dist;
						nMaxIdx = i;
					}
					if ( dist < dMinDist) 
					{
						dMinDist = dist;
					}
				}

				CVector vecStart[12];
				CVector vecDir[12];
				CVector texStart[12];
				CVector texDir[12];

				float lambda[12];
				float lambda_inc[12];
				double denom;

				int m_nNumSlices = vert_res;

				float EPSILON = 0.001;
				dMinDist += EPSILON;
				dMaxDist -= EPSILON;
				double dPlaneDist    =  dMinDist;   
				double dPlaneDistInc =  (dMaxDist-dMinDist)/double(m_nNumSlices);

				for(int ii = 0; ii < 12; ii++) 
				{
					vecStart[ii] = m_pVertices[(int)m_pEdges[m_pEdgeList[nMaxIdx][ii]].x];
					vecDir[ii]   = m_pVertices[(int)m_pEdges[m_pEdgeList[nMaxIdx][ii]].y] - m_pVertices[(int)m_pEdges[m_pEdgeList[nMaxIdx][ii]].x];

					denom = vecDir[ii] *viewVec;

					if (1.0 + denom != 1.0) 
					{
						lambda_inc[ii] =  dPlaneDistInc/denom;
						lambda[ii]     = (dPlaneDist - vecStart[ii] * viewVec)/denom;
					} 
					else
					{
						lambda[ii]     = -1.0;
						lambda_inc[ii] =  0.0;	
					}
				}

			//glPopMatrix();

			//glPushMatrix();
				//glMatrixMode(GL_TEXTURE);
				//glLoadIdentity();

				//glMatrixMode(GL_MODELVIEW);
				//glLoadIdentity();
				
				//upload to gfx current 3d texture brick
				load_3dTexture_brick(brick_centresrot[brick_index].w);
				
				//load_1DLookupTablesBricks(brick_index, m_nNumSlices);
/*
				//enable and attach texture units
				glextensions.glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);
				glEnable(GL_TEXTURE_3D);
				glextensions.glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_GREEN);
				glEnable(GL_TEXTURE_3D);
				glextensions.glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_BLUE);
				glEnable(GL_TEXTURE_3D);
				glextensions.glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
				glEnable(GL_TEXTURE_2D);
				glextensions.glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
				glEnable(GL_TEXTURE_2D);
				glextensions.glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, TEXTURE2D_JITTER);
				glEnable(GL_TEXTURE_2D);

				//enable our GLSL shaders if we support them
				if(volume_render_mode!=1 && glextensions.shader_support==1) 
						vol_shaderobj.enable();

				//glEnable(GL_LIGHTING);
				glEnable(GL_TEXTURE_3D);
				glEnable(GL_BLEND);
				blend_mode(bldmode);

				vol_shaderobj.sendUniform1i("channel", 3);
				vol_shaderobj.sendUniform1i("volumeTexture", 0);
*/				/*	
					CVector intersection[6];
					float lmb[12];


					for(int e = 0; e < 12; e++) 
					{
						lmb[e] = lambda[e] + n*lambda_inc[e];
					}

					if      ((lmb[0] >= 0.0) && (lmb[0] < 1.0)) intersection[0] = vecStart[0] + lmb[0] * vecDir[0];
					else if ((lmb[1] >= 0.0) && (lmb[1] < 1.0)) intersection[0] = vecStart[1] + lmb[1] * vecDir[1];
					else if ((lmb[3] >= 0.0) && (lmb[3] < 1.0)) intersection[0] = vecStart[3] + lmb[3] * vecDir[3];
					else continue;

					if	    ((lmb[2] >= 0.0) && (lmb[2] < 1.0)) intersection[1] = vecStart[2] + lmb[2] * vecDir[2];
					else if ((lmb[0] >= 0.0) && (lmb[0] < 1.0)) intersection[1] = vecStart[0] + lmb[0] * vecDir[0];
					else if ((lmb[1] >= 0.0) && (lmb[1] < 1.0)) intersection[1] = vecStart[1] + lmb[1] * vecDir[1];
					else intersection[1] = vecStart[3] + lmb[3] * vecDir[3];

					if      ((lmb[4] >= 0.0) && (lmb[4] < 1.0)) intersection[2] = vecStart[4] + lmb[4] * vecDir[4];
					else if ((lmb[5] >= 0.0) && (lmb[5] < 1.0)) intersection[2] = vecStart[5] + lmb[5] * vecDir[5];
					else intersection[2] = vecStart[7] + lmb[7] * vecDir[7];
					
					if	    ((lmb[6] >= 0.0) && (lmb[6] < 1.0)) intersection[3] = vecStart[6] + lmb[6] * vecDir[6];
					else if ((lmb[4] >= 0.0) && (lmb[4] < 1.0)) intersection[3] = vecStart[4] + lmb[4] * vecDir[4];
					else if ((lmb[5] >= 0.0) && (lmb[5] < 1.0)) intersection[3] = vecStart[5] + lmb[5] * vecDir[5];
					else intersection[3] = vecStart[7] + lmb[7] * vecDir[7];

					if	    ((lmb[8] >= 0.0) && (lmb[8] < 1.0)) intersection[4] = vecStart[8] + lmb[8] * vecDir[8] ;
					else if ((lmb[9] >= 0.0) && (lmb[9] < 1.0)) intersection[4] = vecStart[9] + lmb[9] * vecDir[9] ;
					else intersection[4] = vecStart[11]+ lmb[11]* vecDir[11];
					
					if	    ((lmb[10]>= 0.0) && (lmb[10]< 1.0)) intersection[5] = vecStart[10]+ lmb[10]* vecDir[10];
					else if ((lmb[8] >= 0.0) && (lmb[8] < 1.0)) intersection[5] = vecStart[8] + lmb[8] * vecDir[8] ;
					else if ((lmb[9] >= 0.0) && (lmb[9] < 1.0)) intersection[5] = vecStart[9] + lmb[9] * vecDir[9] ;
					else intersection[5] = vecStart[11]+ lmb[11]* vecDir[11];
				
					//float c = 1.0f-float(n)/float(m_nNumSlices);
					//glColor3f(c,c,c);
					/*glColor3f(1.0,1.0,1.0);
					glBegin(GL_TRIANGLE_FAN);
					for(int ii = 0; ii < 6; ++ii)
					{
						intersection[ii].glVertex(false,true);
					}
					glEnd();*/
/*
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glColor3f(0.0,1.0,0.0);
					glBegin(GL_TRIANGLE_FAN);
					for(int ii = 0; ii < 6; ++ii)
					{
						intersection[ii].glVertex(false,true);
					}
					glEnd();							
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
/*
					glextensions.glActiveTexture(GL_TEXTURE0);
					glDisable(GL_TEXTURE_3D);
					glextensions.glActiveTexture(GL_TEXTURE1);
					glDisable(GL_TEXTURE_3D);
					glextensions.glActiveTexture(GL_TEXTURE2);
					glDisable(GL_TEXTURE_3D);
					glextensions.glActiveTexture(GL_TEXTURE3);
					glDisable(GL_TEXTURE_2D);
					glextensions.glActiveTexture(GL_TEXTURE4);
					glDisable(GL_TEXTURE_2D);
					glextensions.glActiveTexture(GL_TEXTURE5);
					glDisable(GL_TEXTURE_2D);

					//reset default to texture unit 0
					glextensions.glActiveTexture(GL_TEXTURE0);

					glDisable(GL_TEXTURE_3D);
					glDisable(GL_BLEND);
					glDisable(GL_LIGHTING);

					//disable our GLSL shaders if we support them
					if(volume_render_mode!=1 && glextensions.shader_support==1) 
							vol_shaderobj.disable();
	*/									

			//glPopMatrix();
/*		glPopMatrix();
		}
	}*/
}
void VolumeRender::render_3dTextureSlicePlane_i(int i, GLenum surface,int mode, int text)
{	
	if(i>volrenderview.cutting_planes.plane_clippoints.size()-1) return;
	if(volrenderview.cutting_planes.plane_clippoints[i].empty()) return;

	volrenderview.cutting_planes.enable();
	volrenderview.cutting_planes.disable(i);

	//ENABLE SHADER PROGRAM
	vol_rgbsectionshaderobj.enable();	
	vol_rgbsectionshaderobj.sendUniform1i("channel", mode);	
	vol_rgbsectionshaderobj.sendUniform1i("volumeTexture", text);

	float maxsize =  max((float)volobj->xsize, (float)volobj->ysize);
	maxsize =  max((float)maxsize, (float)volobj->zsize);
	Vector res = Vector(volobj->texwidth*(volobj->xsize/maxsize), volobj->texheight*(volobj->ysize/maxsize), volobj->texdepth*(volobj->zsize/maxsize));
	
	glColor3f(1,1,1);
	glBegin(surface);
	for(int j=0; j<volrenderview.cutting_planes.plane_clippoints[i].size(); j++)
	{
		v1 = volrenderview.cutting_planes.plane_clippoints[i][j];//-(1.0f*volrenderview.cutting_planes.planes[i].normal);
		t1 = v1/res;
		glTexCoord3fv(&t1.x);
		glVertex3fv(&v1.x);
	}
	glEnd();

	//DISABLE SHADER PROGRAM
	vol_rgbsectionshaderobj.disable();
}
void VolumeRender::render_3dTextureSliceCurvedPlane_i(int i, GLenum surface,int mode, int text)
{
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, 0.0);
	glMultMatrixf(volrenderview.arcball.Transform.M);
	//glMatrixMode(GL_TEXTURE);
	//glLoadIdentity();*/

	//------------------------------------
	//First we compute our grid points
	//------------------------------------
	float size = 1.5;

	float maxsize =  max((float)volobj->xsize, (float)volobj->ysize);
	maxsize =  max((float)maxsize, (float)volobj->zsize);
	Vector res = Vector(volobj->texwidth*(volobj->xsize/maxsize), volobj->texheight*(volobj->ysize/maxsize), volobj->texdepth*(volobj->zsize/maxsize));

	Vector v_x = (volrenderview.cutting_planes.planes[i]._v3)*size - (volrenderview.cutting_planes.planes[i]._v2)*size;
	Vector v_y = (volrenderview.cutting_planes.planes[i]._v1)*size - (volrenderview.cutting_planes.planes[i]._v2)*size;

	float n_subdivs = 64.0;
	Vector interp_x = v_x/(n_subdivs-1);
	Vector interp_y = v_y/(n_subdivs-1);

	Vector centre1 = Vector(0,0,0);
	Vector centre2 = Vector(0,0,0);

	vector < Vector > grid_points;
	if(curve_coords==true) gridtexcoords.clear();
	for(int x=0; x<(int)n_subdivs; x++)
	{
		float angx = PI * ((float)(x)/(n_subdivs-1));
		float sx = sin(angx);

		for(int y=0; y<(int)n_subdivs; y++)
		{
			float angy = PI * ((float)(y)/(n_subdivs-1));
			float sy = sin(angy);

			Vector v = volrenderview.cutting_planes.planes[i]._v2*size;

			v += (float)x*interp_x;
			v += (float)y*interp_y;

			v -= curveXang*volrenderview.cutting_planes.planes[i].normal;
			v -= curveYang*volrenderview.cutting_planes.planes[i].normal;
			v += curveXang*(sx)*volrenderview.cutting_planes.planes[i].normal;
			v += curveYang*(sy)*volrenderview.cutting_planes.planes[i].normal;
			
			v -= curve_vectranslate;

			grid_points.push_back(v);
			centre1 += v;
			if(curve_coords==true) gridtexcoords.push_back(v/res);
		}
	}
	
	centre1 /= (float)grid_points.size();

	glDisable(GL_LIGHTING);

	//------------------------------------
	//Next render plane in black
	//------------------------------------
	/*volrenderview.cutting_planes.disable();
	glBegin(GL_TRIANGLES);
	glColor3f(0.0,0.0,0.0);
	for(int x=0; x<(int)n_subdivs-1; x++)
	{
		for(int y=0; y<(int)n_subdivs-1; y++)
		{
			Vector curvev1 = grid_points[x*n_subdivs+y];
			Vector curvev2 = grid_points[(x+1)*n_subdivs+y];
			Vector curvev3 = grid_points[x*n_subdivs+(y+1)];

			glVertex3fv(&curvev1.x);
			glVertex3fv(&curvev2.x);
			glVertex3fv(&curvev3.x);

			curvev1 = grid_points[(x+1)*n_subdivs+y];
			curvev2 = grid_points[(x+1)*n_subdivs+(y+1)];
			curvev3 = grid_points[x*n_subdivs+(y+1)];

			glVertex3fv(&curvev1.x);
			glVertex3fv(&curvev2.x);
			glVertex3fv(&curvev3.x);
		}
	}
	glEnd();

	glPointSize(10.0);

	glColor3f(1,1,0);
	glBegin(GL_POINTS);
	glVertex3fv(&debugv.x);
	glEnd();

	glColor3f(0.0,0.0,0.0);
	glBegin(GL_POINTS);
	glVertex3fv(&volrenderview.cutting_planes.planes[i]._v1.x);
	glVertex3fv(&volrenderview.cutting_planes.planes[i]._v2.x);
	glVertex3fv(&volrenderview.cutting_planes.planes[i]._v3.x);
	glVertex3fv(&volrenderview.cutting_planes.planes[i]._v4.x);
	glEnd();
	glPointSize(1.0);
*/
	//------------------------------------
	//Now the section
	//------------------------------------

	//enable and attach texture units
	glextensions.glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_GREEN);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_BLUE);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
	glEnable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
	glEnable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_JITTER);
	glEnable(GL_TEXTURE_2D);

	//Turn blending off
	glDisable(GL_BLEND);

	//ENABLE SHADER PROGRAM
	//if(volobj->is_greyscale)
	{
		vol_rgbsectionshaderobj.enable();	
		
		vol_rgbsectionshaderobj.sendUniform1i("channel", mode);	
		vol_rgbsectionshaderobj.sendUniform1i("volumeTexture", text);
	}
	/*else
	{
		vol_sectionshaderobj.enable();
	}*/

	/*for(int j=0; j<6; j++)
	{
		if(j!=i) volrenderview.cutting_planes.enable(j);
	}*/

	Vector curvev1, curvev2, curvev3, curvet1, curvet2, curvet3;

	if(curve_translate==0)
	{
	glBegin(GL_TRIANGLES);
	glColor3f(1.0,1.0,1.0);
	for(int x=0; x<(int)n_subdivs-1; x++)
	{
		for(int y=0; y<(int)n_subdivs-1; y++)
		{
			curvev1 = grid_points[x*n_subdivs+y];
			curvev2 = grid_points[(x+1)*n_subdivs+y];
			curvev3 = grid_points[x*n_subdivs+(y+1)];	
			curvet1 = gridtexcoords[x*n_subdivs+y];
			curvet2 = gridtexcoords[(x+1)*n_subdivs+y];
			curvet3 = gridtexcoords[x*n_subdivs+(y+1)];

			//glColor3f(1.0,0.0,0.0);
			//glBegin(GL_LINE_LOOP);
			glTexCoord3fv(&curvet1.x);
			glVertex3fv(&curvev1.x);
			glTexCoord3fv(&curvet2.x);
			glVertex3fv(&curvev2.x);
			glTexCoord3fv(&curvet3.x);
			glVertex3fv(&curvev3.x);
			//glEnd();

			curvev1 = grid_points[(x+1)*n_subdivs+y];
			curvev2 = grid_points[(x+1)*n_subdivs+(y+1)];
			curvev3 = grid_points[x*n_subdivs+(y+1)];
			curvet1 = gridtexcoords[(x+1)*n_subdivs+y];
			curvet2 = gridtexcoords[(x+1)*n_subdivs+(y+1)];
			curvet3 = gridtexcoords[x*n_subdivs+(y+1)];
			

			//glColor3f(0.0,1.0,0.0);
			//glBegin(GL_LINE_LOOP);
			glTexCoord3fv(&curvet1.x);
			glVertex3fv(&curvev1.x);
			glTexCoord3fv(&curvet2.x);
			glVertex3fv(&curvev2.x);
			glTexCoord3fv(&curvet3.x);
			glVertex3fv(&curvev3.x);
			//glEnd();

		}
	}
	glEnd();
	}

	volrenderview.cutting_planes.disable();

	//DISABLE SHADER PROGRAM
	//if(volobj->is_greyscale)
	{
		vol_rgbsectionshaderobj.disable();	
	}
	/*else
	{
		vol_sectionshaderobj.disable();
	}*/

	//Turn blending back on
	//glEnable(GL_BLEND);

	//disable texture units
	glextensions.glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE3);
	glDisable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE4);
	glDisable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE5);
	glDisable(GL_TEXTURE_2D);

	//reset default to texture unit 0
	glextensions.glActiveTexture(GL_TEXTURE0);

	volrenderview.cutting_planes.disable();
	if(curve_translate==0)
	{
	//glBegin(GL_TRIANGLES);
	glColor3f(1.0,1.0,1.0);
	for(int x=0; x<(int)n_subdivs-1; x++)
	{
		for(int y=0; y<(int)n_subdivs-1; y++)
		{
			curvev1 = grid_points[x*n_subdivs+y];
			curvev2 = grid_points[(x+1)*n_subdivs+y];
			curvev3 = grid_points[x*n_subdivs+(y+1)];	
			curvet1 = gridtexcoords[x*n_subdivs+y];
			curvet2 = gridtexcoords[(x+1)*n_subdivs+y];
			curvet3 = gridtexcoords[x*n_subdivs+(y+1)];

			/*curvev1 -= t;
			curvev2 -= t;
			curvev3 -= t;*/

			//glColor3f(1.0,1.0,1.0);
			glBegin(GL_LINE_LOOP);
			glTexCoord3fv(&curvet1.x);
			glVertex3fv(&curvev1.x);
			glTexCoord3fv(&curvet2.x);
			glVertex3fv(&curvev2.x);
			/*glTexCoord3fv(&curvet3.x);
			glVertex3fv(&curvev3.x);*/
			glEnd();

			curvev1 = grid_points[(x+1)*n_subdivs+y];
			curvev2 = grid_points[(x+1)*n_subdivs+(y+1)];
			curvev3 = grid_points[x*n_subdivs+(y+1)];
			curvet1 = gridtexcoords[(x+1)*n_subdivs+y];
			curvet2 = gridtexcoords[(x+1)*n_subdivs+(y+1)];
			curvet3 = gridtexcoords[x*n_subdivs+(y+1)];
			
			/*curvev1 -= t;
			curvev2 -= t;
			curvev3 -= t;*/

			//glColor3f(0.0,1.0,0.0);
			glBegin(GL_LINE_LOOP);
			glTexCoord3fv(&curvet1.x);
			glVertex3fv(&curvev1.x);
			glTexCoord3fv(&curvet2.x);
			glVertex3fv(&curvev2.x);
			/*glTexCoord3fv(&curvet3.x);
			glVertex3fv(&curvev3.x);*/
			glEnd();

		}
	}
	//glEnd();
	}

	//enable and attach texture units
	glextensions.glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_GREEN);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_BLUE);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
	glEnable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
	glEnable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_JITTER);
	glEnable(GL_TEXTURE_2D);

	//Turn blending off
	glDisable(GL_BLEND);

	//ENABLE SHADER PROGRAM
	//if(volobj->is_greyscale)
	{
		vol_rgbsectionshaderobj.enable();	
		vol_rgbsectionshaderobj.sendUniform1i("channel", mode);	
		vol_rgbsectionshaderobj.sendUniform1i("volumeTexture", text);
	}
	vector< vector< float  > > xdist;
	vector< vector< float  > > ydist;

	xdist.resize(n_subdivs);
	ydist.resize(n_subdivs);
	for(int x=0; x<(int)n_subdivs; x++)
	{
		xdist[x].resize(n_subdivs);
		ydist[x].resize(n_subdivs);
	}

	float xl, yl;
	if(curve_coords==false)
	{
		for(int x=0; x<(int)n_subdivs; x++)
		{
			for(int y=0; y<(int)n_subdivs; y++)
			{			
				curvev1 = grid_points[x*n_subdivs+y];
				if(x!=n_subdivs-1) curvev2 = grid_points[(x+1)*n_subdivs+y];
				if(y!=n_subdivs-1) curvev3 = grid_points[x*n_subdivs+(y+1)];

				if(x==0 || x==n_subdivs-1)
				{
					xdist[x][y] = 0.0;
				}
				else
				{
					Vector v = curvev2 - curvev1;
					xdist[x][y] = v.length();
				}

				if(y==0 || y==n_subdivs-1)
				{
					ydist[x][y] = 0.0;
				}
				else
				{
					Vector v = curvev3 - curvev1;
					ydist[x][y] = v.length();
				}

				//cout<<"(xdist, ydist): "<<xdist[x][y]<<" "<<ydist[x][y]<<endl;
			}
		}

		interp_x.normalize();
		interp_y.normalize();

		Vector xdir = interp_x;
		Vector ydir = interp_y;

		grid_points.clear();
		for(int x=0; x<(int)n_subdivs; x++)
		{
			for(int y=0; y<(int)n_subdivs; y++)
			{
				//Vector v = Vector(0,0,0);
				Vector v = volrenderview.cutting_planes.planes[i]._v2*size;

				//v += (float)x*interp_x;

				for(int i=0; i<x; i++) v += xdist[i][y]*xdir;
				for(int j=0; j<y; j++) v += ydist[x][j]*ydir;

//				v.x += xdist[x][y];
//				v.y += ydist[x][y];

				v -= curve_vectranslate;
				
				grid_points.push_back(v);
				centre2 += v;
			}
		}

		centre2 /= (float)grid_points.size();
	}

	Vector t = centre2 - centre1;

	if(curve_translate==1)
	{
	glBegin(GL_TRIANGLES);
	glColor3f(1.0,1.0,1.0);
	for(int x=0; x<(int)n_subdivs-1; x++)
	{
		for(int y=0; y<(int)n_subdivs-1; y++)
		{
			curvev1 = grid_points[x*n_subdivs+y];
			curvev2 = grid_points[(x+1)*n_subdivs+y];
			curvev3 = grid_points[x*n_subdivs+(y+1)];	
			curvet1 = gridtexcoords[x*n_subdivs+y];
			curvet2 = gridtexcoords[(x+1)*n_subdivs+y];
			curvet3 = gridtexcoords[x*n_subdivs+(y+1)];

			curvev1 -= t;
			curvev2 -= t;
			curvev3 -= t;

			//glColor3f(1.0,1.0,1.0);
			//glBegin(GL_LINE_LOOP);
			glTexCoord3fv(&curvet1.x);
			glVertex3fv(&curvev1.x);
			glTexCoord3fv(&curvet2.x);
			glVertex3fv(&curvev2.x);
			glTexCoord3fv(&curvet3.x);
			glVertex3fv(&curvev3.x);
			//glEnd();

			curvev1 = grid_points[(x+1)*n_subdivs+y];
			curvev2 = grid_points[(x+1)*n_subdivs+(y+1)];
			curvev3 = grid_points[x*n_subdivs+(y+1)];
			curvet1 = gridtexcoords[(x+1)*n_subdivs+y];
			curvet2 = gridtexcoords[(x+1)*n_subdivs+(y+1)];
			curvet3 = gridtexcoords[x*n_subdivs+(y+1)];
			
			curvev1 -= t;
			curvev2 -= t;
			curvev3 -= t;

			//glColor3f(0.0,1.0,0.0);
			//glBegin(GL_LINE_LOOP);
			glTexCoord3fv(&curvet1.x);
			glVertex3fv(&curvev1.x);
			glTexCoord3fv(&curvet2.x);
			glVertex3fv(&curvev2.x);
			glTexCoord3fv(&curvet3.x);
			glVertex3fv(&curvev3.x);
			//glEnd();

		}
	}
	glEnd();
	}


	volrenderview.cutting_planes.disable();

	//DISABLE SHADER PROGRAM
	//if(volobj->is_greyscale)
	{
		vol_rgbsectionshaderobj.disable();	
	}
	/*else
	{
		vol_sectionshaderobj.disable();
	}*/

	//Turn blending back on
	//glEnable(GL_BLEND);

	//disable texture units
	glextensions.glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE3);
	glDisable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE4);
	glDisable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE5);
	glDisable(GL_TEXTURE_2D);

	//reset default to texture unit 0
	glextensions.glActiveTexture(GL_TEXTURE0);

	volrenderview.cutting_planes.disable();


	if(curve_translate==1)
	{
	//glBegin(GL_TRIANGLES);
	glColor3f(1.0,1.0,1.0);
	for(int x=0; x<(int)n_subdivs-1; x++)
	{
		for(int y=0; y<(int)n_subdivs-1; y++)
		{
			curvev1 = grid_points[x*n_subdivs+y];
			curvev2 = grid_points[(x+1)*n_subdivs+y];
			curvev3 = grid_points[x*n_subdivs+(y+1)];	
			curvet1 = gridtexcoords[x*n_subdivs+y];
			curvet2 = gridtexcoords[(x+1)*n_subdivs+y];
			curvet3 = gridtexcoords[x*n_subdivs+(y+1)];

			curvev1 -= t;
			curvev2 -= t;
			curvev3 -= t;

			//glColor3f(1.0,1.0,1.0);
			glBegin(GL_LINES);
			glTexCoord3fv(&curvet1.x);
			glVertex3fv(&curvev1.x);
			glTexCoord3fv(&curvet2.x);
			glVertex3fv(&curvev2.x);
			/*glTexCoord3fv(&curvet3.x);
			glVertex3fv(&curvev3.x);*/
			glEnd();

			curvev1 = grid_points[(x+1)*n_subdivs+y];
			curvev2 = grid_points[(x+1)*n_subdivs+(y+1)];
			curvev3 = grid_points[x*n_subdivs+(y+1)];
			curvet1 = gridtexcoords[(x+1)*n_subdivs+y];
			curvet2 = gridtexcoords[(x+1)*n_subdivs+(y+1)];
			curvet3 = gridtexcoords[x*n_subdivs+(y+1)];
			
			curvev1 -= t;
			curvev2 -= t;
			curvev3 -= t;

			//glColor3f(0.0,1.0,0.0);
			glBegin(GL_LINES);
			glTexCoord3fv(&curvet1.x);
			glVertex3fv(&curvev1.x);
			glTexCoord3fv(&curvet2.x);
			glVertex3fv(&curvev2.x);
			/*glTexCoord3fv(&curvet3.x);
			glVertex3fv(&curvev3.x);*/
			glEnd();

		}
	}
	//glEnd();
	}
	glPopMatrix();
}
void VolumeRender::render_measure_patch(int mode, int text)
{
	glPushMatrix();
	volrenderview.cutting_planes.disable();

	float maxsize =  max((float)volobj->xsize, (float)volobj->ysize);
	maxsize =  max((float)maxsize, (float)volobj->zsize);
	Vector res = Vector(volobj->texwidth*(volobj->xsize/maxsize), volobj->texheight*(volobj->ysize/maxsize), volobj->texdepth*(volobj->zsize/maxsize));

	//apply offset to centre texture matrix
//	glMatrixMode(GL_TEXTURE);
//	glTranslatef(0.5,0.5,0.5);

	glEnable(GL_BLEND);
	blend_mode(bldmode);

	//return to modelview mode
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMultMatrixf(volrenderview.arcball.Transform.M);
	glTranslatef(volrenderview.eye.x, volrenderview.eye.y, volrenderview.eye.z);
	
	//enable and attach texture units
	glextensions.glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_GREEN);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_BLUE);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
	glEnable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
	glEnable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_JITTER);
	glEnable(GL_TEXTURE_2D);

	//Turn blending off
	glDisable(GL_BLEND);

	//ENABLE SHADER PROGRAM
	vol_rgbsectionshaderobj.enable();	
	vol_rgbsectionshaderobj.sendUniform1i("channel", mode);	
	vol_rgbsectionshaderobj.sendUniform1i("volumeTexture", text);

	//glColor3f(1,1,1);
	//if(current_measure_drawmode==10) currentMeasureObject.display_beizer_patch_texture(res);

	//DISABLE SHADER PROGRAM
	vol_rgbsectionshaderobj.disable();	

	//disable texture units
	glextensions.glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE3);
	glDisable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE4);
	glDisable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE5);
	glDisable(GL_TEXTURE_2D);

	//reset default to texture unit 0
	glextensions.glActiveTexture(GL_TEXTURE0);

	glPopMatrix();
}
void VolumeRender::render_3dTextureMeasurements(int mode, int text)
{
	glPushMatrix();

	glMatrixMode(GL_TEXTURE);
	glTranslatef(0.5,0.5,0.5);

	//return to modelview mode
	glMatrixMode(GL_MODELVIEW);

	glDisable(GL_BLEND);

	//enable and attach texture units
	glextensions.glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_GREEN);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_BLUE);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
	glEnable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
	glEnable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_JITTER);
	glEnable(GL_TEXTURE_2D);

	rebuild_texcoords();

	vol_rgbsectionshaderobj.enable();	
	vol_rgbsectionshaderobj.sendUniform1i("channel", mode);	
	vol_rgbsectionshaderobj.sendUniform1i("volumeTexture", text);

	float maxsize =  max((float)volobj->xsize, (float)volobj->ysize);
	maxsize =  max((float)maxsize, (float)volobj->zsize);
	Vector res = Vector(volobj->texwidth*(volobj->xsize/maxsize), volobj->texheight*(volobj->ysize/maxsize), volobj->texdepth*(volobj->zsize/maxsize));

	glColor3f(1,1,1);
	/*for(int i=0; i<measures.size(); i++)
	{
		if(i!=current_measure_index && measure_objects_visible[i]==true)
		{
			for(int j=0; j<measures[i].measure_objects.size(); j++)
			{
				glBegin(GL_POLYGON);
				for(int k=0; k<measures[i].measure_objects[j].size(); k++)
				{
					v1 = measure_objects[i][measure_faces[i][j][k]];
					t1 = v1/res;
					glTexCoord3fv(&t1.x);
					glVertex3fv(&v1.x);
				}
				glEnd();
			}
		}
	}
*/
	vol_rgbsectionshaderobj.disable();	

	//disable texture units
	glextensions.glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE3);
	glDisable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE4);
	glDisable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE5);
	glDisable(GL_TEXTURE_2D);

	//reset default to texture unit 0
	glextensions.glActiveTexture(GL_TEXTURE0);

	//volrenderview.cutting_planes.disable();

	glPopMatrix();
}
void VolumeRender::render_3dTextureSlice1(void)
{
	//if we have no data return
	if(volobj->texture3d==NULL) return;

	int i;
	
	glPushMatrix();

	glColor3f(0,0,0);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_BLEND);
	glPolygonOffset(1.0, 2.0f);
	for(int j=0; j<6; j++)
	{
		i = volrenderview.cutting_planes.viewsorted[j];
		volrenderview.cutting_planes.enable();
		volrenderview.cutting_planes.disable(i);

		if(volrenderview.plane_rendermode[i]==2)
		{
			glBegin(GL_TRIANGLE_FAN);
			for(int j=0; j<volrenderview.cutting_planes.plane_clippoints[i].size(); j++)
			{
				v1 = volrenderview.cutting_planes.plane_clippoints[i][j];//-(0.1f*volrenderview.cutting_planes.planes[i].normal);
				//v1 = volrenderview.cutting_planes.plane_clippoints[i][j]-((1.0f*res)*volrenderview.cutting_planes.planes[i].normal);

				glVertex3fv(&v1.x);
			}
			glEnd();
		}
	}
	glDisable(GL_POLYGON_OFFSET_FILL);		
	volrenderview.cutting_planes.disable();
	glPopMatrix();
}
void VolumeRender::render_3dTextureSlice(void)
{
	//if we have no data return
	if(volobj->texture3d==NULL) return;
	
	glPushMatrix();

	//apply offset to centre texture matrix
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glTranslatef(0.5,0.5,0.5);

	//return to modelview mode
	glMatrixMode(GL_MODELVIEW);
	load_1DLookupTables(false);

	//volrenderview.cutting_planes.enable();
	Vector eye = Vector(0,0,-1);		
	eye = eye.mult_matrix((float*)&texrot_mat.m[0]);
	volrenderview.cutting_planes.viewsort(eye);

	glextensions.glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_RED);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_GREEN);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, volobj->TEXTURE3D_BLUE);
	glEnable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
	glEnable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
	glEnable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, TEXTURE2D_JITTER);
	glEnable(GL_TEXTURE_2D);

	//DRAW THE CLIPPING PLANES
	int i;
	for(int j=0; j<6; j++)
	{
		if(!volrenderview.cutting_planes.viewsorted.empty()) i = volrenderview.cutting_planes.viewsorted[j];
		else i = j;

		if(volrenderview.plane_rendermode[i]==0)
		{
			//------------------
			//off
			//------------------
			glDisable(GL_BLEND);
		}
		else if(volrenderview.plane_rendermode[i]==1)
		{
			//------------------
			//wireframe
			//------------------
			glDisable(GL_BLEND);
			volrenderview.cutting_planes.display(i, GL_LINE_LOOP);
		}
		else if(volrenderview.plane_rendermode[i]==2)
		{
			//------------------
			//texture mapped
			//------------------
			render_3dTextureSlice1();

			glEnable(GL_BLEND);
			if(volume_render_mode==4)
			{
				if(volobj->is_greyscale) render_3dTextureSlicePlane_i_bricks(i, GL_TRIANGLE_FAN, 3, 0);
				else
				{
					if(volrenderview.redclip_toggle) render_3dTextureSlicePlane_i_bricks(i, GL_TRIANGLE_FAN, 0, 0);
					if(volrenderview.greenclip_toggle) render_3dTextureSlicePlane_i_bricks(i, GL_TRIANGLE_FAN, 1, 1);
					if(volrenderview.blueclip_toggle) render_3dTextureSlicePlane_i_bricks(i, GL_TRIANGLE_FAN, 2, 2);
				}
			}
			else
			{
				if(volobj->is_greyscale) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 3, 0);
				else
				{
					if(volrenderview.redclip_toggle) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 0, 0);
					if(volrenderview.greenclip_toggle) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 1, 1);
					if(volrenderview.blueclip_toggle) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 2, 2);
				}
			}

		}
		else if(volrenderview.plane_rendermode[i]==3)
		{
			//------------------
			//texture alpha masked
			//------------------
			glEnable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.0);

			if(volume_render_mode==4)
			{
				if(volobj->is_greyscale) render_3dTextureSlicePlane_i_bricks(i, GL_TRIANGLE_FAN, 3, 0);
				else
				{
					if(volrenderview.redclip_toggle) render_3dTextureSlicePlane_i_bricks(i, GL_TRIANGLE_FAN, 0, 0);
					if(volrenderview.greenclip_toggle) render_3dTextureSlicePlane_i_bricks(i, GL_TRIANGLE_FAN, 1, 1);
					if(volrenderview.blueclip_toggle) render_3dTextureSlicePlane_i_bricks(i, GL_TRIANGLE_FAN, 2, 2);
				}
			}
			else
			{
				glPushMatrix();

				glextensions.glBlendEquationEXT(GL_FUNC_ADD_EXT);
				glBlendFunc(GL_ZERO, GL_ZERO);

				if(volobj->is_greyscale) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 3, 0);
				else
				{
					if(volrenderview.redclip_toggle) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 0, 0);
					if(volrenderview.greenclip_toggle) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 1, 1);
					if(volrenderview.blueclip_toggle) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 2, 2);
				}
				
				blend_mode(bldmode);
		
				if(volobj->is_greyscale) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 3, 0);
				else
				{
					if(volrenderview.redclip_toggle) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 0, 0);
					if(volrenderview.greenclip_toggle) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 1, 1);
					if(volrenderview.blueclip_toggle) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 2, 2);
				}
				
				glPopMatrix();
			}

			glDisable(GL_ALPHA_TEST);
		}
		else if(volrenderview.plane_rendermode[i]==4)
		{
			glEnable(GL_BLEND);
			blend_mode(bldmode);

			if(volume_render_mode==4)
			{
				//texture solid
				if(volobj->is_greyscale) render_3dTextureSlicePlane_i_bricks(i, GL_TRIANGLE_FAN, 3, 0);
				else
				{
					if(volrenderview.redclip_toggle) render_3dTextureSlicePlane_i_bricks(i, GL_TRIANGLE_FAN, 0, 0);
					if(volrenderview.greenclip_toggle) render_3dTextureSlicePlane_i_bricks(i, GL_TRIANGLE_FAN, 1, 1);
					if(volrenderview.blueclip_toggle) render_3dTextureSlicePlane_i_bricks(i, GL_TRIANGLE_FAN, 2, 2);
				}
			}
			else
			{
				//texture solid
				if(volobj->is_greyscale) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 3, 0);
				else
				{
					if(volrenderview.redclip_toggle) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 0, 0);
					if(volrenderview.greenclip_toggle) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 1, 1);
					if(volrenderview.blueclip_toggle) render_3dTextureSlicePlane_i(i, GL_TRIANGLE_FAN, 2, 2);
				}
			}

		}
	}

	//disable texture units
	glextensions.glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_3D);
	glextensions.glActiveTexture(GL_TEXTURE3);
	glDisable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE4);
	glDisable(GL_TEXTURE_2D);
	glextensions.glActiveTexture(GL_TEXTURE5);
	glDisable(GL_TEXTURE_2D);

	//reset default to texture unit 0
	glextensions.glActiveTexture(GL_TEXTURE0);	
//	glDisable(GL_POLYGON_OFFSET_LINE);		
	load_1DLookupTables(true);

	//volrenderview.cutting_planes.disable();
	glPopMatrix();
}

void VolumeRender::render_2dMultiTextureMultiPass(void)
{
	if(!texNames1) return;
	if(!texNames2) return;
	if(!texNames3) return;

	//Convert from euler angles to rotation matrix
 	glPushMatrix();

	//switch back to modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glTranslatef(tx, ty, 0.0);

	// Apply our volrenderview.arcball transformation matrix
	glMultMatrixf(volrenderview.arcball.Transform.M);

	//get the transformation matrix
	GLfloat m[4][4];
 	glGetFloatv(GL_MODELVIEW_MATRIX, &m[0][0]);

	//work out our view direction
	//ie: which stack we need to display
	a1 = (m[0][2]-m[0][3])/(m[3][2]-m[3][3]);
	a2 = (m[1][2]-m[1][3])/(m[3][2]-m[3][3]);
	a3 = (m[2][2]-m[2][3])/(m[3][2]-m[3][3]);

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glColor3f(1,1,1);

	vol2dmulti_shaderobj.enable();
	
	rebuild_texcoords();

	if ( (fabs(a1)>fabs(a2)) && (fabs(a1)>fabs(a3)) ) render_2dMultiTextureMultiPass_stack3(a1);
	else if ( (fabs(a2)>fabs(a1)) && (fabs(a2)>fabs(a3)) ) render_2dMultiTextureMultiPass_stack2(a2);
	else render_2dMultiTextureMultiPass_stack1(a3);

	glextensions.glActiveTexture(GL_TEXTURE0);

	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	glPopMatrix();
}
void VolumeRender::render_2dMultiTextureMultiPass_stack1(GLfloat dir)
{
	//printf("1\n");
  if (dir < 0)
  {
	double dZpos = -1.0;
	double dZstep = 2.0/double(numbslices);

	double dAlpha;
	double dTexPos;
	int texIndex;
	double dZpos2;

	for(int slices=0; slices<numbslices; ++slices)
	{
	    dTexPos = double(volobj->texdepth)*(dZpos+1.0)/2.0;
		texIndex = int(dTexPos);
		dAlpha = dTexPos/(double)volobj->texdepth;//-double(texIndex);
		dZpos2 = (dZpos/2.0)*volobj->texdepth;

		glextensions.glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texNames1[texIndex]);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,texNames1[texIndex+1]);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
		glEnable(GL_TEXTURE_2D);

		if(volobj->is_greyscale)
		{
			vol2dmulti_shaderobj.sendUniform1i("channel", 3);
			glBegin(GL_QUADS);
			glTexCoord3d(0.0, 0.0, dAlpha);
			glVertex3f(minX, minY, dZpos2);
			glTexCoord3d(0.0, 1.0, dAlpha);
			glVertex3f(minX, maxY, dZpos2);
			glTexCoord3d(1.0, 1.0, dAlpha);
			glVertex3f(maxX, maxY, dZpos2);
			glTexCoord3d(1.0, 0.0, dAlpha);
			glVertex3f(maxX, minY, dZpos2);
			glEnd();	 
		}
		else
		{
			if(r_channel)
			{
				if(volrenderview.redclip_toggle==0) volrenderview.cutting_planes.disable();
				else volrenderview.cutting_planes.enable();

				vol2dmulti_shaderobj.sendUniform1i("channel", 0);
				glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(minX, minY, dZpos2);
				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(minX, maxY, dZpos2);
				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(maxX, maxY, dZpos2);
				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(maxX, minY, dZpos2);
				glEnd();	 

			}
			if(g_channel)
			{
				if(volrenderview.greenclip_toggle==0) volrenderview.cutting_planes.disable();
				else volrenderview.cutting_planes.enable();

				vol2dmulti_shaderobj.sendUniform1i("channel", 1);
				glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(minX, minY, dZpos2);
				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(minX, maxY, dZpos2);
				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(maxX, maxY, dZpos2);
				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(maxX, minY, dZpos2);
				glEnd();	 

			}
			if(b_channel)
			{

				if(volrenderview.blueclip_toggle==0) volrenderview.cutting_planes.disable();
				else volrenderview.cutting_planes.enable();

				vol2dmulti_shaderobj.sendUniform1i("channel", 2);
				glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(minX, minY, dZpos2);
				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(minX, maxY, dZpos2);
				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(maxX, maxY, dZpos2);
				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(maxX, minY, dZpos2);
				glEnd();	 
			}
		}
		glextensions.glActiveTexture(GL_TEXTURE3);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_2D);

		glextensions.glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);

		glextensions.glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
	
		dZpos += dZstep;
	}
  }
  else
  {
//	int numbslices = volobj->texdepth;
	float dZpos = 1;
	float dZstep = 2.0/double(numbslices);

	float dAlpha;
	float dTexPos;
	int texIndex;
	float dZpos2;

	for(int slices=numbslices; slices>0; --slices)
	{
	    dTexPos = ((double)volobj->texdepth*(dZpos+1.0)/2.0);
		texIndex = int(dTexPos);
		dAlpha = ((double)volobj->texdepth-dTexPos)/(double)volobj->texdepth;//-double(texIndex);
		dZpos2 = (dZpos/2.0)*volobj->texdepth;

		glextensions.glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texNames1[texIndex]);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,texNames1[texIndex-1]);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
		glEnable(GL_TEXTURE_2D);

		if(volobj->is_greyscale)
		{
			vol2dmulti_shaderobj.sendUniform1i("channel", 3);
			glBegin(GL_QUADS);
			glTexCoord3d(0.0, 0.0, dAlpha);
			glVertex3f(minX, minY, dZpos2);

			glTexCoord3d(0.0, 1.0, dAlpha);
			glVertex3f(minX, maxY, dZpos2);

			glTexCoord3d(1.0, 1.0, dAlpha);
			glVertex3f(maxX, maxY, dZpos2);

			glTexCoord3d(1.0, 0.0, dAlpha);
			glVertex3f(maxX, minY, dZpos2);
			glEnd();	 
		}
		else
		{
			if(r_channel)
			{
				if(volrenderview.redclip_toggle==0) volrenderview.cutting_planes.disable();
				else volrenderview.cutting_planes.enable();

				vol2dmulti_shaderobj.sendUniform1i("channel", 0);
				glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(minX, minY, dZpos2);
				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(minX, maxY, dZpos2);
				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(maxX, maxY, dZpos2);
				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(maxX, minY, dZpos2);
				glEnd();	 

			}
			if(g_channel)
			{
				if(volrenderview.greenclip_toggle==0) volrenderview.cutting_planes.disable();
				else volrenderview.cutting_planes.enable();

				vol2dmulti_shaderobj.sendUniform1i("channel", 1);
				glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(minX, minY, dZpos2);
				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(minX, maxY, dZpos2);
				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(maxX, maxY, dZpos2);
				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(maxX, minY, dZpos2);
				glEnd();	 

			}
			if(b_channel)
			{

				if(volrenderview.blueclip_toggle==0) volrenderview.cutting_planes.disable();
				else volrenderview.cutting_planes.enable();

				vol2dmulti_shaderobj.sendUniform1i("channel", 2);
				glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(minX, minY, dZpos2);
				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(minX, maxY, dZpos2);
				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(maxX, maxY, dZpos2);
				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(maxX, minY, dZpos2);
				glEnd();	 
			}
		}
		glextensions.glActiveTexture(GL_TEXTURE3);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
	
		dZpos -= dZstep;
	}
  }
}
void VolumeRender::render_2dMultiTextureMultiPass_stack2(GLfloat dir)
{
	//printf("2\n");
 if (dir < 0)
  {
//	int numbslices = volobj->texheight;
	float dYpos = -1;
	float dYstep = 2.0/double(numbslices);

	float dAlpha;
	float dTexPos;
	int texIndex;
	float dYpos2;

	for(int slices=0; slices<numbslices; ++slices)
	{
	    dTexPos = ((double)volobj->texheight*(dYpos+1.0)/2.0);
		texIndex = int(dTexPos);
		dAlpha = dTexPos/(double)volobj->texheight; //dTexPos-double(texIndex);
		dYpos2 = (dYpos/2.0)*volobj->texheight;

		glextensions.glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texNames2[texIndex]);
		glEnable(GL_TEXTURE_2D);

		glextensions.glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,texNames2[texIndex-1]);
		glEnable(GL_TEXTURE_2D);

		glextensions.glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
		glEnable(GL_TEXTURE_2D);

		glextensions.glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
		glEnable(GL_TEXTURE_2D);

		if(volobj->is_greyscale)
		{
			vol2dmulti_shaderobj.sendUniform1i("channel", 3);

			glBegin(GL_QUADS);
			glTexCoord3d(0.0, 0.0, dAlpha);
			glVertex3f(minX,dYpos2, minZ);
			glTexCoord3d(0.0, 1.0, dAlpha);
			glVertex3f(minX, dYpos2,maxZ);
			glTexCoord3d(1.0, 1.0, dAlpha);
			glVertex3f(maxX, dYpos2,maxZ);
			glTexCoord3d(1.0, 0.0, dAlpha);
			glVertex3f(maxX, dYpos2,minZ);
			glEnd();	 
		}
		else
		{
			if(r_channel)
			{
				if(volrenderview.redclip_toggle==0) volrenderview.cutting_planes.disable();
				else volrenderview.cutting_planes.enable();

				vol2dmulti_shaderobj.sendUniform1i("channel", 0);
				glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(minX,dYpos2, minZ);
				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(minX, dYpos2,maxZ);
				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(maxX, dYpos2,maxZ);
				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(maxX, dYpos2,minZ);
				glEnd();	 

			}
			if(g_channel)
			{
				if(volrenderview.greenclip_toggle==0) volrenderview.cutting_planes.disable();
				else volrenderview.cutting_planes.enable();

				vol2dmulti_shaderobj.sendUniform1i("channel", 1);
				glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(minX,dYpos2, minZ);
				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(minX, dYpos2,maxZ);
				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(maxX, dYpos2,maxZ);
				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(maxX, dYpos2,minZ);
				glEnd();	 

			}
			if(b_channel)
			{

				if(volrenderview.blueclip_toggle==0) volrenderview.cutting_planes.disable();
				else volrenderview.cutting_planes.enable();

				vol2dmulti_shaderobj.sendUniform1i("channel", 2);
				glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(minX,dYpos2, minZ);
				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(minX, dYpos2,maxZ);
				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(maxX, dYpos2,maxZ);
				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(maxX, dYpos2,minZ);
				glEnd();	 
			}
		}
		glextensions.glActiveTexture(GL_TEXTURE3);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
	
		dYpos += dYstep;
	}
  }
  else
  {
//	int numbslices = volobj->texheight;
	float dYpos = 1;
	float dYstep = 2.0/double(numbslices);

	float dAlpha;
	float dTexPos;
	int texIndex;
	float dYpos2;

	for(int slices=numbslices; slices>0; --slices)
	{
	    dTexPos = ((float)volobj->texheight*(dYpos+1.0)/2.0);
		texIndex = int(dTexPos);
		dAlpha = ((float)volobj->texheight-dTexPos)/(double)volobj->texheight; //dTexPos-double(texIndex);
		dYpos2 = (dYpos/2.0)*volobj->texheight;

		glextensions.glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texNames2[texIndex]);
		glEnable(GL_TEXTURE_2D);

		glextensions.glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,texNames2[texIndex+1]);
		glEnable(GL_TEXTURE_2D);

		glextensions.glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
		glEnable(GL_TEXTURE_2D);

		glextensions.glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
		glEnable(GL_TEXTURE_2D);

		if(volobj->is_greyscale)
		{
			vol2dmulti_shaderobj.sendUniform1i("channel", 3);

			glBegin(GL_QUADS);
			glTexCoord3d(0.0, 0.0, dAlpha);
			glVertex3f(minX,dYpos2, minZ);
			glTexCoord3d(0.0, 1.0, dAlpha);
			glVertex3f(minX, dYpos2,maxZ);
			glTexCoord3d(1.0, 1.0, dAlpha);
			glVertex3f(maxX, dYpos2,maxZ);
			glTexCoord3d(1.0, 0.0, dAlpha);
			glVertex3f(maxX, dYpos2,minZ);
			glEnd();	 
		}
		else
		{
			if(r_channel)
			{
				if(volrenderview.redclip_toggle==0) volrenderview.cutting_planes.disable();
				else volrenderview.cutting_planes.enable();

				vol2dmulti_shaderobj.sendUniform1i("channel", 0);
				glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(minX,dYpos2, minZ);
				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(minX, dYpos2,maxZ);
				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(maxX, dYpos2,maxZ);
				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(maxX, dYpos2,minZ);
				glEnd();	 

			}
			if(g_channel)
			{
				if(volrenderview.greenclip_toggle==0) volrenderview.cutting_planes.disable();
				else volrenderview.cutting_planes.enable();

				vol2dmulti_shaderobj.sendUniform1i("channel", 1);
				glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(minX,dYpos2, minZ);
				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(minX, dYpos2,maxZ);
				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(maxX, dYpos2,maxZ);
				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(maxX, dYpos2,minZ);
				glEnd();	 

			}
			if(b_channel)
			{

				if(volrenderview.blueclip_toggle==0) volrenderview.cutting_planes.disable();
				else volrenderview.cutting_planes.enable();

				vol2dmulti_shaderobj.sendUniform1i("channel", 2);
				glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(minX,dYpos2, minZ);
				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(minX, dYpos2,maxZ);
				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(maxX, dYpos2,maxZ);
				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(maxX, dYpos2,minZ);
				glEnd();	 
			}
		}
		glextensions.glActiveTexture(GL_TEXTURE3);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
	
		dYpos -= dYstep;
	}
  }
}
void VolumeRender::render_2dMultiTextureMultiPass_stack3(GLfloat dir)
{
	//printf("3\n");
	//printf("numbslices: %d\n", numbslices);
	if(dir>0)
	{

//		int numbslices = volobj->texwidth;
		float dXpos = 1;
		float dXstep = 2.0/double(numbslices);

		float dAlpha;
		float dTexPos;
		int texIndex;
		float dXpos2;

		for(int slices=numbslices; slices>0; --slices)
		{
			dTexPos = ((double)volobj->texwidth*(dXpos+1.0)/2.0);
			texIndex = int(dTexPos);
			dAlpha = ((double)volobj->texwidth-dTexPos)/(double)volobj->texwidth;//dTexPos-double(texIndex);
			dXpos2 = (dXpos/2.0)*volobj->texwidth;

			glextensions.glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,texNames3[texIndex]);
			glEnable(GL_TEXTURE_2D);

			glextensions.glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,texNames3[texIndex+1]);
			glEnable(GL_TEXTURE_2D);

			glextensions.glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
			glEnable(GL_TEXTURE_2D);

			glextensions.glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
			glEnable(GL_TEXTURE_2D);

			if(volobj->is_greyscale)
			{
				vol2dmulti_shaderobj.sendUniform1i("channel", 3);
				glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(dXpos2,minY, minZ);
				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(dXpos2,maxY, minZ);
				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(dXpos2,maxY, maxZ);
				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(dXpos2,minY, maxZ);
				glEnd();	 
			}
			else
			{
				if(r_channel)
				{
					if(volrenderview.redclip_toggle==0) volrenderview.cutting_planes.disable();
					else volrenderview.cutting_planes.enable();

					vol2dmulti_shaderobj.sendUniform1i("channel", 0);
					glBegin(GL_QUADS);
					glTexCoord3d(0.0, 0.0, dAlpha);
					glVertex3f(dXpos2,minY, minZ);
					glTexCoord3d(0.0, 1.0, dAlpha);
					glVertex3f(dXpos2,maxY, minZ);
					glTexCoord3d(1.0, 1.0, dAlpha);
					glVertex3f(dXpos2,maxY, maxZ);
					glTexCoord3d(1.0, 0.0, dAlpha);
					glVertex3f(dXpos2,minY, maxZ);
					glEnd();	 

				}
				if(g_channel)
				{
					if(volrenderview.greenclip_toggle==0) volrenderview.cutting_planes.disable();
					else volrenderview.cutting_planes.enable();

					vol2dmulti_shaderobj.sendUniform1i("channel", 1);
					glBegin(GL_QUADS);
					glTexCoord3d(0.0, 0.0, dAlpha);
					glVertex3f(dXpos2,minY, minZ);
					glTexCoord3d(0.0, 1.0, dAlpha);
					glVertex3f(dXpos2,maxY, minZ);
					glTexCoord3d(1.0, 1.0, dAlpha);
					glVertex3f(dXpos2,maxY, maxZ);
					glTexCoord3d(1.0, 0.0, dAlpha);
					glVertex3f(dXpos2,minY, maxZ);
					glEnd();	 
				}
				if(b_channel)
				{

					if(volrenderview.blueclip_toggle==0) volrenderview.cutting_planes.disable();
					else volrenderview.cutting_planes.enable();

					vol2dmulti_shaderobj.sendUniform1i("channel", 2);
					glBegin(GL_QUADS);
					glTexCoord3d(0.0, 0.0, dAlpha);
					glVertex3f(dXpos2,minY, minZ);
					glTexCoord3d(0.0, 1.0, dAlpha);
					glVertex3f(dXpos2,maxY, minZ);
					glTexCoord3d(1.0, 1.0, dAlpha);
					glVertex3f(dXpos2,maxY, maxZ);
					glTexCoord3d(1.0, 0.0, dAlpha);
					glVertex3f(dXpos2,minY, maxZ);
					glEnd();	 
				}
			}

		glextensions.glActiveTexture(GL_TEXTURE3);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_2D);
			glextensions.glActiveTexture(GL_TEXTURE1);
			glDisable(GL_TEXTURE_2D);
			glextensions.glActiveTexture(GL_TEXTURE0);
			glDisable(GL_TEXTURE_2D);
		
			dXpos -= dXstep;
		}
	}
	else
	{
//		int numbslices = volobj->texwidth;
		float dXpos = -1;
		float dXstep = 2.0/double(numbslices);

		float dAlpha;
		float dTexPos;
		int texIndex;
		float dXpos2;

		for(int slices=0; slices<numbslices; ++slices)
		{
			dTexPos = ((float)volobj->texwidth*(dXpos+1.0)/2.0);
			texIndex = int(dTexPos);
			dAlpha = (dTexPos)/(double)volobj->texwidth;//dTexPos-double(texIndex);
			dXpos2 = (dXpos/2.0)*volobj->texwidth;

			glextensions.glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,texNames3[texIndex]);
			glEnable(GL_TEXTURE_2D);

			glextensions.glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,texNames3[texIndex-1]);
			glEnable(GL_TEXTURE_2D);

		glextensions.glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
		glEnable(GL_TEXTURE_2D);

		glextensions.glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
		glEnable(GL_TEXTURE_2D);

			if(volobj->is_greyscale)
			{
				vol2dmulti_shaderobj.sendUniform1i("channel", 3);
				glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(dXpos2,minY, minZ);
				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(dXpos2,maxY, minZ);
				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(dXpos2,maxY, maxZ);
				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(dXpos2,minY, maxZ);
				glEnd();	 
			}
			else
			{
				if(r_channel)
				{
					if(volrenderview.redclip_toggle==0) volrenderview.cutting_planes.disable();
					else volrenderview.cutting_planes.enable();

					vol2dmulti_shaderobj.sendUniform1i("channel", 0);
					glBegin(GL_QUADS);
					glTexCoord3d(0.0, 0.0, dAlpha);
					glVertex3f(dXpos2,minY, minZ);
					glTexCoord3d(0.0, 1.0, dAlpha);
					glVertex3f(dXpos2,maxY, minZ);
					glTexCoord3d(1.0, 1.0, dAlpha);
					glVertex3f(dXpos2,maxY, maxZ);
					glTexCoord3d(1.0, 0.0, dAlpha);
					glVertex3f(dXpos2,minY, maxZ);
					glEnd();	 

				}
				if(g_channel)
				{
					if(volrenderview.greenclip_toggle==0) volrenderview.cutting_planes.disable();
					else volrenderview.cutting_planes.enable();

					vol2dmulti_shaderobj.sendUniform1i("channel", 1);
					glBegin(GL_QUADS);
					glTexCoord3d(0.0, 0.0, dAlpha);
					glVertex3f(dXpos2,minY, minZ);
					glTexCoord3d(0.0, 1.0, dAlpha);
					glVertex3f(dXpos2,maxY, minZ);
					glTexCoord3d(1.0, 1.0, dAlpha);
					glVertex3f(dXpos2,maxY, maxZ);
					glTexCoord3d(1.0, 0.0, dAlpha);
					glVertex3f(dXpos2,minY, maxZ);
					glEnd();	 
				}
				if(b_channel)
				{

					if(volrenderview.blueclip_toggle==0) volrenderview.cutting_planes.disable();
					else volrenderview.cutting_planes.enable();

					vol2dmulti_shaderobj.sendUniform1i("channel", 2);
					glBegin(GL_QUADS);
					glTexCoord3d(0.0, 0.0, dAlpha);
					glVertex3f(dXpos2,minY, minZ);
					glTexCoord3d(0.0, 1.0, dAlpha);
					glVertex3f(dXpos2,maxY, minZ);
					glTexCoord3d(1.0, 1.0, dAlpha);
					glVertex3f(dXpos2,maxY, maxZ);
					glTexCoord3d(1.0, 0.0, dAlpha);
					glVertex3f(dXpos2,minY, maxZ);
					glEnd();	 
				}
			}

			glextensions.glActiveTexture(GL_TEXTURE3);
			glDisable(GL_TEXTURE_2D);
			glextensions.glActiveTexture(GL_TEXTURE2);
			glDisable(GL_TEXTURE_2D);
			glextensions.glActiveTexture(GL_TEXTURE1);
			glDisable(GL_TEXTURE_2D);
			glextensions.glActiveTexture(GL_TEXTURE0);
			glDisable(GL_TEXTURE_2D);
		
			dXpos += dXstep;
		}
	}
}
void VolumeRender::render_2dTexture(void)
{
	if(!texNames1) return;
	if(!texNames2) return;
	if(!texNames3) return;

	//Convert from euler angles to rotation matrix
 	glPushMatrix();

	glMultMatrixf(volrenderview.arcball.Transform.M);		

	//get the transformation matrix
	GLfloat m[4][4];
 	glGetFloatv(GL_MODELVIEW_MATRIX, &m[0][0]);

	//work out our view direction
	//ie: which stack we need to display
	a1 = (m[0][2]-m[0][3])/(m[3][2]-m[3][3]);
	a2 = (m[1][2]-m[1][3])/(m[3][2]-m[3][3]);
	a3 = (m[2][2]-m[2][3])/(m[3][2]-m[3][3]);

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glColor3f(1,1,1);

	vol2d_shaderobj.enable();

	rebuild_texcoords();

	if ( (fabs(a1)>fabs(a2)) && (fabs(a1)>fabs(a3)) ) render_2dTexture_stack3(a1);
	else if ( (fabs(a2)>fabs(a1)) && (fabs(a2)>fabs(a3)) ) render_2dTexture_stack2(a2);
	else render_2dTexture_stack1(a3);

	vol2d_shaderobj.disable();

	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	glPopMatrix();
}
void VolumeRender::render_2dTexture_stack1(GLfloat dir)
{
  if (dir < 0)
  {
	int numbslices = volobj->texdepth;
	float dZpos = -1;
	float dZstep = 2.0/double(numbslices);

	float dAlpha;
	float dTexPos;
	int texIndex;
	float dZpos2;

	for(int slices=0; slices<numbslices; ++slices)
	{
		dTexPos = ((volobj->texdepth)*(dZpos+1.0)/2.0);
		texIndex = int(dTexPos);
		dAlpha = dTexPos-double(texIndex);

		glextensions.glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texNames1[texIndex]);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
		glEnable(GL_TEXTURE_2D);

		dZpos2 = (dZpos/2.0)*volobj->texdepth;

		glBegin(GL_QUADS);
			glTexCoord3d(0.0, 0.0, dAlpha);
			glVertex3f(minX, minY, dZpos2);

			glTexCoord3d(0.0, 1.0, dAlpha);
			glVertex3f(minX, maxY, dZpos2);

			glTexCoord3d(1.0, 1.0, dAlpha);
			glVertex3f(maxX, maxY, dZpos2);

			glTexCoord3d(1.0, 0.0, dAlpha);
			glVertex3f(maxX, minY, dZpos2);
		glEnd();	 

		glextensions.glActiveTexture(GL_TEXTURE3);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);

		dZpos += dZstep;
	}
  }
  else
  {
	int numbslices = volobj->texdepth;
	float dZpos = 1;
	float dZstep = 2.0/double(numbslices);

	float dAlpha;
	float dTexPos;
	int texIndex;
	float dZpos2;

	for(int slices=numbslices; slices>0; --slices)
	{
		dTexPos = (volobj->texdepth*(dZpos+1.0)/2.0);
		texIndex = int(dTexPos);
		dAlpha = dTexPos-double(texIndex);

		glextensions.glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texNames1[texIndex]);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
		glEnable(GL_TEXTURE_2D);

		dZpos2 = (dZpos/2.0)*volobj->texdepth;

		glBegin(GL_QUADS);
			glTexCoord3d(0.0, 0.0, dAlpha);
			glVertex3f(minX, minY, dZpos2);

			glTexCoord3d(0.0, 1.0, dAlpha);
			glVertex3f(minX, maxY, dZpos2);

			glTexCoord3d(1.0, 1.0, dAlpha);
			glVertex3f(maxX, maxY, dZpos2);

			glTexCoord3d(1.0, 0.0, dAlpha);
			glVertex3f(maxX, minY, dZpos2);
		glEnd();	 

		glextensions.glActiveTexture(GL_TEXTURE3);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);

		dZpos -= dZstep;
	}
  }
}
void VolumeRender::render_2dTexture_stack2(GLfloat dir)
{
 if (dir < 0)
  {
	int numbslices = volobj->texheight;
	float dYpos = -1;
	float dYstep = 2.0/double(numbslices);

	float dAlpha;
	float dTexPos;
	int texIndex;
	float dYpos2;

	for(int slices=0; slices<numbslices; ++slices)
	{
		dTexPos = (volobj->texheight*(dYpos+1.0)/2.0);
		texIndex = int(dTexPos);
		dAlpha = dTexPos-double(texIndex);

		glextensions.glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texNames2[texIndex]);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
		glEnable(GL_TEXTURE_2D);

		dYpos2 = (dYpos/2.0)*volobj->texheight;
	
		glBegin(GL_QUADS);
			glTexCoord3d(0.0, 0.0, dAlpha);
			glVertex3f(minX,dYpos2, minZ);

			glTexCoord3d(0.0, 1.0, dAlpha);
			glVertex3f(minX, dYpos2,maxZ);

			glTexCoord3d(1.0, 1.0, dAlpha);
			glVertex3f(maxX, dYpos2,maxZ);

			glTexCoord3d(1.0, 0.0, dAlpha);
			glVertex3f(maxX, dYpos2,minZ);
		glEnd();	 

		glextensions.glActiveTexture(GL_TEXTURE3);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);

		dYpos += dYstep;
	}
  }
  else
  {
	int numbslices = volobj->texheight;
	float dYpos = 1;
	float dYstep = 2.0/double(numbslices);

	float dAlpha;
	float dTexPos;
	int texIndex;
	float dYpos2;

	for(int slices=numbslices; slices>0; --slices)
	{
		dTexPos = (volobj->texheight*(dYpos+1.0)/2.0);
		texIndex = int(dTexPos);
		dAlpha = dTexPos-double(texIndex);

		glextensions.glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texNames2[texIndex]);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
		glEnable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
		glEnable(GL_TEXTURE_2D);

		dYpos2 = (dYpos/2.0)*volobj->texheight;

		glBegin(GL_QUADS);
			glTexCoord3d(0.0, 0.0, dAlpha);
			glVertex3f(minX,dYpos2, minZ);

			glTexCoord3d(0.0, 1.0, dAlpha);
			glVertex3f(minX, dYpos2,maxZ);

			glTexCoord3d(1.0, 1.0, dAlpha);
			glVertex3f(maxX, dYpos2,maxZ);

			glTexCoord3d(1.0, 0.0, dAlpha);
			glVertex3f(maxX, dYpos2,minZ);
		glEnd();	 

		glextensions.glActiveTexture(GL_TEXTURE3);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_2D);
		glextensions.glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);

		dYpos -= dYstep;
	}
  }
}
void VolumeRender::render_2dTexture_stack3(GLfloat dir)
{
 	if(dir>0)
	{

		int numbslices = volobj->texwidth;
		float dXpos = 1;
		float dXstep = 2.0/double(numbslices);

		float dAlpha;
		float dTexPos;
		int texIndex;
		float dXpos2;

		for(int slices=numbslices; slices>0; --slices)
		{
			dTexPos = (volobj->texwidth*(dXpos+1.0)/2.0);
			texIndex = int(dTexPos);
			dAlpha = dTexPos-double(texIndex);

			glextensions.glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,texNames3[texIndex]);
			glEnable(GL_TEXTURE_2D);
			glextensions.glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
			glEnable(GL_TEXTURE_2D);
			glextensions.glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
			glEnable(GL_TEXTURE_2D);

			dXpos2 = (dXpos/2.0)*volobj->texwidth;

			glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(dXpos2,minY, minZ);

				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(dXpos2,maxY, minZ);

				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(dXpos2,maxY, maxZ);

				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(dXpos2,minY, maxZ);
			glEnd();	 

			glextensions.glActiveTexture(GL_TEXTURE3);
			glDisable(GL_TEXTURE_2D);
			glextensions.glActiveTexture(GL_TEXTURE2);
			glDisable(GL_TEXTURE_2D);
			glextensions.glActiveTexture(GL_TEXTURE0);
			glDisable(GL_TEXTURE_2D);

			dXpos -= dXstep;
		}
	}
	else
	{
		int numbslices = volobj->texwidth;
		float dXpos = -1;
		float dXstep = 2.0/double(numbslices);

		float dAlpha;
		float dTexPos;
		int texIndex;
		float dXpos2;

		for(int slices=0; slices<numbslices; ++slices)
		{
			dTexPos = (volobj->texwidth*(dXpos+1.0)/2.0);
			texIndex = int(dTexPos);
			dAlpha = dTexPos-double(texIndex);

			glextensions.glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,texNames3[texIndex]);
			glEnable(GL_TEXTURE_2D);
			glextensions.glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RGB);
			glEnable(GL_TEXTURE_2D);
			glextensions.glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, TEXTURE2D_RAGABA);
			glEnable(GL_TEXTURE_2D);

			dXpos2 = (dXpos/2.0)*volobj->texwidth;

			glBegin(GL_QUADS);
				glTexCoord3d(0.0, 0.0, dAlpha);
				glVertex3f(dXpos2,minY, minZ);

				glTexCoord3d(0.0, 1.0, dAlpha);
				glVertex3f(dXpos2,maxY, minZ);

				glTexCoord3d(1.0, 1.0, dAlpha);
				glVertex3f(dXpos2,maxY, maxZ);

				glTexCoord3d(1.0, 0.0, dAlpha);
				glVertex3f(dXpos2,minY, maxZ);
			glEnd();	 

			glextensions.glActiveTexture(GL_TEXTURE3);
			glDisable(GL_TEXTURE_2D);
			glextensions.glActiveTexture(GL_TEXTURE2);
			glDisable(GL_TEXTURE_2D);
			glextensions.glActiveTexture(GL_TEXTURE0);
			glDisable(GL_TEXTURE_2D);

			dXpos += dXstep;
		}
	}
}
//=================================================================================================================================
QImage VolumeRender::render_framebuffer( bool withAlpha ) // default is false
{
	//glViewport( 0, 0, (GLint)render_size, (GLint)render_size );

	QImage res;

	if(offscreen_toggle==1)
	{
		res = QImage( render_size, render_size, 32 );
		glReadPixels( 0, 0, render_size, render_size, GL_RGBA, GL_UNSIGNED_BYTE, res.bits() );
	
		if ( QImage::systemByteOrder() == QImage::BigEndian ) 
		{
			// OpenGL gives RGBA; Qt wants ARGB
			uint *p = (uint*)res.bits();
			uint *end = p + render_size*render_size;

			while ( p < end )
				*p++ >>= 8;
		}
	}
	else
	{
		res = QImage( volrenderview.winWidth, volrenderview.winHeight, 32 );
		glReadPixels( 0, 0, volrenderview.winWidth, volrenderview.winHeight, GL_RGBA, GL_UNSIGNED_BYTE, res.bits() );
	

		if ( QImage::systemByteOrder() == QImage::BigEndian ) 
		{
			// OpenGL gives RGBA; Qt wants ARGB
			uint *p = (uint*)res.bits();
			uint *end = p + volrenderview.winWidth*volrenderview.winHeight;

			while ( p < end )
				*p++ >>= 8;
		}
	}

	res = res.swapRGB();
	QSize size = QSize(volobj->maxres*2.0, volobj->maxres*2.0);
	res = res.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	return res.mirror();
} 
//=================================================================================================================================
void VolumeRender::render_light(Vector lightpos, Vector lightdir)
{
	glPushMatrix();

	glDisable(GL_LIGHTING);
	
//		glMatrixMode(GL_MODELVIEW);
//		glLoadIdentity();
//		printf("\nlightpos: %f, %f, %f\n", lightpos.x, lightpos.y, lightpos.z);
//		printf("lightdir: %f, %f, %f\n", 50.0*lightdir.x, 50.0*lightdir.y, 50.0*lightdir.z);

		glColor3f(1.0, 0.0, 0.0);
		glPointSize(5.0);
		glBegin(GL_POINTS);
			glVertex3f(lightpos.x, lightpos.y, lightpos.z);
		glEnd();
		glPointSize(1.0);

		glColor3f(1.0, 1.0, 0.0);
		glLineWidth(2.0);
		glBegin(GL_LINES);
			glVertex3f(lightpos.x, lightpos.y, lightpos.z);
			glVertex3f(lightpos.x+(100.0*lightdir.x), lightpos.y+(100.0*lightdir.y), lightpos.z+(100.0*lightdir.z));
		glEnd();
		glLineWidth(1.0);

		int i;
		int numSides=10;
		float angle = (PI*20.0)/180.0;
		float s = 100.0 * sinf(angle);
		float c = 100.0 * cosf(angle);

		Vector OriginalDir, NewDir, v;
		OriginalDir = Vector(0.0,0.0,1.0);
		NewDir = -lightdir;

	/*	int skip=0;

		Vector N;
		N.cross(OriginalDir, NewDir);
		float cosa = OriginalDir.dot(NewDir);

	//	if (cosa > 0.9999999f) // both vectors aligned
	//	return;

		if (cosa < -0.9999999f) // vectors are opposite
		{
		Vector M(0.0f, 1.0f, 1.0f);
		float fmax = max((float)fabs(OriginalDir.x), (float)0.0);

		if (fabs(OriginalDir.x) > fabs(OriginalDir.y))
		{
			fmax = fabs(OriginalDir.y);
			M = Vector(1.0f, 0.0f, 1.0f);
			}

			if (fabs(OriginalDir.z) > fmax)
			{
			M = Vector(1.0f, 1.0f, 0.0f);
			}

			N.cross(OriginalDir, M);
			N.normalize();
	//		glTranslatef(lightpos.x, lightpos.y, lightpos.z);
	//		glRotatef(180.0f, N.x, N.y, N.z); // rotate to opposite direction
	//		skip=1;
		}

	//	if(skip)
	//	{
			float sina = N.length();
			float a = (atan2f(sina, cosa)) / PI * 180.0f;
			N /= sina; // normalising the axis
			
			//if(skip) a +=180.0;

			glTranslatef(lightpos.x, lightpos.y, lightpos.z);
			glRotatef(a, N.x, N.y, N.z);
	//	}
*/
		/*//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);
		glBegin(GL_TRIANGLE_FAN);				//GL_TRIANGLE_FAN
		glColor4f(1.0, 1.0, 0.5, 0.75);
		glVertex3f(0.0, 0.0, 0.0);
		glColor4f(1.0, 1.0, 1.0, 0.25);
		for (i = 0; i <= numSides; i++)
		{
			v = Vector(s * cos(2 * i * PI / numSides), s * sin(2 * i * PI / numSides), -c);
			glVertex3fv(&v.x);	
		}
		glEnd();
		//glCullFace(GL_BACK);
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1.0, 1.0, 0.5, 0.75);
		glVertex3f(0.0, 0.0, 0.0);
		glColor4f(1.0, 1.0, 1.0, 0.25);
		for (i = 0; i <= numSides; i++)
		{
			v = Vector(s * cos(2 * i * PI / numSides), s * sin(2 * i * PI / numSides), -c);
			glVertex3fv(&v.x);
		}
		glEnd();
		//glDisable(GL_CULL_FACE);*/

		glTranslatef(lightpos.x, lightpos.y, lightpos.z);

		glBegin(GL_LINE_LOOP);
		glColor4f(0.0, 0.0, 0.0, 1.0);
		for (i = 0; i <= numSides; i++)
		{
			v = Vector(s * cosf(2 * i * PI / numSides), s * sinf(2 * i * PI / numSides), -c);
			v.rotate3D(Vector(1,0,0),(3.14159265358979323846/180.0)*light_altitude);
			v.rotate3D(Vector(0,1,0),(3.14159265358979323846/180.0)*light_azimuth);
			glVertex3fv(&v.x);
		}
		glEnd();

		glBegin(GL_LINES);
		glColor4f(0.0, 0.0, 0.0, 1.0);
		for (i = 0; i <= numSides; i++)
		{
			v = Vector(s * cosf(2 * i * PI / numSides), s * sinf(2 * i * PI / numSides), -c);
			v.rotate3D(Vector(1,0,0),(3.14159265358979323846/180.0)*light_altitude);
			v.rotate3D(Vector(0,1,0),(3.14159265358979323846/180.0)*light_azimuth);			
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3fv(&v.x);
		}
		glEnd();


	glEnable(GL_LIGHTING);

   glPopMatrix();

}
void VolumeRender::render_3DCursor(void)
{
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glLineWidth(2.0);

	float size = volobj->texdepth/50.0;

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(cursor3D_screenspace.x-size, cursor3D_screenspace.y,    cursor3D_screenspace.z);
	glVertex3f(cursor3D_screenspace.x+size, cursor3D_screenspace.y,    cursor3D_screenspace.z);
	glEnd();
	
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(cursor3D_screenspace.x,    cursor3D_screenspace.y-size, cursor3D_screenspace.z);
	glVertex3f(cursor3D_screenspace.x,    cursor3D_screenspace.y+size, cursor3D_screenspace.z);
	glEnd();

	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(cursor3D_screenspace.x,    cursor3D_screenspace.y,    cursor3D_screenspace.z-size);
	glVertex3f(cursor3D_screenspace.x,    cursor3D_screenspace.y,    cursor3D_screenspace.z+size);
	glEnd();
	glLineWidth(1.0);
	glEnable(GL_LIGHTING);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);

	//render_slices(1);
}

void VolumeRender::render_scalebar(void)
{
	if(volobj->maxres==1) return;

	//----------------------------------------
	// Calculate a scale bar
	//----------------------------------------
	float maxres =  max(volobj->texwidth*(volobj->xsize), volobj->texheight*(volobj->ysize));
	maxres = max(maxres, volobj->texdepth*(volobj->zsize));
	
	//float maxres = volobj->maxres*volobj->maxsize;

	float offset1 = ((volrenderview.winWidth/volrenderview.winHeight)*maxres)/2.0f;
	//maxres += offset;	
	//maxres /= 2.0;

	//printf("volobj->size: %f, %f, %f\n", volobj->xsize, volobj->ysize, volobj->zsize);
	//printf("volobj->scale: %f, %f, %f\n", volobj->xscale, volobj->xscale, volobj->xscale);
	//printf("volobj->resize: %f, %f, %f\n", volobj->x_resize, volobj->y_resize, volobj->z_resize);

	float f, factor;
	if(volrenderview.projection_toggle==ORTHO_PROJECTION)
	{		
		f = (((maxres+offset1)/2.0)+volrenderview.zoom);
		if(f==0) f = 1;
		factor = maxres/f;
		//float factor = 1.0;//maxres/volrenderview.zoom;
	}
	else
	{
		f=1.0;
		factor = maxres/f;
//		volrenderview.zoom
	}

//	printf("f: %f\n", f);
//	printf("factor: %f\n", factor);

	float maxsize = volobj->xsize;
	maxsize = max(maxsize, volobj->ysize);
	maxsize = max(maxsize, volobj->zsize);
	
	//printf("factor: %f\n", maxsize);

	float length = 100.0*(factor);
	float resize = volobj->x_resize;
	float unit = ((volobj->xscale)*resize)*maxsize;
	float scalebar_length = (((100.0)*unit));

/*
	//----------------------------------------
	// Get a nice divisible by 10 number
	//----------------------------------------
	float divisibleby = 1.0;
	if (scalebar_length>1000000.0) divisibleby = 1000000.0;
	else if(scalebar_length>100000.0) divisibleby = 100000.0;
	else if(scalebar_length>10000.0) divisibleby = 10000.0;
	else if(scalebar_length>1000.0) divisibleby = 1000.0;
	else if(scalebar_length>100.0) divisibleby = 100.0;
	else if(scalebar_length>10.0) divisibleby = 10.0;
	else if(scalebar_length<10.0) divisibleby = 1.0;
	else if(scalebar_length<1.0) divisibleby = 0.5;

	float divs = scalebar_length/divisibleby;
	float floor_divs = floor(divs);
	float fract = divs - floor_divs;
	if(fract>=0.5) divs = floor_divs+1;
	else divs = floor_divs;
	float resizer = scalebar_length/(divs*divisibleby);
	if(resizer==0.0) resizer = 1.0;
	scalebar_length = divs*divisibleby;
	length *= resizer;

	//avoid negatives!
	if(length<0.0) length = -length;
*/
	if(length<0.0) length = -length;
	//----------------------------------------
	// Autoscale bar to desired min/max sizes
	//----------------------------------------

	float mintargetsize = 0.2;
	float maxtargetsize = 0.4;

	//printf("zoom: %f\n", (float)volrenderview.zoom);
	//printf("length: %f\n", length);
	//printf("scalebar_length: %f\n", scalebar_length);
	//printf("min l: %f, max l: %f\n", volobj->maxres*mintargetsize, volobj->maxres*maxtargetsize);

	if(length>volobj->maxres*maxtargetsize)
	{
	//	printf("MAX!\n");
		while(length>volobj->maxres*maxtargetsize)
		{
			length /= 2.0;
			scalebar_length /= 2.0;
		}
	}
	else if(length<volobj->maxres*mintargetsize && length>0.0)
	{
	//	printf("MIN!\n");
		while(length<volobj->maxres*mintargetsize)
		{
			length *= 2.0;
			scalebar_length *= 2.0;
		}
	}
	
	//----------------------------------------
	//Build our text label
	//----------------------------------------
	QString scalebarunit = "";
	if(scalebar_length>=10000.0f)	//CM
	{
		scalebar_length /= 10000.0f;
		scalebartext.setNum(scalebar_length);
		scalebarunit += "c";
		scalebarunit += "m";
	}
	else if(scalebar_length>=1000.0f)	//MM
	{
		scalebar_length /= 1000.0f;
		scalebartext.setNum(scalebar_length);
		scalebarunit += "m";
		scalebarunit += "m";
	}
	else if(scalebar_length>=1.0f)							//MICRON
	{
		scalebartext.setNum(scalebar_length);
		scalebarunit += " ";
		scalebarunit += QChar(0x00B5);
		scalebarunit += "m";
	}
	else if(scalebar_length>=0.001f)							//NANO
	{
		scalebar_length *= 1000.0f;
		scalebartext.setNum(scalebar_length);
		scalebarunit += "n";
		scalebarunit += "m";
	}


	if(scalebar_length>50.0)
	{
		float divisibleby = 50.0;
		float divs = scalebar_length/divisibleby;
		float floor_divs = floor(divs);
		float fract = divs - floor_divs;
		if(fract>=0.5) divs = floor_divs+1;
		else divs = floor_divs;
		float resizer = (divs*divisibleby)/scalebar_length;
		if(resizer==0.0) resizer = 1.0;
		
		scalebar_length = divs*divisibleby;
		length *= resizer;
	}
	else if(scalebar_length>10.0)
	{
		float divisibleby = 10.0;
		float divs = scalebar_length/divisibleby;
		float floor_divs = floor(divs);
		float fract = divs - floor_divs;
		if(fract>=0.5) divs = floor_divs+1;
		else divs = floor_divs;
		float resizer = (divs*divisibleby)/scalebar_length;
		if(resizer==0.0) resizer = 1.0;
		
		scalebar_length = divs*divisibleby;
		length *= resizer;
	}
	else if(scalebar_length>5.0)
	{
		float divisibleby = 5.0;
		float divs = scalebar_length/divisibleby;
		float floor_divs = floor(divs);
		float fract = divs - floor_divs;
		if(fract>=0.5) divs = floor_divs+1;
		else divs = floor_divs;
		float resizer = (divs*divisibleby)/scalebar_length;
		if(resizer==0.0) resizer = 1.0;
		
		scalebar_length = divs*divisibleby;
		length *= resizer;
	}
	else if(scalebar_length>2.0)
	{
		float divisibleby = 2.0;
		float divs = scalebar_length/divisibleby;
		float floor_divs = floor(divs);
		float fract = divs - floor_divs;
		if(fract>=0.5) divs = floor_divs+1;
		else divs = floor_divs;
		float resizer = (divs*divisibleby)/scalebar_length;
		if(resizer==0.0) resizer = 1.0;
		
		scalebar_length = divs*divisibleby;
		length *= resizer;
	}
	else if(scalebar_length>0.5)
	{
		float divisibleby = 0.5;
		float divs = scalebar_length/divisibleby;
		float floor_divs = floor(divs);
		float fract = divs - floor_divs;
		if(fract>=0.5) divs = floor_divs+1;
		else divs = floor_divs;
		float resizer = (divs*divisibleby)/scalebar_length;
		if(resizer==0.0) resizer = 1.0;
		
		scalebar_length = divs*divisibleby;
		length *= resizer;
	}

	scalebartext.setNum(scalebar_length);
	scalebartext+= scalebarunit;

	//printf("adjusted scalebar_length: %f\n", scalebar_length);
	//printf("adjusted length: %f\n", length);

	//----------------------------------------
	//Position our bar + label
	//----------------------------------------

	float offset = -volobj->maxres;
	float marginsize = volobj->maxres*0.05f;
	scalebarfontpos = Vector(offset+marginsize, -volobj->maxres+(volobj->maxres*0.025f), 0.0);
	Vector scalebarpos = scalebarfontpos;
	scalebarpos.y += 64.0*(float)volobj->maxres/(float)volrenderview.winHeight;

	glPushMatrix();
//		glColor3f(0,0,0);
		glColor3f(background_fontcolour.red()/255.0, background_fontcolour.blue()/255.0, background_fontcolour.green()/255.0);
		glLineWidth(5.0);
		glBegin(GL_LINES);

		//horizontal bar
		glVertex3f(scalebarpos.x, scalebarpos.y, scalebarpos.z);
		glVertex3f(scalebarpos.x+length, scalebarpos.y, scalebarpos.z);

		glEnd();
		glLineWidth(1.0);
	glPopMatrix();
/*
	float voxX = 1.0;
	float voxY = 1.0;
	float voxZ = 1.0;

	float unitX = volobj->xscale;
	float unitY = volobj->yscale;
	float unitZ = volobj->zscale;

	float spacingX = x;
	float spacingY = y;
	float spacingZ = z;

	Vector temp;

	temp.x *= unitX/voxX;
	temp.y *= unitY/voxY;
	temp.z *= unitZ/voxZ;

	temp.x /= spacingX;
	temp.y /= spacingY;
	temp.z /= spacingZ;
	
	temp.x *= rescaleX;
	temp.y *= rescaleY;
	temp.z *= rescaleZ;

	printf("maxres: %f\n",maxres);
	printf("maxsize: %f\n",maxsize);
	printf("volobj->xscale: %f\n",volobj->xscale);
	printf("volobj->xsize: %f\n",volobj->xsize);
	printf("length: %f\n",length);
	printf("resX: %f\n",resX);
	printf("(resX)/(volobj->xsize/maxsize): %f\n",(resX)/(volobj->xsize/maxsize));
	printf("maxvolsize: %f\n",maxvolsize);
	printf("maxvolscale: %f\n",maxvolscale);
*/	
/*
	float resX = volobj->texwidth;
	float resY = volobj->texheight;
	float resZ = volobj->texdepth;

	//proportion autoscale helpers
	float minsize, maxsize, targetsize;
	minsize = 0.1;
	maxsize = 0.3;
	targetsize = 0.2;

	float offset = -volobj->maxres;
	float marginsize = volobj->maxres*0.05f;

	float scalefac = 10000.0/((volobj->xscale)/(volobj->xsize/volobj->maxsize));
	scalefac /= factor;

	float mres0, mres1;

	if(resX>volobj->maxres*targetsize)
	{
		while(resX>volobj->maxres*targetsize)
		{
			resX = scalefac;
			scalefac /= 10.0;
		}
	}

	scalefac *= volobj->xscale;

	resY = resZ = volobj->maxres;
	mres0 = -volobj->maxres+((resX)*(volobj->maxres/((volobj->maxres)+volrenderview.zoom)))+(resX-((resX)*(volobj->maxres/((volobj->maxres)+volrenderview.zoom))));
	mres1 = -volobj->maxres+((resX*2.0)*(volobj->maxres/((volobj->maxres)+volrenderview.zoom))+(resX-((resX)*(volobj->maxres/((volobj->maxres)+volrenderview.zoom)))));

	float zoomfactor=1.0;
	float length=0.0;
	if((mres1-mres0)>volobj->maxres*maxsize)
	{
		while((mres1-mres0)>volobj->maxres*maxsize)
		{
			resX /= 2.0;
			mres0 = -volobj->maxres+((resX)*(volobj->maxres/((volobj->maxres)+volrenderview.zoom)))+(resX-((resX)*(volobj->maxres/((volobj->maxres)+volrenderview.zoom))));
			mres1 = -volobj->maxres+((resX*2.0)*(volobj->maxres/((volobj->maxres)+volrenderview.zoom))+(resX-((resX)*(volobj->maxres/((volobj->maxres)+volrenderview.zoom)))));
		}
	}
	else if((mres1-mres0)<volobj->maxres*minsize)
	{
		while((mres1-mres0)<volobj->maxres*minsize)
		{
			resX *= 2.0;
			mres0 = -volobj->maxres+((resX)*(volobj->maxres/((volobj->maxres)+volrenderview.zoom)))+(resX-((resX)*(volobj->maxres/((volobj->maxres)+volrenderview.zoom))));
			mres1 = -volobj->maxres+((resX*2.0)*(volobj->maxres/((volobj->maxres)+volrenderview.zoom))+(resX-((resX)*(volobj->maxres/((volobj->maxres)+volrenderview.zoom)))));
		}
	}
	
	length =  mres1-mres0;

	/*printf("maxres: %f\n",maxres);
	printf("maxsize: %f\n",maxsize);
	printf("volobj->xscale: %f\n",volobj->xscale);
	printf("volobj->xsize: %f\n",volobj->xsize);
	printf("length: %f\n",length);
	printf("resX: %f\n",resX);
	printf("(resX)/(volobj->xsize/maxsize): %f\n",(resX)/(volobj->xsize/maxsize));
	printf("maxvolsize: %f\n",maxvolsize);
	printf("maxvolscale: %f\n",maxvolscale);*/
/*
	//make length divisible by 0.5
	float scalebar_length = resX*(volobj->xscale)/(volobj->xsize/volobj->maxsize)*volobj->x_resize;	
	float divisibleby = 1.0;
	if(scalebar_length>10.0) divisibleby = 10.0;
	else if(scalebar_length<10.0) divisibleby = 1.0;
	else if(scalebar_length<1.0) divisibleby = 0.5;

	float divs = scalebar_length/divisibleby;
	float floor_divs = floor(divs);
	float fract = divs - floor_divs;
	if(fract>=0.5) divs = floor_divs+1;
	else divs = floor_divs;
	float resizer = scalebar_length/(divs*divisibleby);
	if(resizer==0.0) resizer = 1.0;
	scalebar_length = divs*divisibleby;
	length *= resizer;

	scalebarfontpos = Vector(offset+marginsize, -volobj->maxres+(volobj->maxres*0.025f), 0.0);
	Vector scalebarpos = scalebarfontpos;
	scalebarpos.y += 64.0*(float)volobj->maxres/(float)volrenderview.winHeight;

	scalebartext.setNum(scalebar_length);
	if(scalebar_length>=10000.0f)	//CM
	{
		scalebar_length /= 10000.0f;
		scalebartext.setNum(scalebar_length);
		scalebartext += "c";
	}
	else if(scalebar_length>=1000.0f)	//MM
	{
		scalebar_length /= 1000.0f;
		scalebartext.setNum(scalebar_length);
		scalebartext += "m";
	}
	else							//MICRON
	{
		scalebartext += QString::fromStdWString (L" \u00B5");
	}
	scalebartext += "m";

	glPushMatrix();
	float linescale = ((float)volrenderview.winHeight/100.0f)*(float)volobj->maxres/(float)volrenderview.winHeight;
	glColor3f(background_fontcolour.red()/255.0, background_fontcolour.blue()/255.0, background_fontcolour.green()/255.0);
	glLineWidth(3.0);
	glBegin(GL_LINES);
	//horizontal bar
	glVertex3f(scalebarpos.x, scalebarpos.y, scalebarpos.z);
	glVertex3f(scalebarpos.x+length, scalebarpos.y, scalebarpos.z);
	//vertical caps
	glVertex3f(scalebarpos.x, scalebarpos.y+linescale, scalebarpos.z);
	glVertex3f(scalebarpos.x, scalebarpos.y-linescale, scalebarpos.z);
	glVertex3f(scalebarpos.x+length, scalebarpos.y+linescale, scalebarpos.z);
	glVertex3f(scalebarpos.x+length, scalebarpos.y-linescale, scalebarpos.z);
	glEnd();
	glLineWidth(1.0);

	glPopMatrix();*/
}
void VolumeRender::render_axis(void)
{
	glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_LIGHTING);
	
		glTranslatef(volrenderview.winWidth/1.125, volrenderview.winHeight/1.125, 128.0);
		glMultMatrixf(volrenderview.arcball.Transform.M);		

		float size = volrenderview.winWidth/16.0;

		glLineWidth(2.0);
		glBegin(GL_LINES);

		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(size, 0.0, 0.0);

		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, size, 0.0);
 
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, size);

		glEnd();

		glEnable(GL_LIGHTING);
	glPopMatrix();
}
void VolumeRender::render_slices(GLenum draw_mode)
{		
	glDisable(GL_LIGHTING);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

	if(draw_mode==3)
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER,0.0f);
	}

	//----------------------------
	//OUR 3 Slice planes
	//----------------------------
	
	if(draw_mode==1)
	{
		//YZ (X SLICE)
		glColor4f(0.0, 0.0, 1.0, 1.0);
		glBegin(GL_LINE_LOOP);	
			glVertex3f(-volobj->maxres*(0.5-(x_slice/(float)volobj->texwidth)), -volobj->maxres/2.0, -volobj->maxres/2.0);
			glVertex3f(-volobj->maxres*(0.5-(x_slice/(float)volobj->texwidth)), volobj->maxres/2.0, -volobj->maxres/2.0);
			glVertex3f(-volobj->maxres*(0.5-(x_slice/(float)volobj->texwidth)), volobj->maxres/2.0, volobj->maxres/2.0);
			glVertex3f(-volobj->maxres*(0.5-(x_slice/(float)volobj->texwidth)), -volobj->maxres/2.0, volobj->maxres/2.0);
		glEnd();
	}
	if(draw_mode==2 || draw_mode==3) 
	{
		glColor3f(1.0, 1.0, 1.0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, SLICEX);
		glBegin(GL_QUADS);	
			glTexCoord2d(0,0); glVertex3f(-volobj->maxres*(0.5-(x_slice/(float)volobj->texwidth)), -volobj->maxres/2.0, -volobj->maxres/2.0);
			glTexCoord2d(1,0); glVertex3f(-volobj->maxres*(0.5-(x_slice/(float)volobj->texwidth)), volobj->maxres/2.0, -volobj->maxres/2.0);
			glTexCoord2d(1,1); glVertex3f(-volobj->maxres*(0.5-(x_slice/(float)volobj->texwidth)), volobj->maxres/2.0, volobj->maxres/2.0);
			glTexCoord2d(0,1); glVertex3f(-volobj->maxres*(0.5-(x_slice/(float)volobj->texwidth)), -volobj->maxres/2.0, volobj->maxres/2.0);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	if(draw_mode==1)
	{
		//XZ (Y SLICE)
		glColor4f(1.0, 0.0, 0.0, 1.0);
		glBegin(GL_LINE_LOOP);	
			glVertex3f(-volobj->maxres/2.0, -volobj->maxres*(0.5-(y_slice/(float)volobj->texheight)), -volobj->maxres/2.0);
			glVertex3f(volobj->maxres/2.0, -volobj->maxres*(0.5-(y_slice/(float)volobj->texheight)), -volobj->maxres/2.0);
			glVertex3f(volobj->maxres/2.0, -volobj->maxres*(0.5-(y_slice/(float)volobj->texheight)), volobj->maxres/2.0);
			glVertex3f(-volobj->maxres/2.0, -volobj->maxres*(0.5-(y_slice/(float)volobj->texheight)), volobj->maxres/2.0);
		glEnd();
	}
	if(draw_mode==2 || draw_mode==3)
	{
		glColor3f(1.0, 1.0, 1.0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, SLICEY);
		glBegin(GL_QUADS);	
			glTexCoord2d(0,0); glVertex3f(-volobj->maxres/2.0, -volobj->maxres*(0.5-(y_slice/(float)volobj->texheight)), -volobj->maxres/2.0);
			glTexCoord2d(1,0); glVertex3f(volobj->maxres/2.0, -volobj->maxres*(0.5-(y_slice/(float)volobj->texheight)), -volobj->maxres/2.0);
			glTexCoord2d(1,1); glVertex3f(volobj->maxres/2.0, -volobj->maxres*(0.5-(y_slice/(float)volobj->texheight)), volobj->maxres/2.0);
			glTexCoord2d(0,1); glVertex3f(-volobj->maxres/2.0, -volobj->maxres*(0.5-(y_slice/(float)volobj->texheight)), volobj->maxres/2.0);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	if(draw_mode==1)
	{
		//XY (Z SLICE)
		glColor4f(0.0, 1.0, 0.0, 1.0);
		glBegin(GL_LINE_LOOP);	
			glVertex3f(-volobj->maxres/2.0, -volobj->maxres/2.0, -volobj->maxres*(0.5-(z_slice/(float)volobj->texdepth)));
			glVertex3f(volobj->maxres/2.0, -volobj->maxres/2.0, -volobj->maxres*(0.5-(z_slice/(float)volobj->texdepth)));
			glVertex3f(volobj->maxres/2.0, volobj->maxres/2.0, -volobj->maxres*(0.5-(z_slice/(float)volobj->texdepth)));
			glVertex3f(-volobj->maxres/2.0, volobj->maxres/2.0, -volobj->maxres*(0.5-(z_slice/(float)volobj->texdepth)));
		glEnd();
	}
	if(draw_mode==2 || draw_mode==3) 
	{
		glColor3f(1.0, 1.0, 1.0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, SLICEZ);
		glBegin(GL_QUADS);	
			glTexCoord2d(0,0); glVertex3f(-volobj->maxres/2.0, -volobj->maxres/2.0, -volobj->maxres*(0.5-(z_slice/(float)volobj->texdepth)));
			glTexCoord2d(1,0); glVertex3f(volobj->maxres/2.0, -volobj->maxres/2.0, -volobj->maxres*(0.5-(z_slice/(float)volobj->texdepth)));
			glTexCoord2d(1,1); glVertex3f(volobj->maxres/2.0, volobj->maxres/2.0, -volobj->maxres*(0.5-(z_slice/(float)volobj->texdepth)));
			glTexCoord2d(0,1); glVertex3f(-volobj->maxres/2.0, volobj->maxres/2.0, -volobj->maxres*(0.5-(z_slice/(float)volobj->texdepth)));
		glEnd();	
		glDisable(GL_TEXTURE_2D);
	}	

	if(draw_mode==3)
	{
		glDisable(GL_ALPHA_TEST);
	}
}
void VolumeRender::render_boundingbox(void)
{
	float scene_size = (float)LOD_FBO_SIZE;
	if(fbo_lod) scene_size = (float)LOD_FBO_SIZE/4.0f;
	float linesize = scene_size*(1.0/(volobj->maxres*0.5));
	float pointsize = scene_size*(1.0/volobj->maxres);

	//Vector bb_scale = Vector(volobj->maxres,volobj->maxres,volobj->maxres);
	
	glDisable(GL_LIGHTING);
	glLineWidth(linesize);
	glPointSize(pointsize);
	glColor3ub(bb_colour.red(), bb_colour.green(), bb_colour.blue());

	glBegin(GL_LINE_LOOP);
		glVertex3fv(&volrenderview.bb_v2.x);
		glVertex3fv(&volrenderview.bb_v6.x);
		glVertex3fv(&volrenderview.bb_v8.x);
		glVertex3fv(&volrenderview.bb_v4.x);
	glEnd();
	glBegin(GL_LINE_LOOP);
		glVertex3fv(&volrenderview.bb_v3.x);
		glVertex3fv(&volrenderview.bb_v7.x);
		glVertex3fv(&volrenderview.bb_v5.x);
		glVertex3fv(&volrenderview.bb_v1.x);
	glEnd();
	glBegin(GL_LINE_LOOP);
		glVertex3fv(&volrenderview.bb_v5.x);
		glVertex3fv(&volrenderview.bb_v6.x);
		glVertex3fv(&volrenderview.bb_v2.x);
		glVertex3fv(&volrenderview.bb_v1.x);
	glEnd();
	glBegin(GL_LINE_LOOP);
		glVertex3fv(&volrenderview.bb_v4.x);
		glVertex3fv(&volrenderview.bb_v8.x);
		glVertex3fv(&volrenderview.bb_v7.x);
		glVertex3fv(&volrenderview.bb_v3.x);
	glEnd();
	glEnable(GL_LIGHTING);
}
void VolumeRender::render_stereo(int render)
{
	if(!volobj->texture3d) return;

   //stereo_eye_seperation = 0.5;
   float PIXELS_PER_INCH = 60;

    double xfactor=1.0, yfactor=1.0;

	if(volrenderview.camera.width < volrenderview.camera.height)
	{
		xfactor = 1.0;
		yfactor = volrenderview.camera.height/volrenderview.camera.width;
	}
	else if(volrenderview.camera.height < volrenderview.camera.width)
	{
		xfactor = volrenderview.camera.width/volrenderview.camera.height;
		yfactor = 1.0;
	}

//	glDrawBuffer(GL_BACK);                                   //draw into both back buffers
	initialize_DrawBufferGL();

	if(offscreen_toggle==0) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      //clear color and depth buffers
 	
	for(int i=0;i<2;i++)
	{

			if(stereo_mode==0) 
			{
				glClearColor( background_colour.red()/255.0, background_colour.green()/255.0, background_colour.blue()/255.0, 1.0f );
			}

			glClear(GL_DEPTH_BUFFER_BIT);

			if(i==0) // right eye - RED 
			{
				stereo_eye_seperation = stereo_eye_seperation;
				
				if(stereo_mode==0) initializeLeft_DrawBufferGL();//glDrawBuffer(GL_BACK_RIGHT);//initializeLeft_DrawBufferGL();                              //draw into back left buffer
				else glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE);
			}
			else	// left eye - BLUE  <--Change to GREEN for green lenses  
			{
				stereo_eye_seperation = -stereo_eye_seperation;
				
				if(stereo_mode==0) initializeRight_DrawBufferGL();  //glDrawBuffer(GL_BACK_LEFT);//initializeRight_DrawBufferGL();                           //draw into back right buffer
				else if(stereo_mode==1) glColorMask(GL_FALSE,GL_FALSE,GL_TRUE,GL_TRUE);
				else if(stereo_mode==2) glColorMask(GL_FALSE,GL_TRUE,GL_FALSE,GL_TRUE);
				else if(stereo_mode==3) glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_TRUE);
			}
				
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
	
				/*float maxres =  max(volobj->texwidth*(volobj->xsize), volobj->texheight*(volobj->ysize));
				float offset = ((volrenderview.winWidth/volrenderview.winHeight)*maxres)/2.0f;

				maxres = max(maxres, volobj->texdepth*(volobj->zsize));
				maxres += offset;	
				maxres /= 2.0;	*/

				//glFrustum(-maxres-volrenderview.zoom, maxres+volrenderview.zoom, -maxres-volrenderview.zoom, maxres+volrenderview.zoom, -1024, 1024);

				glFrustum(
				(-(volrenderview.camera.width/(2.0*PIXELS_PER_INCH))+stereo_eye_seperation)*(volrenderview.camera.znear/volrenderview.camera.zscreen)*xfactor,
				(volrenderview.camera.width/(2.0*PIXELS_PER_INCH)+stereo_eye_seperation)*(volrenderview.camera.znear/volrenderview.camera.zscreen)*xfactor,
				-(volrenderview.camera.width/(2.0*PIXELS_PER_INCH))*(volrenderview.camera.znear/volrenderview.camera.zscreen)*yfactor,
				(volrenderview.camera.width/(2.0*PIXELS_PER_INCH))*(volrenderview.camera.znear/volrenderview.camera.zscreen)*yfactor,
				volrenderview.camera.znear, 
				volrenderview.camera.zfar);

			//glOrtho(-volobj->maxres-volrenderview.zoom, volobj->maxres+volrenderview.zoom, -volobj->maxres-volrenderview.zoom, volobj->maxres+volrenderview.zoom, -1024, 1024);

			glTranslatef(stereo_eye_seperation, 0.0, -(volrenderview.zoom+230.0f));
			
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		
			volrenderview.cutting_planes.disable();
			render_scene(render);			
	//if(offscreen_toggle==1) return; 
	}

	//3D cursor if in stereo mode
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		float cursor_size = (float)volrenderview.winWidth/40.0;
		float gap_size = cursor_size/5.0;
		
		glColor3f(background_fontcolour.red()/255.0, background_fontcolour.blue()/255.0, background_fontcolour.green()/255.0);
		glLineWidth(3.0);
		glBegin(GL_LINES);
		glVertex3f(stereo_cursor.x-cursor_size, stereo_cursor.y, stereo_cursor.z);
		glVertex3f(stereo_cursor.x-gap_size, stereo_cursor.y, stereo_cursor.z);

		glVertex3f(stereo_cursor.x+cursor_size, stereo_cursor.y, stereo_cursor.z);
		glVertex3f(stereo_cursor.x+gap_size, stereo_cursor.y, stereo_cursor.z);

		glVertex3f(stereo_cursor.x, stereo_cursor.y-cursor_size, stereo_cursor.z);
		glVertex3f(stereo_cursor.x, stereo_cursor.y-gap_size, stereo_cursor.z);

		glVertex3f(stereo_cursor.x, stereo_cursor.y+cursor_size, stereo_cursor.z);
		glVertex3f(stereo_cursor.x, stereo_cursor.y+gap_size, stereo_cursor.z);
		glEnd();

		glColor3f(1.0-background_fontcolour.red()/255.0, 1.0-background_fontcolour.blue()/255.0, 1.0-background_fontcolour.green()/255.0);
		glLineWidth(1.0);
		glBegin(GL_LINES);
		glVertex3f(stereo_cursor.x-cursor_size, stereo_cursor.y, stereo_cursor.z);
		glVertex3f(stereo_cursor.x-gap_size, stereo_cursor.y, stereo_cursor.z);

		glVertex3f(stereo_cursor.x+cursor_size, stereo_cursor.y, stereo_cursor.z);
		glVertex3f(stereo_cursor.x+gap_size, stereo_cursor.y, stereo_cursor.z);

		glVertex3f(stereo_cursor.x, stereo_cursor.y-cursor_size, stereo_cursor.z);
		glVertex3f(stereo_cursor.x, stereo_cursor.y-gap_size, stereo_cursor.z);

		glVertex3f(stereo_cursor.x, stereo_cursor.y+cursor_size, stereo_cursor.z);
		glVertex3f(stereo_cursor.x, stereo_cursor.y+gap_size, stereo_cursor.z);
		glEnd();

	glPopMatrix();

	if(stereo_mode!=0) glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	stereo_eye_seperation = -stereo_eye_seperation;

 //  glextensions.fbo_support=1;


}
void VolumeRender::render_meshes(void)
{
	glPushMatrix();
	glEnable(GL_LIGHTING);
	
	if(glextensions.shader_support) mesh_shaderobj.enable();

	for(int i=0; i<meshlist.meshes.size(); i++)
	{
		if(!meshlist.clipstate.empty()) if(meshlist.clipstate[i]) volrenderview.cutting_planes.enable();

		//glColor3fv(&meshlist.colours[i].x);
		if(!meshlist.colours.empty()) if(glextensions.shader_support) mesh_shaderobj.sendUniform3fv("surfcol",1, &meshlist.colours[i].x);
		meshlist.meshes[i].r_vbo(GL_TRIANGLES);
		//meshlist.meshes[i].r_normal(GL_TRIANGLES, 0);

		if(!meshlist.clipstate.empty()) if(meshlist.clipstate[i]) volrenderview.cutting_planes.disable();

		//meshlist.meshes[i].r_normal(GL_TRIANGLES, 0);
		
		//glColor3f(1,1,0);
		//meshlist.meshes[i].display_normals();
	}

	if(glextensions.shader_support) mesh_shaderobj.disable();
	
	glDisable(GL_LIGHTING);
	glPopMatrix();
}
void VolumeRender::render_aabb(Vector cen, Vector dim)
{
	Vector bb[8];
	bb[0] = Vector(-1.0,  1.0,  1.0);
	bb[1] = Vector( 1.0,  1.0,  1.0);
	bb[2] = Vector(-1.0, -1.0,  1.0);
	bb[3] = Vector( 1.0, -1.0,  1.0);
	bb[4] = Vector(-1.0,  1.0, -1.0);
	bb[5] = Vector( 1.0,  1.0, -1.0);
	bb[6] = Vector(-1.0, -1.0, -1.0);
	bb[7] = Vector( 1.0, -1.0, -1.0);

	//scale it by size
	dim /= 2.0;
	bb[0] = bb[0]*dim; bb[1] = bb[1]*dim; bb[2] = bb[2]*dim; bb[3] = bb[3]*dim;
	bb[4] = bb[4]*dim; bb[5] = bb[5]*dim; bb[6] = bb[6]*dim; bb[7] = bb[7]*dim;

	bb[0] += cen;
	bb[1] += cen;
	bb[2] += cen;
	bb[3] += cen;
	bb[4] += cen;
	bb[5] += cen;
	bb[6] += cen;
	bb[7] += cen;

	glPushMatrix();
	glDisable(GL_LIGHTING);

//	glScalef(scale, scale, scale);
//	glTranslatef(-centre.x, -centre.y, -centre.z);
/*
	PlaneWidget p1(v1,v2,v4,v3,1);
	PlaneWidget p2(v7,v8,v6,v5,2);
	PlaneWidget p3(v2,v6,v8,v4,3);
	PlaneWidget p4(v3,v7,v5,v1,4);
	PlaneWidget p5(v5,v6,v2,v1,5);
	PlaneWidget p6(v4,v8,v7,v3,6);
*/
//FRONT
	glBegin(GL_LINE_LOOP);
		glVertex3fv(&bb[0].x);
		glVertex3fv(&bb[1].x);
		glVertex3fv(&bb[3].x);
		glVertex3fv(&bb[2].x);
	glEnd();

	//LEFT
	glBegin(GL_LINE_LOOP);
		glVertex3fv(&bb[6].x);
		glVertex3fv(&bb[7].x);
		glVertex3fv(&bb[5].x);
		glVertex3fv(&bb[4].x);
	glEnd();

	//BACK
	glBegin(GL_LINE_LOOP);
		glVertex3fv(&bb[1].x);
		glVertex3fv(&bb[5].x);
		glVertex3fv(&bb[7].x);
		glVertex3fv(&bb[3].x);
	glEnd();

	//RIGHT
	glBegin(GL_LINE_LOOP);
		glVertex3fv(&bb[2].x);
		glVertex3fv(&bb[6].x);
		glVertex3fv(&bb[4].x);
		glVertex3fv(&bb[0].x);
	glEnd();

	//TOP
	glBegin(GL_LINE_LOOP);
		glVertex3fv(&bb[4].x);
		glVertex3fv(&bb[5].x);
		glVertex3fv(&bb[1].x);
		glVertex3fv(&bb[0].x);
	glEnd();

	//BOTTOM
	glBegin(GL_LINE_LOOP);
		glVertex3fv(&bb[3].x);
		glVertex3fv(&bb[7].x);
		glVertex3fv(&bb[6].x);
		glVertex3fv(&bb[2].x);
	glEnd();

	glEnable(GL_LIGHTING);
	glPopMatrix();
}
void VolumeRender::render_sphere(Vector c, float r,int n, GLenum drawmode)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	double i,j;
	double theta1,theta2,theta3;
	Vector rr = Vector(1.0, 1.0, 1.0);
	Vector e,p, p1;

	//glBegin(drawmode);
	for (j=0;j<n;j++) 
	{
		theta1 = j * TWOPI / n - (PI/2.0);
		theta2 = (j + 1) * TWOPI / n - (PI/2.0);

		glBegin(drawmode);
		//glBegin(GL_QUADS);
		//glBegin(GL_POINTS);
		for (i=0;i<=n;i++) 
		{
			theta3 = i * TWOPI / n;

			e.x = cos(theta2) * cos(theta3);
			e.y = sin(theta2);
			e.z = cos(theta2) * sin(theta3);

			//generate point on unit sphere
			p = r*e;
			
			//transform it to our ellipsoid space
			//p1 = rot*p;

			//apply the centre translation
			p = c + p;

			glNormal3f(-e.x,-e.y,-e.z);
			//glTexCoord2f(i/(double)n,2*(j+1)/(double)n);
			glVertex3fv(&p.x);

			e.x = cos(theta1) * cos(theta3);
			e.y = sin(theta1);
			e.z = cos(theta1) * sin(theta3);

			//generate point on unit sphere
			p = r*e;

			//transform it to our ellipsoid
//			p1 = rot*p;

			//apply the centre translation
			p = c + p;

			glNormal3f(-e.x,-e.y,-e.z);
			//glTexCoord2f(i/(double)n,2*j/(double)n);
			glVertex3fv(&p.x);		
		}
		glEnd();
	}
	//glEnd();
	glDisable(GL_CULL_FACE);
}
void VolumeRender::render_cropshape(void)
{
	Vector v;
	Vector v2;
	float offset = 0.2f;

	glPushMatrix();
	glDisable(GL_LIGHTING);

	float screenzoom = (float)(volobj->maxres+volrenderview.zoom)/(float)volobj->maxres;
	float linesize;
	float pointsize = ((float)volobj->maxres/100.0)/screenzoom;

	Vector bb_size = Vector(volobj->texwidth*volobj->xsize/volobj->maxsize, volobj->texheight*volobj->ysize/volobj->maxsize, volobj->texdepth*volobj->zsize/volobj->maxsize);
	bb_size /= 2.0;

	float maxl = bb_size.x;
	maxl = max (maxl, bb_size.y);
	maxl = max (maxl, bb_size.z);
	
	pointsize = (float)((2*maxl)/20.0)*screenzoom;
	if(pointsize>1.0) pointsize = 1.0;
	if(pointsize<0.25) pointsize = 0.25;
	linesize = pointsize*2.0;

	//printf("screenvolrenderview.zoom: %f\n", screenvolrenderview.zoom);
	//printf("pointsize: %f\n", pointsize);
	//printf("pointsize: %f\n", pointsize);

	//=================================================================================================
	//CROP SHAPE
	//=================================================================================================
	glLineWidth(linesize);

	glEnable(GL_LIGHTING);
	for(int iii=0; iii<crop_shape.size(); iii++)
	{	
			glColor4f(1,1,0,1);
			render_sphere(crop_shape[iii], pointsize, 12, GL_QUAD_STRIP);

			if(crop_shape_selected==iii)
			{
				glColor3f(1,1,0);
				render_aabb(crop_shape[iii], Vector(pointsize*2.0));
			}
	}
	glDisable(GL_LIGHTING);

	if(crop_shape.size()>2)
	{
		glLineWidth(linesize);
		glBegin(GL_LINES);
		for(int iii=1; iii<crop_shape.size(); iii++)
		{
			glColor4f(1,0,0,1);
			glVertex3fv(&crop_shape[iii].x);
			glVertex3fv(&crop_shape[iii-1].x);
		}
		if(crop_shape.size()>1)
		{
			glColor4f(1,0,0,1);
			glVertex3fv(&crop_shape[0].x);
			glVertex3fv(&crop_shape[crop_shape.size()-1].x);
		}
		glEnd();
	}

	glPointSize(1.0);
	glLineWidth(1.0);
	glEnable(GL_LIGHTING);
	//glEnable(GL_BLEND);

	//glDisable(GL_LINE_STIPPLE);
	//glDisable(GL_POLYGON_OFFSET_FILL);
	glPopMatrix();
}
void VolumeRender::render_measurements(void)
{
	glPushMatrix();
	glDisable(GL_LIGHTING);

	//=================================================================================================
	// CURRENT (ACTIVE) OBJECT
	//=================================================================================================
	glLineWidth(measure_line_size);
	glPointSize(measure_point_size);

	//faces
	glColor3fv(&measure_active_face_col.x);
	if(measure_tris) currentMeasureObject.display_faces();

	//edges
	glColor3fv(&measure_active_edge_col.x);
	currentMeasureObject.display_edges();

	//verts
	glColor3fv(&measure_active_vert_col.x);
	currentMeasureObject.display_verts();

	//arrow_heads
	if(measure_arrowlabel) currentMeasureObject.display_arrowheads(measure_arrowhead_size);

	//=================================================================================================
	//DRAWING HELPERS
	//=================================================================================================
	if(current_measure_drawmode==MEASURE_DRAWMODE_MAGNETLINE)
	{
		glPointSize(measure_active_point_size);
		glLineWidth(measure_active_line_size);

		//magnet line end point
		glColor3fv(&measure_magnet_col.x);
		currentMeasureObject.display_selected_vert(measure_magnetline_vert_old);
		
		//face creation helper (edges)
		glColor3fv(&measure_selected_col.x);
		glBegin(GL_LINE_LOOP);
		for(int i=0; i<current_face.size(); i++)
			glVertex3fv(&currentMeasureObject.vertices[current_face[i]].x);
		glEnd();

		//face creation helper (verts)
		glColor3fv(&measure_selected_col.x);
		for(int i=0; i<current_face.size(); i++)
		{
			currentMeasureObject.display_selected_vert(current_face[i]);
		}
	}
	
	//=================================================================================================
	// SELECTED
	//=================================================================================================
	glPointSize(measure_active_point_size);
	glLineWidth(measure_active_line_size);
	glColor3fv(&measure_selected_col.x);
	currentMeasureObject.display_selected_face(current_measure_faceindex);
	currentMeasureObject.display_selected_edge(current_measure_edgeindex);
	currentMeasureObject.display_selected_vert(current_measure_vertindex);
	
	//=================================================================================================
	// ALL OTHER OBJECTS
	//=================================================================================================
	for(int i=0; i<measures.measure_objects.size(); i++)
	{
		if(i!=current_measure_index)
		{
			glLineWidth(measure_line_size);
			glPointSize(measure_point_size);

			//faces
			glColor3fv(&measure_face_col.x);
			if(measure_tris) measures.measure_objects[i].display_faces();

			//edges
			glColor3fv(&measure_edge_col.x);
			measures.measure_objects[i].display_edges();

			//verts
			glColor3fv(&measure_vert_col.x);
			measures.measure_objects[i].display_verts();

			//arrow_heads
			//if(measure_arrowlabel) measures.measure_objects[i].display_arrowheads();
		}
	}

	//glColor3f(0,0,0);
	//if(current_measure_drawmode==10) currentMeasureObject.display_beizer_patch();
	//glEnable(GL_LIGHTING);
	//=================================================================================================

	glPointSize(1.0);
	glLineWidth(1.0);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}
void VolumeRender::render_ellipsoid(Vector c,Vector r,  Matrix4x4* rotmat, int n, GLenum drawmode)
{
	int i,j;
	double theta1,theta2,theta3;
	Vector rr = Vector(1.0, 1.0, 1.0);
	Vector e,p, p1;

	for (j=0;j<n;j++) 
	{
		theta1 = j * TWOPI / n - (PI/2.0);
		theta2 = (j + 1) * TWOPI / n - (PI/2.0);

		glBegin(drawmode);
		//glBegin(GL_QUADS);
		//glBegin(GL_POINTS);
		for (i=0;i<=n;i++) 
		{
			theta3 = i * TWOPI / n;

			e.x = cos(theta2) * cos(theta3);
			e.y = sin(theta2);
			e.z = cos(theta2) * sin(theta3);

			//generate point on unit sphere
			p = r*e;
			
			//transform it to our ellipsoid
			p1 = *rotmat*p;

			//apply the centre translation
			p1 = c + p1;

			glNormal3f(e.x,e.y,e.z);
			glTexCoord2f(i/(double)n,2*(j+1)/(double)n);
			glVertex3fv(&p1.x);

			e.x = cos(theta1) * cos(theta3);
			e.y = sin(theta1);
			e.z = cos(theta1) * sin(theta3);

			//generate point on unit sphere
			p = r*e;

			//transform it to our ellipsoid
			p1 = *rotmat*p;

			//apply the centre translation
			p1 = c + p1;

			glNormal3f(e.x,e.y,e.z);
			glTexCoord2f(i/(double)n,2*j/(double)n);
			glVertex3fv(&p1.x);
			
		}
		glEnd();
	}
}
//=================================================================================================================================
void VolumeRender::update_transfer_functions(VolumeRenderView vview)
{
	apply_transfer_function(1, vview.luminance);
	apply_transfer_function(2, vview.alpha);

	apply_transfer_function(3, vview.redluminance);
	apply_transfer_function(4, vview.greenluminance);
	apply_transfer_function(5, vview.blueluminance);

	apply_transfer_function(6, vview.redalpha);
	apply_transfer_function(7, vview.greenalpha);
	apply_transfer_function(8, vview.bluealpha);
}
void VolumeRender::apply_transfer_functions(int* rgb, int* ragaba)
{
	for(int i =0; i<256*3; i++)
	{
		rgb_tfunc[i] = rgb[i];
		ragaba_tfunc[i] = ragaba[i];
	}
	load_1DLookupTables(true);
}

void VolumeRender::compute_transfer_function(int mode, TfuncState tfunc, int* myarray)
{
	float K_os;
	float K_oe;

	K_os = (float)tfunc.brightness/100.0;
	K_oe = (float)tfunc.contrast/100.0;
//	K_oe = -1.0+K_oe;

	//if(K_oe>0.85) K_oe=0.85;

	//if(K_oe>0.0) K_oe = 1.0/(1.0-K_oe);
	//else K_oe = 1.0-(-K_oe); 

	int index, value;
	for(int i =0; i<256; i++)
	{
		//get the value from our bspline curve
		//transform value to 0 -> 255 range.
		value = 255-(int)(tfunc.functioneditor.bs.curve_points[i].y+0.5);
		
		//apply brightness & contrast
		value = pow((K_os*value),K_oe);

		//clamp value to 0->255
		if(value<0) value = 0;
		if(value>255) value = 255;

		//threshold the value
		if(i<=tfunc.threshold_min) value = 0;
		else if(i>tfunc.threshold_max) value = 0;

		index = 3*i;

		if(mode == 1)	//luminance
		{
			myarray[index] = myarray[index+1] = myarray[index+2] = value;
		}
		else if(mode == 2) //alpha
		{
			myarray[index] = myarray[index+1] = myarray[index+2] = value;
		}
		else if(mode == 3) //red luminance
		{
			myarray[index] = value;
		}
		else if( mode == 4) //green luminance
		{
			myarray[index+1] = value;
		}
		else if(mode == 5) //blue luminance
		{
			myarray[index+2] = value;
		}
		else if(mode == 6) //red alpha
		{
			myarray[index] = value;
		}
		else if(mode == 7) //green alpha
		{		
			myarray[index+1] = value;
		}
		else if(mode == 8) //blue alpha
		{		
			myarray[index+2] = value;
		}
	}
}

void VolumeRender::apply_transfer_function(int mode, TfuncState tfunc)
{
	if(mode == 1)	//luminance
	{
		volrenderview.redluminance = volrenderview.greenluminance = volrenderview.blueluminance = volrenderview.luminance = tfunc;
	}
	else if(mode == 2) //alpha
	{
		volrenderview.redalpha = volrenderview.greenalpha = volrenderview.bluealpha = volrenderview.alpha = tfunc;
	}
	else if(mode == 3) //red luminance
	{
		volrenderview.redluminance = tfunc;
	}
	else if( mode == 4) //green luminance
	{
		volrenderview.greenluminance = tfunc;
	}
	else if(mode == 5) //blue luminance
	{
		volrenderview.blueluminance = tfunc;
	}
	else if(mode == 6) //red alpha
	{
		volrenderview.redalpha = tfunc;
	}
	else if(mode == 7) //green alpha
	{		
		volrenderview.greenalpha = tfunc;
	}
	else if(mode == 8) //blue alpha
	{		
		volrenderview.bluealpha = tfunc;
	}

	float K_os;
	float K_oe;

	K_os = (float)tfunc.brightness/100.0;
	K_oe = (float)tfunc.contrast/100.0;
	//K_oe = -1.0+K_oe;

//	printf("K_oe orig: %f\n", K_oe);
	//if(K_oe>0.85) K_oe=0.85;

	//if(K_oe>0.0) K_oe = 1.0/(1.0-K_oe);
	//else K_oe = 1.0-(-K_oe); 

//	printf("K_oe: %f\n", K_oe);
	//printf("val: %f\n", (float) pow(128.0, (double)K_oe));

	int index, value;
	for(int i =0; i<256; i++)
	{
		//get the value from our bspline curve
		//transform value to 0 -> 255 range.
		value = 255-(int)(tfunc.functioneditor.bs.curve_points[i].y+0.5);
		
		//apply brightness & contrast
		value = pow((K_os*value),K_oe);

		//clamp value to 0->255
		if(value<0) value = 0;
		if(value>255) value = 255;

		//threshold the value
		if(i<=tfunc.threshold_min) value = 0;
		else if(i>tfunc.threshold_max) value = 0;

		index = 3*i;

		if(mode == 1)	//luminance
		{
			rgb_tfunc[index] = rgb_tfunc[index+1] = rgb_tfunc[index+2] = value;
		}
		else if(mode == 2) //alpha
		{
			ragaba_tfunc[index] = ragaba_tfunc[index+1] = ragaba_tfunc[index+2] = value;
		}
		else if(mode == 3) //red luminance
		{
			rgb_tfunc[index] = value;
		}
		else if( mode == 4) //green luminance
		{
			rgb_tfunc[index+1] = value;
		}
		else if(mode == 5) //blue luminance
		{
			rgb_tfunc[index+2] = value;
		}
		else if(mode == 6) //red alpha
		{
			ragaba_tfunc[index] = value;
		}
		else if(mode == 7) //green alpha
		{		
			ragaba_tfunc[index+1] = value;
		}
		else if(mode == 8) //blue alpha
		{		
			ragaba_tfunc[index+2] = value;
		}
	}

	load_1DLookupTables(true);
	build_histogram();
}
