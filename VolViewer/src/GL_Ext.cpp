/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "GL_Ext.h"

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include <stdlib.h>
#include <string.h>

static void *NSGLGetProcAddress (const GLubyte *name)
{
	NSSymbol symbol;
	char *symbolName;
	/* prepend a '_' for the Unix C symbol mangling convention */
	symbolName = (char*)malloc(strlen((const char *)name) + 2);
	strcpy(symbolName+1, (const char *)name);
	symbolName[0] = '_';
	symbol = NULL;
	if (NSIsSymbolNameDefined(symbolName))
		symbol = NSLookupAndBindSymbol(symbolName);
	free(symbolName);
	return symbol ? NSAddressOfSymbol(symbol) : NULL;
}
#endif /* __APPLE__ */

int print_Ogl_Error(char *file, int line)
{
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
      //  cout<<"GLERROR: in file "<<file<<" @ line: "<<line<<" "<<gluErrorString(glErr)<<endl;
        
		retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}
void print_glError(void)
{
    GLenum glErr;
    int    retCode = 0;
    char* error_msg;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
		    retCode = 1;
        glErr = glGetError();
		    error_msg = (char*) gluErrorString(glErr);
    }

	printf("GL_ERROR: %s\n", error_msg);
}

GL_Ext::GL_Ext()
{
	blend_support = multitexture_support = texture3d_support = vbo_support = fbo_support = shader_support=0;

}

GL_Ext::~GL_Ext()
{

}

int GL_Ext::check_GLextension(char* extensionName)
{
  // get the list of supported extensions
  char* extensionList = (char*) glGetString(GL_EXTENSIONS);

  if (!extensionName || !extensionList)
  {
	cout<<"ERROR: No extensionName or extensionList... [check_GLextension(), GL_Ext_Loader.cpp]"<<endl;
    return 0;
  }

  while (*extensionList)
  {
    // find the length of the first extension substring
    unsigned int firstExtensionLength = strcspn(extensionList, " ");


    if (strlen(extensionName) == firstExtensionLength &&
      strncmp(extensionName, extensionList, firstExtensionLength) == 0)
    {
	  cout<<"OPENGL: Using: "<<extensionName<<endl;
      return 1;
    }

    // move to the next substring
    extensionList += firstExtensionLength + 1;
  }

  cout<<"ERROR: Extension not supported: "<<extensionName<<endl;
  return 0;
}

void GL_Ext::load_GLextension(void)
{
	int error=0;

	int max_clips=0;
	glGetIntegerv(GL_MAX_CLIP_PLANES, &max_clips);
	printf("OPENGL: Max clip planes: %d\n", max_clips);

	if(!check_GLextension("GL_ARB_vertex_buffer_object") || !check_GLextension("GL_ARB_pixel_buffer_object"))
	{
        //cout<<"ERROR: GL_ARB_vertex_buffer_object extension not available... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
	}
	else
	{
		vbo_support = 1;
		pbo_support = 1;

		error |= GETGLPROC(PFNGLGENBUFFERSARBPROC, glGenBuffersARB);
		error |= GETGLPROC(PFNGLBINDBUFFERARBPROC, glBindBufferARB);
		error |= GETGLPROC(PFNGLBUFFERDATAARBPROC, glBufferDataARB);
		error |= GETGLPROC(PFNGLDELETEBUFFERSARBPROC, glDeleteBuffersARB);
		error |= GETGLPROC(PFNGLBUFFERSUBDATAARBPROC, glBufferSubDataARB);
		error |= GETGLPROC(PFNGLGETBUFFERPARAMETERIVARBPROC, glGetBufferParameterivARB);
		error |= GETGLPROC(PFNGLMAPBUFFERARBPROC, glMapBufferARB);
		error |= GETGLPROC(PFNGLUNMAPBUFFERARBPROC, glUnmapBufferARB);
		error |= GETGLPROC(PFNGLDRAWRANGEELEMENTSPROC, glDrawRangeElementsEXT);

		if( error )
		{
			cout<<"ERROR: from GETGLPROC... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
		}
	}

	if(!check_GLextension("GL_EXT_framebuffer_object"))
	{
        //cout<<"ERROR: GL_EXT_framebuffer_object extension not available... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
	}
	else
	{
		fbo_support = 1;

		error |= GETGLPROC(PFNGLISRENDERBUFFEREXTPROC, glIsRenderbufferEXT);
		error |= GETGLPROC(PFNGLBINDRENDERBUFFEREXTPROC, glBindRenderbufferEXT);
		error |= GETGLPROC(PFNGLDELETERENDERBUFFERSEXTPROC, glDeleteRenderbuffersEXT);
		error |= GETGLPROC(PFNGLGENRENDERBUFFERSEXTPROC, glGenRenderbuffersEXT);
		error |= GETGLPROC(PFNGLRENDERBUFFERSTORAGEEXTPROC, glRenderbufferStorageEXT);
		error |= GETGLPROC(PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC, glGetRenderbufferParameterivEXT);
		error |= GETGLPROC(PFNGLISFRAMEBUFFEREXTPROC, glIsFramebufferEXT);
		error |= GETGLPROC(PFNGLBINDFRAMEBUFFEREXTPROC, glBindFramebufferEXT);
		error |= GETGLPROC(PFNGLDELETEFRAMEBUFFERSEXTPROC, glDeleteFramebuffersEXT);
		error |= GETGLPROC(PFNGLGENFRAMEBUFFERSEXTPROC, glGenFramebuffersEXT);
		error |= GETGLPROC(PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC, glCheckFramebufferStatusEXT);
		error |= GETGLPROC(PFNGLFRAMEBUFFERTEXTURE1DEXTPROC, glFramebufferTexture1DEXT);
		error |= GETGLPROC(PFNGLFRAMEBUFFERTEXTURE2DEXTPROC, glFramebufferTexture2DEXT);
		error |= GETGLPROC(PFNGLFRAMEBUFFERTEXTURE3DEXTPROC, glFramebufferTexture3DEXT);
		error |= GETGLPROC(PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC, glFramebufferRenderbufferEXT);
		error |= GETGLPROC(PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC, glGetFramebufferAttachmentParameterivEXT);
		error |= GETGLPROC(PFNGLGENERATEMIPMAPEXTPROC, glGenerateMipmapEXT);

		if( error )
		{
			cout<<"ERROR: from GETGLPROC... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
		}
	}
    

    if ( !check_GLextension( "GL_ARB_shader_objects" )   ||
         !check_GLextension( "GL_ARB_fragment_shader" )  ||
         !check_GLextension( "GL_ARB_vertex_shader" )    ||
         !check_GLextension( "GL_ARB_shading_language_100" ) )
    {
        //cout<<"WARNING: OpenGL Shading Language extensions not available... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
		//return;
    }
	else 
	{
		shader_support=1;

		error |= GETGLPROC(PFNGLCREATEPROGRAMOBJECTARBPROC, glCreateProgramObjectARB);
		error |= GETGLPROC(PFNGLCREATESHADEROBJECTARBPROC, glCreateShaderObjectARB);
		error |= GETGLPROC(PFNGLDELETEOBJECTARBPROC, glDeleteObjectARB);
		error |= GETGLPROC(PFNGLDETACHOBJECTARBPROC, glDetachObjectARB);
		error |= GETGLPROC(PFNGLATTACHOBJECTARBPROC, glAttachObjectARB);
		error |= GETGLPROC(PFNGLSHADERSOURCEARBPROC, glShaderSourceARB);
		error |= GETGLPROC(PFNGLCOMPILESHADERARBPROC, glCompileShaderARB);
		error |= GETGLPROC(PFNGLLINKPROGRAMARBPROC, glLinkProgramARB);
		error |= GETGLPROC(PFNGLGETINFOLOGARBPROC, glGetInfoLogARB);
		error |= GETGLPROC(PFNGLUSEPROGRAMOBJECTARBPROC, glUseProgramObjectARB);
		error |= GETGLPROC(PFNGLGETOBJECTPARAMETERIVARBPROC, glGetObjectParameterivARB);
		error |= GETGLPROC(PFNGLGETOBJECTPARAMETERFVARBPROC, glGetObjectParameterfvARB);
		error |= GETGLPROC(PFNGLGETUNIFORMLOCATIONARBPROC, glGetUniformLocationARB);
		error |= GETGLPROC(PFNGLBINDATTRIBLOCATIONARBPROC, glBindAttribLocationARB);
		error |= GETGLPROC(PFNGLUNIFORM1FARBPROC, glUniform1fARB);
		error |= GETGLPROC(PFNGLUNIFORM2FARBPROC, glUniform2fARB);
		error |= GETGLPROC(PFNGLUNIFORM3FARBPROC, glUniform3fARB);
		error |= GETGLPROC(PFNGLUNIFORM4FARBPROC, glUniform4fARB);
		error |= GETGLPROC(PFNGLUNIFORM1IARBPROC, glUniform1iARB);
		error |= GETGLPROC(PFNGLUNIFORM2IARBPROC, glUniform2iARB);
		error |= GETGLPROC(PFNGLUNIFORM3IARBPROC, glUniform3iARB);
		error |= GETGLPROC(PFNGLUNIFORM4IARBPROC, glUniform4iARB);
		error |= GETGLPROC(PFNGLUNIFORM1FVARBPROC, glUniform1fvARB);
		error |= GETGLPROC(PFNGLUNIFORM2FVARBPROC, glUniform2fvARB);
		error |= GETGLPROC(PFNGLUNIFORM3FVARBPROC, glUniform3fvARB);
		error |= GETGLPROC(PFNGLUNIFORM4FVARBPROC, glUniform4fvARB);
		error |= GETGLPROC(PFNGLUNIFORM1IVARBPROC, glUniform1ivARB);
		error |= GETGLPROC(PFNGLUNIFORM2IVARBPROC, glUniform2ivARB);
		error |= GETGLPROC(PFNGLUNIFORM3IVARBPROC, glUniform3ivARB);
		error |= GETGLPROC(PFNGLUNIFORM4IVARBPROC, glUniform4ivARB);
		error |= GETGLPROC(PFNGLVERTEXATTRIB4FVARBPROC, glVertexAttrib4fvARB);
		error |= GETGLPROC(PFNGLVERTEXATTRIB3FVARBPROC, glVertexAttrib3fvARB);
		error |= GETGLPROC(PFNGLVERTEXATTRIB2FVARBPROC, glVertexAttrib2fvARB);
		error |= GETGLPROC(PFNGLVERTEXATTRIB1FVARBPROC, glVertexAttrib1fvARB);
		error |= GETGLPROC(PFNGLVERTEXATTRIB4FARBPROC, glVertexAttrib4fARB);
		error |= GETGLPROC(PFNGLVERTEXATTRIB3FARBPROC, glVertexAttrib3fARB);
		error |= GETGLPROC(PFNGLVERTEXATTRIB2FARBPROC, glVertexAttrib2fARB);
		error |= GETGLPROC(PFNGLVERTEXATTRIB1FARBPROC, glVertexAttrib1fARB);

		error |= GETGLPROC(PFNGLVERTEXATTRIBPOINTERARBPROC, glVertexAttribPointerARB);
		error |= GETGLPROC(PFNGLENABLEVERTEXATTRIBARRAYARBPROC, glEnableVertexAttribArrayARB);
		error |= GETGLPROC(PFNGLDISABLEVERTEXATTRIBARRAYARBPROC, glDisableVertexAttribArrayARB);

		if( error )
		{
			cout<<"ERROR: from GETGLPROC... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
		}
	}


	if(!check_GLextension("GL_ARB_multitexture"))
	{
        //cout<<"ERROR: GL_ARB_MULTITEXTURE extension not available... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
	}
	else
	{
		multitexture_support = 1;
		int texture_units;

		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &texture_units);
	    error |= GETGLPROC(PFNGLACTIVETEXTUREPROC, glActiveTexture);
	    error |= GETGLPROC(PFNGLCLIENTACTIVETEXTUREARBPROC, glClientActiveTextureARB);

		cout<<"OPENGL: Support for "<<texture_units<<" Texture Mapping Units"<<endl;
		
		if( error )
		{
			cout<<"ERROR: from GETGLPROC... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
		}
	}

//	if(!check_GLextension("GL_EXT_texture3D"))
	{
        //cout<<"ERROR: GL_ARB_MULTITEXTURE extension not available... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
	}
//	else
	{

		int max_size=0;

		//glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE_EXT, &max_size);
		//printf("OPENGL: Max Texture Size: %d\n", max_size);

		texture3d_support = 1;

   		error |= GETGLPROC(PFNGLTEXIMAGE3DPROC, glTexImage3D);
		if( error )
		{
			cout<<"ERROR: from GETGLPROC... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
		}
	}


/*	if(!check_GLextension("GL_ARB_texture_non_power_of_two"))
	{
        //cout<<"ERROR: GL_ARB_MULTITEXTURE extension not available... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
	}
	else
	{
		texture3d_support = 1;

		if( error )
		{
			cout<<"ERROR: from GETGLPROC... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
		}
	}
*/


	if(!check_GLextension("GL_EXT_blend_minmax"))
	{
        //cout<<"ERROR: glBlendEquationEXT extension not available... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
	}
	else
	{
		blend_support = 1;	
   		error |= GETGLPROC(PFNGLBLENDEQUATIONEXTPROC, glBlendEquationEXT);
		if( error )
		{
			cout<<"ERROR: from GETGLPROC... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
		}
	}

	if(!check_GLextension("glBlendColor"))
	{
        //cout<<"ERROR: glBlendEquationEXT extension not available... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
		
		if(!check_GLextension("GL_EXT_blend_color"))
		{
			//cout<<"ERROR: glBlendEquationEXT extension not available... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
		}
		else
		{
   			error |= GETGLPROC(PFNGLBLENDCOLOREXTPROC, glBlendColorEXT);

			if( error )
			{
				cout<<"ERROR: from GETGLPROC... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
			}
		}
	}
	else
	{
   		/*error |= GETGLPROC(PFNGLBLENDCOLORPROC, glBlendColorEXT);

		if( error )
		{
			cout<<"ERROR: from GETGLPROC... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
		}*/
	}
	if(!check_GLextension("GL_EXT_blend_equation_separate"))
	{
	}
	else
	{
		error |= GETGLPROC(PFNGLBLENDFUNCSEPARATEEXTPROC, glBlendFuncSeparateEXT);

		if( error )
		{
			cout<<"ERROR: from GETGLPROC... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
		}
	}

	if(!check_GLextension("GL_ARB_transpose_matrix"))
	{
	}
	else
	{
		error |= GETGLPROC(PFNGLLOADTRANSPOSEMATRIXFPROC, glLoadTransposeMatrixf);
		error |= GETGLPROC(PFNGLMULTTRANSPOSEMATRIXFPROC, glMultTransposeMatrixf);

		if( error )
		{
			cout<<"ERROR: from GETGLPROC... [load_GLextension(), GL_Ext_Loader.h]"<<endl;
		}
	}					
}
