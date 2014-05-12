/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef MESH_H
#define MESH_H

#define MESH_CENTRE			2
#define MESH_VERTEX_NORMALS	3
#define MESH_FACE_NORMALS	4
#define MESH_TEXCOORDS		5

#include <qgl.h>
#include <stdarg.h>
#include <vector>
#include <deque>
#include <iostream>
#include <fstream>

using namespace std; 

#include "glext.h"
#include "Vector.h"
#include "Face.h"
#include "GL_Ext.h"

class Mesh
{
	public:
		GL_Ext*			glextensions;

		bool	has_vbo;
		unsigned int	vbo1_name;
		unsigned int	vbo2_name;
		unsigned int	vbo3_name;
		GLuint*				vbo_indices;
		GLfloat*			vbo_verts;
		GLfloat*			vbo_norms;

		int				nv, nf;			//number of vertices / faces	

		vector <Vector>	vert;			//the vertices array		
		vector <Vector>	vertnorm;		//the normals array
		vector <Vector>  vertcol;		//the colours array
		vector <Vector>  texCoords;		//the texcoord array
		vector <Face*>	faces;			//the faces pointer array
		vector <Face>	localfaces;		//the faces array	
				
		//-----------------------------------------------------

		Mesh();
		Mesh(const Mesh& m);
		~Mesh();

		void reset(void);
		void set_numverts(int n);
		void set_numfaces(int n);

		bool load_OBJ_data(const char *Filename);
		bool load_NET_data(const char *Filename);
		void set_data(const vector<Vector>& newVerts,
					  const vector<Vector>& newNorms,
					  const vector<Vector>& newColours,
					  const vector<Vector>& newTexcoords,
					  const vector<vector<int> >& newFaces);
						
		bool save_NET_data(const char *Filename);
		bool save_OBJ_data(const char *Filename);
		bool save_MARKCANVAS_data(const char *Filename);
		
		void r_vbo(GLenum surface);
		void init_vbo(void);

		//utility functions
		void scale(float size);
		void scalev(Vector scalefactor);

		//evaluators
		void evaluate(int ...);
		void evaluate_centre(void );
		void evaluate_normals(void);
		void evaluate_texture_coords(void);
		
		//Render Modes
		void r_normal(GLenum surface, GLint texname);

		//Debug
		void display_normals(void);
		void display_vertex_normal(int x, int y);

		const Mesh& operator= (const Mesh& right);

};

#endif // MESH_H