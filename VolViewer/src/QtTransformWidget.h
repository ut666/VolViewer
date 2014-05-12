/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTTRANSFORM_H
#define QTTRANSFORM_H

#include <qpainter.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qslider.h>
//#include <qmotifplusstyle.h>	//style

class QtTransformWidget: public QWidget
{
	Q_OBJECT
		
public:
	QtTransformWidget(QWidget* parent = 0, const char* name = 0);
	~QtTransformWidget();

	void create(void);
	void reset(void);
	void rebuild(void);

private slots:

};

#endif // QTTRANSFORM_H