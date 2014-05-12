/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef VOLIO_H
#define VOLIO_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <cmath>

#include <qgl.h>
#include <qdir.h> 
#include <qimage.h> 
#include <qfile.h>
#include <qstring.h>
#include <q3progressdialog.h>
#include <qmessagebox.h>
#include <QThreadPool>
#include <QTime>
#include <QFutureWatcher>
#include <QFuture>
#include <QtCore>
#include <QApplication>
#include <QProgressDialog>

#ifdef LIBTIFF_SUPPORT
	#include <tiffio.h>
	#include <tiff.h>
#endif

#include "glext.h"
#include "VolumeObject.h"
#include "RAW.h"
#include "Global.h"
#include "Tokenizer.h"

#include "QtThreadedSliceLoader.h"

#include "MyDefines.h"

#ifdef OMERO_SUPPORT
	#include "OMEROLoader.h"
	#include "QtOMEROLoader.h"
#endif

//I/O OBJECT TYPES
#define VOL_PNG		0
#define VOL_RAW		1
#define VOL_WLZ		2
#define VOL_TIF		3

// nonzero if not power of 2 
#define NOTPOW2(num) ((num) & (num - 1))
typedef QFutureWatcher<void> SliceLoader;

class VolIO
{

public:

	bool showGUI;

	SliceLoader *watcher;

	bool greyscale;
	bool red, green, blue;

	Q3ProgressDialog*		progressbar;

	float					WLZ_rescale;
	VolumeObject*			volobj;

	int res_x, res_y, res_z;
	int offset_x, offset_y, offset_z;
	
	VolIO();
	~VolIO();
	
	void		load_xyzspacing(QString filename);
	void		load_xyzscale(QString filename);
	void		load_xyzscale_fromlogfile(QString dirname);
	
    void		load_PNG(QString dirname, int mode);
    void		load_RAW(QString filename, int mode);
#ifdef LIBTIFF_SUPPORT
	void		load_TIFF(QString dirname, int mode);
#endif

	void		save_PNG(QString filename);
	void		save_RAW(QString filename);


#ifdef OMERO_SUPPORT
	void		load_OMERO(OMEROLoader* omeroLoader);
#endif

	void		reload_data(void);
	void		eval_resize_dimensions(void);
	int			makepower2(int val);
	void		resize(void);
	void		pad_subimage(int pad_size, int xstart, int ystart, int zstart, int brickresX, int brickresY, int brickresZ, unsigned char* data);
	void		pad(int pad_size);

	//experimental code....
	void		load_sliceThread(const QtThreadedSliceLoader &threadloader);
	void		init_subimageloading(QString dirname, int mode);
	QStringList load_subimage_strngList(void);
	bool		load_subimage(QStringList strList, int xstart, int ystart, int zstart, int brickresX, int brickresY, int brickresZ, unsigned char* data);
	void		set_metadata(float x, float y, QString descp);

	//Libtiff stuff (obsolete)
	//void		load_TIFF(QString dirname, int mode);
	//void		set_metadata(float x, float y, QString descp);
};

#endif //VOLIO_H
