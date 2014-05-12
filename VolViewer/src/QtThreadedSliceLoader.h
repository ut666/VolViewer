/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTTHREADEDSLICELOADER_H
#define QTTHREADEDSLICELOADER_H

#include <QRunnable>
#include <QString> 
#include <QImage>

#include <vector>

#include "VolumeObject.h"

using namespace std;

class QtThreadedSliceLoader //:  public QRunnable

{
	public:

		VolumeObject* volobj;

		QString fn;
		QString ext;
		int x2;
		int y2;
		int z2;
		int mode;

		QtThreadedSliceLoader(VolumeObject* vo, QString fn, QString e, int x2, int y2, int z2, int mode);
		~QtThreadedSliceLoader();
		void run();
};

#endif
