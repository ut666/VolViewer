#ifndef GL_SHADEROBJ_H
#define GL_SHADEROBJ_H

//#include <GL\glut.h>
#include <QtOpenGL>
#include "glext.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "GL_Ext.h"

typedef enum 
{
    EVertexShader,
    EFragmentShader,
} EShaderType;

class GL_ShaderOBJ
{
	public:

		GL_ShaderOBJ();
		~GL_ShaderOBJ();

		GL_Ext* glext;
		GLhandleARB ProgramObject;
		GLhandleARB VertexShaderObject;
		GLhandleARB FragmentShaderObject;

		void enable(void);
		void disable(void);

		void shader_info(GLhandleARB object);
		int shader_size(const char *fileName, EShaderType shaderType);
		int read_shader(const char *fileName, EShaderType shaderType, char *shaderText, int size);
		int read_shaderSRC(const char *fileName, GLcharARB **vertexShader, GLcharARB **fragmentShader);
		int install_shader(const GLcharARB *vertexShaderSRC, const GLcharARB *fragmentShaderSRC);
		GLint GetUniLoc(const GLcharARB *name);      // get location of a variable
		GLint GetAttribLoc(const GLcharARB *name);
		void BindAttrib(int loc, const GLcharARB *name);

		void sendUniform1f(char* varname, GLfloat v0);
		void sendUniform2f(char* varname, GLfloat v0, GLfloat v1);
		void sendUniform3f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2);
		void sendUniform4f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

		void sendUniform1fv(char* varname, GLsizei count, GLfloat* value);
		void sendUniform2fv(char* varname, GLsizei count, GLfloat* value);
		void sendUniform3fv(char* varname, GLsizei count, GLfloat* value);
		void sendUniform4fv(char* varname, GLsizei count, GLfloat* value);

		void sendUniform1i(char* varname, GLint v0);
		void sendUniform2i(char* varname, GLint v0, GLint v1);
		void sendUniform3i(char* varname, GLint v0, GLint v1, GLint v2);
		void sendUniform4i(char* varname, GLint v0, GLint v1, GLint v2, GLint v3);

		void sendUniform1iv(char* varname, GLsizei count, GLint* value);
		void sendUniform2iv(char* varname, GLsizei count, GLint* value);
		void sendUniform3iv(char* varname, GLsizei count, GLint* value);
		void sendUniform4iv(char* varname, GLsizei count, GLint* value);

		void sendUniformMatrix2fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value);
		void sendUniformMatrix3fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value);
		void sendUniformMatrix4fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value);
		
};

#endif //GL_SHADEROBJ_H