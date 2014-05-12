/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "GL_ShaderObj.h"

GL_ShaderOBJ::GL_ShaderOBJ()
{
}
GL_ShaderOBJ::~GL_ShaderOBJ()
{
//	glext->glDeleteObjectARB(ProgramObject);
//	print_OpenGL_Error();  // Check for OpenGL errors
}


void GL_ShaderOBJ::shader_info(GLhandleARB obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    GLcharARB *infoLog;

    print_OpenGL_Error();  // Check for OpenGL errors

    glext->glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,
                                         &infologLength);
    print_OpenGL_Error();  // Check for OpenGL errors

    if (infologLength > 0)
    {
        infoLog = (GLcharARB*)malloc(infologLength);
        if (infoLog == NULL)
        {
            cout<<"ERROR: Could not allocate InfoLog buffer... [shader_info(), GL_ShaderOBJ.cpp]"<<endl;
//            exit(1);
        }
        glext->glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
       // cout<<"SHADERS: Log: "<<infoLog<<endl;
        free(infoLog);
    }
    print_OpenGL_Error();  // Check for OpenGL errors
}

int GL_ShaderOBJ::shader_size(const char *fileName, EShaderType shaderType)
{
    //
    // Returns the size in bytes of the shader fileName.
    // If an error occurred, it returns -1.
    //
    // File name convention:
    //
    // <fileName>.vert
    // <fileName>.frag
    //

    FILE* shader;
    string name;
    int count;

	name = fileName;

    switch (shaderType)
    {
        case EVertexShader:
            name += ".vert";
            break;
        case EFragmentShader:
            name += ".frag";
            break;
        default:
            break;
    }

    //
    // Open the file
    //
    shader = fopen(name.data(), "r");
    if (!shader)
	{
		cout<<"ERROR: Shader file not found..."<<name<<" [shaderSize(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
		return -1;
	}
    //
    // Seek to the end and find its position
    //
    fseek(shader, 0, SEEK_END);
	count = ftell(shader)+1;

    fclose(shader);
    return count;
}

int GL_ShaderOBJ::read_shader(const char *fileName, EShaderType shaderType, char *shaderText, int size)
{
    //
    // Reads a shader from the supplied file and returns the shader in the
    // arrays passed in. Returns 1 if successful, 0 if an error occurred.
    // The parameter size is an upper limit of the amount of bytes to read.
    // It is ok for it to be too big.
    //
    FILE *shader;
    string name;
	//char name[512];
    int count;

	name = fileName;
//    strcpy(name, fileName);

    switch (shaderType) 
    {
        case EVertexShader:
            name += ".vert";
			//strcat(name, ".vert");
            break;
        case EFragmentShader:
            //strcat(name, ".frag");
            name += ".frag";
            break;
        default:
            //_ASSERT(0);
            break;
    }

    //
    // Open the file
    //
	shader = fopen(name.data(), "r");
    if (!shader)
		return -1;

    //
    // Get the shader from a file.
    //
    fseek(shader, 0, SEEK_SET);
    count = fread(shaderText, 1, size, shader);
    shaderText[count] = '\0';

    if (ferror(shader))
        count = 0;
    else count = 1;

    fclose(shader);
    return count;
}

int GL_ShaderOBJ::read_shaderSRC(const char *fileName, GLcharARB **vertexShader, GLcharARB **fragmentShader)
{
    int vSize, fSize;

    //
    // Allocate memory to hold the source of our shaders.
    //
    vSize = shader_size(fileName, EVertexShader);
    fSize = shader_size(fileName, EFragmentShader);

    if ((vSize == -1) || (fSize == -1))
    {
 		cout<<"ERROR: Cannot determine size of the shader... "<<fileName<<" [readShaderSource(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
        return 0;
    }

    *vertexShader = (GLcharARB *) malloc(vSize);
    *fragmentShader = (GLcharARB *) malloc(fSize);

    //
    // Read the source code
    //
    if (!read_shader(fileName, EVertexShader, *vertexShader, vSize))
    {
  		cout<<"ERROR: Cannot read the file... "<<fileName<<".vert [readShaderSource(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
       return 0;
    }

    if (!read_shader(fileName, EFragmentShader, *fragmentShader, fSize))
	{
  		cout<<"ERROR: Cannot read the file... "<<fileName<<".frag [readShaderSource(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
        return 0;
    }

  //	cout<<"SHADERS: Loaded successfully... "<<fileName<<".vert"<<endl;
  //	cout<<"SHADERS: Loaded successfully... "<<fileName<<".frag"<<endl;
    return 1;
}
void GL_ShaderOBJ::enable(void)
{
    glext->glUseProgramObjectARB(ProgramObject);
}
void GL_ShaderOBJ::disable(void)
{
    glext->glUseProgramObjectARB(0);
}
void GL_ShaderOBJ::BindAttrib(int loc, const GLcharARB *name)
{
	glext->glBindAttribLocationARB(ProgramObject, loc, name);

    print_OpenGL_Error();
}
GLint GL_ShaderOBJ::GetAttribLoc(const GLcharARB *name)
{
	GLint loc;

	loc = glext->glGetAttribLocationARB(ProgramObject, name);
	if (loc == -1) 
	{
        cout << "Error: can't find attribute variable \"" << name << "\"\n";
	}
    print_OpenGL_Error();
	return loc;
}
GLint GL_ShaderOBJ::GetUniLoc(const GLcharARB *name)
{
	GLint loc;

	loc = glext->glGetUniformLocationARB(ProgramObject, name);
	if (loc == -1) 
	{
        cout << "Error: can't find uniform variable \"" << name << "\"\n";
	}
    print_OpenGL_Error();
	return loc;
}

int GL_ShaderOBJ::install_shader(const GLcharARB *vertexShaderSRC, const GLcharARB *fragmentShaderSRC)
{
    GLint       vertCompiled, fragCompiled;    // status values
    GLint       linked;

    // Create a vertex shader object and a fragment shader object
    VertexShaderObject = glext->glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    FragmentShaderObject = glext->glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    // Load source code strings into shaders

    glext->glShaderSourceARB(VertexShaderObject, 1, &vertexShaderSRC, NULL);
    glext->glShaderSourceARB(FragmentShaderObject, 1, &fragmentShaderSRC, NULL);

    // Compile the particle vertex shader, and print out
    // the compiler log file.

    glext->glCompileShaderARB(VertexShaderObject);
    print_OpenGL_Error();  // Check for OpenGL errors
    glext->glGetObjectParameterivARB(VertexShaderObject, GL_OBJECT_COMPILE_STATUS_ARB, &vertCompiled);
    shader_info(VertexShaderObject);

    // Compile the particle vertex shader, and print out
    // the compiler log file.

    glext->glCompileShaderARB(FragmentShaderObject);
    print_OpenGL_Error();  // Check for OpenGL errors
    glext->glGetObjectParameterivARB(FragmentShaderObject, GL_OBJECT_COMPILE_STATUS_ARB, &fragCompiled);
    shader_info(FragmentShaderObject);

    if (!vertCompiled || !fragCompiled)
        return 0;

    // Create a program object and attach the two compiled shaders
    ProgramObject = glext->glCreateProgramObjectARB();
    glext->glAttachObjectARB(ProgramObject, VertexShaderObject);
    glext->glAttachObjectARB(ProgramObject, FragmentShaderObject);

    // Bind generic attribute indices to attribute variable names
    //glBindAttribLocationARB(ProgramObject, VELOCITY_ARRAY, "Velocity");
    //glBindAttribLocationARB(ProgramObject, START_TIME_ARRAY, "StartTime");

    // Link the program object and print out the info log

    glext->glLinkProgramARB(ProgramObject);
    print_OpenGL_Error();  // Check for OpenGL errors
    glext->glGetObjectParameterivARB(ProgramObject, GL_OBJECT_LINK_STATUS_ARB, &linked);
    shader_info(ProgramObject);

    if (!linked) return 0;

    return 1;
}
void GL_ShaderOBJ::sendUniform1f(char* varname, GLfloat v0)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform1f(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform1fARB(loc, v0);
}

void GL_ShaderOBJ::sendUniform2f(char* varname, GLfloat v0, GLfloat v1)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform2f(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform2fARB(loc, v0, v1);
}

void GL_ShaderOBJ::sendUniform3f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform3f(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform3fARB(loc, v0, v1, v2);
}
void GL_ShaderOBJ::sendUniform4f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform4f(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform4fARB(loc, v0, v1, v2, v3);
}
void GL_ShaderOBJ::sendUniform1i(char* varname, GLint v0)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform1i(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform1iARB(loc, v0);
}

void GL_ShaderOBJ::sendUniform2i(char* varname, GLint v0, GLint v1)
{ 
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform2i(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform2iARB(loc, v0, v1);
}

void GL_ShaderOBJ::sendUniform3i(char* varname, GLint v0, GLint v1, GLint v2)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform3i(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform3iARB(loc, v0, v1, v2);
}
void GL_ShaderOBJ::sendUniform4i(char* varname, GLint v0, GLint v1, GLint v2, GLint v3)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform4i(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform4iARB(loc, v0, v1, v2, v3);
}

void GL_ShaderOBJ::sendUniform1iv(char* varname, GLsizei count, GLint *value)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform1iv(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform1ivARB(loc, count, value);
}
void GL_ShaderOBJ::sendUniform2iv(char* varname, GLsizei count, GLint *value)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform2iv(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform2ivARB(loc, count, value);
}
void GL_ShaderOBJ::sendUniform3iv(char* varname, GLsizei count, GLint *value)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform3iv(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform3ivARB(loc, count, value);
}
void GL_ShaderOBJ::sendUniform4iv(char* varname, GLsizei count, GLint *value)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform4iv(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform4ivARB(loc, count, value);
}

void GL_ShaderOBJ::sendUniform1fv(char* varname, GLsizei count, GLfloat *value)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform1fv(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform1fvARB(loc, count, value);
}
void GL_ShaderOBJ::sendUniform2fv(char* varname, GLsizei count, GLfloat *value)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform2fv(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform2fvARB(loc, count, value);
}
void GL_ShaderOBJ::sendUniform3fv(char* varname, GLsizei count, GLfloat *value)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform3fv(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform3fvARB(loc, count, value);
}
void GL_ShaderOBJ::sendUniform4fv(char* varname, GLsizei count, GLfloat *value)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniform4fv(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniform4fvARB(loc, count, value);
}

void GL_ShaderOBJ::sendUniformMatrix2fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniformMatrix2fv(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniformMatrix2fvARB(loc, count, transpose, value);
}

void GL_ShaderOBJ::sendUniformMatrix3fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniformMatrix3fv(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniformMatrix3fvARB(loc, count, transpose, value);
}

void GL_ShaderOBJ::sendUniformMatrix4fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
    GLint loc = GetUniLoc(varname);
    if (loc==-1)
	{
  		cout<<"ERROR: Cannot find variable... "<<varname<<"[sendUniformMatrix4fv(), GL_ShaderOBJ.cpp]"<<endl;
		system("pause");
//		exit(1);
	}
    
    glext->glUniformMatrix4fvARB(loc, count, transpose, value);
}
