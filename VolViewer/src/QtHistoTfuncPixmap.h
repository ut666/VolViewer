/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QtHistoTfuncPixmap_H
#define QtHistoTfuncPixmap_H

#include <qpainter.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qsizepolicy.h>
#include <qcursor.h>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPainterPath>
#include <QRectF>
#include <qfiledialog.h>

#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include "FunctionEditor.h"
#include "Tokenizer.h"
#include "Vector.h"

class QtHistoTfuncPixmap: public QWidget
{
	Q_OBJECT
		
public:

	int mode;
	
	//current working directory
	QString currentworkingdir;

	//display toggles
	int hist_enable;
	int cumfreq_enable;
	int tfunc_enable;
	int logscale;
	int remaponfly;
	
	//transfer functions
	FunctionEditor funceditor;

	//bright/contrast
	int brightness;
	int contrast;
	
	//threshold
	int minthr;
	int maxthr;

	//Pixmap Graph, we could use QCanvas, TO DO...
	float canvas_size;
	float graph_size;
	float border;
	float pensize;

	//pointer to our histogram data
	double* data;
	double max_histogram;

	QtHistoTfuncPixmap(float size=1);
	~QtHistoTfuncPixmap();

	void reset(void);
    void create(void);
	void save(void);
	void load(void);
	void loadfile(QString);
    void sethistogramdata(double* histodata);

	void tfunc_remap16bit(void);
	
	void draw_functioneditor(QPainter* p);
	void draw_functioneditoronpixmap(QPainter* p, FunctionEditor* func, QColor col);
	void draw_histogram(QPainter* p);

protected:
	void mouseMoveEvent   ( QMouseEvent * e );
	void mousePressEvent  ( QMouseEvent * e );
	void mouseReleaseEvent(QMouseEvent * e );
	void paintEvent( QPaintEvent * pe);
	void resizeEvent ( QResizeEvent * );

public slots:
	void tfunc16_setcurrentworkingdir_slot(QString dn);

signals:
	void tfunc_remap16bit_signal(int, int, const vector<Vector>&);
	void tfunc_remapsignal(void);
};

#endif // QtHistoTfuncPixmap_H
