/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTIMEASUREWIDGET_H
#define QTIMEASUREWIDGET_H

#include <qpainter.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qslider.h>
#include <QListWidget>
#include <qimage.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <QDialog>
#include <QMessageBox>
#include <qlineedit.h>
#include <qcolordialog.h>
#include <QPixmap>
#include <QFileDialog>
#include <QModelIndex>
#include <QLineEdit>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QTableWidget>
#include <QHeaderView>

#include <q3buttongroup.h>
#include <q3boxlayout.h>
#include <q3dragobject.h>

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include "Global.h"

#include "VolumeObject.h"
#include "Vector.h"
#include "Tokenizer.h"
#include "PCA.h"
#include "Edge.h"
#include "Face.h"
#include "MeasureObject.h"
#include "MeasureListObject.h"
#include "MeasureDefinitions.h"
#include "SVNObject.h"

class MySrcListWidget : public QListWidget 
{
	Q_OBJECT
public:
    MySrcListWidget(QWidget * parent = 0):QListWidget(parent){}
protected:
    void dragMoveEvent(QDragMoveEvent *e) 
	{	
		if (e->source() != this) e->accept();
        else e->ignore();
    }
	void dropEvent(QDropEvent* e)
	{
		//printf("SRC DROP\n");
	    QListWidget* source = qobject_cast< QListWidget* >(e->source());
		QListWidgetItem* item = source->currentItem();
		int cur = source->currentRow();
		source->takeItem(cur);
		//source->removeItemWidget(item);
		
		QString txt = item->text();

		//printf("src label: %s\n", txt.toAscii().data());
		for(int i=0; i<this->count(); i++)
		{
			QListWidgetItem* dstitem = this->item(i);
			QString dsttxt = dstitem->text();

			//printf("dst label: %s\n", dsttxt.toAscii().data());
			if(dsttxt==txt)
			{
				//printf("FOUND ITEM\n");
				dstitem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled|Qt::ItemIsEnabled);
				dstitem->setBackgroundColor(Qt::white);
			}
		}
		 e->ignore();
		//QListWidget::dropEvent(e);
	}
};

class MyDstListWidget : public QListWidget 
{
	Q_OBJECT
public:
    MyDstListWidget(QWidget * parent = 0):QListWidget(parent){}
protected:
    void dragMoveEvent(QDragMoveEvent *e) 
	{	
		if (e->source() != this) e->accept();
        else e->ignore();
    }
	void dropEvent(QDropEvent* e)
	{
	    QListWidget* source = qobject_cast< QListWidget* >(e->source());
		QListWidgetItem* item = source->currentItem();
		item->setFlags(Qt::NoItemFlags);
		item->setBackgroundColor(Qt::darkGray);
		QListWidget::dropEvent(e);
	}
};

class QtMeasuringWidget: public QWidget
{
	Q_OBJECT
		
public:

	QString apppath;
	VolumeObject* volobj;
	//===========================
	//GUI STUFF
	//===========================
	int dataset_index;
	QString dataset_src_alignfn;
	QString dataset_dst_alignfn;

	int selected;
	int lb_index;
	QTableWidget*	 listbox;

	QDialog* label_dialogue;

	QDialog* add_dialogue;
	QString originalfilename;
	QString currentworkingdir;
	QPushButton* selec;

	QTabWidget* tabWidget;

	Q3ButtonGroup *vertices_tablayout;
	Q3ButtonGroup *edges_tablayout;
	Q3ButtonGroup *faces_tablayout;
	Q3ButtonGroup *labels_tablayout;
	Q3ButtonGroup *lists_tablayout;

	QPushButton* vertices_add_button;
	QPushButton* vertices_rmv_button;
	QPushButton* edges_add_button;
	QPushButton* edges_rmv_button;
	QPushButton* faces_add_button;
	QPushButton* faces_rmv_button;
	QPushButton* labels_add_button;
	QPushButton* labels_rmv_button;

	QLineEdit* name_le;
	QPushButton* resetplanesbutton;
	QCheckBox* solidplanescheckbox;
	QCheckBox* flipplanescheckbox;

	QPushButton* facealign_orient_button;

	QPushButton* sectiontoggle_button;
	QLabel* section_thickn_lb;
	QSlider* section_thickn_slider;
	int section_thickn;

	QLabel* section_trans_lb;
	QSlider* section_trans_slider;
	int section_trans;

	QPushButton* pointmode_button;
	QPushButton* linemode_button;
	QPushButton* lsegmentmode_button;
	QPushButton* lloopmode_button;
	QPushButton* polygon_button;
	QPushButton* sphere_button;
	QPushButton* magnetline_button;
	QPushButton* trianglesheet_button;
	QPushButton* labeller_button;
	QPushButton* facebuilder_button;
	QPushButton* beizierbuilder_button;

	bool modifyshow_toggle;
	QPushButton* modifygroupbutton;

	QListWidget* vertices_listbox;
	QListWidget* edges_listbox;
	QListWidget* faces_listbox;
	QListWidget* labels_listbox;
	
	QString editlabels_oldname;
	int editlabels_mode;
	Q3ButtonGroup *mode_grp;
	QLineEdit* labelname_le;
	MySrcListWidget* editsrclabels_listbox;
	MyDstListWidget* editdstlabels_listbox;

	QLineEdit* vertices_count;
	QLineEdit* vertices_curX;
	QLineEdit* vertices_curY;
	QLineEdit* vertices_curZ;
	QLineEdit* vertices_curR;
	QLineEdit* vertices_curG;
	QLineEdit* vertices_curB;
	QLineEdit* vertices_meanX;
	QLineEdit* vertices_meanY;
	QLineEdit* vertices_meanZ;
	QLineEdit* vertices_meanR;
	QLineEdit* vertices_meanG;
	QLineEdit* vertices_meanB;

	QLineEdit* edges_count;
	QLineEdit* edges_sum;
	QLineEdit* edges_cur;
	QLineEdit* edges_min;
	QLineEdit* edges_max;

	QLineEdit* faces_count;
	QLineEdit* faces_curperim;
	QLineEdit* faces_curarea;
	QLineEdit* faces_sumarea;

	QLineEdit* pnts_area;
	QLineEdit* pnts_volume;

	QLineEdit* voxX_le;
	QLineEdit* voxY_le;
	QLineEdit* voxZ_le;
	QLineEdit* unitX_le;
	QLineEdit* unitY_le;
	QLineEdit* unitZ_le;
	
	QLineEdit* time_le;

	int vert_label;
	QCheckBox *vert_cb;
	int edge_label;
	QCheckBox *edge_cb;
	int face_label;
	QCheckBox *face_cb;
	int arrow_label;
	QCheckBox *arrow_cb;
	int labels_label;
	QCheckBox *labels_cb;
	int lists_label;
	QCheckBox *lists_cb;
	int tris_label;
	QCheckBox *tris_cb;
	QPushButton* okbutton;
	QPushButton* cancelbutton;

	QPushButton* labelokbutton;
	QPushButton* labelcancelbutton;

	QCheckBox* sectiontoggle__cb;
	Q3ButtonGroup* section_slider_grp;
	QCheckBox* sectionupdateview__cb;

	//===========================
	//TIME AND SCALE
	//===========================
	float time_value;
	float voxX, voxZ, voxY;
	float unitX, unitZ, unitY;
	float rescaleX, rescaleY, rescaleZ;
	float spacingX, spacingY, spacingZ;

	//===========================
	//MEASURING OBJECTS
	//===========================
	
	//saved objects
	//vector< MeasureObject > measure_objects;

	// measuring_drawmodes
	MeasureListObject measure_objects;
	vector< Vector > measuring_planes;

	//current item
	MeasureObject current_measure_object;
	vector< int > current_facemeasurement;
	int current_drawmode;
	int current_viewmode;
	int current_listbox_index;
	int current_vert;
	int current_face;
	int current_edge;
	int current_label;

	//===========================

	QtMeasuringWidget(QWidget* parent = 0, const char* name = 0, QString apppath="");
	~QtMeasuringWidget();

	void create(void);
	void reset(void);
	void rebuild(void);
	void clear_all(void);
	void clear_list(void);

	void eval_length(void);
	float eval_area(void);

private slots:

	void open_event(void);
	void save_event(void);
	
	void export_event(void);
	void drawmode_event(int sel);

	void setscale_event(void);
	void okscalebutton_event(void);

	void settime_event(void);
	void oktimebutton_event(void);

	void del_event(void);
	void resetplanesbutton_event(void);
	void solidplanescheckbox_event(void);
	void flipplanescheckbox_event(void);

	void create_MeasureGUI(void);
	void updateGUI_withCurrent(void);

	void create_LabelGUI(void);
	void updateGUI_withCurrentLabel(void);

	void cancbutton_event(void);
	void okaddmeasurebutton_event(void);
	void okeditmeasurebutton_event(void);
	void okdatasetmeasurebutton_event(void);

	void vertices_add_button_event(void);
	void vertices_rmv_button_event(void);
	void edges_add_button_event(void);
	void edges_rmv_button_event(void);
	void faces_add_button_event(void);
	void faces_rmv_button_event(void);
	void labels_add_button_event(void);
	void labels_rmv_button_event(void);

	void listboxitemChanged_event(QTableWidgetItem* item);

	void tab_currentChanged_slot(int index);

	void add_event(void);
	void edit_event(QTableWidgetItem* item);

	void vertices_clicked_slot(QListWidgetItem* item);
	void vertices_currentItemChanged_slot(QListWidgetItem* curr, QListWidgetItem* prev);

	void edges_clicked_slot(QListWidgetItem* item);
	void edges_currentItemChanged_slot(QListWidgetItem* curr,QListWidgetItem* prev);

	void faces_clicked_slot(QListWidgetItem* item);
	void faces_currentItemChanged_slot(QListWidgetItem* curr,QListWidgetItem* prev);

	void labels_clicked_slot(QListWidgetItem* item);
	void labels_currentItemChanged_slot(QListWidgetItem* curr, QListWidgetItem* prev);

	void labels_doubleclicked_slot(const QModelIndex&);

	void label_listmode_event(int id);
	void editlabels_currentItemChanged_slot(QListWidgetItem*, QListWidgetItem*);

	void modifygroupbutton_event(void);

	void vert_cb_event(void);
	void edge_cb_event(void);
	void face_cb_event(void);
	void arrow_cb_event(void);
	void labels_cb_event(void);
	void lists_cb_event(void);
	void tris_cb_event(void);

	void build_verticesTab(void);
	void build_edgesTab(void);
	void build_facesTab(void);
	void build_labelsTab(void);
	void build_listsTab(void);

	void computeInfo(void);

	void facealign_orient_button_event(void);
	void facealign_updateview_event(void);

	void sectiontoggle_button_event(void);
	void section_thickn_event(int val);
	void section_trans_event(int val);

public slots:
	
	void setData(void);

	void dataset_event(QString src_fn, QString dst_fn, int ds_index);

	void open_file(QString fn);
	void save_file(QString fn);

	void measure_point_slot(Vector v);
	void measure_editpoint_slot(Vector v);
	void measure_editgrppoint_slot(int i, Vector v);

	void measure_editpoint_select_slot(int index);
	void measure_edge_select_slot(int index);
	void measure_face_select_slot(int index);


	void measure_setscale2_slot(float x, float y, float z);
	void measure_setspacing_slot(float x, float y, float z);
	void measure_setoriginalfn_slot(QString fn);
	void measure_savecutplane_slot(int, float, float, float);
	void measure_close_slot(void);
	void measure_setrescale_slot(float x, float y, float z);
	void measure_setcurrentworkingdir_slot(QString dn);
	
	void measure_clearcurrent_slot(void);

	void measure_updatecurrent_slot(MeasureObject obj);

	void measure_rmvvertface_slot(int v);
	void measure_addvertface_slot(int v);
	void measure_endvertface_slot(void);

	void labelokbutton_event(void);
	void labelcancbutton_event(void);
	void labelokeditbutton_event(void);

	void dragndrop_slot(QString sFileName);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

signals:
	void measure_start_signal(void);
	void measure_stop_signal(void);
	void measure_pause_signal(void);
	void measure_edit_signal(void);
	void measure_cancel_signal(void);
		 
	void measure_add_item_signal(MeasureObject obj, int draw_mode);
	void measure_delete_item_signal(int index);

	void measure_update_item_signal(int index, MeasureObject obj, int draw_mode);
	void measure_update_current_signal(int index);
	void measure_update_current_signal(MeasureObject obj);

	void measure_update_current_drawmode_signal(int drawmode);
	void measure_update_current_editmode_signal(int editmode);
	void measure_update_current_viewmode_signal(int viewmode);

	void measure_setscale_signal(float x, float y, float z);
	void measure_setcutplane_signal(int, float, float, float);
	void measure_enableunselect_signal(void);
	void measure_disableunselect_signal(void);

	void measure_update_visible_signal(int, bool);

	void measure_clearcurrent_signal(void);
	void measure_clearall_signal(void);

	void measure_modify_start_signal(void);
	void measure_modify_stop_signal(void);

	void measureCursorOn_signal(void);
	void measureCursorOff_signal(void);

	void cutplane_resetall_signal(void);
	void cutplane_solid_signal(bool on);
	void cutplane_flip_signal(bool on);

	void measure_labels_signal(int, int, int, int, int, int, int);

	void measure_rmv_currentvert_signal(MeasureObject obj, int index);
	void measure_selected_vertindex_signal(int index);

	void measure_rmv_currentface_signal(int index);
	void measure_selected_faceindex_signal(int index);

	void measure_rmv_currentedge_signal(int index);
	void measure_selected_edgeindex_signal(int index);

	void measure_rmv_currentlabel_signal(int index, bool delgeom);

	void  measure_facealign_orient_signal(bool on);

	void  measure_section_toggle_signal(bool on);
	void  measure_section_trans_signal(int trans);
	void  measure_section_thickn_signal(int thickn);

	void measure_facealign_updateview_signal(bool state);

	void meausre_setuprendering_signal(void);
};

#endif // QTIMEASUREWIDGET_H
