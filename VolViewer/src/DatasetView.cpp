/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "DatasetView.h"

DatasetView::DatasetView()
{
	numbimages = 0;
	current_image = 0;
	datafolder = "";
}
DatasetView::~DatasetView()
{
}
void DatasetView::clear(void)
{
	numbimages = 0;
	current_image = 0;
	datafolder = "";

	datasubfolders.clear();
	datathumbnails.clear();
}
void DatasetView::save(QString fn)
{
}
void DatasetView::load(QString fn)
{
}
