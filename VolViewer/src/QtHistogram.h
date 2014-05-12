/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTHISTOGRAM_H
#define QTHISTOGRAM_H

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
#include "TfuncState.h"

class QtHistogram: public QLabel
{
	Q_OBJECT
		
public:

	QString currentworkingdir;
	
	//toggles update of slices upon
	//tfunc change
	int slice_toggle;

	int hist_enable;
	int cumfreq_enable;
	int tfunc_enable;

	int fe_seleted;
	int fe_l_update;
	int fe_la_update;
	int fe_r_update;
	int fe_g_update;
	int fe_b_update;
	int fe_ra_update;
	int fe_ga_update;
	int fe_ba_update;

	//transfer functions
	TfuncState luminance;
	TfuncState alpha;
	TfuncState redluminance;
	TfuncState redalpha;
	TfuncState greenluminance;
	TfuncState greenalpha;
	TfuncState blueluminance;
	TfuncState bluealpha;

	//Pixmap Graph, we could use QCanvas, TO DO...
	float canvas_size;
	float graph_size;
	float border;

	//pointer to our histogram data
	double* data;
	double max_histogram;

	int minthr;
	int maxthr;
	int logscale;

	QtHistogram(QWidget* parent=0, const char* name=0, float size=1);
	~QtHistogram();

	void reset(void);
	void reload_transfer_function_parameters(void);
	void reset_l(void);
	void reset_la(void);
	void reset_r(void);
	void reset_g(void);
	void reset_b(void);
	void reset_ra(void);
	void reset_ga(void);
	void reset_ba(void);
	void create(void);
	void save(void);
	void load(void);
	void loadfile(QString);
	void savefile(QString);

	void set_transfer_mode(int mode);
	
	void draw_functioneditor(QPainter* p);
	void draw_functioneditoronpixmap(QPainter* p, FunctionEditor* func, QColor col);

	void draw_histogram(QPainter* p);
	void draw_transfer(void);
	void mouse_release(void);
	void emit_tfunc_all(void);
	void emit_tfunc_reloadall(void);
	void emit_tfunc_current(void);
	void emit_redraw(void);
protected:
	void mouseDoubleClickEvent(QMouseEvent * e);	
	void mouseMoveEvent   ( QMouseEvent * e );
	void mousePressEvent  ( QMouseEvent * e );
	void mouseReleaseEvent(QMouseEvent * e );
    void paintEvent( QPaintEvent * pe);
	void resizeEvent ( QResizeEvent * );

public slots:

	void hist_setcurrentworkingdir_slot(QString dn);

signals:

	void hist_redraw_signal(void);
	void hist_tfunc_signal(int mode, TfuncState tfunc);
};

#endif // QTHISTOGRAM_H
