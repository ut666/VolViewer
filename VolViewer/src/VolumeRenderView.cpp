/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.translate.xt
 */
#include "VolumeRenderView.h"

VolumeRenderView::VolumeRenderView()
{
	eye = Vector(0,0,-1.0);
	at = Vector(0,0,0);
	up = Vector(0,1,0);

	projection_toggle = ORTHO_PROJECTION;

	zoom=0; 
    mouse_active=0;

	winWidth = 512;
	winHeight = 512;
	
	arcball = ArcBall(winWidth, winHeight);

	camera.set(Vector(0, 0, -10), Vector(0,0,0), Vector(0,1,0));
	camera.width = winWidth;
	camera.height = winHeight;
	camera.znear = 0.01f;
	camera.zfar = 1000000.0f;

	luminance.init(256, 0, 10, 100, 100, 0, 255);
	alpha.init(256, 0, 10, 100, 100, 0, 255);
	redluminance.init(256, 0, 10, 100, 100, 0, 255);
	redalpha.init(256, 0, 10, 100, 100, 0, 255);
	greenluminance.init(256, 0, 10, 100, 100, 0, 255);
	greenalpha.init(256, 0, 10, 100, 100, 0, 255);
	blueluminance.init(256, 0, 10, 100, 100, 0, 255);
	bluealpha.init(256, 0, 10, 100, 100, 0, 255);

	redclip_toggle = greenclip_toggle  = blueclip_toggle = true;
}
VolumeRenderView::~VolumeRenderView()
{
	cutting_planes.clear();
}
void VolumeRenderView::reset_view(void)
{
	zoom = 0.0;
	at = Vector(0,0,0);
	eye = Vector(0,0,-1.0);
	up = Vector(0,1,0);
	lookat();
}
void VolumeRenderView::reset_zoom(void)
{
	zoom = 0.0;
}
void VolumeRenderView::reset_translate(void)
{
	at = Vector(0,0,0);
	eye = Vector(0,0,-1.0);
	up = Vector(0,1,0);
	lookat();
}
void VolumeRenderView::lookat(void)
{
	arcball.LastRot = arcball.ThisRot;

	Vector forward = at-eye;
	forward.normalize();

	Vector side;
	side.cross(forward, up);
	side.normalize();

	up.cross(side, forward);
	up.normalize();
	
	arcball.ThisRot.M[0] = side.x;  
	arcball.ThisRot.M[3] = side.y;
	arcball.ThisRot.M[6] = side.z;

	arcball.ThisRot.M[1] = up.x;
	arcball.ThisRot.M[4] = up.y;
	arcball.ThisRot.M[7] = up.z;

	arcball.ThisRot.M[2] = -forward.x;
	arcball.ThisRot.M[5] = -forward.y;
	arcball.ThisRot.M[8] = -forward.z;

//	Matrix3fMulMatrix3f(&arcball.ThisRot, &arcball.LastRot);				// Accumulate Last Rotation Into This One
	Matrix4fSetIdentity(&arcball.Transform);
	Matrix4fSetRotationFromMatrix3f(&arcball.Transform, &arcball.ThisRot);	// Set Our Final Transform's Rotation From This One*/
}
void VolumeRenderView::rotateX(float angle)
{
	Vector forward = eye-at;
	forward.normalize();

	Quaternion rot;
	rot.from_angle(up, angle);
	eye = at+rot.apply(forward);

	lookat();
}
void VolumeRenderView::rotateY(float angle)
{
	Vector forward = eye-at;
	forward.normalize();

	Vector axis;
	axis.cross(forward, up);
	axis.normalize();

	Quaternion rot;
	rot.from_angle(axis, angle);
	eye = at + rot.apply(forward);

	lookat();
}
void VolumeRenderView::rotateZ(float angle)
{
	Vector forward = eye-at;
	forward.normalize();

	Quaternion rot;
	rot.from_angle(forward, angle);
	up = rot.apply(up);

	lookat();
}
void VolumeRenderView::pitchUp(float angle)
{
	Vector temp;
	temp.cross(eye - at, up);
	Vector axis = temp.unit();
	Quaternion rot;
	rot.from_angle(axis, angle);
	at =  eye + rot.apply(at - eye);

	lookat();
}
void VolumeRenderView::headingLeft(float angle)
{
	Quaternion rot;
	rot.from_angle(up, angle);
	at =  eye + rot.apply(at - eye);

	lookat();
}
void VolumeRenderView::move_left(float amount)
{
   Vector temp;
   temp.cross(up, at - eye);
   Vector disp = amount * temp.unit();
   at+=disp;
   eye += disp;

   lookat();
}
void VolumeRenderView::move_up(float amount)
{
   Vector disp = amount * up;
   at+=disp;
   eye += disp;

   lookat();
}
void VolumeRenderView::move_forward(float amount)
{
   Vector disp = amount * (at - eye).unit();
   at += disp;
   eye += disp;

   lookat();
}
void VolumeRenderView::mouse_move(QMouseEvent* e)
{	
	int x = e->x();
	int y = e->y();
	
	if(mouse_active==0) return;

	if(mouse_active==1)
	{
		//printf("mouse_move\n");

		int deltaX( x - mouse_centre.x );
		int deltaY( y - mouse_centre.y );
		if( deltaX == 0 && deltaY == 0 ) return;

		mouseLook(-deltaX*0.125, deltaY*0.125);

		//glutWarpPointer( mouse_centre.x, mouse_centre.y );
	}
	if(mouse_active==2)
	{
		int deltaX( x - mouse_current.x );
		int deltaY( y - mouse_current.y );
		if( deltaX == 0 && deltaY == 0 ) return;
		
		mouse_current.x = x;
		mouse_current.y = y;

		mouseRotate(-deltaY*0.125, deltaX*0.125);
	}

	mouse_centre.x = x;
	mouse_centre.y = y;

	lookat();
}
void VolumeRenderView::mouse_press(QMouseEvent* e)
{
 	int x = e->x();
	int y = e->y();

	if ((e->button()&Qt::LeftButton)) 
	{
		//printf("mouse_active = 1\n");
		mouse_active=1;
		mouse_centre.x = x;
		mouse_centre.y = y;

		//cursor_id = glutGet( GLUT_WINDOW_CURSOR );
		//glutSetCursor( GLUT_CURSOR_NONE );
    }
	if((e->button()&Qt::RightButton))
	{
		//printf("mouse_active = 2\n");
		mouse_active=2;
		mouse_current.x=x;
		mouse_current.y=y;
	 }
}
void VolumeRenderView::mouse_release(void)
{
	mouse_active=0;
	//printf("mouse_active = 0\n");

	//glutSetCursor(cursor_id);
}
void VolumeRenderView::mouseRotate(double x_angle, double y_angle)
{
	Vector temp;
	temp.cross(eye - at, up);
	Vector axis = temp.unit();

	Quaternion x_rot, y_rot, rot;
	
	x_rot.from_angle(up, y_angle);
	y_rot.from_angle(axis, x_angle);

	rot = x_rot * y_rot;
	
	eye = at + rot.apply(eye-at);
}
void VolumeRenderView::mouseLook( double heading_angle, double pitch_angle)
{
	Vector temp;
	temp.cross(eye - at, up);
	Vector axis = temp.unit();

	Quaternion pitch_rot, heading_rot, rot;
	
	pitch_rot.from_angle(axis, pitch_angle);
	heading_rot.from_angle(up, heading_angle);
	
	rot = pitch_rot * heading_rot;
	 
	at = eye + rot.apply(at-eye);

}
void VolumeRenderView::save(QString fn)
{
	ofstream fout;
	fout.precision(10);
	fout.setf(ios::fixed); 

	fout.open(fn.latin1());

	if(fout==NULL)
	{
		printf("ERROR: SAVING FILE .vrv File! VolumeRenderView::save(QString filename)\n");
	}
	
	save(fout);

	fout.close();
}
void VolumeRenderView::save(ofstream& fout)
{
	fout.precision(10);
	fout.setf(ios::fixed); 
	
	fout<<"BOUNDINGBOX"<<endl;
		fout<<"BB_VONE = "<<bb_v1.x<<" "<<bb_v1.y<<" "<<bb_v1.z<<endl;
		fout<<"BB_VTWO = "<<bb_v2.x<<" "<<bb_v2.y<<" "<<bb_v2.z<<endl;
		fout<<"BB_VTHREE = "<<bb_v3.x<<" "<<bb_v3.y<<" "<<bb_v3.z<<endl;
		fout<<"BB_VFOUR = "<<bb_v4.x<<" "<<bb_v4.y<<" "<<bb_v4.z<<endl;
		fout<<"BB_VFIVE = "<<bb_v5.x<<" "<<bb_v5.y<<" "<<bb_v5.z<<endl;
		fout<<"BB_VSIX = "<<bb_v6.x<<" "<<bb_v6.y<<" "<<bb_v6.z<<endl;
		fout<<"BB_VSEVEN = "<<bb_v7.x<<" "<<bb_v7.y<<" "<<bb_v7.z<<endl;
		fout<<"BB_VEIGHT = "<<bb_v8.x<<" "<<bb_v8.y<<" "<<bb_v8.z<<endl;

	fout<<"CAMERA"<<endl;

		fout<<"WINWIDTH = "<<winWidth<<endl;
		fout<<"WINHEIGT = "<<winHeight<<endl;
		fout<<"ZOOM = "<<zoom<<endl;
		fout<<"CAMERA_PROJECTION_MODE = "<<projection_toggle<<endl;
		fout<<"CAMERA_EYE = "<<eye.x<<" "<<eye.y<<" "<<eye.z<<endl;
		fout<<"CAMERA_UP = "<<up.x<<" "<<up.y<<" "<<up.z<<endl;
		fout<<"CAMERA_AT = "<<at.x<<" "<<at.y<<" "<<at.z<<endl;

		fout<<"CAMERA_FOV = "<<camera.fov<<endl;
		fout<<"CAMERA_ZNEAR = "<<camera.znear<<endl;
		fout<<"CAMERA_ZFAR = "<<camera.zfar<<endl;
		fout<<"CAMERA_ZSCREEN = "<<camera.zscreen<<endl;

		fout<<"CAMERA_WIDTH = "<<camera.width<<endl;
		fout<<"CAMERA_HEIGHT = "<<camera.height<<endl;

	fout<<"ARCBALL"<<endl;

		fout<<"ARCBALL_ROTTYPE = "<<arcball.rot_type<<endl;
		fout<<"ARCBALL_ORIENTTYPE = "<<arcball.orien_type<<endl;

		fout<<"ARCBALL_DRAGGING = "<<arcball.isDragging<<endl;
		fout<<"ARCBALL_ZOOMING = "<<arcball.isZooming<<endl;
		fout<<"ARCBALL_TRANSLATING = "<<arcball.isTranslate<<endl;
		fout<<"ARCBALL_OLDPOS = "<<arcball.oldpos.x<<" "<<arcball.oldpos.y<<" "<<arcball.oldpos.z<<endl;

		fout<<"ARCBALL_TRANSFORM_MATRIX = ";
		for(int i=0; i<16; i++)
			fout<<arcball.Transform.M[i]<<" ";
		fout<<endl;

		fout<<"ARCBALL_LASTROT_MATRIX = ";
		for(int i=0; i<9; i++)
			fout<<arcball.LastRot.M[i]<<" ";
		fout<<endl;

		fout<<"ARCBALL_THISROT_MATRIX = ";
		for(int i=0; i<9; i++)
			fout<<arcball.ThisRot.M[i]<<" ";
		fout<<endl;

		fout<<"ARCBALL_STVEC = "<<arcball.StVec.x<<" "<<arcball.StVec.y<<" "<<arcball.StVec.z<<endl;
		fout<<"ARCBALL_ENVEC = "<<arcball.EnVec.x<<" "<<arcball.EnVec.y<<" "<<arcball.EnVec.z<<endl;

		fout<<"ARCBALL_ADJWIDTH = "<<arcball.AdjustWidth<<endl;
		fout<<"ARCBALL_ADJHEIGHT = "<<arcball.AdjustHeight<<endl;

	fout<<"MATRICES"<<endl;
	
		fout<<"MODELVIEW = ";
		for(int i=0; i<16; i++)
			fout<<mvmatrix[i]<<" ";
		fout<<endl;

		fout<<"PROJECTION = ";
		for(int i=0; i<16; i++)
			fout<<projmatrix[i]<<" ";
		fout<<endl;

		fout<<"VIEWPORT = ";
		for(int i=0; i<4; i++)
			fout<<viewport[i]<<" ";
		fout<<endl;

	fout<<"CUTTING_PLANES"<<endl;
		fout<<"CUTTING_PLANES_SIZE = "<<cutting_planes.planes.size()<<endl;
		for(int i=0; i<cutting_planes.planes.size(); i++)
		{
			fout<<"P = "<<cutting_planes.planes[i].centre.x<<" "<<cutting_planes.planes[i].centre.y<<" "<<cutting_planes.planes[i].centre.z<<" ";
			fout<<cutting_planes.planes[i].normal.x<<" "<<cutting_planes.planes[i].normal.y<<" "<<cutting_planes.planes[i].normal.z<<" ";
			fout<<cutting_planes.planes[i]._v1.x<<" "<<cutting_planes.planes[i]._v1.y<<" "<<cutting_planes.planes[i]._v1.z<<" ";
			fout<<cutting_planes.planes[i]._v2.x<<" "<<cutting_planes.planes[i]._v2.y<<" "<<cutting_planes.planes[i]._v2.z<<" ";
			fout<<cutting_planes.planes[i]._v3.x<<" "<<cutting_planes.planes[i]._v3.y<<" "<<cutting_planes.planes[i]._v3.z<<" ";
			fout<<cutting_planes.planes[i]._v4.x<<" "<<cutting_planes.planes[i]._v4.y<<" "<<cutting_planes.planes[i]._v4.z<<" ";
			fout<<cutting_planes.planes[i].v1.x<<" "<<cutting_planes.planes[i].v1.y<<" "<<cutting_planes.planes[i].v1.z<<" ";
			fout<<cutting_planes.planes[i].v2.x<<" "<<cutting_planes.planes[i].v2.y<<" "<<cutting_planes.planes[i].v2.z<<" ";
			fout<<cutting_planes.planes[i].v3.x<<" "<<cutting_planes.planes[i].v3.y<<" "<<cutting_planes.planes[i].v3.z<<" ";
			fout<<cutting_planes.planes[i].v4.x<<" "<<cutting_planes.planes[i].v4.y<<" "<<cutting_planes.planes[i].v4.z<<" ";
			fout<<(float)cutting_planes.planes[i].plane_equation[0]<<" "<<(float)cutting_planes.planes[i].plane_equation[1]<<" "<<(float)cutting_planes.planes[i].plane_equation[2]<<" "<<(float)cutting_planes.planes[i].plane_equation[3]<<" ";
			fout<<cutting_planes.planes[i].arcball.orien_type<<" ";
			fout<<cutting_planes.planes[i].arcball.Transform.M[0]<<" "<<cutting_planes.planes[i].arcball.Transform.M[1]<<" "<<cutting_planes.planes[i].arcball.Transform.M[2]<<" "<<cutting_planes.planes[i].arcball.Transform.M[3]<<" "<<
				  cutting_planes.planes[i].arcball.Transform.M[4]<<" "<<cutting_planes.planes[i].arcball.Transform.M[5]<<" "<<cutting_planes.planes[i].arcball.Transform.M[6]<<" "<<cutting_planes.planes[i].arcball.Transform.M[7]<<" "<<
				  cutting_planes.planes[i].arcball.Transform.M[8]<<" "<<cutting_planes.planes[i].arcball.Transform.M[9]<<" "<<cutting_planes.planes[i].arcball.Transform.M[10]<<" "<<cutting_planes.planes[i].arcball.Transform.M[11]<<" "<<
				  cutting_planes.planes[i].arcball.Transform.M[12]<<" "<<cutting_planes.planes[i].arcball.Transform.M[13]<<" "<<cutting_planes.planes[i].arcball.Transform.M[14]<<" "<<cutting_planes.planes[i].arcball.Transform.M[15]<<" ";
			fout<<cutting_planes.planes[i].arcball.LastRot.M[0]<<" "<<cutting_planes.planes[i].arcball.LastRot.M[1]<<" "<<cutting_planes.planes[i].arcball.LastRot.M[2]<<" "<<
				  cutting_planes.planes[i].arcball.LastRot.M[3]<<" "<<cutting_planes.planes[i].arcball.LastRot.M[4]<<" "<<cutting_planes.planes[i].arcball.LastRot.M[5]<<" "<<
				  cutting_planes.planes[i].arcball.LastRot.M[6]<<" "<<cutting_planes.planes[i].arcball.LastRot.M[7]<<" "<<cutting_planes.planes[i].arcball.LastRot.M[8]<<" ";
			fout<<cutting_planes.planes[i].arcball.ThisRot.M[0]<<" "<<cutting_planes.planes[i].arcball.ThisRot.M[1]<<" "<<cutting_planes.planes[i].arcball.ThisRot.M[2]<<" "<<
				  cutting_planes.planes[i].arcball.ThisRot.M[3]<<" "<<cutting_planes.planes[i].arcball.ThisRot.M[4]<<" "<<cutting_planes.planes[i].arcball.ThisRot.M[5]<<" "<<
				  cutting_planes.planes[i].arcball.ThisRot.M[6]<<" "<<cutting_planes.planes[i].arcball.ThisRot.M[7]<<" "<<cutting_planes.planes[i].arcball.ThisRot.M[8]<<endl;
				   /*fout<<"A = "<<cutting_planes.planes[i].angleX<<" "<<cutting_planes.planes[i].angleY<<endl;*/
		}
		fout<<"PLANE_RENDERMODE = ";
		for(int i=0; i<6; i++)
			fout<<plane_rendermode[i]<<" ";
		fout<<endl;

		fout<<"TRANSFER_FUNCTIONS"<<endl;
		fout<<"TF_LUMINANCE = ";
		luminance.save(fout);
		fout<<"TF_ALPHA = ";
		alpha.save(fout);
		fout<<"TF_REDLUM = ";
		redluminance.save(fout);
		fout<<"TF_REDALPHA = ";
		redalpha.save(fout);
		fout<<"TF_GREENLUM = ";
		greenluminance.save(fout);
		fout<<"TF_GREENALPHA = ";
		greenalpha.save(fout);
		fout<<"TF_BLUELUM = ";
		blueluminance.save(fout);
		fout<<"TF_BLUEALPHA = ";
		bluealpha.save(fout);
}
float VolumeRenderView::check_file_version(QString fn)
{
	float versionnumb=-1;

	if(!fn.isEmpty())
	{
		string label;
		ifstream input_file; 
		input_file.open(fn.latin1());

		if(!input_file.is_open()) return -3;

		Tokenizer token(input_file);
		token.eolIsSignificant(true);

		while(!input_file.eof())
		{
			if(token.ttype()==TT_WORD)
			{
				//get the first string lable
				label = token.sval();
				
				if(label=="VRV_VERSION")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						versionnumb = token.nval();
						input_file.close();
						return versionnumb;
					}
				}
				else
				{
					token.nextToken();
				}
			}
			else token.nextToken();
		}

		input_file.close();
	}
	else return -3;

	return versionnumb;}
void VolumeRenderView::load_version0(QString fn)
{
	projection_toggle = ORTHO_PROJECTION;

	if(!fn.isEmpty())
	{
		ifstream input_file; 
		input_file.open(fn.latin1());

		if(!input_file.is_open()) return;

		Tokenizer token(input_file);
		token.eolIsSignificant(true);
		string label;

		int current_index = -1;
		int current_plane_index = -1;

		//until oef is reached step through each token
		while(!input_file.eof())
		{
			if(token.ttype()==TT_WORD)
			{
				//get the first string lable
				label = token.sval();
				
				if(label=="WINWIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("WINWIDTH: %f\n", var);

						winWidth = var;
					}
				}
				else if(label=="WINHEIGT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("WINHEIGT: %f\n", var);

						winHeight = var;
					}
				}
				else if(label=="WINMAXDIM")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("WINMAXDIM: %f\n", var);

						winMaxDim = var;
					}
				}
				else if(label=="ZOOM")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("ZOOM: %f\n", var);

						zoom = var;
					}
				}
				else if(label=="TX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						printf("TX: %f\n", var);

						eye.x = var;
					}
				}
				else if(label=="TY")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						printf("TY: %f\n", var);

						eye.y = var;
					}
				}
				else if(label=="TZ")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						printf("TZ: %f\n", var);

						eye.z = var;
					}
				}
				else if(label=="BB_VONE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						printf("BB: %f\n", var);
						bb_v1.x = var;
						token.nextToken();
						var = token.nval();
						printf("BB: %f\n", var);
						bb_v1.y = var;
						token.nextToken();
						var = token.nval();
						printf("BB: %f\n", var);
						bb_v1.z = var;
					}
				}
				else if(label=="BB_VTWO")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v2.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v2.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v2.z = var;
					}
				}
				else if(label=="BB_VTHREE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v3.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v3.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v3.z = var;
					}
				}
				else if(label=="BB_VFOUR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v4.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v4.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v4.z = var;
					}
				}
				else if(label=="BB_VFIVE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v5.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v5.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v5.z = var;
					}
				}
				else if(label=="BB_VSIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v6.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v6.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v6.z = var;
					}
				}
				else if(label=="BB_VSEVEN")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v7.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v7.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v7.z = var;
					}
				}
				else if(label=="BB_VEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v8.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v8.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v8.z = var;
					}
				}
				else if(label=="TEX_TRANSLATE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("TEX_TRANSLATE: %f\n", var);
						tex_translate.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("TEX_TRANSLATE: %f\n", var);
						tex_translate.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("TEX_TRANSLATE: %f\n", var);
						tex_translate.z = var;
					}
				}
				else if(label=="CAMERA_EYE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_EYE: %f\n", var);
						camera.eye.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE: %f\n", var);
						camera.eye.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE: %f\n", var);
						camera.eye.z = var;
					}
				}
				else if(label=="CAMERA_EYE_OLD")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_EYE_OLD: %f\n", var);
						camera.eyeOld.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE_OLD: %f\n", var);
						camera.eyeOld.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE_OLD: %f\n", var);
						camera.eyeOld.z = var;
					}
				}
				else if(label=="CAMERA_EYE_NEW")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_EYE_NEW: %f\n", var);
						camera.eyeNew.x = var;
						token.nextToken();
						var = token.nval();
			//			printf("CAMERA_EYE_NEW: %f\n", var);
						camera.eyeNew.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE_NEW: %f\n", var);
						camera.eyeNew.z = var;
					}
				}
				else if(label=="CAMERA_AT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_AT: %f\n", var);
						camera.at.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT: %f\n", var);
						camera.at.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT: %f\n", var);
						camera.at.z = var;
					}
				}
				else if(label=="CAMERA_AT_OLD")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_AT_OLD: %f\n", var);
						camera.atOld.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT_OLD: %f\n", var);
						camera.atOld.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT_OLD: %f\n", var);
						camera.atOld.z = var;
					}
				}
				else if(label=="CAMERA_AT_NEW")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_AT_NEW: %f\n", var);
						camera.atNew.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT_NEW: %f\n", var);
						camera.atNew.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT_NEW: %f\n", var);
						camera.atNew.z = var;
					}
				}
				else if(label=="CAMERA_UP")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_UP: %f\n", var);
						camera.up.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_UP: %f\n", var);
						camera.up.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_UP: %f\n", var);
						camera.up.z = var;
					}
				}
				else if(label=="CAMERA_STEPS")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
			//			printf("CAMERA_STEPS: %f\n", var);

						camera.steps = var;
					}
				}
				else if(label=="CAMERA_MAXSTEPS")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
			//			printf("CAMERA_MAXSTEPS: %f\n", var);

						camera.maxSteps = var;
					}
				}
				else if(label=="CAMERA_SMOOTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_SMOOTH: %f\n", var);

						camera.smooth = var;
					}
				}
				else if(label=="CAMERA_FOV")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_FOV: %f\n", var);

						camera.fov = var;
					}
				}
				else if(label=="CAMERA_ZNEAR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_ZNEAR: %f\n", var);

						camera.znear = var;
					}
				}
				else if(label=="CAMERA_ZFAR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_ZFAR: %f\n", var);

						camera.zfar = var;
					}
				}
				else if(label=="CAMERA_ZSCREEN")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_ZSCREEN: %f\n", var);

						camera.zscreen = var;
					}
				}
				else if(label=="CAMERA_RATIO")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_RATIO: %f\n", var);

						camera.ratio = var;
					}
				}
				else if(label=="CAMERA_WIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_WIDTH: %f\n", var);

						camera.width = var;
					}
				}
				else if(label=="CAMERA_HEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_HEIGHT: %f\n", var);

						camera.height = var;
					}
				}
				else if(label=="CAMERA_MOUSE_ACTIVE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_MOUSE_ACTIVE: %f\n", var);

						camera.mouse_active = var;
					}
				}
				else if(label=="CAMERA_CURSORID")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_CURSORID: %f\n", var);

						camera.cursor_id = var;
					}
				}
				else if(label=="CAMERA_MODE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_MODE: %f\n", var);

						camera.mode = var;
					}
				}
				else if(label=="CAMERA_MOUSECENTRE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_MOUSECENTRE: %f\n", var);
						camera.mouse_centre.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_MOUSECENTRE: %f\n", var);
						camera.mouse_centre.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_MOUSECENTRE: %f\n", var);
						camera.mouse_centre.z = var;
					}
				}
				else if(label=="CAMERA_MOUSECURRENT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_MOUSECURRENT: %f\n", var);
						camera.mouse_current.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_MOUSECURRENT: %f\n", var);
						camera.mouse_current.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_MOUSECURRENT: %f\n", var);
						camera.mouse_current.z = var;
					}
				}
				else if(label=="ARCBALL_ROTTYPE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ROTTYPE: %f\n", var);

						arcball.rot_type = var;
					}
				}
				else if(label=="ARCBALL_ORIENTTYPE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ORIENTTYPE: %f\n", var);

						arcball.orien_type = var;
					}
				}
				else if(label=="ARCBALL_DRAGGING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_DRAGGING: %f\n", var);

						arcball.isDragging = var;
					}
				}
				else if(label=="ARCBALL_ZOOMING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ZOOMING: %f\n", var);

						arcball.isZooming = var;
					}
				}
				else if(label=="ARCBALL_TRANSLATING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_TRANSLATING: %f\n", var);

						arcball.isTranslate = var;
					}
				}
				else if(label=="ARCBALL_OLDPOS")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						arcball.oldpos.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						arcball.oldpos.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						arcball.oldpos.z = var;
					}
				}
				else if(label=="ARCBALL_TRANSFORM_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
					//		printf("ARCBALL_TRANSFORM_MATRIX: %f\n", var);
							arcball.Transform.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_LASTROT_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<9; i++)
						{
							var = token.nval();
					//		printf("ARCBALL_LASTROT_MATRIX: %f\n", var);
							arcball.LastRot.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_THISROT_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<9; i++)
						{
							var = token.nval();
					//		printf("ARCBALL_THISROT_MATRIX: %f\n", var);
							arcball.ThisRot.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_STVEC")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						arcball.StVec.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						arcball.StVec.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						arcball.StVec.z = var;
					}
				}
				else if(label=="ARCBALL_ENVEC")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ENVEC: %f\n", var);
						arcball.EnVec.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_ENVEC: %f\n", var);
						arcball.EnVec.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_ENVEC: %f\n", var);
						arcball.EnVec.z = var;
					}
				}
				else if(label=="ARCBALL_ADJWIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("ARCBALL_ADJWIDTH: %f\n", var);

						arcball.AdjustWidth = var;
					}
				}
				else if(label=="ARCBALL_ADJHEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ADJHEIGHT: %f\n", var);

						arcball.AdjustHeight = var;
					}
				}
				else if(label=="MODELVIEW")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
					//		printf("MODELVIEW: %f\n", var);
							mvmatrix[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="PROJECTION")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
					//		printf("PROJECTION: %f\n", var);
							projmatrix[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="VIEWPORT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<4; i++)
						{
							var = token.nval();
					//		printf("VIEWPORT: %f\n", var);
							viewport[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="CUTTING_PLANES_SIZE")
				{
					current_plane_index = -1;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						var = token.nval();
					//	printf("CUTTING_PLANES_SIZE: %f\n", var);
						cutting_planes.planes.resize((int) var);
					}
				}
				else if(label=="P")
				{
					current_plane_index++;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;

						var = token.nval();
						cutting_planes.planes[current_plane_index].centre.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].centre.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].centre.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index].normal.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].normal.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].normal.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v1.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v1.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v1.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v2.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v2.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v2.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v3.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v3.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v3.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v4.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v4.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v4.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index].v1.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v1.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v1.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index].v2.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v2.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v2.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index].v3.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v3.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v3.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index].v4.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v4.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v4.z = var;
						token.nextToken();
						
						for(int i=0; i<4; i++)
						{
							var = token.nval();
							cutting_planes.planes[current_plane_index].plane_equation[i] = var;
							token.nextToken();
						}

						var = token.nval();
						cutting_planes.planes[current_plane_index].arcball.orien_type = var;
						token.nextToken();

						for(int i=0; i<16; i++)
						{
							var = token.nval();
							cutting_planes.planes[current_plane_index].arcball.Transform.M[i] = var;
							token.nextToken();
						}

						for(int i=0; i<9; i++)
						{
							var = token.nval();
							cutting_planes.planes[current_plane_index].arcball.LastRot.M[i] = var;
							token.nextToken();
						}

						for(int i=0; i<9; i++)
						{
							var = token.nval();
							cutting_planes.planes[current_plane_index].arcball.ThisRot.M[i] = var;
							token.nextToken();
						}

						cutting_planes.planes[current_plane_index].translate = 0.0;
					}
				}
				else if(label=="PLANE_RENDERMODE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						cutting_planes.eval_planeintersections();

						float var;
						for(int i=0; i<6; i++)
						{
							var = token.nval();
					//		printf("PLANE_RENDERMODE: %f\n", var);
							plane_rendermode[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="TF_LUMINANCE")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					luminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					luminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					luminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					luminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					luminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<luminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						luminance.functioneditor.bs.control_points[i] = v;
					}
					luminance.functioneditor.bs.compute();
				}
				else if(label=="TF_ALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					alpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					alpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					alpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					alpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					alpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<alpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						alpha.functioneditor.bs.control_points[i] = v;
					}
					alpha.functioneditor.bs.compute();
				}
				else if(label=="TF_REDLUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					redluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					redluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					redluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					redluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					redluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<redluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						redluminance.functioneditor.bs.control_points[i] = v;
					}
					redluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_REDALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					redalpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					redalpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					redalpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					redalpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					redalpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<redalpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						redalpha.functioneditor.bs.control_points[i] = v;
					}
					redalpha.functioneditor.bs.compute();
				}
				else if(label=="TF_GREENLUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					greenluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					greenluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					greenluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					greenluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					greenluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<greenluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						greenluminance.functioneditor.bs.control_points[i] = v;
					}
					greenluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_GREENALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					greenalpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					greenalpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					greenalpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					greenalpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					greenalpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<greenalpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						greenalpha.functioneditor.bs.control_points[i] = v;
					}
					greenalpha.functioneditor.bs.compute();
				}				
				else if(label=="TF_BLUELUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					blueluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					blueluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					blueluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					blueluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					blueluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<blueluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						blueluminance.functioneditor.bs.control_points[i] = v;
					}
					blueluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_BLUEALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					bluealpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					bluealpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					bluealpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					bluealpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					bluealpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<bluealpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						bluealpha.functioneditor.bs.control_points[i] = v;
					}

					bluealpha.functioneditor.bs.compute();
				}
				else
				{
					token.nextToken();
				}
			}
			else token.nextToken();
		}

		input_file.close();

	}
}
void VolumeRenderView::load_version1(QString fn)
{
	if(!fn.isEmpty())
	{
		ifstream input_file; 
		input_file.open(fn.latin1());

		if(!input_file.is_open()) return;

		Tokenizer token(input_file);
		token.eolIsSignificant(true);
		string label;

		int current_index = -1;
		int current_plane_index = -1;

		//until oef is reached step through each token
		while(!input_file.eof())
		{
			if(token.ttype()==TT_WORD)
			{
				//get the first string lable
				label = token.sval();
				
				if(label=="WINWIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("WINWIDTH: %f\n", var);

						winWidth = var;
					}
				}
				else if(label=="WINHEIGT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("WINHEIGT: %f\n", var);

						winHeight = var;
					}
				}
				else if(label=="ZOOM")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("ZOOM: %f\n", var);

						zoom = var;
					}
				}
				else if(label=="BB_VONE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						printf("BB: %f\n", var);
						bb_v1.x = var;
						token.nextToken();
						var = token.nval();
						printf("BB: %f\n", var);
						bb_v1.y = var;
						token.nextToken();
						var = token.nval();
						printf("BB: %f\n", var);
						bb_v1.z = var;
					}
				}
				else if(label=="BB_VTWO")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v2.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v2.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v2.z = var;
					}
				}
				else if(label=="BB_VTHREE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v3.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v3.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v3.z = var;
					}
				}
				else if(label=="BB_VFOUR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v4.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v4.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v4.z = var;
					}
				}
				else if(label=="BB_VFIVE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v5.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v5.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v5.z = var;
					}
				}
				else if(label=="BB_VSIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v6.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v6.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v6.z = var;
					}
				}
				else if(label=="BB_VSEVEN")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v7.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v7.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v7.z = var;
					}
				}
				else if(label=="BB_VEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v8.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v8.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						bb_v8.z = var;
					}
				}
				else if(label=="CAMERA_PROJECTION_MODE")
				{
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						int var = token.nval();
						projection_toggle = var;
					}

				}
				else if(label=="CAMERA_EYE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						eye.x = var;
						token.nextToken();
						var = token.nval();
						eye.y = var;
						token.nextToken();
						var = token.nval();
						eye.z = var;
					}
				}
				else if(label=="CAMERA_UP")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						up.x = var;
						token.nextToken();
						var = token.nval();
						up.y = var;
						token.nextToken();
						var = token.nval();
						up.z = var;
					}
				}
				else if(label=="CAMERA_AT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						at.x = var;
						token.nextToken();
						var = token.nval();
						at.y = var;
						token.nextToken();
						var = token.nval();
						at.z = var;
					}
				}
				else if(label=="CAMERA_FOV")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_FOV: %f\n", var);

						camera.fov = var;
					}
				}
				else if(label=="CAMERA_ZNEAR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_ZNEAR: %f\n", var);

						camera.znear = var;
					}
				}
				else if(label=="CAMERA_ZFAR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_ZFAR: %f\n", var);

						camera.zfar = var;
					}
				}
				else if(label=="CAMERA_ZSCREEN")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_ZSCREEN: %f\n", var);

						camera.zscreen = var;
					}
				}
				else if(label=="CAMERA_RATIO")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_RATIO: %f\n", var);

						camera.ratio = var;
					}
				}
				else if(label=="CAMERA_WIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_WIDTH: %f\n", var);

						camera.width = var;
					}
				}
				else if(label=="CAMERA_HEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_HEIGHT: %f\n", var);

						camera.height = var;
					}
				}
				else if(label=="ARCBALL_ROTTYPE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ROTTYPE: %f\n", var);

						arcball.rot_type = var;
					}
				}
				else if(label=="ARCBALL_ORIENTTYPE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ORIENTTYPE: %f\n", var);

						arcball.orien_type = var;
					}
				}
				else if(label=="ARCBALL_DRAGGING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_DRAGGING: %f\n", var);

						arcball.isDragging = var;
					}
				}
				else if(label=="ARCBALL_ZOOMING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ZOOMING: %f\n", var);

						arcball.isZooming = var;
					}
				}
				else if(label=="ARCBALL_TRANSLATING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_TRANSLATING: %f\n", var);

						arcball.isTranslate = var;
					}
				}
				else if(label=="ARCBALL_OLDPOS")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						arcball.oldpos.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						arcball.oldpos.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						arcball.oldpos.z = var;
					}
				}
				else if(label=="ARCBALL_TRANSFORM_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
					//		printf("ARCBALL_TRANSFORM_MATRIX: %f\n", var);
							arcball.Transform.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_LASTROT_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<9; i++)
						{
							var = token.nval();
					//		printf("ARCBALL_LASTROT_MATRIX: %f\n", var);
							arcball.LastRot.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_THISROT_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<9; i++)
						{
							var = token.nval();
					//		printf("ARCBALL_THISROT_MATRIX: %f\n", var);
							arcball.ThisRot.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_STVEC")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						arcball.StVec.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						arcball.StVec.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						arcball.StVec.z = var;
					}
				}
				else if(label=="ARCBALL_ENVEC")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ENVEC: %f\n", var);
						arcball.EnVec.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_ENVEC: %f\n", var);
						arcball.EnVec.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_ENVEC: %f\n", var);
						arcball.EnVec.z = var;
					}
				}
				else if(label=="ARCBALL_ADJWIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("ARCBALL_ADJWIDTH: %f\n", var);

						arcball.AdjustWidth = var;
					}
				}
				else if(label=="ARCBALL_ADJHEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ADJHEIGHT: %f\n", var);

						arcball.AdjustHeight = var;
					}
				}
				else if(label=="MODELVIEW")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
					//		printf("MODELVIEW: %f\n", var);
							mvmatrix[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="PROJECTION")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
					//		printf("PROJECTION: %f\n", var);
							projmatrix[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="VIEWPORT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<4; i++)
						{
							var = token.nval();
					//		printf("VIEWPORT: %f\n", var);
							viewport[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="CUTTING_PLANES_SIZE")
				{
					current_plane_index = -1;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						var = token.nval();
					//	printf("CUTTING_PLANES_SIZE: %f\n", var);
						cutting_planes.planes.resize((int) var);
					}
				}
				else if(label=="P")
				{
					current_plane_index++;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;

						var = token.nval();
						cutting_planes.planes[current_plane_index].centre.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].centre.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].centre.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index].normal.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].normal.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].normal.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v1.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v1.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v1.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v2.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v2.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v2.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v3.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v3.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v3.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v4.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v4.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index]._v4.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index].v1.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v1.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v1.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index].v2.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v2.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v2.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index].v3.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v3.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v3.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						cutting_planes.planes[current_plane_index].v4.x = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v4.y = var;
						token.nextToken();
						var = token.nval();
						cutting_planes.planes[current_plane_index].v4.z = var;
						token.nextToken();
						
						for(int i=0; i<4; i++)
						{
							var = token.nval();
							cutting_planes.planes[current_plane_index].plane_equation[i] = var;
							token.nextToken();
						}

						var = token.nval();
						cutting_planes.planes[current_plane_index].arcball.orien_type = var;
						token.nextToken();

						for(int i=0; i<16; i++)
						{
							var = token.nval();
							cutting_planes.planes[current_plane_index].arcball.Transform.M[i] = var;
							token.nextToken();
						}

						for(int i=0; i<9; i++)
						{
							var = token.nval();
							cutting_planes.planes[current_plane_index].arcball.LastRot.M[i] = var;
							token.nextToken();
						}

						for(int i=0; i<9; i++)
						{
							var = token.nval();
							cutting_planes.planes[current_plane_index].arcball.ThisRot.M[i] = var;
							token.nextToken();
						}

						cutting_planes.planes[current_plane_index].translate = 0.0;
					}
				}
				else if(label=="PLANE_RENDERMODE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						cutting_planes.eval_planeintersections();

						float var;
						for(int i=0; i<6; i++)
						{
							var = token.nval();
					//		printf("PLANE_RENDERMODE: %f\n", var);
							plane_rendermode[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="TF_LUMINANCE")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					luminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					luminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					luminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					luminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					luminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<luminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						luminance.functioneditor.bs.control_points[i] = v;
					}
					luminance.functioneditor.bs.compute();
				}
				else if(label=="TF_ALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					alpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					alpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					alpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					alpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					alpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<alpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						alpha.functioneditor.bs.control_points[i] = v;
					}
					alpha.functioneditor.bs.compute();
				}
				else if(label=="TF_REDLUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					redluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					redluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					redluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					redluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					redluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<redluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						redluminance.functioneditor.bs.control_points[i] = v;
					}
					redluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_REDALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					redalpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					redalpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					redalpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					redalpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					redalpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<redalpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						redalpha.functioneditor.bs.control_points[i] = v;
					}
					redalpha.functioneditor.bs.compute();
				}
				else if(label=="TF_GREENLUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					greenluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					greenluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					greenluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					greenluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					greenluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<greenluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						greenluminance.functioneditor.bs.control_points[i] = v;
					}
					greenluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_GREENALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					greenalpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					greenalpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					greenalpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					greenalpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					greenalpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<greenalpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						greenalpha.functioneditor.bs.control_points[i] = v;
					}
					greenalpha.functioneditor.bs.compute();
				}				
				else if(label=="TF_BLUELUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					blueluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					blueluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					blueluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					blueluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					blueluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<blueluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						blueluminance.functioneditor.bs.control_points[i] = v;
					}
					blueluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_BLUEALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					bluealpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					bluealpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					bluealpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					bluealpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					bluealpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<bluealpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						bluealpha.functioneditor.bs.control_points[i] = v;
					}

					bluealpha.functioneditor.bs.compute();
				}
				else
				{
					token.nextToken();
				}
			}
			else token.nextToken();
		}

		input_file.close();

	}
}
void VolumeRenderView::load(QString fn)
{
	float versumb = check_file_version(fn);
	versumb += 0;

	if(versumb==-1.0f)
	{
		printf("VRV_VERSION=0\n");
		load_version0(fn);
	}
	else if(versumb==1.0f)
	{
		printf("VRV_VERSION=1.0\n");
		load_version1(fn);
	}
}
const VolumeRenderView& VolumeRenderView::operator= (const VolumeRenderView& right)
{
	projection_toggle = right.projection_toggle;

	bb_v1 = right.bb_v1;
	bb_v2 = right.bb_v2;
	bb_v3 = right.bb_v3;
	bb_v4 = right.bb_v4;
	bb_v5 = right.bb_v5;
	bb_v6 = right.bb_v6;
	bb_v7 = right.bb_v7;
	bb_v8 = right.bb_v8;

	at = right.at;
	eye = right.eye;
	up = right.up;

	zoom = right.zoom;
	//winWidth = right.winWidth;
	//winHeight = right.winHeight;

	arcball = right.arcball;
	camera = right.camera;

	cutting_planes.clear();
	cutting_planes = right.cutting_planes;

	for(int i=0; i<6; i++)
		plane_rendermode[i] = right.plane_rendermode[i];

	luminance = right.luminance;
	alpha = right.alpha;
	redluminance = right.redluminance;
	redalpha = right.redalpha;
	greenluminance = right.greenluminance;
	greenalpha = right.greenalpha;
	blueluminance = right.blueluminance;
	bluealpha = right.bluealpha;

	redclip_toggle = right.redclip_toggle;
	blueclip_toggle = right.blueclip_toggle;
	greenclip_toggle = right.greenclip_toggle;

	return *this;
}
