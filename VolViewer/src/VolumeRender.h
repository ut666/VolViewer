/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef VOLREN_H
#define VOLREN_H

#include <qgl.h>
#include <qimage.h> 
#include <qstring.h> 
#include <qmessagebox.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qcolordialog.h>
#include <QThreadPool>

#include <math.h>
#include <vector>
using namespace std;

#include "glext.h"

#include "VolumeObject.h"
#include "VolIO.h"
#include "VolumeRenderView.h"

#include "Vector.h"
//#include "Camera.h"
//#include "ArcBall.h"
#include "MeshListObject.h"
#include "Matrix4x4.h"
#include "PlaneWidgetInCube.h"
#include "PlaneWidget.h" 
//#include "LineSegmentObject.h"
//#include "FTGL_FontManager.h"
#include "ColourFun.h"
#include "GL_Ext.h"
#include "GL_ShaderObj.h"
#include "GL_FrameBufferObject.h"
#include "Timer.h"
#include "PCA.h"
#include "Triangulate.h"
#include "CVector.h"
#include "Edge.h"
#include "MeasureObject.h"
#include "MeasureListObject.h"
#include "MeasureDefinitions.h"
#include "ShaderDefinitions.h"

#include "QtTransferFunctionEditorDialogue.h"
#include "QtThreadedBrickLoader.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

enum {X, Y, Z, W};

struct vbo_vert
{
    float x, y, z;        //Vertex
    float tx, ty, tz;     //Texture
};

class VolumeRender
{

public:

	double nonfbo_mvmatrix[16];
	double nonfbo_projmatrix[16];
	int nonfbo_viewport[4];
	Vector stereo_cursor;
	float cam_step_size;

	QString appPath;
	GLint chan_loc;

	vector< Vector > gridtexcoords;
	bool curve_coords;
	Vector curve_vectranslate;
	float curveXang;
	float curveYang;
	int curve_translate;
	Vector debugv;

	//==================================
	//global helper objects
	//==================================
	Timer timer;
	VolumeRenderView volrenderview;
	MeshListObject meshlist;
	GL_Ext glextensions;

	//==================================
	//pointers to helper objects
	//==================================
	VolumeObject* volobj;
	VolIO* volio;

	//==================================
	//Frame Buffer Objects
	//==================================
	int FBO_SIZE;
	int LOD_FBO_SIZE;
	int render_size;
	int fbo_lod;
	int fbo_depth;
	int vbo_size;
	GL_FrameBufferObject fbo_object;
	GL_FrameBufferObject lodfbo_object;

	//==================================
	//Texture indices
	//==================================
	GLuint SLICEX;
	GLuint SLICEY;
	GLuint SLICEZ;

	GLuint TEXTURE2D_RGB;
	GLuint TEXTURE2D_RAGABA;
	GLuint TEXTURE2D_FBO_FB;
	GLuint TEXTURE2D_FBO_DP;
	GLuint TEXTURE2D_LODFBO_FB;
	GLuint TEXTURE2D_LODFBO_DP;
	GLuint TEXTURE2D_JITTER;
	GLuint TEXTURE3D_PBO;
	GLuint TEXTURE2D_FBO_DEPTH;
	GLuint TEXTURE2D_FBO_COL;
	
	//==================================
	//VBO Object
	//==================================
	GLuint vertexVBO_ID;
	GLuint indiciesVBO_ID;

	vbo_vert* vbo_array;
	unsigned short* vbo_indices;

	int vbo_vert_size;
	int vbo_indices_size;

	//array to store our depth buffer
	unsigned int* pealdepthdata;
	unsigned int* depthdata;
	unsigned char* coldata;

	//==================================
	//Texture Coordinates
	//==================================
	Vector texrot_t1, texrot_t2, texrot_t3, texrot_t4;
	Matrix4x4 texrot_mat;
	float texrot_s;
	int texrot_index;
	int texrot_i;
	int texrot_indexhelper;
	Vector v1, v2, v3, v4;
	Vector t1, t2, t3, t4;
	int numbslices;
	
	//==================================
	//OpenGL Shaders
	//==================================
	int				volshader_id;
	GL_ShaderOBJ	vol_sectionshaderobj;
	GL_ShaderOBJ	vol_shaderobj;
	GL_ShaderOBJ	vol2d_shaderobj;
	GL_ShaderOBJ	vol2dmulti_shaderobj;
	GL_ShaderOBJ	vol_rgbsectionshaderobj;
	GL_ShaderOBJ	vol_shadersinglepassobj;
	GL_ShaderOBJ	mesh_shaderobj;
	GL_ShaderOBJ	postrendering_shaderobj;
	
	//depth cue
	float depthcue;

	//gooch lighting
	Vector goochwarm;
	Vector goochcool;
	float goochcontrib;
	float goochexp;
	float goochalpha;
	float goochdegen;

	//silhouttes
	int silhouette_enable;
	float silhouette_amount;

	//gooch tone lighting
	int gooch_enable;
	float gooch_toneCont;
	float gooch_warmCol[3];
	float gooch_coolCol[3];

	//depth peal
	bool detphpeal;
	float depthoffset;
	float depththickness;
	float olddepthoffset;
	float olddepththickness;

	//2d post process
	float postprocess2d_amount;

	//==================================
	//Transfer Functions
	//==================================
	unsigned char rgb_tfunc[256*3];
	unsigned char ragaba_tfunc[256*3];

	/*
	//bright & cont for tfuncts
	int l_bright, l_cont, la_bright, la_cont, r_bright, r_cont, g_bright, g_cont, b_bright, b_cont, ra_bright, ra_cont, ga_bright, ga_cont, ba_bright, ba_cont;

	//thresholds for tfuncts;
	int l_tmax, l_tmin, la_tmax, la_tmin, r_tmax, r_tmin, g_tmax, g_tmin, b_tmax, b_tmin, ra_tmax, ra_tmin, ga_tmax, ga_tmin, ba_tmax, ba_tmin;
	//bool inv_l, inv_la, inv_r, inv_g, inv_b, inv_ra, inv_ga, inv_ba;
	*/

	//a backup copy of transfer functions
	//used to make changing threshold values quicker
	//so we have to send less data between widgets.
	unsigned char backup_rgb_tfunc[256*3];
	unsigned char backup_ragaba_tfunc[256*3];

	//==================================
	//Background Colour
	//==================================
	QColor old_background_colour;
	QColor background_colour;
	QColor background_fontcolour;

	//==================================
	//Bounding Box
	//==================================
	//our bounding box (8 vertices)
	//Vector bb_v1, bb_v2, bb_v3, bb_v4, bb_v5, bb_v6, bb_v7, bb_v8;
	//bounding box colour
	QColor bb_colour;

	//==================================
	//Brick Rendering (EXPERIMENTAL)
	//==================================
	int loaded;
	int brick;
	int BRICKS_SUBDIV;
	vector< Vector > brick_centresrot;
	vector< int > sorted_brick_indices;
	vector< bool > empty_brick;
	vector< Vector > brick_centres;
	vector< vector< unsigned char > > brick_textures;

	//==================================
	// 2D Texture rendering
	//==================================
	GLuint *texNames1, *texNames2, *texNames3;
	float maxX, maxY, maxZ, minX, minY, minZ;
	int fillSlices;
    GLfloat a1,a2,a3;

	//==================================
	// 3D Texture rendering
	//==================================
	int slices, sliceslod, max_slices;
    GLfloat offS, offT, offR; 	
	GLfloat splane[4], rplane[4], tplane[4];

	//==================================
	//Cropping
	//==================================
	bool crop_inv;
	int crop_shape_selected;
	int cropping_mode;
	vector<Vector> crop_triangles;
	vector<Vector> crop_shape2d;
	vector<Vector> crop_shape;
	vector<Vector> crop_normals;
	vector<Vector> crop_centroids;

	//==================================
	//Lighting
	//==================================
	int lighting_enable;
	Vector light_pos0;
	Vector light_dir0;
	Vector light_pos1;
	Vector light_dir1;
	float light_radius, light_azimuth, light_altitude;

	//==================================
	//Measuring Objects
	//==================================
	int measure_vertlabel;
	int measure_edgelabel;
	int measure_facelabel;
	int measure_arrowlabel;
	int measure_labelslabel;
	int measure_listslabel;
	int measure_tris;

	int measure_editmode;
	int measure_dragmode;
	int measure_modify;
	int measure_mode;
	
	int current_measure_drawmode;
	int current_measure_editmode;
	int current_measure_viewmode;

	int current_measure_index;
	int current_measure_vertindex;
	int current_measure_faceindex;
	int current_measure_edgeindex;
	int measure_magnetline_vert_old;
	int measure_magnetline_vert;
	int measure_magnetline_destvert;
	int measure_magnetline_helper;
	int measure_current_facemode;
	bool measure_facehover;
	bool measure_face_invert;
	bool measure_section_toggle;
	int measure_section_thickn;
	int measure_section_trans;
	int measure_section_face;
	bool measure_updateview_toggle;

	float measure_selection_window;
	float measure_arrowhead_size;
	float measure_active_line_size;
	float measure_active_point_size;
	float measure_line_size;
	float measure_point_size;
	Vector measure_active_face_col;
	Vector measure_active_edge_col;
	Vector measure_active_vert_col;
	Vector measure_face_col;
	Vector measure_edge_col;
	Vector measure_vert_col;
	Vector measure_selected_col;
	Vector measure_magnet_col;

	Vector current_measure_centre;
	MeasureListObject measures;
	MeasureObject currentMeasureObject;
	vector<int> current_face;
	
	//==================================
	//GUI Toggle States
	//==================================
	int is_greyscale;
	bool r_channel, g_channel, b_channel, isosurface;
	int	lights_toggle, volume_toggle, stereo_mode, stereo_toggle, volume_render_mode, isosurface_toggle, BB_toggle;
	int scalebar_toggle, measure_toggle, cursor_toggle, fps_toggle, axis_toggle, isoclip_toggle, FBOmoveLOD_toggle, SlicesmoveLOD_toggle, PostProcess_toggle;
	GLenum slice_draw_mode;

	//pointer to our remapdialogue (histogram updates)
	QtTransferFunctionEditorDialogue* editor;
	int updatehisto;

	Vector scalebarfontpos;
	QString scalebartext;

	//gl draw buffer mode
	QString	gldrawbuffer;

	//slice blending mode
	int bldmode;

	//string containing our screenspace cursor coord;
	QString cursor3D; 

	//3D Cursor position in screen space
	Vector cursor3D_screenspace;
	
	//3D Cursor position in texture space
	Vector cursor3D_texturespace;
	
	int x_slice, y_slice, z_slice;
	
	int offscreen_toggle;
	float stereo_eye_seperation;

	int timelapsefolder;
	int timelapsefoldersize;

	VolumeRender();
	~VolumeRender();

	//----------------------------------------------------
	//Main OpenGL functions
	//----------------------------------------------------
	void		initialize_DrawBufferGL();
	void		initializeLeft_DrawBufferGL();
	void		initializeRight_DrawBufferGL();

	void		initializeGL();
    void		displayGL(int render);
    void		resizeGL( int w, int h );

	void		initvariables(void);
	void		init_FBO(void);
	void		init_VBO(void);

	void		rebuild_texcoords(void);
	void		rebuildwithscaling_texcoords(void);

	//----------------------------------------------------
	//Texture Loading
	//----------------------------------------------------
	void		compute_boundingbox(void);
	void		compute_boundingboxDS(Vector min, Vector max);
	void		load_3dTexture_brick(int brickindex);
	void		load_TextureMap(void);
	void		load_2DTextureMaps(void);
	void		load_3DTextureMap(void);
	void		load_1DLookupTables(bool contrib_correct);
	void		load_1DLookupTablesBricks(int brickres, int brickslices);
	void		load_2DJitterTables(void);

	void		update_FBOsize(int size);
	void		select_channel(int channel);

	void		auto_zoom(void);
	//----------------------------------------------------
	//Volume Rendering
	//----------------------------------------------------
	//Brick rendering
	void		sort_bricks(void);
	void		init_3dTexture_bricks(int subdivs);
	void		render_3dTexture_bricks(int subdivs);
	//3D Texture Single Pass (RGB)
	void		render_3dTexture(void);
	//2D Texture Single Pass (RGB)
	void		render_2dTexture(void);
	void		render_2dTexture_stack1(GLfloat dir);
	void		render_2dTexture_stack2(GLfloat dir);
	void		render_2dTexture_stack3(GLfloat dir);
	//2D Multi-Texture MultiPass
	void		render_2dMultiTextureMultiPass(void);
	void		render_2dMultiTextureMultiPass_stack1(GLfloat dir);
	void		render_2dMultiTextureMultiPass_stack2(GLfloat dir);
	void		render_2dMultiTextureMultiPass_stack3(GLfloat dir);
	//3D Texture MultiPass
	void		render_3dTextureMultiPass(void);
	void		render_3dTextureMultiPass_cpu(void);

	//clipping plane slices
	void		render_3dTextureSlice1(void);
	void		render_3dTextureSlice(void);
	void		render_3dTextureSlicePlane_i(int i, GLenum surface, int mode, int text);
	void		render_3dTextureSlicePlane_i_bricks(int i, GLenum surface, int mode, int text);
	void		render_3dTextureSliceCurvedPlane_i(int i, GLenum surface,int mode, int text);

	void		render_3dTextureMeasurements(int mode, int tex);

	QImage		render_framebuffer(bool withAlpha);
	void		render_meshes(void);
	void		render_slices(GLenum draw_mode);
	void		render_boundingbox(void);
	void		render_scalebar(void);
	void		render_3DCursor(void);
	void        render_stereo(int render);
	void		render_scene(int render);
	void		render_setup_fbo(void);
	void		render_display_fbo(void);

	void		render_axis(void);
	void		render_light(Vector lightpos, Vector lightdir);
	void		render_measurements(void);
	void		render_measure_patch(int mode, int text);
	void		render_aabb(Vector cen, Vector dim);
	void		render_sphere(Vector c,float r,int n, GLenum drawmode);
	void		render_ellipsoid(Vector c,Vector r, Matrix4x4* rotmat, int n, GLenum drawmode);
	void		render_cropshape(void);

	void		render_VBO(void);

	void		render_VBO_start(void);
	void		render_VBO_end(void);
	void render_VBO_range(int start, int end);

	void		blend_mode(int blendmode);
	void		check_extensions(void);

	//----------------------------------------------------
	//Transfer Functions
	//----------------------------------------------------
	void		compute_transfer_function(int mode, TfuncState tfunc, int* myarray);
	void		apply_transfer_functions(int* rgb, int* ragaba);
	void		apply_transfer_function(int mode, TfuncState tfunc);
	void		build_histogram(void);
	void		build_volhistogram(void);
	void		update_transfer_functions(VolumeRenderView vview);

	//----------------------------------------------------
	//Camera
	//----------------------------------------------------
	void		setViewfromEuler(float angleX, float angleY, float angleZ);
	void		setViewfromEuler2(float angleX, float angleY, float angleZ);

	//----------------------------------------------------
	//Shaders
	//----------------------------------------------------
	void		aim_light(void);
	void		light_colours(Vector amb, Vector diff, Vector spec);
	void		silhouettes_shader(void);
	void		postprocess2d_shader(void);
	void		silhouette_shader(void);
	void		volshader_event(int id);
	void		volshader_update_eye(void);
	void		shaderGoochEvent(Vector warm, Vector cool, int contrib, int expon, int alpha, int degen);
	void		shaderDepthCueEvent(Vector col, float contrib);
};

#endif //VOLREN_H
