/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "GL_FrameBufferObject.h"

GL_FrameBufferObject::GL_FrameBufferObject()
{
  stencilBufferIndex  = -1;
  frameBufferIndex    = -1;
  depthBufferIndex    = -1;
  height              = -1;
  width               = -1;
}

bool  GL_FrameBufferObject::initialize(int width_, int height_, int format)
{
	if(height_==0 || width_==0) return false;
	height = height_;
	width  = width_;
		
	//stop();
	glext->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glext->glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	glext->glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);

	if(frameBufferIndex!=-1) glext->glDeleteFramebuffersEXT(1, &frameBufferIndex);
	if(depthBufferIndex!=-1) glext->glDeleteRenderbuffersEXT(1, &depthBufferIndex);
    if(stencilBufferIndex) glext->glDeleteRenderbuffersEXT(1, &stencilBufferIndex);
	

	glext->glGenFramebuffersEXT(1, &frameBufferIndex);
	glext->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferIndex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint depth = (format & FBO_DEPTH_16) ? GL_DEPTH_COMPONENT16 :
				 (format & FBO_DEPTH_24) ? GL_DEPTH_COMPONENT24 :
				 (format & FBO_DEPTH_32) ? GL_DEPTH_COMPONENT32 : 0;

	//if(depth)
	//{
	glext->glGenRenderbuffersEXT(1, &depthBufferIndex);
	glext->glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBufferIndex);
	glext->glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
	glext->glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
								GL_RENDERBUFFER_EXT, depthBufferIndex);
	//}

	/*  if(format & FBO_STENCIL)
	{
	glext->glGenRenderbuffersEXT(1, &stencilBufferIndex);
	glext->glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, stencilBufferIndex);
	glext->glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX, width, height);
	glext->glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
								 GL_RENDERBUFFER_EXT, stencilBufferIndex);
	}*/

	glext->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	bool result = checkFrameBufferStatus();
	return result;
}
bool  GL_FrameBufferObject::initialize(int width_, int height_, GLenum depthformat, GLuint colid, GLuint depthid)
{
	if(height_==0 || width_==0) return false;

	height = height_;
	width  = width_;
	
//	printf("Unbind...\n");
//	printf("W x H: %d, %d\n", width, height);
//	printf("TextureIDs: %d, %d\n", colid, depthid);

	//stop();
	glext->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glext->glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	glext->glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);

//	printf("Delete...\n");

	if(frameBufferIndex!=-1) glext->glDeleteFramebuffersEXT(1, &frameBufferIndex);
	if(depthBufferIndex!=-1) glext->glDeleteRenderbuffersEXT(1, &depthBufferIndex);
    if(stencilBufferIndex) glext->glDeleteRenderbuffersEXT(1, &stencilBufferIndex);
	
//	printf("Gen FB...\n");

	glext->glGenFramebuffersEXT(1, &frameBufferIndex);
	glext->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferIndex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//	printf("Gen Depth...\n");

	glext->glGenRenderbuffersEXT(1, &depthBufferIndex);
	glext->glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBufferIndex);
	glext->glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT32, width, height);
	glext->glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBufferIndex);

//	printf("Attach Textures...\n");

	glext->glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colid, 0/*mipmap level*/);
	glext->glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthid, 0/*mipmap level*/);


/*  if(format & FBO_STENCIL)
  {
    glext->glGenRenderbuffersEXT(1, &stencilBufferIndex);
    glext->glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, stencilBufferIndex);
    glext->glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX, width, height);
    glext->glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, stencilBufferIndex);
  }*/

  glext->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

//  printf("All done...\n");

  bool result = checkFrameBufferStatus();
  return result;
}
void GL_FrameBufferObject::attachTexture( GLenum attachment, GLenum texType,
								  GLuint texId)
{
	GLint id = 0;
	glext->glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
											 GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
											 &id);
	if( id != texId ) 
    {
		glext->glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, attachment,
								   texType, texId, 0);
    }
}
bool GL_FrameBufferObject::checkFrameBufferStatus()
{
  GLenum status = glext->glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

  switch(status)
  {
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				printf("Error!  %s missing a required image/buffer attachment!\n", 0);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				printf("Error!  %s has no images/buffers attached!\n", 0);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
				printf("Error!  %s has an image/buffer attached in multiple locations!\n", 0);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				printf("Error!  %s has mismatched image/buffer dimensions!\n", 0);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				printf("Error!  %s's colorbuffer attachments have different types!\n", 0);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				printf("Error!  %s trying to draw to non-attached color buffer!\n", 0);
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				printf("Error!  %s trying to read from a non-attached color buffer!\n", 0);
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
				printf("Error!  %s format is not supported by current graphics card/driver!\n", 0);
				break;
			case GL_FRAMEBUFFER_STATUS_ERROR_EXT:
				printf("Error!  Non-framebuffer passed to glCheckFramebufferStatusEXT()!\n");
				break;
			default:
				printf("*UNKNOWN ERROR* reported from glCheckFramebufferStatusEXT() for %s!\n", 0);
				break;
  }
  return true;
}

void GL_FrameBufferObject::switchTarget(int textureID)
{
	glext->glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureID, 0);
	
	checkFrameBufferStatus();
}

void GL_FrameBufferObject::bind()
{
  if(frameBufferIndex)
    glext->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferIndex);
  else
    cout<<"Invalid GL_FrameBufferObject index"<<endl;
}
void GL_FrameBufferObject::bind(GLuint index)
{
  if(index)
    glext->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, index);
  else
    cout<<"Invalid GL_FrameBufferObject index"<<endl;
}
void GL_FrameBufferObject::stop()
{
  if(frameBufferIndex)
  	glext->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

//  if(frameBufferIndex)
//	glext->glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, 0);

//  if(depthBufferIndex)
}

GLuint GL_FrameBufferObject::getHeight(){ return height; }
GLuint GL_FrameBufferObject::getWidth() { return width;  }

GL_FrameBufferObject::~GL_FrameBufferObject()
{
  if(stencilBufferIndex)
    glext->glDeleteRenderbuffersEXT(1, &stencilBufferIndex);

  if(depthBufferIndex)
    glext->glDeleteRenderbuffersEXT(1, &depthBufferIndex);

  if(frameBufferIndex)
    glext->glDeleteFramebuffersEXT(1, &frameBufferIndex);
}