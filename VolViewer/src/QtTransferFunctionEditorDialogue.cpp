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

#include "QtTransferFunctionEditorDialogue.h"

QtTransferFunctionEditorDialogue::QtTransferFunctionEditorDialogue(QWidget * parent, const char * name, bool modal, Qt::WFlags f) 
:Q3TabDialog(parent, name, modal,f)
{
//	QSizePolicy sizepolicy;
//	sizepolicy.setHeightForWidth(TRUE);
//	setSizePolicy(sizepolicy);

	setCaption("Transfer Function Editor");
	//setIcon( QPixmap( "icons/Simple/function.png" ));

	histogram_clear();
	build_layout();
	
	resize(700,500);
}

QtTransferFunctionEditorDialogue::~QtTransferFunctionEditorDialogue(void)
{
}
void QtTransferFunctionEditorDialogue::resizeEvent ( QResizeEvent *e )
{
//	printf("width: %d\n", width());
//	printf("height: %d\n", height());
}
void QtTransferFunctionEditorDialogue::reset(void)
{
	tranfer_function_tab->reset();
	//threshold_tab->reset();
	//bricon_tab->reset();
	//hsv_remap_tab->reset();
	//rgb_remap_tab->reset();
	//misc_tab->reset();
}

void QtTransferFunctionEditorDialogue::build_layout(void)
{
	//transfer function tab, needs a pointer to our histogram data
	tranfer_function_tab = new QtTransferFunctionWidget(this, "transfertab"); 
	tranfer_function_tab->create();
	tranfer_function_tab->hist->data = histogram;
		
//	threshold_tab  = new QtThresholdWidget(this, "threshold_tab");
//	threshold_tab->create();

	//HSV remap
/*	hsv_remap_tab = new QtHSVRemapWidget(this, "hsv_remap_tab");
	hsv_remap_tab->create();
*/
	/*//RGB remap
	rgb_remap_tab = new QtRGBRemapWidget(this, "rgb_remap_tab");
	rgb_remap_tab->create();*/
/*
	//BRI/CONT tab
	bricon_tab = new QtBrightContWidget(this, "bricon_tab");
	bricon_tab->create();

	//MISC
	misc_tab = new QtMiscWidget(this, "misc_tab");
	misc_tab->create();
*/
	//our tab pages
	addTab(tranfer_function_tab, "Transfer Function");
//	addTab(threshold_tab, "Threshold");
//	addTab(bricon_tab, "Brightness/Contrast");
	//addTab(hsv_remap_tab, "HSV Remap");
	//addTab(rgb_remap_tab, "RGB Remap");
//	addTab(misc_tab, "Misc");

	//some buttons
//	setApplyButton ("Apply");
//	setDefaultButton("Undo");
//	setHelpButton("Reload");
	setCancelButton("Close");
	setOkButton(0);

//	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(apply_event()));
//	connect(this, SIGNAL(defaultButtonPressed()), this, SLOT(undo_event()));
//	connect(this, SIGNAL(helpButtonPressed()), this, SLOT(reload_event()));
	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(close_event()));
}
void QtTransferFunctionEditorDialogue::undo_event(void)
{
	emit undo_signal();
}
void QtTransferFunctionEditorDialogue::reload_event(void)
{
	emit reload_signal();
}
void QtTransferFunctionEditorDialogue::close_event(void)
{
//	reset();
//	emit close_signal();
}
void QtTransferFunctionEditorDialogue::apply_event(void)
{
/*	if(threshold_tab==currentPage())
	{
		threshold_tab->emit_threshold();
	}*/
/*	if(bricon_tab==currentPage())
	{
		bricon_tab->apply();
	}
*/
	if(tranfer_function_tab==currentPage())
	{
		tranfer_function_tab->emit_transfer_function();
	}
	/*else if(hsv_remap_tab==currentPage())
	{
		hsv_remap_tab->emit_hsv_remap();
	}
	else if(rgb_remap_tab==currentPage())
	{
		rgb_remap_tab->emit_rgb_remap();
	}*/
	/*else if(misc_tab==currentPage())
	{
		misc_tab->apply();
	}*/
}

void QtTransferFunctionEditorDialogue::histogram_clear(void)
{
	//lets avoid divisions by zero...
	max_histogram=0;
	for(int i=0; i<256; i++)	
	{
		histogram[i] = 0;
	}
}
void QtTransferFunctionEditorDialogue::rebuild_tabs(void)
{	
	tranfer_function_tab->rebuild();
//	hsv_remap_tab->rebuild();
}