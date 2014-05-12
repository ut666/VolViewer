/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTTHREADEDBRICKLOADER_H
#define QTTHREADEDBRICKLOADER_H

#include <QRunnable>
#include <QStringList>

#include <vector>

#include "VolIO.h"

using namespace std;

class QtThreadedBrickLoader :  public QRunnable
{
	private:
		int index;
		VolIO* volio;
		vector<bool>* empty_brick;
		vector<unsigned char>* data;
		QStringList strList;
		int xstart;
		int ystart;
		int zstart;
		int brickresX;
		int brickresY;
		int brickresZ;
	public:
		QtThreadedBrickLoader(int ind, VolIO* vio, QStringList sList, int xs, int ys, int zs, int bX, int bY, int bZ, vector<unsigned char>* d, vector<bool>* eb);
		~QtThreadedBrickLoader();
		void run();
};

#endif
