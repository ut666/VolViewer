/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef DATASETVIEW_H
#define DATASETVIEW_H

#include <QString>
#include <QPixmap>

#include <vector>
using namespace std;

#include "VolumeRenderView.h"

class DatasetView
{

public:

	int numbimages;
	int current_image;

	QString datafolder;
	vector< QString > datasubfolders;
	vector< QPixmap > datathumbnails;

	DatasetView();
	~DatasetView();

	void clear(void);

	void load(QString fn);
	void save(QString fn);
};

#endif //DATASETVIEW_H