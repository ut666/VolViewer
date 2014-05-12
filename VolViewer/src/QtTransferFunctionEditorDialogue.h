/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTTRANSFEREDITOR_H
#define QTTRANSFEREDITOR_H

#include <q3tabdialog.h> 
#include <qpainter.h>
#include <qlabel.h>

#include "QtHistogram.h"

//our tab widgets
#include "QtTransferFunctionWidget.h"
//#include "QtHSVRemapWidget.h"
//#include "QtRGBRemapWidget.h"
//#include "QtThresholdWidget.h"
//#include "QtBrightContWidget.h"
//#include "QtMiscWidget.h"


class QtTransferFunctionEditorDialogue: public Q3TabDialog
{
	 Q_OBJECT

public:
	//our tab widgets
	QtTransferFunctionWidget* tranfer_function_tab;
/*	QtHSVRemapWidget*	hsv_remap_tab;
	QtRGBRemapWidget*	rgb_remap_tab;
	QtBrightContWidget*	bricon_tab;
	QtThresholdWidget*	threshold_tab;
	QtMiscWidget*		misc_tab;*/

	//our histogram data
	double max_histogram;
	double histogram[256];

	QtTransferFunctionEditorDialogue(QWidget * parent=0, const char * name=0, bool modal=FALSE, Qt::WFlags f=0);
	~QtTransferFunctionEditorDialogue();
	
	void reset(void);
	void build_layout(void);
	void rebuild_tabs(void);
	void histogram_clear(void);

private slots:
	void reload_event(void);
	void undo_event(void);
	void close_event(void);
	void apply_event(void);
	void resizeEvent ( QResizeEvent *e );

signals:
	void undo_signal(void);
	void reload_signal(void);
	void close_signal(void);
};

#endif // QTTRANSFEREDITOR_H