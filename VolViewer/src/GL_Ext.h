/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef GL_EXT_H
#define GL_EXT_H

#include <QtOpenGL>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <glut.h>
using namespace std;

//
// GL Extension loading wrapper class
//
// Supports: GLSLANG v1.0, FBO, VBO, Multitexturing
//

//windows
//----------
#ifdef WIN32
#include "glext.h"
#	define GETGLPROC(functype,funcname) ((funcname = (functype) wglGetProcAddress( #funcname )) == NULL)
//apple
//----------
#elif defined(__APPLE__)
#  include <OpenGL/glext.h>
#  define APIENTRY
#	define GETGLPROC(functype,funcname) ((funcname = (functype) NSGLGetProcAddress( (const GLubyte *)#funcname )) == NULL)
//#  include <OpenGL/glx.h>
//#  include <OpenGL/glxext.h>
#elif defined(__linux__)
//linux
//----------
#include <GL/glx.h>
#include <GL/glxext.h>
#	define GETGLPROC(functype,funcname) ((funcname = (functype) glXGetProcAddressARB( (const GLubyte *)#funcname )) == NULL)
#endif

#define print_OpenGL_Error() print_Ogl_Error(__FILE__, __LINE__)
int print_Ogl_Error(char *file, int line);
void print_glError(void);


//// GLSLANG Extension Definitions, From glext.h
typedef GLhandleARB (APIENTRY * PFNGLCREATEPROGRAMOBJECTARBPROC) ();
typedef GLhandleARB (APIENTRY * PFNGLCREATESHADEROBJECTARBPROC) (GLenum shaderType);
typedef void (APIENTRY * PFNGLDELETEOBJECTARBPROC) (GLhandleARB obj);
typedef void (APIENTRY * PFNGLDETACHOBJECTARBPROC) (GLhandleARB containerObj, GLhandleARB attachedObj);
typedef void (APIENTRY * PFNGLATTACHOBJECTARBPROC) (GLhandleARB containerObject, GLhandleARB obj);
typedef void (APIENTRY * PFNGLSHADERSOURCEARBPROC) (GLhandleARB shaderObj, GLsizei count, const GLcharARB **string, const GLint *length);
typedef void (APIENTRY * PFNGLCOMPILESHADERARBPROC) (GLhandleARB shaderObj);
typedef void (APIENTRY * PFNGLLINKPROGRAMARBPROC) (GLhandleARB programObj);
typedef void (APIENTRY * PFNGLGETINFOLOGARBPROC) (GLhandleARB obj,GLsizei maxLength, GLsizei *length, GLcharARB *infoLog);
typedef void (APIENTRY * PFNGLUSEPROGRAMOBJECTARBPROC) (GLhandleARB programObj);
typedef void (APIENTRY * PFNGLGETOBJECTPARAMETERIVARBPROC)(GLhandleARB obj, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETOBJECTPARAMETERFVARBPROC)(GLhandleARB obj, GLenum pname, GLfloat *params);

typedef int (APIENTRY * PFNGLGETUNIFORMLOCATIONARBPROC) (GLhandleARB programObj, const GLcharARB *name);
typedef int (APIENTRY * PFNGLGETATTRIBLOCATIONARBPROC) (GLhandleARB programObj, const GLcharARB *name);
						
typedef void (APIENTRY * PFNGLBINDATTRIBLOCATIONARBPROC) (GLhandleARB programObj, GLuint index, const GLcharARB *name);
typedef void (APIENTRY * PFNGLUNIFORM1FARBPROC) (GLint location, GLfloat v0);
typedef void (APIENTRY * PFNGLUNIFORM2FARBPROC) (GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRY * PFNGLUNIFORM3FARBPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRY * PFNGLUNIFORM4FARBPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRY * PFNGLUNIFORM1IARBPROC) (GLint location, GLint v0);
typedef void (APIENTRY * PFNGLUNIFORM2IARBPROC) (GLint location, GLint v0, GLint v1);
typedef void (APIENTRY * PFNGLUNIFORM3IARBPROC) (GLint location, GLint v0, GLint v1, GLint v2);
typedef void (APIENTRY * PFNGLUNIFORM4IARBPROC) (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4FVARBPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3FVARBPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2FVARBPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1FVARBPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4FARBPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3FARBPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2FARBPROC) (GLuint index, GLfloat x, GLfloat y);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1FARBPROC) (GLuint index, GLfloat x);
typedef void (APIENTRY * PFNGLVERTEXATTRIBPOINTERARBPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLENABLEVERTEXATTRIBARRAYARBPROC) (GLuint index);
typedef void (APIENTRY * PFNGLDISABLEVERTEXATTRIBARRAYARBPROC) (GLuint index);

#if defined(__APPLE__)
	typedef void (APIENTRY *PFNGLUNIFORMMATRIX2FVARBPROC)(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);
	typedef void (APIENTRY *PFNGLUNIFORMMATRIX3FVARBPROC)(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);
	typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FVARBPROC)(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);
	typedef void (APIENTRY * PFNGLUNIFORM1FVARBPROC) (GLint location, GLsizei count, GLfloat *value);
	typedef void (APIENTRY * PFNGLUNIFORM2FVARBPROC) (GLint location, GLsizei count, GLfloat *value);
	typedef void (APIENTRY * PFNGLUNIFORM3FVARBPROC) (GLint location, GLsizei count, GLfloat *value);
	typedef void (APIENTRY * PFNGLUNIFORM4FVARBPROC) (GLint location, GLsizei count, GLfloat *value);
	typedef void (APIENTRY * PFNGLUNIFORM1IVARBPROC) (GLint location, GLsizei count, GLint *value);
	typedef void (APIENTRY * PFNGLUNIFORM2IVARBPROC) (GLint location, GLsizei count, GLint *value);
	typedef void (APIENTRY * PFNGLUNIFORM3IVARBPROC) (GLint location, GLsizei count, GLint *value);
	typedef void (APIENTRY * PFNGLUNIFORM4IVARBPROC) (GLint location, GLsizei count, GLint *value);
#endif

//// FBO Extension Definitions, From glext.h
#define GL_FRAMEBUFFER_EXT                     0x8D40
#define GL_RENDERBUFFER_EXT                    0x8D41
#define GL_STENCIL_INDEX_EXT                   0x8D45
#define GL_STENCIL_INDEX1_EXT                  0x8D46
#define GL_STENCIL_INDEX4_EXT                  0x8D47
#define GL_STENCIL_INDEX8_EXT                  0x8D48
#define GL_STENCIL_INDEX16_EXT                 0x8D49
#define GL_RENDERBUFFER_WIDTH_EXT              0x8D42
#define GL_RENDERBUFFER_HEIGHT_EXT             0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT_EXT    0x8D44
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT            0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT            0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT          0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT  0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT     0x8CD4
#define GL_COLOR_ATTACHMENT0_EXT                0x8CE0
#define GL_COLOR_ATTACHMENT1_EXT                0x8CE1
#define GL_COLOR_ATTACHMENT2_EXT                0x8CE2
#define GL_COLOR_ATTACHMENT3_EXT                0x8CE3
#define GL_COLOR_ATTACHMENT4_EXT                0x8CE4
#define GL_COLOR_ATTACHMENT5_EXT                0x8CE5
#define GL_COLOR_ATTACHMENT6_EXT                0x8CE6
#define GL_COLOR_ATTACHMENT7_EXT                0x8CE7
#define GL_COLOR_ATTACHMENT8_EXT                0x8CE8
#define GL_COLOR_ATTACHMENT9_EXT                0x8CE9
#define GL_COLOR_ATTACHMENT10_EXT               0x8CEA
#define GL_COLOR_ATTACHMENT11_EXT               0x8CEB
#define GL_COLOR_ATTACHMENT12_EXT               0x8CEC
#define GL_COLOR_ATTACHMENT13_EXT               0x8CED
#define GL_COLOR_ATTACHMENT14_EXT               0x8CEE
#define GL_COLOR_ATTACHMENT15_EXT               0x8CEF
#define GL_DEPTH_ATTACHMENT_EXT                 0x8D00
#define GL_STENCIL_ATTACHMENT_EXT               0x8D20
#define GL_FRAMEBUFFER_COMPLETE_EXT                          0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT             0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT     0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT   0x8CD8
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT             0x8CD9
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT                0x8CDA
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT            0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT            0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT                       0x8CDD
#define GL_FRAMEBUFFER_STATUS_ERROR_EXT                      0x8CDE
#define GL_FRAMEBUFFER_BINDING_EXT             0x8CA6
#define GL_RENDERBUFFER_BINDING_EXT            0x8CA7
#define GL_MAX_COLOR_ATTACHMENTS_EXT           0x8CDF
#define GL_MAX_RENDERBUFFER_SIZE_EXT           0x84E8
#define GL_INVALID_FRAMEBUFFER_OPERATION_EXT   0x0506
typedef GLboolean (APIENTRY * PFNGLISRENDERBUFFEREXTPROC) (GLuint renderbuffer);
typedef void (APIENTRY * PFNGLBINDRENDERBUFFEREXTPROC) (GLenum target, GLuint renderbuffer);
typedef void (APIENTRY * PFNGLDELETERENDERBUFFERSEXTPROC) (GLsizei n, const GLuint *renderbuffers);
typedef void (APIENTRY * PFNGLGENRENDERBUFFERSEXTPROC) (GLsizei n, GLuint *renderbuffers);
typedef void (APIENTRY * PFNGLRENDERBUFFERSTORAGEEXTPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRY * PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC) (GLenum target, GLenum pname, GLint* params);
typedef GLboolean (APIENTRY * PFNGLISFRAMEBUFFEREXTPROC) (GLuint framebuffer);
typedef void (APIENTRY * PFNGLBINDFRAMEBUFFEREXTPROC) (GLenum target, GLuint framebuffer);
typedef void (APIENTRY * PFNGLDELETEFRAMEBUFFERSEXTPROC) (GLsizei n, const GLuint *framebuffers);
typedef void (APIENTRY * PFNGLGENFRAMEBUFFERSEXTPROC) (GLsizei n, GLuint *framebuffers);
typedef GLenum (APIENTRY * PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) (GLenum target);
typedef void (APIENTRY * PFNGLFRAMEBUFFERTEXTURE1DEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRY * PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRY * PFNGLFRAMEBUFFERTEXTURE3DEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
typedef void (APIENTRY * PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRY * PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) (GLenum target, GLenum attachment, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGENERATEMIPMAPEXTPROC) (GLenum target);

//// VBO Extension Definitions, From glext.h
#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_STATIC_DRAW_ARB 0x88E4
typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
//typedef void (APIENTRYP* PFNGLDRAWRANGEELEMENTSPROC) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);

#if defined (__APPLE__)
	typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, int size, const GLvoid *data, GLenum usage);
#endif

typedef void (APIENTRY * PFNGLBUFFERSUBDATAARBPROC) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid *data);
typedef void (APIENTRY * PFNGLGETBUFFERPARAMETERIVARBPROC) (GLenum target, GLenum pname, GLint *params);
typedef void *    (APIENTRY * PFNGLMAPBUFFERARBPROC) (GLenum target, GLenum access);
typedef GLboolean   (APIENTRY * PFNGLUNMAPBUFFERARBPROC) (GLenum target);

//// 3D Texture Extension Definitions, From glext.h
typedef void (APIENTRY * PFNGLTEXIMAGE3DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRY * PFNGLCOPYTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);

//// Multitexturing Extension Definitions, From glext.h
typedef void (APIENTRY * PFNGLACTIVETEXTUREPROC)				(GLenum texture );
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC)		(GLenum target);


// Blend Equation & Colour
typedef void (APIENTRY * PFNGLBLENDCOLOREXTPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

#if defined (__APPLE__)
	typedef void (APIENTRY * PFNGLBLENDEQUATIONEXTPROC) (int mode);
#endif

//#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
//#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

//Matrix Transpose
typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXFPROC) (const GLfloat *m);
typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXFPROC) (const GLfloat *m);

class GL_Ext
{
	public:


		// OpenGL Shading Language entry points

		int shader_support;
		//Shading Language Objects
		GLhandleARB ProgramObject;
		GLhandleARB VertexShaderObject;
		GLhandleARB FragmentShaderObject;
		PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
		PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
		PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
		PFNGLDETACHOBJECTARBPROC glDetachObjectARB;
		PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
		PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
		PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
		PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
		PFNGLGETINFOLOGARBPROC glGetInfoLogARB;
		PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;
		PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
		PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfvARB;
		PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
		PFNGLGETATTRIBLOCATIONARBPROC glGetAttribLocationARB;
		PFNGLBINDATTRIBLOCATIONARBPROC glBindAttribLocationARB;
		PFNGLUNIFORM1FARBPROC glUniform1fARB;
		PFNGLUNIFORM2FARBPROC glUniform2fARB;
		PFNGLUNIFORM3FARBPROC glUniform3fARB;
		PFNGLUNIFORM4FARBPROC glUniform4fARB;
		PFNGLUNIFORM1IARBPROC glUniform1iARB;
		PFNGLUNIFORM2IARBPROC glUniform2iARB;
		PFNGLUNIFORM3IARBPROC glUniform3iARB;
		PFNGLUNIFORM4IARBPROC glUniform4iARB;
		PFNGLUNIFORM1FVARBPROC glUniform1fvARB;
		PFNGLUNIFORM2FVARBPROC glUniform2fvARB;
		PFNGLUNIFORM3FVARBPROC glUniform3fvARB;
		PFNGLUNIFORM4FVARBPROC glUniform4fvARB;
		PFNGLUNIFORM1IVARBPROC glUniform1ivARB;
		PFNGLUNIFORM2IVARBPROC glUniform2ivARB;
		PFNGLUNIFORM3IVARBPROC glUniform3ivARB;
		PFNGLUNIFORM4IVARBPROC glUniform4ivARB;
		PFNGLUNIFORMMATRIX2FVARBPROC glUniformMatrix2fvARB;
		PFNGLUNIFORMMATRIX3FVARBPROC glUniformMatrix3fvARB;
		PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB;
		PFNGLVERTEXATTRIB4FVARBPROC glVertexAttrib4fvARB;
		PFNGLVERTEXATTRIB3FVARBPROC glVertexAttrib3fvARB;
		PFNGLVERTEXATTRIB2FVARBPROC glVertexAttrib2fvARB;
		PFNGLVERTEXATTRIB1FVARBPROC glVertexAttrib1fvARB;
		PFNGLVERTEXATTRIB4FARBPROC glVertexAttrib4fARB;
		PFNGLVERTEXATTRIB3FARBPROC glVertexAttrib3fARB;
		PFNGLVERTEXATTRIB2FARBPROC glVertexAttrib2fARB;
		PFNGLVERTEXATTRIB1FARBPROC glVertexAttrib1fARB;
		PFNGLVERTEXATTRIBPOINTERARBPROC glVertexAttribPointerARB;
		PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB;
		PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB;
		
		//FBO objects
		int fbo_support;
		PFNGLISRENDERBUFFEREXTPROC glIsRenderbufferEXT;
		PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
		PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT;
		PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
		PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
		PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT;
		PFNGLISFRAMEBUFFEREXTPROC glIsFramebufferEXT;
		PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
		PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;
		PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
		PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;
		PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT;
		PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
		PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT;
		PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
		PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT;
		PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT;

		// VBO Extension Function Pointers
		int vbo_support;
		int pbo_support;
		PFNGLGENBUFFERSARBPROC glGenBuffersARB;					// VBO Name Generation Procedure
		PFNGLBINDBUFFERARBPROC glBindBufferARB;					// VBO Bind Procedure
		PFNGLBUFFERDATAARBPROC glBufferDataARB;					// VBO Data Loading Procedure
		PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;          // VBO Sub Data Loading Procedure
		PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;			// VBO Deletion Procedure
		PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;	// return various parameters of VBO
		PFNGLMAPBUFFERARBPROC glMapBufferARB;                       // map VBO procedure
		PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;                   // unmap VBO procedure
		PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElementsEXT;

		// 3D Texture function pointer
		int texture3d_support;
		PFNGLTEXIMAGE3DPROC glTexImage3D;
		//PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;

		// Mulitexturing
		int multitexture_support;
		PFNGLACTIVETEXTUREPROC glActiveTexture;
		PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;

		// Blend Equation and Colour Pointers;
		int blend_support;
		PFNGLBLENDCOLOREXTPROC					glBlendColorEXT;
		PFNGLBLENDEQUATIONEXTPROC				glBlendEquationEXT;
		PFNGLBLENDFUNCSEPARATEEXTPROC			glBlendFuncSeparateEXT;

		//Matrix Transpose
		PFNGLLOADTRANSPOSEMATRIXFPROC			glLoadTransposeMatrixf;
		PFNGLMULTTRANSPOSEMATRIXFPROC			glMultTransposeMatrixf;

		//Member functions
		~GL_Ext();
		GL_Ext();
		void load_GLextension(void);
		int check_GLextension(char* extensionName);
};

#endif //GL_EXT_H
