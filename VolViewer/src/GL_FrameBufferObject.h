/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef FRAME_BUFFER_OBJECT_H
#define FRAME_BUFFER_OBJECT_H
//#include <GL\glut.h>
#include <QtOpenGL>
#include "glext.h"

#define FBO_DEPTH_16                0x00000001
#define FBO_DEPTH_24                0x00000002
#define FBO_DEPTH_32                0x00000004
#define FBO_STENCIL                 0x00000010

#include "GL_Ext.h"

class GL_FrameBufferObject
{
  public:
	  GLuint width,
           height,
           frameBufferIndex,
           depthBufferIndex,
           stencilBufferIndex;


	 GL_Ext* glext;
     GL_FrameBufferObject();
    ~GL_FrameBufferObject();

     bool initialize(int width, int height, int format = 0);
     bool initialize(int width, int height, GLenum depthformat, GLuint colid, GLuint depthid);
     bool  checkFrameBufferStatus();

     void attachTexture( GLenum attachment, GLenum texType, GLuint texId);
     void switchTarget(int textureID);
     void bind();
	 void bind(GLuint index);
     void stop();

     GLuint getHeight();
     GLuint getWidth();
};

#endif