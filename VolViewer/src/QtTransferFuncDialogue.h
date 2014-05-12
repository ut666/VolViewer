/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QtTransferFuncDialogue_H
#define QtTransferFuncDialogue_H

#include <q3tabdialog.h> 
#include <qpainter.h>
#include <qlabel.h>

#include "QtHistoTfuncPixmap.h"
#include "QtTransferFuncWidget.h"

class QtTransferFuncDialogue: public Q3TabDialog
{
	 Q_OBJECT

public:
	//our tab widgets
        QtTransferFuncWidget* tranfer_func_tab;

        QtTransferFuncDialogue(QWidget * parent=0, const char * name=0, bool modal=FALSE, Qt::WFlags f=0);
        ~QtTransferFuncDialogue();
	
	void reset(void);
	void build_layout(void);
	void rebuild_tabs(void);
	void resizeEvent ( QResizeEvent *e );
	
};

#endif // QtTransferFuncDialogue_H
