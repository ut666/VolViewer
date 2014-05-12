/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
 #include <qlayout.h>
#include <qlabel.h> 
#include <qslider.h>
//Added by qt3to4:
#include <QPixmap>
//#include <qmotifplusstyle.h>	//style

#include <math.h> 

#include "QtTransferFuncDialogue.h"

QtTransferFuncDialogue::QtTransferFuncDialogue(QWidget * parent, const char * name, bool modal, Qt::WFlags f)
:Q3TabDialog(parent, name, modal,f)
{
	/*QSizePolicy sizepolicy;
	sizepolicy.setHeightForWidth(TRUE);
	setSizePolicy(sizepolicy);*/

	setCaption("Transfer Function Editor");
	setIcon( QPixmap( "icons/Simple/function.png" ));

	build_layout();
	resize(780,700);
}

QtTransferFuncDialogue::~QtTransferFuncDialogue(void)
{
}
void QtTransferFuncDialogue::resizeEvent ( QResizeEvent *e )
{
//	printf("width: %d\n", width());
//	printf("height: %d\n", height());
}
void QtTransferFuncDialogue::reset(void)
{
        tranfer_func_tab->reset();
}

void QtTransferFuncDialogue::build_layout(void)
{
	//transfer function tab, needs a pointer to our histogram data
        tranfer_func_tab = new QtTransferFuncWidget(this, "transfertab");
        tranfer_func_tab->create();

        addTab(tranfer_func_tab, "Transfer Function");

	setCancelButton("Close");
	setOkButton(0);
}

void QtTransferFuncDialogue::rebuild_tabs(void)
{	
        tranfer_func_tab->rebuild();
}
