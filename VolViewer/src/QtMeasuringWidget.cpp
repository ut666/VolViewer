/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtMeasuringWidget.h"
//Added by qt3to4:
#include <Q3Frame>
#include <QPixmap>
#include <QLabel>


QtMeasuringWidget::QtMeasuringWidget(QWidget* parent, const char* name, QString abbsapppath)
: QWidget( parent, name )
{
	apppath = abbsapppath;

	lb_index=0;
	edges_sum=0;

	originalfilename="";
	currentworkingdir="";
	time_value = 1.0;
	voxX = voxY = voxZ = 1.0;
	unitX = unitY = unitZ = 1.0;
	setAcceptDrops(true);

	current_drawmode = -1;
	current_viewmode = -1;
	current_listbox_index = -1;
	current_vert = -1;
	current_edge = -1;
	current_face = -1;
	current_label = -1;

	vert_label = 0;
	face_label = 0;
	edge_label = 0;
	arrow_label = 0;
	labels_label = 0;
	lists_label = 0;
	tris_label = 1;

	vertices_listbox = 0;
	edges_listbox = 0;
	faces_listbox = 0;
	labels_listbox = 0;

	editsrclabels_listbox = 0;
	editdstlabels_listbox = 0;
	modifyshow_toggle = false;

	section_thickn = 10;
	section_trans = 0;
}
QtMeasuringWidget::~QtMeasuringWidget()
{
}
void QtMeasuringWidget::dragndrop_slot(QString sFileName)
{	
	if (!sFileName.isEmpty()) 
	{
		if(sFileName.contains(".msr", true) || sFileName.contains(".MSR", true)) 
		{
			open_file(sFileName);
		}
	}
}
void QtMeasuringWidget::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept(Q3TextDrag::canDecode(event));
}
void QtMeasuringWidget::dropEvent(QDropEvent* event)
{
	Q3StrList URIs;

	if ( Q3UriDrag::decode(event, URIs) ) 
	{
		QString text = Q3UriDrag::uriToLocalFile(URIs.getFirst());
		dragndrop_slot(text);
	}
}
void QtMeasuringWidget::rebuild(void)
{
}
void QtMeasuringWidget::reset(void)
{

}
void QtMeasuringWidget::create(void)
{
    //connect(this, SIGNAL(dragndrop_signal(QString)), this, SLOT(dragndrop_slot(QString)));

	Q3ButtonGroup* opensave_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "File:", this );
    opensave_grp->setExclusive( FALSE );
	opensave_grp->setAlignment(Qt::AlignLeft);
	//opensave_grp->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));

		QPushButton* open = new QPushButton(QIcon(QPixmap(apppath+"/Icons/fileopen.xpm")), "Open", opensave_grp, "open");
		connect( open, SIGNAL( clicked () ), this, SLOT( open_event() ) );
		//open->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		
		QPushButton* save = new QPushButton(QIcon(QPixmap(apppath+"/Icons/filesave.xpm")), "Save", opensave_grp, "save");
		connect( save, SIGNAL( clicked () ), this, SLOT( save_event() ) );
		//save->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));

	Q3ButtonGroup* addrmv_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Actions:", this );
    addrmv_grp->setExclusive( FALSE );

		QPushButton* add = new QPushButton("New", addrmv_grp, "new");
		add->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		connect( add, SIGNAL( clicked() ), this, SLOT( add_event() ) );
		
		QPushButton* del = new QPushButton("Delete", addrmv_grp, "del");
		del->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		connect( del, SIGNAL( clicked() ), this, SLOT( del_event() ) );
		
	Q3ButtonGroup* scale_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Properties:", this );
    scale_grp->setExclusive( FALSE );

		QPushButton* settime = new QPushButton("Set Time", scale_grp, "settime");
		settime->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		connect( settime, SIGNAL( clicked() ), this, SLOT( settime_event() ) );

		QPushButton* setscale = new QPushButton("Set Scale", scale_grp, "setscale");
		setscale->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		connect( setscale, SIGNAL( clicked() ), this, SLOT( setscale_event() ) );
	
/*		Q3ButtonGroup *labels_layout = new Q3ButtonGroup( 3, Qt::Horizontal, "View Toggles:", this);
		vert_cb = new QCheckBox("Vertex Indices",	 labels_layout );
		edge_cb = new QCheckBox("Edge Indices", labels_layout );
		face_cb = new QCheckBox("Face Indices",   labels_layout );
		arrow_cb = new QCheckBox("Arrow Heads",   labels_layout );
		labels_cb = new QCheckBox("Labels",   labels_layout );
		labels_cb->hide();
		tris_cb = new QCheckBox("Solid Faces",   labels_layout );
		tris_cb->setOn(true);
		lists_cb = new QCheckBox("Lists",   labels_layout );
		lists_cb->hide();
		labels_layout->hide();

		connect( vert_cb,	 SIGNAL( clicked() ), this, SLOT( vert_cb_event() ) );
		connect( edge_cb,	 SIGNAL( clicked() ), this, SLOT( edge_cb_event() ) );
		connect( face_cb,	 SIGNAL( clicked() ), this, SLOT( face_cb_event() ) );
		connect( arrow_cb,	 SIGNAL( clicked() ), this, SLOT( arrow_cb_event() ) );
		connect( labels_cb,	 SIGNAL( clicked() ), this, SLOT( labels_cb_event() ) );
		connect( lists_cb,	 SIGNAL( clicked() ), this, SLOT( lists_cb_event() ) );
		connect( tris_cb,	 SIGNAL( clicked() ), this, SLOT( tris_cb_event() ) );
		vert_cb->setOn(FALSE);
		edge_cb->setOn(FALSE);
		face_cb->setOn(FALSE);
		arrow_cb->setOn(FALSE);
		labels_cb->setOn(FALSE);
		lists_cb->setOn(FALSE);
*/
	Q3ButtonGroup* list_grp = new Q3ButtonGroup( 2, Qt::Horizontal, "", this );
    list_grp->setExclusive( FALSE );

			listbox = new QTableWidget(list_grp);
		    listbox->setColumnCount(2);	
			QStringList headerlist;
			headerlist << "Name" << "Visible";
			listbox->setHorizontalHeaderLabels(headerlist);
			listbox->verticalHeader()->hide();
			listbox->setAlternatingRowColors(true);
			listbox->setColumnWidth(0, 160);
			listbox->setColumnWidth(1, 50);

			connect(listbox, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(edit_event(QTableWidgetItem*)));
			connect(listbox, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(listboxitemChanged_event(QTableWidgetItem*)));

	Q3BoxLayout * left_blayout = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "left_blayout");
	left_blayout->setAlignment(Qt::AlignTop);
    left_blayout->addWidget( opensave_grp, 0, Qt::AlignTop);
    left_blayout->addWidget( addrmv_grp, 0, Qt::AlignTop);
    left_blayout->addWidget( scale_grp, 0, Qt::AlignTop);

	Q3BoxLayout * rigth_blayout = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "rigth_blayout");
    //rigth_blayout->addWidget( labels_layout, 0, Qt::AlignTop);
    rigth_blayout->addWidget( list_grp, 0);
	
	//our transfertab top level layout
	Q3BoxLayout * isotab_blayout = new Q3BoxLayout ( this, Q3BoxLayout::LeftToRight, 10, 5, "isotab_blayout");
    isotab_blayout->addLayout( left_blayout, 1);
    isotab_blayout->addLayout( rigth_blayout, 10);
}

void QtMeasuringWidget::setData(void)
{
	emit measure_clearall_signal();

	QTableWidgetItem *newItem;
	for(int i=0; i<measure_objects.size(); i++)
	{
		listbox->setRowCount(listbox->rowCount()+1);

		newItem = new QTableWidgetItem();
		newItem->setText(QString::fromStdString(measure_objects.measure_objects[i].name));
		listbox->setItem(listbox->rowCount()-1,0, newItem);

		newItem = new QTableWidgetItem();
		newItem->setTextAlignment(Qt::AlignCenter);
		newItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		newItem->setCheckState(Qt::Checked);
		listbox->setItem(listbox->rowCount()-1,1, newItem);
		
		MeasureObject obj = measure_objects.get(i);
		emit measure_add_item_signal(obj, obj.drawmode);
	}
}
void QtMeasuringWidget::open_file(QString fn)
{
	clear_list();
	measure_objects.open_file(fn);
	setData();

	emit meausre_setuprendering_signal();
}
void QtMeasuringWidget::clear_list(void)
{
	for(int i=listbox->rowCount()-1; i>=0; i--)
	{
		listbox->removeRow(i);
	}
	listbox->clear();

	printf("listbox->rowCount(): %d\n", listbox->rowCount());
}
void QtMeasuringWidget::clear_all(void)
{
	for(int i=listbox->rowCount()-1; i>=0; i--)
	{
		listbox->removeRow(i);
	}
	listbox->clear();

	measure_objects.clear();

	measuring_planes.clear();
	//measuring_drawmodes.clear();

	current_measure_object.clear();

	emit measure_clearall_signal();
}
void QtMeasuringWidget::modifygroupbutton_event(void)
{
	if(modifygroupbutton->isOn())
	{
		//printf("Start\n");
		vertices_add_button->setOn(false);
		//vertices_add_button_event();

		emit measure_modify_start_signal();
	}
	else
	{
		//printf("Stop\n");
		emit measure_modify_stop_signal();
	}
}

void QtMeasuringWidget::open_event(void)
{
	QString fn = QFileDialog::getOpenFileName(currentworkingdir, "*.msr", this );
	open_file(fn);
}
void QtMeasuringWidget::save_file(QString fn)
{	
	SVNObject svnobj;
	measure_objects.time = time_value;
	measure_objects.scale.x = unitX/voxX;
	measure_objects.scale.y = unitY/voxY;
	measure_objects.scale.z = unitZ/voxZ;
	Vector res = Vector(volobj->texwidth, volobj->texheight, volobj->texdepth);
	measure_objects.save_file(fn, originalfilename, "VolViewer rev"+svnobj.revision, res);
}
void QtMeasuringWidget::save_event(void)
{
	QString fn = QFileDialog::getSaveFileName(currentworkingdir, "*.msr", this );
	save_file(fn);
}
void QtMeasuringWidget::listboxitemChanged_event(QTableWidgetItem* item)
{
	printf("listboxitemChanged_event\n");

	int row = item->row();
	int column = item->column();
	if(column==1)
	{
		//printf("row: %d, column: %d\n", row, column);
		//QTableWidgetItem* item = listbox->itemAt(row, column);
		
		bool checked;
		if(item->checkState()==Qt::Checked)
		{
			//printf("checked\n");
			checked = true;
		}
		else if(item->checkState()==Qt::Unchecked)
		{
			//printf("un-checked\n");
			checked = false;
		}

		emit measure_update_visible_signal(row, checked);
	}
}
void QtMeasuringWidget::measure_clearcurrent_slot(void)
{
	vertices_listbox->clear();

	current_measure_object.clear();
}
void QtMeasuringWidget::vertices_currentItemChanged_slot(QListWidgetItem* curr, QListWidgetItem* prev)
{
	int index = vertices_listbox->row(curr);
	vertices_listbox->setItemSelected(curr, true);
	//printf("vertices_currentItemChanged_slot %d\n", index);
	current_vert = index;
	emit measure_selected_vertindex_signal(index);

	computeInfo();
}
void QtMeasuringWidget::edges_currentItemChanged_slot(QListWidgetItem* curr,QListWidgetItem* prev)
{
	int index = edges_listbox->row(curr);
	edges_listbox->setItemSelected(curr, true);
	//printf("faces_currentItemChanged_slot %d\n", index);
	current_edge = index;
	emit measure_selected_edgeindex_signal(index);

	computeInfo();
}
void QtMeasuringWidget::faces_currentItemChanged_slot(QListWidgetItem* curr,QListWidgetItem* prev)
{
	int index = faces_listbox->row(curr);
	faces_listbox->setItemSelected(curr, true);
	//printf("faces_currentItemChanged_slot %d\n", index);
	current_face = index;
	emit measure_selected_faceindex_signal(index);

	computeInfo();
}
void QtMeasuringWidget::labels_currentItemChanged_slot(QListWidgetItem* curr,QListWidgetItem* prev)
{
	int index = labels_listbox->row(curr);
	labels_listbox->setItemSelected(curr, true);
	current_label = index;

	//emit measure_selected_faceindex_signal(index);
//	printf("CURRENT LABEL: %d\n", current_label);
}
void QtMeasuringWidget::labels_clicked_slot(QListWidgetItem* item)
{
	int index = labels_listbox->row(item);
	current_label = index;
//	printf("CURRENT LABEL: %d\n", current_label);
}
void QtMeasuringWidget::edges_clicked_slot(QListWidgetItem* item)
{
	int index = edges_listbox->row(item);
//	printf("Clicked\n");
//	printf("vertices_clicked_slot %d\n", index);
	current_edge = index;
	emit measure_selected_edgeindex_signal(index);
}

void QtMeasuringWidget::vertices_clicked_slot(QListWidgetItem* item)
{
	int index = vertices_listbox->row(item);
//	printf("Clicked\n");
//	printf("vertices_clicked_slot %d\n", index);
	current_vert = index;
	emit measure_selected_vertindex_signal(index);
}
void QtMeasuringWidget::faces_clicked_slot(QListWidgetItem* item)
{
	int index = faces_listbox->row(item);
//	printf("Clicked\n");
//	printf("faces_clicked_slot %d\n", index);
	current_vert = index;
	emit measure_selected_faceindex_signal(index);
}

void QtMeasuringWidget::drawmode_event(int selected)
{
	printf("DRAWMODE\n");

/*	if(selected==0)
	{
	}
	else if(selected==0)
	{
	}*/
	if(pointmode_button->isOn())
	{
		if(current_drawmode==MEASURE_DRAWMODE_LINES || current_drawmode==MEASURE_DRAWMODE_MAGNETLINE)
		{
			QMessageBox msgBox;
			msgBox.setText("Are you sure you would like to change the measuring mode?");
			msgBox.setInformativeText("The change could result in a loss of data.");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::No);
			int ret = msgBox.exec();
			if(ret!=QMessageBox::Yes)
			{
				pointmode_button->setOn(false);

				if(current_drawmode==MEASURE_DRAWMODE_LINES)
				{
					linemode_button->setOn(true);
					linemode_button->setFocus();
				}
				if(current_drawmode==MEASURE_DRAWMODE_MAGNETLINE)
				{
					magnetline_button->setOn(true);
					magnetline_button->setFocus();
				}
				return;
			}
		}

		printf("pointmode_button\n");
		pointmode_button->setFocus();
		emit measure_update_current_editmode_signal(MEASURE_EDIT_NONE);
		emit measure_pause_signal();

		vertices_add_button->setOn(false);
		edges_add_button->setOn(false);
		faces_add_button->setOn(false);

		linemode_button->setOn(false);
		magnetline_button->setOn(false);

		tabWidget->setTabEnabled(0, true);
		tabWidget->setTabEnabled(1, false);
		tabWidget->setTabEnabled(2, false);

		current_drawmode = MEASURE_DRAWMODE_POINTS;
		emit measure_update_current_drawmode_signal(MEASURE_DRAWMODE_POINTS);
	}
	else if(linemode_button->isOn())
	{
		if(current_drawmode==MEASURE_DRAWMODE_MAGNETLINE)
		{
			QMessageBox msgBox;
			msgBox.setText("Are you sure you would like to change the measuring mode?");
			msgBox.setInformativeText("The change could result in a loss of data.");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::No);
			int ret = msgBox.exec();

			if(ret!=QMessageBox::Yes)
			{
				linemode_button->setOn(false);
				if(current_drawmode==MEASURE_DRAWMODE_MAGNETLINE)
				{
					magnetline_button->setFocus();
					magnetline_button->setOn(true);
				}
				return;
			}
		}

		printf("linemode_button\n");
		linemode_button->setFocus();
		emit measure_update_current_editmode_signal(MEASURE_EDIT_NONE);
		emit measure_pause_signal();

		vertices_add_button->setOn(false);
		edges_add_button->setOn(false);
		faces_add_button->setOn(false);

		pointmode_button->setOn(false);
		magnetline_button->setOn(false);

		edges_add_button->setEnabled(false);
		edges_rmv_button->setEnabled(false);

		tabWidget->setTabEnabled(0, true);
		tabWidget->setTabEnabled(1, true);
		tabWidget->setTabEnabled(2, false);

		current_drawmode = MEASURE_DRAWMODE_LINES;
		emit measure_update_current_drawmode_signal(MEASURE_DRAWMODE_LINES);
	}
	else if(magnetline_button->isOn())
	{
		printf("magnetline_button\n");
		magnetline_button->setFocus();
		emit measure_update_current_editmode_signal(MEASURE_EDIT_NONE);
		emit measure_pause_signal();

		vertices_add_button->setOn(false);
		edges_add_button->setOn(false);
		faces_add_button->setOn(false);

		pointmode_button->setOn(false);
		linemode_button->setOn(false);

		edges_add_button->setEnabled(true);
		edges_rmv_button->setEnabled(true);

		tabWidget->setTabEnabled(0, true);
		tabWidget->setTabEnabled(1, true);
		tabWidget->setTabEnabled(2, true);

		current_drawmode = MEASURE_DRAWMODE_MAGNETLINE;
		emit measure_update_current_drawmode_signal(MEASURE_DRAWMODE_MAGNETLINE);
	}
	
	eval_length();
}
void QtMeasuringWidget::del_event(void)
{
//	if(listbox->count()<0) return;
	if(!listbox->currentItem()) return;

	switch( QMessageBox::warning(this, "Deleting...",
	"Are you sure you want to delete the selected item? \n",
	QMessageBox::Yes,
	QMessageBox::No | QMessageBox::Default )) 
	{
	case QMessageBox::Yes: 
		current_listbox_index = listbox->row(listbox->currentItem());
		
		printf("DELETE: current_listbox_index: %d\n", current_listbox_index);

		listbox->takeItem(current_listbox_index, 0);

		if(current_listbox_index<measure_objects.size()) measure_objects.erase(current_listbox_index);
		if(current_listbox_index<measuring_planes.size()) measuring_planes.erase(measuring_planes.begin()+current_listbox_index);
		listbox->removeRow(current_listbox_index);

		emit measure_delete_item_signal(current_listbox_index);
		break;
	case QMessageBox::No: // Abort or Cancel
		// abort
		break;
	}
}
void QtMeasuringWidget::create_LabelGUI(void)
{
	label_dialogue = new QDialog(this, "Label Editor", FALSE);
	Q3BoxLayout* label_layout = new Q3BoxLayout(label_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "label_layout");

		Q3ButtonGroup *name_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this);
			QLabel* name_lb = new QLabel("Name:	", name_grp);
			labelname_le = new QLineEdit("", name_grp);

		mode_grp = new Q3ButtonGroup( 3, Qt::Horizontal, "Label Type:", this);
		mode_grp->setExclusive(true);
		connect(mode_grp, SIGNAL(clicked(int)), this, SLOT(label_listmode_event(int)));

			QPushButton* vertlistbutton = new QPushButton("Vertex",	mode_grp );
			vertlistbutton->setToggleButton(true);
			vertlistbutton->setOn(true);
			
			QPushButton* edgelistbutton = new QPushButton("Edge",	mode_grp );
			edgelistbutton->setToggleButton(true);
			edgelistbutton->setOn(false);

			QPushButton* facelistbutton = new QPushButton("Face",	mode_grp );
			facelistbutton->setToggleButton(true);
			facelistbutton->setOn(false);

		Q3ButtonGroup *lists_grp = new Q3ButtonGroup( 3, Qt::Horizontal, "", this);
			
			editsrclabels_listbox = new MySrcListWidget(lists_grp);
			editsrclabels_listbox->setMaximumWidth(100);
			editsrclabels_listbox->setSelectionMode(QAbstractItemView::SingleSelection); // single item can be draged or droped
			editsrclabels_listbox->setDragEnabled(true);
			editsrclabels_listbox->viewport()->setAcceptDrops(true);
			editsrclabels_listbox->setDropIndicatorShown(true);

			QLabel* dnd_lb = new QLabel("Drag and Drop\n<- ->", lists_grp);
			dnd_lb->setAlignment(Qt::AlignCenter);
			//dnd_lb->setSizePolicy
			
			editdstlabels_listbox = new MyDstListWidget(lists_grp);
			editdstlabels_listbox->setMaximumWidth(100);
			editdstlabels_listbox->setSelectionMode(QAbstractItemView::SingleSelection);
			//editdstlabels_listbox->setDragDropMode(QAbstractItemView::InternalMove);
			editdstlabels_listbox->setDragEnabled(true);
			editdstlabels_listbox->viewport()->setAcceptDrops(true); 
			//editdstlabels_listbox->setDefaultDropAction(Qt::LinkAction);
			editdstlabels_listbox->setDropIndicatorShown(true);

//			connect(editsrclabels_listbox, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(editlabels_clicked_slot(QListWidgetItem*)) );
//			connect(editdstlabels_listbox, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(editlabels_clicked_slot(QListWidgetItem*)) );
			connect(editsrclabels_listbox, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(editlabels_currentItemChanged_slot(QListWidgetItem*,QListWidgetItem*)) );

	labelokbutton		= new QPushButton("OK",	this);
	labelcancelbutton		= new QPushButton("Cancel",	this);
	
	label_layout->addWidget(name_grp);
	label_layout->addWidget(mode_grp);
	label_layout->addWidget(lists_grp);
	label_layout->addWidget(labelokbutton);
	label_layout->addWidget(labelcancelbutton);
}
void QtMeasuringWidget::updateGUI_withCurrentLabel(void)
{
}
void QtMeasuringWidget::editlabels_currentItemChanged_slot(QListWidgetItem* curr, QListWidgetItem* prev)
{
	if(editlabels_mode==0)
	{
		//VERTS
		int index = editsrclabels_listbox->row(curr);
		editsrclabels_listbox->setItemSelected(curr, true);

		//curr->setFlags(Qt::NoItemFlags);

		//printf("vertices_currentItemChanged_slot %d\n", index);
		current_vert = index;
		emit measure_selected_vertindex_signal(index);
	}
	else if(editlabels_mode==1)
	{
		//EDGES
		int index = editsrclabels_listbox->row(curr);
		editsrclabels_listbox->setItemSelected(curr, true);
		//printf("vertices_currentItemChanged_slot %d\n", index);
		current_edge = index;
		emit measure_selected_edgeindex_signal(index);
	}
	else
	{
		//FACES
		int index = editsrclabels_listbox->row(curr);
		editsrclabels_listbox->setItemSelected(curr, true);
		//printf("vertices_currentItemChanged_slot %d\n", index);
		current_face = index;
		emit measure_selected_faceindex_signal(index);
	}
}
void QtMeasuringWidget::label_listmode_event(int id)
{
//	if(editsrclabels_listbox==0) return;

	editlabels_mode = id;

	QString numb;
	if(id==0)
	{
		if(editdstlabels_listbox) editdstlabels_listbox->clear();

		if(!current_measure_object.vertices.empty())
		{

			if(editsrclabels_listbox)
			{
				editsrclabels_listbox->clear();
				QString vert_string;
				for(int i=0; i<current_measure_object.vertices.size(); i++)
				{
					numb.setNum(i);
					vert_string = "p"+numb;
					editsrclabels_listbox->insertItem(i, vert_string);
				}
			}
		}
		else
		{
			if(editsrclabels_listbox) editsrclabels_listbox->clear();
		}
	}
	else if(id==1)
	{
		if(editdstlabels_listbox) editdstlabels_listbox->clear();

		if(!current_measure_object.edges.empty())
		{
			if(editsrclabels_listbox)
			{
				editsrclabels_listbox->clear();
				QString edge_string;
				for(int i=0; i<current_measure_object.edges.size(); i++)
				{
					numb.setNum(i);

					edge_string = "e" + numb + "\n   (";			
					numb.setNum(current_measure_object.edges[i].v0);
					edge_string += "p" + numb + ",";
					numb.setNum(current_measure_object.edges[i].v1);
					edge_string += "p" + numb + ")";

					editsrclabels_listbox->insertItem(i, edge_string);
				}
			}
		}
		else
		{
			if(editsrclabels_listbox) editsrclabels_listbox->clear();
		}
	}
	else
	{
		if(editdstlabels_listbox) editdstlabels_listbox->clear();

		if(!current_measure_object.faces.empty())
		{
			if(editsrclabels_listbox)
			{
				editsrclabels_listbox->clear();
				QString face_string;
				for(int i=0; i<current_measure_object.faces.size(); i++)
				{
					numb.setNum(i);

					face_string = "f" + numb + "\n   (";
					for(int j=0; j<current_measure_object.faces[i].size(); j++)
					{
						numb.setNum(current_measure_object.faces[i][j]);
						face_string += "p" + numb;

						if(j!=current_measure_object.faces[i].size()-1)
						face_string +=",";
					}
					face_string+=")";

					editsrclabels_listbox->insertItem(i, face_string);
				}
			}
		}	
		else
		{
			if(editsrclabels_listbox) editsrclabels_listbox->clear();
		}
	}
}

void QtMeasuringWidget::create_MeasureGUI(void)
{
	add_dialogue = new QDialog(this, "Measurement Editor", FALSE);
	Q3BoxLayout* addmeasure_layout = new Q3BoxLayout(add_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "add_dialogue_layout");

		Q3ButtonGroup *name_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this);
			QLabel* name_lb = new QLabel("Name:	", name_grp);
			name_le = new QLineEdit("", name_grp);

		Q3ButtonGroup *measureobj_layout = new Q3ButtonGroup( 3, Qt::Horizontal, "", this);

			Q3ButtonGroup *measuremode_layout = new Q3ButtonGroup(6, Qt::Horizontal, "Measuring Mode: ", measureobj_layout);
			measuremode_layout->setExclusive(true);
			connect(measuremode_layout, SIGNAL( clicked (int) ), this, SLOT( drawmode_event(int) ) );
			 
				QPixmap pointsIcon = QPixmap( apppath+"/Icons/points.png" );
				pointmode_button		= new QPushButton(pointsIcon, "",	measuremode_layout);
				pointmode_button->setToolTip("Points");
				pointmode_button->setToggleButton(true);
				pointmode_button->setOn(false);
				pointmode_button->setFixedSize(66, 66);
				pointmode_button->setIconSize(QSize(64,64));
				//connect(pointmode_button, SIGNAL( clicked () ), this, SLOT( drawmode_event() ) );

				QPixmap linesIcon = QPixmap( apppath+"/Icons/lines.png" );
				linemode_button		= new QPushButton(linesIcon, "",	measuremode_layout);
				linemode_button->setToolTip("Line(s)");
				linemode_button->setToggleButton(true);
				linemode_button->setOn(false);
				linemode_button->setFixedSize(66, 66);
				linemode_button->setIconSize(QSize(64,64));
				//connect(linemode_button, SIGNAL( clicked () ), this, SLOT( drawmode_event() ) );

				QPixmap magnetlineIcon = QPixmap( apppath+"/Icons/facebuilder.png" ); //magneticlines.png
				magnetline_button		= new QPushButton(magnetlineIcon, "",	measuremode_layout);
				magnetline_button->setToolTip("Magnetic Lines");
				magnetline_button->setToggleButton(true);
				magnetline_button->setOn(false);
				magnetline_button->setFixedSize(66, 66);
				magnetline_button->setIconSize(QSize(64,64));
				//connect(magnetline_button, SIGNAL( clicked () ), this, SLOT( drawmode_event() ) );

			Q3ButtonGroup *modify_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Transform: ", measureobj_layout);
				modifygroupbutton		= new QPushButton("Transform Group",	modify_layout );
				modifygroupbutton->setToolTip("Translate, Rotate and Scale the group.");
				modifygroupbutton->setToggleButton(true);
				modifygroupbutton->setOn(false);
				modifygroupbutton->setFixedHeight(66);
				connect(modifygroupbutton, SIGNAL( clicked () ), this, SLOT( modifygroupbutton_event() ) );
				if(!modifyshow_toggle) modify_layout->hide();

			build_verticesTab();
			build_edgesTab();
			build_facesTab();
			build_labelsTab();
			build_listsTab();

			tabWidget = new QTabWidget(this);
			tabWidget->addTab(vertices_tablayout, "Vertices");
			tabWidget->addTab(edges_tablayout, "Edges");
			tabWidget->addTab(faces_tablayout, "Faces");
			tabWidget->addTab(labels_tablayout, "Labels");
//			tabWidget->addTab(lists_tablayout, "Lists");
					
			tabWidget->setTabEnabled(0, true);
			tabWidget->setTabEnabled(1, false);
			tabWidget->setTabEnabled(2, false);
			tabWidget->setTabEnabled(3, true);
//			tabWidget->setTabEnabled(4, true);
			
			lists_tablayout->hide();

			emit measure_update_current_viewmode_signal(MEASURE_VIEW_VERTS);
			connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tab_currentChanged_slot(int)));

	okbutton		= new QPushButton("OK",	this);
	cancelbutton		= new QPushButton("Cancel",	this);
	
	addmeasure_layout->addWidget(name_grp);
	addmeasure_layout->addWidget(measureobj_layout);
	addmeasure_layout->addWidget(tabWidget);	
	addmeasure_layout->addWidget(okbutton);
	addmeasure_layout->addWidget(cancelbutton);
}
void QtMeasuringWidget::build_verticesTab(void)
{
	vertices_tablayout = new Q3ButtonGroup( 1, Qt::Vertical, "", this);
	vertices_tablayout->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
	vertices_tablayout->setLineWidth(0);
	vertices_tablayout->setMidLineWidth(0);
	vertices_tablayout->setMargin(0);
	vertices_tablayout->setInsideMargin(5);
	vertices_tablayout->setInsideSpacing(5);
	
		Q3ButtonGroup *vertices_layout = new Q3ButtonGroup( 2, Qt::Vertical, "", vertices_tablayout);
		vertices_layout->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
		vertices_layout->setMaximumWidth(100);

			vertices_listbox = new QListWidget(vertices_layout);
			vertices_listbox->setMaximumWidth(100);
			//vertices_listbox->setDragDropMode(QAbstractItemView::InternalMove);
			connect(vertices_listbox, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(vertices_clicked_slot(QListWidgetItem*)) );
			connect(vertices_listbox, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(vertices_currentItemChanged_slot(QListWidgetItem*,QListWidgetItem*)) );

		Q3ButtonGroup* verticesright_grp = new Q3ButtonGroup( 8, Qt::Vertical, "", vertices_tablayout);
		verticesright_grp->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
		verticesright_grp->setLineWidth(0);
		verticesright_grp->setMidLineWidth(0);
		verticesright_grp->setMargin(0);
		verticesright_grp->setInsideMargin(0);
		verticesright_grp->setInsideSpacing(5);

			Q3ButtonGroup* addexprt_grp = new Q3ButtonGroup( 8, Qt::Horizontal, "", verticesright_grp);
			addexprt_grp->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
			addexprt_grp->setLineWidth(0);
			addexprt_grp->setMidLineWidth(0);
			addexprt_grp->setMargin(0);
			addexprt_grp->setInsideMargin(0);
			addexprt_grp->setInsideSpacing(5);

				Q3ButtonGroup *addrmv_layout = new Q3ButtonGroup( 2, Qt::Horizontal, "Add/Del: ", addexprt_grp);
				//QBoxLayout* addrmv_layout = new QBoxLayout(QBoxLayout::LeftToRight, verticesright_grp);

					QPixmap addIcon = QPixmap( apppath+"/Icons/add.png" );
					vertices_add_button		= new QPushButton(addIcon, "",	addrmv_layout );
					vertices_add_button->setToolTip("Add Items");
					vertices_add_button->setToggleButton(true);
					vertices_add_button->setOn(false);
					vertices_add_button->setFixedSize(48,48);
					vertices_add_button->setIconSize(QSize(32,32));
					connect(vertices_add_button, SIGNAL( clicked () ), this, SLOT( vertices_add_button_event() ) );

					QPixmap removeIcon = QPixmap( apppath+"/Icons/remove.png" );
					vertices_rmv_button		= new QPushButton(removeIcon, "",	addrmv_layout);
					vertices_rmv_button->setToolTip("Delete Items");
					vertices_rmv_button->setToggleButton(false);
					vertices_rmv_button->setOn(false);
					vertices_rmv_button->setFixedSize(48,48);
					vertices_rmv_button->setIconSize(QSize(32,32));
					connect(vertices_rmv_button, SIGNAL( clicked () ), this, SLOT( vertices_rmv_button_event() ) );
				
				Q3ButtonGroup* export_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Export:", addexprt_grp );
				export_grp->setExclusive( FALSE );

					QPushButton* exportbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/excel.png")), "", export_grp, "open");
					exportbtn->setIconSize(QSize(32,32));
					exportbtn->setFixedSize(48,48);
					connect( exportbtn, SIGNAL( clicked () ), this, SLOT( export_event() ) );
	

				Q3ButtonGroup *labels_layout = new Q3ButtonGroup( 3, Qt::Horizontal, "View:", verticesright_grp);
					vert_cb = new QCheckBox("Indices",	 labels_layout );
					connect( vert_cb,	 SIGNAL( clicked() ), this, SLOT( vert_cb_event() ) );
					vert_cb->setOn(FALSE);
					vert_cb_event();

				Q3ButtonGroup *verticesselected_layout = new Q3ButtonGroup( 4, Qt::Horizontal, "Selected: ", verticesright_grp);
				//verticesselected_layout->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

					QLabel* selname_lb = new QLabel("[x, y, z]:", verticesselected_layout);
					selname_lb->setFixedWidth(50);
					vertices_curX = new QLineEdit("", verticesselected_layout);
					vertices_curX->setFixedWidth(50);
					vertices_curX->setReadOnly(true);
					vertices_curY = new QLineEdit("", verticesselected_layout);
					vertices_curY->setReadOnly(true);
					vertices_curY->setFixedWidth(50);
					vertices_curZ = new QLineEdit("", verticesselected_layout);
					vertices_curZ->setReadOnly(true);
					vertices_curZ->setFixedWidth(50);
					QLabel* selrgb_lb = new QLabel("[r, g, b]:", verticesselected_layout);
					vertices_curR = new QLineEdit("", verticesselected_layout);
					vertices_curR->setReadOnly(true);
					vertices_curR->setFixedWidth(50);
					vertices_curG = new QLineEdit("", verticesselected_layout);
					vertices_curG->setReadOnly(true);
					vertices_curG->setFixedWidth(50);
					vertices_curB = new QLineEdit("", verticesselected_layout);
					vertices_curB->setReadOnly(true);
					vertices_curB->setFixedWidth(50);
				
				Q3ButtonGroup *verticesinfo_layout = new Q3ButtonGroup( 1, Qt::Horizontal, "Info: ", verticesright_grp);

					Q3ButtonGroup *verticescount_layout = new Q3ButtonGroup(2, Qt::Horizontal, "", verticesinfo_layout);
					verticescount_layout->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
					verticescount_layout->setLineWidth(0);
					verticescount_layout->setMidLineWidth(0);
					verticescount_layout->setMargin(0);
					verticescount_layout->setInsideMargin(0);
					verticescount_layout->setInsideSpacing(5);

						QLabel* name_lb = new QLabel("Count of Vertices:", verticescount_layout);
						//name_lb->setFixedWidth(85);
						vertices_count = new QLineEdit("", verticescount_layout);
						vertices_count->setValidator(new QIntValidator(this));
						//vertices_count->setFixedWidth(50);
						vertices_count->setReadOnly(true);

				Q3ButtonGroup *verticesmean_layout = new Q3ButtonGroup( 4, Qt::Horizontal, "Mean:", verticesinfo_layout);
					QLabel* meanvert_lb = new QLabel("(x,y,z):", verticesmean_layout);
					vertices_meanX = new QLineEdit("", verticesmean_layout);
					vertices_meanX->setReadOnly(true);
					vertices_meanY = new QLineEdit("", verticesmean_layout);
					vertices_meanY->setReadOnly(true);
					vertices_meanZ = new QLineEdit("", verticesmean_layout);
					vertices_meanZ->setReadOnly(true);
					QLabel* meanrgb_lb = new QLabel("(r,g,b):", verticesmean_layout);
					vertices_meanR = new QLineEdit("", verticesmean_layout);
					vertices_meanR->setReadOnly(true);
					vertices_meanG = new QLineEdit("", verticesmean_layout);
					vertices_meanG->setReadOnly(true);
					vertices_meanB = new QLineEdit("", verticesmean_layout);
					vertices_meanB->setReadOnly(true);
				verticesmean_layout->hide();
}
void QtMeasuringWidget::build_edgesTab(void)
{
	edges_tablayout = new Q3ButtonGroup( 1, Qt::Vertical, "", this);
	edges_tablayout->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
	edges_tablayout->setLineWidth(0);
	edges_tablayout->setMidLineWidth(0);
	edges_tablayout->setMargin(0);
	edges_tablayout->setInsideMargin(5);
	edges_tablayout->setInsideSpacing(5);
	
		Q3ButtonGroup *edges_layout = new Q3ButtonGroup( 2, Qt::Vertical, "", edges_tablayout);
		edges_layout->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
		edges_layout->setMaximumWidth(150);

			edges_listbox = new QListWidget(edges_layout);
			edges_listbox->setMaximumWidth(150);
			//vertices_listbox->setDragDropMode(QAbstractItemView::InternalMove);
			connect(edges_listbox, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(edges_clicked_slot(QListWidgetItem*)) );
			connect(edges_listbox, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(edges_currentItemChanged_slot(QListWidgetItem*,QListWidgetItem*)) );

		Q3ButtonGroup* edgesright_grp = new Q3ButtonGroup( 8, Qt::Vertical, "", edges_tablayout);
		edgesright_grp->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
		edgesright_grp->setLineWidth(0);
		edgesright_grp->setMidLineWidth(0);
		edgesright_grp->setMargin(0);
		edgesright_grp->setInsideMargin(0);
		edgesright_grp->setInsideSpacing(5);

			Q3ButtonGroup* addexprt_grp = new Q3ButtonGroup( 8, Qt::Horizontal, "", edgesright_grp);
			addexprt_grp->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
			addexprt_grp->setLineWidth(0);
			addexprt_grp->setMidLineWidth(0);
			addexprt_grp->setMargin(0);
			addexprt_grp->setInsideMargin(0);
			addexprt_grp->setInsideSpacing(5);

				Q3ButtonGroup *addrmv_layout = new Q3ButtonGroup( 2, Qt::Horizontal, "Add/Del: ", addexprt_grp);

					QPixmap addIcon = QPixmap( apppath+"/Icons/add.png" );
					edges_add_button		= new QPushButton(addIcon, "",	addrmv_layout );
					edges_add_button->setToolTip("Add Items");
					edges_add_button->setToggleButton(true);
					edges_add_button->setOn(false);
					edges_add_button->setFixedSize(48,48);
					edges_add_button->setIconSize(QSize(32,32));
					connect(edges_add_button, SIGNAL( clicked () ), this, SLOT( edges_add_button_event() ) );

					QPixmap removeIcon = QPixmap( apppath+"/Icons/remove.png" );
					edges_rmv_button		= new QPushButton(removeIcon, "",	addrmv_layout);
					edges_rmv_button->setToolTip("Delete Items");
					edges_rmv_button->setToggleButton(false);
					edges_rmv_button->setOn(false);
					edges_rmv_button->setFixedSize(48,48);
					edges_rmv_button->setIconSize(QSize(32,32));
					connect(edges_rmv_button, SIGNAL( clicked () ), this, SLOT( edges_rmv_button_event() ) );
				
				Q3ButtonGroup* export_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Export:", addexprt_grp );
				export_grp->setExclusive( FALSE );

					QPushButton* exportbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/excel.png")), "", export_grp, "open");
					exportbtn->setIconSize(QSize(32,32));
					exportbtn->setFixedSize(48,48);
					connect( exportbtn, SIGNAL( clicked () ), this, SLOT( export_event() ) );

			Q3ButtonGroup *planes_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Edge Clip-Plane: ", edgesright_grp);

				//QPixmap addIcon = QPixmap( apppath+"/Icons/add.png" );
				resetplanesbutton		= new QPushButton("Reset\n Planes",	planes_layout );
				resetplanesbutton->setToolTip("Reset the clipping planes");
				//resetplanesbutton->setToggleButton(true);
				//resetplanesbutton->setOn(true);
				//resetplanesbutton->setFixedSize(48,48);
				//resetplanesbutton->setIconSize(QSize(32,32));
				connect(resetplanesbutton, SIGNAL( clicked () ), this, SLOT( resetplanesbutton_event() ) );
				
				Q3ButtonGroup *checkbox_layout = new Q3ButtonGroup( 1, Qt::Horizontal, "", planes_layout);
					solidplanescheckbox		= new QCheckBox("Solid Plane",	checkbox_layout );
					solidplanescheckbox->setToolTip("Set the clipping plane to solid.");
					solidplanescheckbox->setOn(false);
					connect(solidplanescheckbox, SIGNAL( clicked () ), this, SLOT( solidplanescheckbox_event() ) );
					
					flipplanescheckbox		= new QCheckBox("Flip Plane",	checkbox_layout );
					flipplanescheckbox->setToolTip("Flip the clipping plane direction.");
					flipplanescheckbox->setOn(false);
					connect(flipplanescheckbox, SIGNAL( clicked () ), this, SLOT( flipplanescheckbox_event() ) );
			

			Q3ButtonGroup *labels_layout = new Q3ButtonGroup( 3, Qt::Horizontal, "View:", edgesright_grp);
				edge_cb = new QCheckBox("Indices", labels_layout );
				connect( edge_cb,	 SIGNAL( clicked() ), this, SLOT( edge_cb_event() ) );
				edge_cb->setOn(FALSE);
				edge_cb_event();

				arrow_cb = new QCheckBox("Arrow Heads",   labels_layout );
				connect( arrow_cb,	 SIGNAL( clicked() ), this, SLOT( arrow_cb_event() ) );
				arrow_cb->setOn(FALSE);
				arrow_cb_event();

			Q3ButtonGroup *edgessel_layout = new Q3ButtonGroup( 2, Qt::Horizontal, "Selected: ", edgesright_grp);
				QLabel* current_lb = new QLabel("Length:", edgessel_layout);
				edges_cur = new QLineEdit("", edgessel_layout);
				edges_cur->setValidator(new QIntValidator(this));
				edges_cur->setMaxLength(21);
				edges_cur->setReadOnly(true);


			Q3ButtonGroup *edgesinfo_layout = new Q3ButtonGroup( 2, Qt::Horizontal, "Info: ", edgesright_grp);
				QLabel* name_lb = new QLabel("Count of Edges:", edgesinfo_layout);
				edges_count = new QLineEdit("", edgesinfo_layout);
				edges_count->setValidator(new QIntValidator(this));
				edges_count->setMaxLength(21);
				edges_count->setReadOnly(true);

				QLabel* sum_lb = new QLabel("Sum of Lengths:", edgesinfo_layout);
				edges_sum = new QLineEdit("", edgesinfo_layout);
				edges_sum->setMaxLength(21);

			Q3ButtonGroup *edgesinfo_layout2 = new Q3ButtonGroup( 2, Qt::Horizontal, "Info: ", edgesright_grp);

				QLabel* min_lb = new QLabel("Min:", edgesinfo_layout2);
				edges_min = new QLineEdit("", edgesinfo_layout2);
				edges_min->setMaxLength(21);

				QLabel* max_lb = new QLabel("Max:", edgesinfo_layout2);
				edges_max = new QLineEdit("", edgesinfo_layout2);
				edges_max->setMaxLength(21);
				edgesinfo_layout2->hide();

	computeInfo();
}
void QtMeasuringWidget::build_facesTab(void)
{
	faces_tablayout = new Q3ButtonGroup( 1, Qt::Vertical, "", this);
	faces_tablayout->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
	faces_tablayout->setLineWidth(0);
	faces_tablayout->setMidLineWidth(0);
	faces_tablayout->setMargin(0);
	faces_tablayout->setInsideMargin(5);
	faces_tablayout->setInsideSpacing(5);
		
		Q3ButtonGroup *faces_layout = new Q3ButtonGroup( 2, Qt::Vertical, "", faces_tablayout);
		faces_layout->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
		faces_layout->setMaximumWidth(200);

			faces_listbox = new QListWidget(faces_layout);
			faces_listbox->setMaximumWidth(200);
			connect(faces_listbox, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(faces_currentItemChanged_slot(QListWidgetItem*,QListWidgetItem*)) );
			connect(faces_listbox, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(faces_clicked_slot(QListWidgetItem*)) );

		Q3ButtonGroup* facesright_grp = new Q3ButtonGroup( 8, Qt::Vertical, "", faces_tablayout);
		facesright_grp->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
		facesright_grp->setLineWidth(0);
		facesright_grp->setMidLineWidth(0);
		facesright_grp->setMargin(0);
		facesright_grp->setInsideMargin(0);
		facesright_grp->setInsideSpacing(5);

			Q3ButtonGroup* addexprt_grp = new Q3ButtonGroup( 8, Qt::Horizontal, "", facesright_grp);
			addexprt_grp->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
			addexprt_grp->setLineWidth(0);
			addexprt_grp->setMidLineWidth(0);
			addexprt_grp->setMargin(0);
			addexprt_grp->setInsideMargin(0);
			addexprt_grp->setInsideSpacing(5);

				Q3ButtonGroup *addrmv_layout = new Q3ButtonGroup( 2, Qt::Horizontal, "Add/Del: ", addexprt_grp);

					QPixmap addIcon = QPixmap( apppath+"/Icons/add.png" );
					faces_add_button		= new QPushButton(addIcon, "",	addrmv_layout );
					faces_add_button->setToolTip("Add Items");
					faces_add_button->setToggleButton(true);
					faces_add_button->setOn(false);
					faces_add_button->setFixedSize(48,48);
					faces_add_button->setIconSize(QSize(32,32));
					connect(faces_add_button, SIGNAL( clicked () ), this, SLOT( faces_add_button_event() ) );

					QPixmap removeIcon = QPixmap( apppath+"/Icons/remove.png" );
					faces_rmv_button		= new QPushButton(removeIcon, "",	addrmv_layout);
					faces_rmv_button->setToolTip("Delete Items");
					faces_rmv_button->setToggleButton(false);
					faces_rmv_button->setOn(false);
					faces_rmv_button->setFixedSize(48,48);
					faces_rmv_button->setIconSize(QSize(32,32));
					connect(faces_rmv_button, SIGNAL( clicked () ), this, SLOT( faces_rmv_button_event() ) );

				Q3ButtonGroup* export_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Export:", addexprt_grp );
				export_grp->setExclusive( FALSE );

					QPushButton* exportbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/excel.png")), "", export_grp, "open");
					exportbtn->setIconSize(QSize(32,32));
					exportbtn->setFixedSize(48,48);
					connect( exportbtn, SIGNAL( clicked () ), this, SLOT( export_event() ) );
							
			Q3ButtonGroup *faceactions_layout = new Q3ButtonGroup( 2, Qt::Vertical, "Align Face-Plane: ", facesright_grp);

				Q3ButtonGroup *faceactions_cb_layout = new Q3ButtonGroup( 2, Qt::Horizontal, "", faceactions_layout);

					sectiontoggle__cb = new QCheckBox("Flip Orientation",   faceactions_cb_layout );
					//sectiontoggle__cb->setToolTip("Switch the view direction");
					sectiontoggle__cb->setOn(FALSE);
					connect( sectiontoggle__cb,	 SIGNAL( clicked() ), this, SLOT( facealign_orient_button_event() ) );

				
					sectionupdateview__cb = new QCheckBox("Update View",   faceactions_cb_layout );
					sectionupdateview__cb->setOn(TRUE);
					connect( sectionupdateview__cb,	 SIGNAL( clicked() ), this, SLOT( facealign_updateview_event() ) );

				Q3ButtonGroup *sectionactions_layout = new Q3ButtonGroup( 2, Qt::Horizontal, "Section: ", faceactions_layout);

					Q3ButtonGroup *sectionactionsbuts_layout = new Q3ButtonGroup( 2, Qt::Vertical, "", sectionactions_layout);
						sectiontoggle_button		= new QPushButton("\nON/OFF\n",	sectionactionsbuts_layout );
						sectiontoggle_button->setToolTip("Toggle section tool");
						sectiontoggle_button->setToggleButton(true);
						sectiontoggle_button->setOn(false);
						connect(sectiontoggle_button, SIGNAL( clicked () ), this, SLOT( sectiontoggle_button_event() ) );			

						resetplanesbutton		= new QPushButton("Reset\n Planes",	sectionactionsbuts_layout );
						resetplanesbutton->setToolTip("Reset the clipping planes");
						//resetplanesbutton->setToggleButton(true);
						//resetplanesbutton->setOn(true);
						//resetplanesbutton->setFixedSize(48,48);
						//resetplanesbutton->setIconSize(QSize(32,32));
						connect(resetplanesbutton, SIGNAL( clicked () ), this, SLOT( resetplanesbutton_event() ) );

					section_slider_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", sectionactions_layout );
						section_thickn_lb = new QLabel( "Thickness: 1", section_slider_grp );
						section_thickn_slider = new QSlider ( 1, 100, 1, 0, Qt::Horizontal, section_slider_grp, "section_thickn_slider" );
						section_thickn_slider->setTickmarks( QSlider::Below );
						section_thickn_slider->setTracking(FALSE);
						section_thickn_slider->setTickInterval(10);
						connect(section_thickn_slider, SIGNAL(valueChanged(int)), this, SLOT(section_thickn_event(int)) );
						section_thickn_event(10);

						section_trans_lb = new QLabel( "Translation: 0", section_slider_grp );
						section_trans_slider = new QSlider ( -100, 100, 1, 0, Qt::Horizontal, section_slider_grp, "section_trans_slider" );
						section_trans_slider->setTickmarks( QSlider::Below );
						section_trans_slider->setTracking(FALSE);
						section_trans_slider->setTickInterval(20);
						connect(section_trans_slider, SIGNAL(valueChanged(int)), this, SLOT(section_trans_event(int)) );
						section_trans_event(0);

					section_slider_grp->setEnabled(false);

			Q3ButtonGroup *labels_layout = new Q3ButtonGroup( 3, Qt::Horizontal, "View:", facesright_grp);
				
					face_cb = new QCheckBox("Indices",   labels_layout );
					connect( face_cb,	 SIGNAL( clicked() ), this, SLOT( face_cb_event() ) );
					face_cb->setOn(FALSE);
					face_cb_event();

					tris_cb = new QCheckBox("Solid Faces",   labels_layout );
					connect( tris_cb,	 SIGNAL( clicked() ), this, SLOT( tris_cb_event() ) );
					tris_cb->setOn(true);
					tris_cb_event();

			Q3ButtonGroup *facessel_layout = new Q3ButtonGroup( 2, Qt::Horizontal, "Selected: ", facesright_grp);
				QLabel* currperim_lb = new QLabel("Perimeter:", facessel_layout);
				faces_curperim = new QLineEdit("", facessel_layout);
				faces_curperim->setValidator(new QIntValidator(this));
				faces_curperim->setMaxLength(21);
				faces_curperim->setReadOnly(true);

				QLabel* currarea_lb = new QLabel("Area:", facessel_layout);
				faces_curarea = new QLineEdit("", facessel_layout);
				faces_curarea->setValidator(new QIntValidator(this));
				faces_curarea->setMaxLength(21);
				faces_curarea->setReadOnly(true);

			Q3ButtonGroup *facesinfo_layout = new Q3ButtonGroup( 2, Qt::Horizontal, "Info: ", facesright_grp);
				QLabel* name_lb = new QLabel("Count of Faces:", facesinfo_layout);
					faces_count = new QLineEdit("", facesinfo_layout);
					faces_count->setValidator(new QIntValidator(this));
					faces_count->setMaxLength(21);
					faces_count->setReadOnly(true);

			QLabel* sumarea_lb = new QLabel("Sum of Area:", facesinfo_layout);
				faces_sumarea = new QLineEdit("", facesinfo_layout);
				faces_sumarea->setValidator(new QIntValidator(this));
				faces_sumarea->setMaxLength(21);
				faces_sumarea->setReadOnly(true);

}
void QtMeasuringWidget::build_labelsTab(void)
{
	labels_tablayout = new Q3ButtonGroup( 1, Qt::Vertical, "", this);
	labels_tablayout->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
	labels_tablayout->setLineWidth(0);
	labels_tablayout->setMidLineWidth(0);
	labels_tablayout->setMargin(0);
	labels_tablayout->setInsideMargin(5);
	labels_tablayout->setInsideSpacing(5);

		Q3ButtonGroup *labels_layout = new Q3ButtonGroup( 2, Qt::Vertical, "", labels_tablayout);
		labels_layout->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
		labels_layout->setMaximumWidth(200);

			labels_listbox = new QListWidget(labels_layout);
			labels_listbox->setMaximumWidth(200);
			connect(labels_listbox, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(labels_clicked_slot(QListWidgetItem*)) );
			connect(labels_listbox, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(labels_currentItemChanged_slot(QListWidgetItem*,QListWidgetItem*)) );
			connect(labels_listbox, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(labels_doubleclicked_slot(const QModelIndex&)) );

		Q3ButtonGroup* labelsright_grp = new Q3ButtonGroup( 8, Qt::Vertical, "", labels_tablayout);
		labelsright_grp->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
		labelsright_grp->setLineWidth(0);
		labelsright_grp->setMidLineWidth(0);
		labelsright_grp->setMargin(0);
		labelsright_grp->setInsideMargin(0);
		labelsright_grp->setInsideSpacing(5);

			Q3ButtonGroup *addrmv_layout = new Q3ButtonGroup( 2, Qt::Horizontal, "Add/Del: ", labelsright_grp);

				QPixmap addIcon = QPixmap( apppath+"/Icons/add.png" );
				labels_add_button		= new QPushButton(addIcon, "",	addrmv_layout );
				labels_add_button->setToolTip("Add Items");
				labels_add_button->setToggleButton(false);
				labels_add_button->setOn(false);
				labels_add_button->setFixedSize(48,48);
				labels_add_button->setIconSize(QSize(32,32));
				connect(labels_add_button, SIGNAL( clicked () ), this, SLOT( labels_add_button_event() ) );

				QPixmap removeIcon = QPixmap( apppath+"/Icons/remove.png" );
				labels_rmv_button		= new QPushButton(removeIcon, "",	addrmv_layout);
				labels_rmv_button->setToolTip("Delete Items");
				labels_rmv_button->setToggleButton(false);
				labels_rmv_button->setOn(false);
				labels_rmv_button->setFixedSize(48,48);
				labels_rmv_button->setIconSize(QSize(32,32));
				connect(labels_rmv_button, SIGNAL( clicked () ), this, SLOT( labels_rmv_button_event() ) );

		Q3ButtonGroup *view_layout = new Q3ButtonGroup( 3, Qt::Horizontal, "View:", labelsright_grp);
				
				labels_cb = new QCheckBox("Labels",   view_layout );
				connect( labels_cb,	 SIGNAL( clicked() ), this, SLOT( labels_cb_event() ) );
				labels_cb->setOn(TRUE);
				labels_cb_event();

}
void QtMeasuringWidget::build_listsTab(void)
{
	lists_tablayout = new Q3ButtonGroup( 1, Qt::Vertical, "", this);
	lists_tablayout->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
	lists_tablayout->setLineWidth(0);
	lists_tablayout->setMidLineWidth(0);
	lists_tablayout->setMargin(0);
	lists_tablayout->setInsideMargin(5);
	lists_tablayout->setInsideSpacing(5);
}
void QtMeasuringWidget::facealign_orient_button_event(void)
{
	if(sectiontoggle__cb->isChecked()) emit measure_facealign_orient_signal(true);
	else emit measure_facealign_orient_signal(false);
}
void QtMeasuringWidget::facealign_updateview_event(void)
{
	if(sectionupdateview__cb->isChecked()) emit measure_facealign_updateview_signal(true);
	else emit measure_facealign_updateview_signal(false);
}

void QtMeasuringWidget::sectiontoggle_button_event(void)
{
	if(sectiontoggle_button->isOn())
	{
		emit measure_section_toggle_signal(true);
		section_slider_grp->setEnabled(true);
	}
	else
	{
		emit measure_section_toggle_signal(false);
		section_slider_grp->setEnabled(false);
	}
}
void QtMeasuringWidget::section_thickn_event(int value)
{
	section_thickn_slider->setValue(value);
	section_thickn = value;

	//update our slider label
	QString s;
	QString text;
	text = "Thickness: ";
	s.setNum(section_thickn);
	text += s;
	section_thickn_lb->setText (text);

	emit measure_section_thickn_signal(section_thickn);
}
void QtMeasuringWidget::section_trans_event(int value)
{

	section_trans_slider->setValue(value);
	section_trans=value;

	//update our slider label
	QString s;
	QString text;
	text = "Translation: ";
	s.setNum(section_trans);
	text += s;
	section_trans_lb->setText (text);

	emit measure_section_trans_signal(section_trans);
}
void QtMeasuringWidget::add_event(void)
{
	if(!volobj->texture3d) return;

	printf("add_event\n");

	//============================
	//CREATE GUI
	//============================
	create_MeasureGUI();

	//============================
	//UPDATE GUI
	//============================
	current_drawmode = MEASURE_DRAWMODE_POINTS;
	current_viewmode = MEASURE_VIEW_VERTS;
	emit measure_update_current_drawmode_signal(MEASURE_DRAWMODE_POINTS);

	QString objstring, numb;
	numb.setNum(measure_objects.size());
	objstring = "object"+numb;
	name_le->setText(objstring);

	connect(okbutton, SIGNAL( clicked () ), this, SLOT( okaddmeasurebutton_event() ) );
	connect(cancelbutton, SIGNAL( clicked () ), this, SLOT( cancbutton_event() ) );
	connect(add_dialogue, SIGNAL(rejected()), this, SLOT( cancbutton_event() ) );

	computeInfo();

	//==========================================
	add_dialogue->show();

}
void QtMeasuringWidget::edit_event(QTableWidgetItem* item)
{
	current_listbox_index = listbox->row(item);
	if(item->column()!=0) return;

	printf("EDIT\n");

	//==========================================
	//CREATE GUI
	//==========================================
	create_MeasureGUI();

	//==========================================
	// UPDATE GUI
	//==========================================
	updateGUI_withCurrent();

	emit measure_update_current_signal(current_listbox_index);
	
	if(current_listbox_index<measuring_planes.size())
	{
		emit measure_setcutplane_signal((int)measuring_planes[current_listbox_index].x, measuring_planes[current_listbox_index].y, measuring_planes[current_listbox_index].z, measuring_planes[current_listbox_index].w);
	}

	QString objstring = item->text();
	name_le->setText(objstring);

	current_drawmode = measure_objects.measure_objects[current_listbox_index].drawmode;
	
	//pointmode_button->setOn(false);
	//linemode_button->setOn(false);
	//lsegmentmode_button->setOn(false);

	emit measure_update_current_drawmode_signal(measure_objects.measure_objects[current_listbox_index].drawmode);

	connect(okbutton, SIGNAL( clicked () ), this, SLOT( okeditmeasurebutton_event() ) );
	connect(cancelbutton, SIGNAL( clicked () ), this, SLOT( cancbutton_event() ) );
	connect(add_dialogue, SIGNAL(rejected()), this, SLOT( cancbutton_event() ) );

	//==========================================
	emit measure_pause_signal();

	add_dialogue->show();
	if(current_drawmode==MEASURE_DRAWMODE_POINTS) pointmode_button->setOn(true);
	else if(current_drawmode==MEASURE_DRAWMODE_LINES) linemode_button->setOn(true);
	else  magnetline_button->setOn(true);

	current_viewmode = MEASURE_VIEW_VERTS;
}
void QtMeasuringWidget::updateGUI_withCurrent(void)
{
	QString numb, pointstr;
	current_measure_object.clear();
	current_measure_object = measure_objects.get(current_listbox_index);

	for(int i=0; i<measure_objects.measure_objects[current_listbox_index].vertices.size(); i++)
	{
		numb.setNum(i);
		pointstr = "p"+numb;
		vertices_listbox->insertItem(i, pointstr);
	}

	//if(current_listbox_index<measure_objects[current_listbox_index].edges.size())
	{
		QString edge_string;
		for(int i=0; i<measure_objects.measure_objects[current_listbox_index].edges.size(); i++)
		{
			numb.setNum(i);

			edge_string = "e" + numb + "\n   (";			
			numb.setNum(measure_objects.measure_objects[current_listbox_index].edges[i].v0);
			edge_string += "p" + numb + ",";
			numb.setNum(measure_objects.measure_objects[current_listbox_index].edges[i].v1);
			edge_string += "p" + numb + ")";

			edges_listbox->insertItem(i, edge_string);
		}
	}
	//if(current_listbox_index<measure_objects[current_listbox_index].faces.size())
	{
		QString face_string;
		for(int i=0; i<measure_objects.measure_objects[current_listbox_index].faces.size(); i++)
		{
			numb.setNum(i);

			face_string = "f" + numb + "\n   (";
			for(int j=0; j<measure_objects.measure_objects[current_listbox_index].faces[i].size(); j++)
			{
				numb.setNum(measure_objects.measure_objects[current_listbox_index].faces[i][j]);
				face_string += "p" + numb;

				if(j!=measure_objects.measure_objects[current_listbox_index].faces[i].size()-1)
				face_string +=",";
			}
			face_string+=")";

			faces_listbox->insertItem(i, face_string);
		}
	}
	//if(!current_measure_object.vertex_labels.empty() || !current_measure_object.edge_labels.empty() || !current_measure_object.face_labels.empty())
	{
		QString label_string;
		QString label_name;
		QString label;
		int list_index;
		list<string> mylist;

		//===============================
		// VERTEX LABELS
		//===============================
		mylist.clear();
		for(int i=0; i<measure_objects.measure_objects[current_listbox_index].vertex_labels.size(); i++)
			for(int j=0; j<measure_objects.measure_objects[current_listbox_index].vertex_labels[i].size(); j++)
				mylist.push_back(measure_objects.measure_objects[current_listbox_index].vertex_labels[i][j]);

		mylist.sort();
		mylist.unique();
	
		list_index=0;
		for (list<string>::iterator it=mylist.begin(); it!=mylist.end(); ++it)
		{
			//		cout<<*it<<endl;
			label_name = label_string = QString::fromStdString(*it);
			label_string  += "\n   (";
			for(int i=0; i<measure_objects.measure_objects[current_listbox_index].vertex_labels.size(); i++)
			{
				bool found=false;
				numb.setNum(i);
				for(int j=0; j<measure_objects.measure_objects[current_listbox_index].vertex_labels[i].size(); j++)
				{
					label = QString::fromStdString(measure_objects.measure_objects[current_listbox_index].vertex_labels[i][j]);

					if(label == label_name)
					{
						found=true;
					}
				}
				if(found)
				{
					label_string += "p" + numb;
					if(i!=measure_objects.measure_objects[current_listbox_index].vertex_labels.size()-1)
						label_string +=", ";
				}
			}
			label_string += ")";
			labels_listbox->insertItem(list_index, label_string);
			list_index++;
		}
		//===============================
		// EDGE LABELS
		//===============================
		mylist.clear();
		for(int i=0; i<measure_objects.measure_objects[current_listbox_index].edge_labels.size(); i++)
			for(int j=0; j<measure_objects.measure_objects[current_listbox_index].edge_labels[i].size(); j++)
				mylist.push_back(measure_objects.measure_objects[current_listbox_index].edge_labels[i][j]);

		mylist.sort();
		mylist.unique();
	
		for (list<string>::iterator it=mylist.begin(); it!=mylist.end(); ++it)
		{
			//		cout<<*it<<endl;
			label_name = label_string = QString::fromStdString(*it);
			label_string  += "\n   (";
			for(int i=0; i<measure_objects.measure_objects[current_listbox_index].edge_labels.size(); i++)
			{
				bool found=false;
				numb.setNum(i);
				for(int j=0; j<measure_objects.measure_objects[current_listbox_index].edge_labels[i].size(); j++)
				{
					label = QString::fromStdString(measure_objects.measure_objects[current_listbox_index].edge_labels[i][j]);

					if(label == label_name)
					{
						found=true;
					}
				}
				if(found)
				{
					label_string += "e" + numb;
					if(i!=measure_objects.measure_objects[current_listbox_index].edge_labels.size()-1)
						label_string +=",";
				}
			}
			label_string += ")";
			labels_listbox->insertItem(list_index, label_string);
			list_index++;
		}
		//===============================
		// FACE LABELS
		//===============================
		mylist.clear();
		for(int i=0; i<measure_objects.measure_objects[current_listbox_index].face_labels.size(); i++)
			for(int j=0; j<measure_objects.measure_objects[current_listbox_index].face_labels[i].size(); j++)
				mylist.push_back(measure_objects.measure_objects[current_listbox_index].face_labels[i][j]);

		mylist.sort();
		mylist.unique();
	
		for (list<string>::iterator it=mylist.begin(); it!=mylist.end(); ++it)
		{
			//		cout<<*it<<endl;
			label_name = label_string = QString::fromStdString(*it);
			label_string  += "\n   (";
			for(int i=0; i<measure_objects.measure_objects[current_listbox_index].face_labels.size(); i++)
			{
				bool found=false;
				numb.setNum(i);
				for(int j=0; j<measure_objects.measure_objects[current_listbox_index].face_labels[i].size(); j++)
				{
					label = QString::fromStdString(measure_objects.measure_objects[current_listbox_index].face_labels[i][j]);

					if(label == label_name)
					{
						found=true;
					}
				}
				if(found)
				{
					label_string += "f" + numb;
					if(i!=measure_objects.measure_objects[current_listbox_index].face_labels.size()-1)
						label_string +=",";
				}
			}
			label_string += ")";
			labels_listbox->insertItem(list_index, label_string);
			list_index++;
		}
	}
	
	if(measure_objects.measure_objects[current_listbox_index].drawmode==MEASURE_DRAWMODE_POINTS) 
	{
		tabWidget->setTabEnabled(0, true);
		tabWidget->setTabEnabled(1, false);
		tabWidget->setTabEnabled(2, false);
	}
	else if(measure_objects.measure_objects[current_listbox_index].drawmode==MEASURE_DRAWMODE_LINES) 
	{
		tabWidget->setTabEnabled(0, true);
		tabWidget->setTabEnabled(1, true);
		tabWidget->setTabEnabled(2, false);
	}
	else if(measure_objects.measure_objects[current_listbox_index].drawmode==MEASURE_DRAWMODE_MAGNETLINE) 
	{
			tabWidget->setTabEnabled(0, true);
			tabWidget->setTabEnabled(1, true);
			tabWidget->setTabEnabled(2, true);
	}
	
	computeInfo();
}
void QtMeasuringWidget::dataset_event(QString src_fn, QString dst_fn, int ds_index)
{
	clear_all();

	dataset_index = ds_index;
	dataset_src_alignfn = src_fn;
	dataset_dst_alignfn = dst_fn;

	printf("ds_index: %d\n", ds_index);
	printf("src_fn: %s \n", dataset_src_alignfn.latin1());
	printf("dst_fn: %s \n", dataset_dst_alignfn.latin1());

	QFile f(dataset_src_alignfn);
	if( f.exists() )
	{
		open_file(dataset_src_alignfn);

		//============================
		//CREATE GUI
		//============================
		create_MeasureGUI();

		//============================
		//UPDATE GUI
		//============================
		connect(okbutton, SIGNAL( clicked () ), this, SLOT( okdatasetmeasurebutton_event() ) );
		connect(cancelbutton, SIGNAL( clicked () ), this, SLOT( cancbutton_event() ) );
		connect(add_dialogue, SIGNAL(rejected()), this, SLOT( cancbutton_event() ) );

		//set last object as current
		current_listbox_index = measure_objects.size()-1;
		updateGUI_withCurrent();

		emit measure_update_current_signal(current_listbox_index);

		current_drawmode = measure_objects.measure_objects[current_listbox_index].drawmode;
		emit measure_update_current_drawmode_signal(measure_objects.measure_objects[current_listbox_index].drawmode);

		QString objstring;
		objstring = "align-msr";
		name_le->setText(objstring);

		//delete the last object
		listbox->setCurrentCell(current_listbox_index, 0);
		del_event();

		//edit object
		emit measure_pause_signal();
		add_dialogue->show();
	}
	else
	{
		current_drawmode = measure_objects.measure_objects[current_listbox_index].drawmode;
		emit measure_update_current_drawmode_signal(MEASURE_DRAWMODE_POINTS);

		//============================
		//CREATE GUI
		//============================
		create_MeasureGUI();

		//============================
		//UPDATE GUI
		//============================
		connect(okbutton, SIGNAL( clicked () ), this, SLOT( okdatasetmeasurebutton_event() ) );
		connect(cancelbutton, SIGNAL( clicked () ), this, SLOT( cancbutton_event() ) );
		connect(add_dialogue, SIGNAL(rejected()), this, SLOT( cancbutton_event() ) );
	
		QString objstring;
		objstring = "align-msr";
		name_le->setText(objstring);

		add_dialogue->show();
		emit measure_start_signal();
	}
}
void QtMeasuringWidget::okdatasetmeasurebutton_event(void)
{
//	printf("okdatasetmeasurebutton_event\n");

	int drmode;
	if(pointmode_button->isOn()) drmode = MEASURE_DRAWMODE_POINTS;
	else if(linemode_button->isOn()) drmode = MEASURE_DRAWMODE_LINES;
	else if(magnetline_button->isOn()) drmode = MEASURE_DRAWMODE_MAGNETLINE;

	QString objstring = name_le->text();	
	
	current_measure_object.name = objstring.toLatin1().data();
	current_measure_object.drawmode = drmode;

	measure_objects.add(current_measure_object);

//	add_dialogue->close();
	add_dialogue->accept();
	delete add_dialogue;
	add_dialogue = 0;
	vertices_listbox = 0;
	edges_listbox = 0;
	faces_listbox = 0;
	labels_listbox = 0;

	save_file(dataset_dst_alignfn);

	emit measure_stop_signal();	
	emit measure_modify_stop_signal();
	emit measure_clearcurrent_signal();

	current_measure_object.clear();
	measure_objects.clear();
}
void QtMeasuringWidget::solidplanescheckbox_event(void)
{
	bool on;
	if(solidplanescheckbox->isOn()) on = true;
	else on = false;

	emit cutplane_solid_signal(on);
}
void QtMeasuringWidget::flipplanescheckbox_event(void)
{
	bool on;
	if(flipplanescheckbox->isOn()) on = true;
	else on = false;

	emit cutplane_flip_signal(on);
}
void QtMeasuringWidget::resetplanesbutton_event(void)
{
	emit cutplane_resetall_signal();
}
void QtMeasuringWidget::measure_addvertface_slot(int v)
{
	//printf("ADDING VERTEX TO CURRENT FACE: %d\n", v);
	current_facemeasurement.push_back(v);
}
void QtMeasuringWidget::measure_rmvvertface_slot(int v)
{
	//printf("REMOVING VERTEX TO CURRENT FACE: %d\n", v);
	int todelete=-1;
	for(int i=0; i<current_facemeasurement.size(); i++)
	{
		if(current_facemeasurement[i]==v) todelete = i;
	}
	current_facemeasurement.erase(current_facemeasurement.begin()+todelete);
}
void QtMeasuringWidget::measure_endvertface_slot(void)
{
	//printf("ADD FACE\n");
	if(current_facemeasurement.empty()) return;

	current_measure_object.addFace(current_facemeasurement);	
	current_facemeasurement.clear();

	QString face_string;
	QString numb;
	numb.setNum(current_measure_object.faces.size()-1);

	face_string = "f" + numb + "\n   (";
	for(int i=0; i<current_measure_object.faces[current_measure_object.faces.size()-1].size(); i++)
	{
		numb.setNum(current_measure_object.faces[current_measure_object.faces.size()-1][i]);
		face_string += "p" + numb;

		if(i!=current_measure_object.faces[current_measure_object.faces.size()-1].size()-1)
		face_string +=",";
	}
	face_string+=")";

	faces_listbox->insertItem(current_measure_object.faces.size()-1, face_string);
}

void QtMeasuringWidget::cancbutton_event(void)
{
//	printf("cancel!\n");

	current_measure_object.clear();

	emit measure_stop_signal();
	emit measure_enableunselect_signal();	
	emit measure_cancel_signal();
	emit measure_modify_stop_signal();
	emit measureCursorOff_signal();

//	add_dialogue->close();
	add_dialogue->accept();
	delete add_dialogue;
	add_dialogue = 0;
	vertices_listbox = 0;
	edges_listbox = 0;
	faces_listbox = 0;
	labels_listbox = 0;

	edges_sum=0;
}
void QtMeasuringWidget::okeditmeasurebutton_event(void)
{
//	printf("ok edit!\n");
	if(modifygroupbutton->isOn()) emit measure_modify_stop_signal();
	emit measure_enableunselect_signal();	

	int b = 0;	
	if(pointmode_button->isOn()) b = MEASURE_DRAWMODE_POINTS;
	else if(linemode_button->isOn()) b = MEASURE_DRAWMODE_LINES;
	else if(magnetline_button->isOn()) b = MEASURE_DRAWMODE_MAGNETLINE;
	current_drawmode=b;

	QTableWidgetItem* item = listbox->item(current_listbox_index,0); 	
	item->setText(name_le->text());

	current_measure_object.drawmode = b;
	current_measure_object.name = name_le->text().toLatin1().data();
	
	measure_objects.replace(current_listbox_index, current_measure_object);

	emit measure_update_item_signal(current_listbox_index, current_measure_object,  b);
	emit measure_stop_signal();
	emit measureCursorOff_signal();
	
	current_measure_object.clear();

	listbox->setCurrentCell(-1, -1);

//	add_dialogue->close();
	add_dialogue->accept();
	delete add_dialogue;
	add_dialogue = 0;
	vertices_listbox = 0;
	edges_listbox = 0;
	faces_listbox = 0;
	labels_listbox = 0;

	edges_sum=0;
}
void QtMeasuringWidget::okaddmeasurebutton_event(void)
{
//	printf("ok add!\n");
	if(modifygroupbutton->isOn()) emit measure_modify_stop_signal();
	emit measure_enableunselect_signal();	

	int drwmd;
	if(pointmode_button->isOn()) drwmd = MEASURE_DRAWMODE_POINTS;
	else if(linemode_button->isOn()) drwmd = MEASURE_DRAWMODE_LINES;
	else if(magnetline_button->isOn()) drwmd = MEASURE_DRAWMODE_MAGNETLINE;
	current_drawmode=drwmd;

	QString objstring = name_le->text();
	
	current_measure_object.name = objstring.toLatin1().data();
	current_measure_object.drawmode = drwmd;
	measure_objects.add(current_measure_object);

	QTableWidgetItem *newItem = new QTableWidgetItem();
	newItem->setText(objstring);
	listbox->setRowCount(measure_objects.size());
	listbox->setItem(measure_objects.size()-1,0, newItem);

	newItem = new QTableWidgetItem();
	newItem->setTextAlignment(Qt::AlignCenter);
	newItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	newItem->setCheckState(Qt::Checked);
	listbox->setItem(measure_objects.size()-1,1, newItem);

	emit measure_add_item_signal(current_measure_object, current_measure_object.drawmode);
	emit measure_stop_signal();	
	emit measure_modify_stop_signal();
	emit measureCursorOff_signal();

	current_measure_object.clear();
	current_facemeasurement.clear();

	listbox->setCurrentCell(-1, -1);

	add_dialogue->accept();
	delete add_dialogue;
	add_dialogue = 0;
	vertices_listbox = 0;
	edges_listbox = 0;
	faces_listbox = 0;
	labels_listbox = 0;

	edges_sum=0;
}
void QtMeasuringWidget::measure_updatecurrent_slot(MeasureObject obj)
{
	//printf("measure_updatecurrent_slot\n");

	current_measure_object.clear();
	current_measure_object = obj;

	QString numb;
	if(!current_measure_object.vertices.empty())
	{
		if(vertices_listbox)
		{
			vertices_listbox->clear();
			QString vert_string;
			for(int i=0; i<current_measure_object.vertices.size(); i++)
			{
				numb.setNum(i);
				vert_string = "p"+numb;
				vertices_listbox->insertItem(i, vert_string);
			}
		}
	}
	else
	{
		if(vertices_listbox) vertices_listbox->clear();
	}
	if(!current_measure_object.edges.empty())
	{
		if(edges_listbox)
		{
			edges_listbox->clear();
			QString edge_string;
			for(int i=0; i<current_measure_object.edges.size(); i++)
			{
				numb.setNum(i);

				edge_string = "e" + numb + "\n   (";			
				numb.setNum(current_measure_object.edges[i].v0);
				edge_string += numb + ",";
				numb.setNum(current_measure_object.edges[i].v1);
				edge_string += numb + ")";

				edges_listbox->insertItem(i, edge_string);
			}
		}
	}
	else
	{
		if(edges_listbox) edges_listbox->clear();
	}
	if(!current_measure_object.faces.empty())
	{
		if(faces_listbox)
		{
			faces_listbox->clear();
			QString face_string;
			for(int i=0; i<current_measure_object.faces.size(); i++)
			{
				numb.setNum(i);

				face_string = "f" + numb + "\n   (";
				for(int j=0; j<current_measure_object.faces[i].size(); j++)
				{
					numb.setNum(current_measure_object.faces[i][j]);
					face_string += "p" + numb;

					if(j!=current_measure_object.faces[i].size()-1)
					face_string +=",";
				}
				face_string+=")";

				faces_listbox->insertItem(i, face_string);
			}
		}
	}	
	else
	{
		if(faces_listbox) faces_listbox->clear();
	}
	if(!current_measure_object.vertex_labels.empty() || !current_measure_object.edge_labels.empty() || !current_measure_object.face_labels.empty())
	{
	//	printf("UPDATING LABELS UI\n");

		if(labels_listbox)
		{
			labels_listbox->clear();

			QString label_string;
			QString label_name;
			QString label;
			int list_index;
			list<string> mylist;

			//===============================
			// VERTEX LABELS
			//===============================
		//	printf("VERTEX LABELS UI\n");
			mylist.clear();
			for(int i=0; i<current_measure_object.vertex_labels.size(); i++)
				for(int j=0; j<current_measure_object.vertex_labels[i].size(); j++)
					mylist.push_back(current_measure_object.vertex_labels[i][j]);

			mylist.sort();
			mylist.unique();
		
			list_index=0;
			for (list<string>::iterator it=mylist.begin(); it!=mylist.end(); ++it)
			{
				//		cout<<*it<<endl;
				label_name = label_string = QString::fromStdString(*it);
				label_string  += "\n   (";
				for(int i=0; i<current_measure_object.vertex_labels.size(); i++)
				{
					bool found=false;
					numb.setNum(i);
					for(int j=0; j<current_measure_object.vertex_labels[i].size(); j++)
					{
						label = QString::fromStdString(current_measure_object.vertex_labels[i][j]);

						if(label == label_name)
						{
							found=true;
						}
					}
					if(found)
					{
						label_string += "p" + numb;
						if(i!=current_measure_object.vertex_labels.size()-1)
							label_string +=", ";
					}
				}
				label_string += ")";
				labels_listbox->insertItem(list_index, label_string);
				list_index++;
			}
			//===============================
			// EDGE LABELS
			//===============================
			mylist.clear();
			for(int i=0; i<current_measure_object.edge_labels.size(); i++)
				for(int j=0; j<current_measure_object.edge_labels[i].size(); j++)
					mylist.push_back(current_measure_object.edge_labels[i][j]);

			mylist.sort();
			mylist.unique();
		
			for (list<string>::iterator it=mylist.begin(); it!=mylist.end(); ++it)
			{
				//		cout<<*it<<endl;
				label_name = label_string = QString::fromStdString(*it);
				label_string  += "\n   (";
				for(int i=0; i<current_measure_object.edge_labels.size(); i++)
				{
					bool found=false;
					numb.setNum(i);
					for(int j=0; j<current_measure_object.edge_labels[i].size(); j++)
					{
						label = QString::fromStdString(current_measure_object.edge_labels[i][j]);

						if(label == label_name)
						{
							found=true;
						}
					}
					if(found)
					{
						label_string += "e" + numb;
						if(i!=current_measure_object.edge_labels.size()-1)
							label_string +=",";
					}
				}
				label_string += ")";
				labels_listbox->insertItem(list_index, label_string);
				list_index++;
			}

			//===============================
			// FACE LABELS
			//===============================
			mylist.clear();
			for(int i=0; i<current_measure_object.face_labels.size(); i++)
				for(int j=0; j<current_measure_object.face_labels[i].size(); j++)
					mylist.push_back(current_measure_object.face_labels[i][j]);

			mylist.sort();
			mylist.unique();
		
			for (list<string>::iterator it=mylist.begin(); it!=mylist.end(); ++it)
			{
				//		cout<<*it<<endl;
				label_name = label_string = QString::fromStdString(*it);
				label_string  += "\n   (";
				for(int i=0; i<current_measure_object.face_labels.size(); i++)
				{
					bool found=false;
					numb.setNum(i);
					for(int j=0; j<current_measure_object.face_labels[i].size(); j++)
					{
						label = QString::fromStdString(current_measure_object.face_labels[i][j]);

						if(label == label_name)
						{
							found=true;
						}
					}
					if(found)
					{
						label_string += "f" + numb;
						if(i!=current_measure_object.face_labels.size()-1)
							label_string +=",";
					}
				}
				label_string += ")";
				labels_listbox->insertItem(list_index, label_string);
				list_index++;
			}
		}
	}
	else
	{
		if(labels_listbox)
		{
			labels_listbox->clear();
		}
	}

	computeInfo();
}

void QtMeasuringWidget::computeInfo(void)
{
	if(!add_dialogue) return;
	if(current_measure_object.vertices.empty()) return;

	QString pointstr, numb;
	
	//=========================
	//VERTICES
	//=========================

	if(!current_measure_object.vertices.empty())
	{
		numb.setNum((int)current_measure_object.vertices.size(),10);
		vertices_count->setText(numb);

		if(current_vert!=-1 && current_vert<current_measure_object.vertices.size())	
		{
			Vector v = volobj->worldspacetoimagespace(current_measure_object.vertices[current_vert]);

			numb.setNum(v.x);
			vertices_curX->setText(numb);
			numb.setNum(v.y);
			vertices_curY->setText(numb);
			numb.setNum(v.z);
			vertices_curZ->setText(numb);

			int index = volobj->get_index1D(v.x, v.y, v.z);
		
			numb.setNum(volobj->texture3d[3*index+0]);
			vertices_curR->setText(numb);
			numb.setNum(volobj->texture3d[3*index+1]);
			vertices_curG->setText(numb);
			numb.setNum(volobj->texture3d[3*index+2]);
			vertices_curB->setText(numb);
		}
		else
		{
			vertices_curX->setText("0");
			vertices_curY->setText("0");
			vertices_curZ->setText("0");
			vertices_curR->setText("0");
			vertices_curG->setText("0");
			vertices_curB->setText("0");
		}

		Vector v, rgb, currv;
		for(int i=0; i<current_measure_object.vertices.size(); i++)
		{
			currv = volobj->worldspacetoimagespace(current_measure_object.vertices[i]);

			int index = volobj->get_index1D(v.x, v.y, v.z);

			rgb += Vector(volobj->texture3d[3*index+0], volobj->texture3d[3*index+1], volobj->texture3d[3*index+2]);
			v += currv;
		}

		v /= (float)current_measure_object.vertices.size();
		rgb /= (float)current_measure_object.vertices.size();

		numb.setNum(v.x);
		vertices_meanX->setText(numb);
		numb.setNum(v.y);
		vertices_meanY->setText(numb);
		numb.setNum(v.z);
		vertices_meanZ->setText(numb);

		numb.setNum(rgb.x);
		vertices_meanR->setText(numb);
		numb.setNum(rgb.y);
		vertices_meanG->setText(numb);
		numb.setNum(rgb.z);
		vertices_meanB->setText(numb);
	}
	else
	{
		vertices_count->setText("0");

		vertices_curX->setText("0");
		vertices_curY->setText("0");
		vertices_curZ->setText("0");
		vertices_curR->setText("0");
		vertices_curG->setText("0");
		vertices_curB->setText("0");

		vertices_meanX->setText("0");
		vertices_meanY->setText("0");
		vertices_meanZ->setText("0");
		vertices_meanR->setText("0");
		vertices_meanG->setText("0");
		vertices_meanB->setText("0");
	}

	/*
	printf("units: %f, %f, %f\n", unitX, unitY, unitZ);
	printf("vox: %f, %f, %f\n", voxX, voxY, voxZ);
	printf("spacing: %f, %f, %f\n", spacingX, spacingY, spacingZ);
	printf("rescale: %f, %f, %f\n", rescaleX, rescaleY, rescaleZ);
	*/
	
	//=========================
	//EDGES
	//=========================
	if(!current_measure_object.edges.empty())
	{

		numb.setNum((int)current_measure_object.edges.size(),10);
		edges_count->setText(numb);

		if(current_edge!=-1 && current_edge<current_measure_object.edges.size())	
		{
			Vector v0 = current_measure_object.vertices[current_measure_object.edges[current_edge].v0] - current_measure_object.vertices[current_measure_object.edges[current_edge].v1];	
			
			v0.x *= unitX/voxX;
			v0.y *= unitY/voxY;
			v0.z *= unitZ/voxZ;

			v0.x /= spacingX;
			v0.y /= spacingY;
			v0.z /= spacingZ;
	
			/*v0.x *= rescaleX;
			v0.y *= rescaleY;
			v0.z *= rescaleZ;*/

			float len = v0.length();
			numb = setUnit(len);
			edges_cur->setText(numb);
		}
		else
		{
			edges_cur->setText("");
		}

		float min = 999999999999.0;
		float max = -999999999999.9;
		for(int i=0; i<current_measure_object.edges.size(); i++)
		{
			Vector v0 = current_measure_object.vertices[current_measure_object.edges[i].v0] - current_measure_object.vertices[current_measure_object.edges[i].v1];
			v0.x *= unitX/voxX;
			v0.y *= unitY/voxY;
			v0.z *= unitZ/voxZ;

			v0.x /= spacingX;
			v0.y /= spacingY;
			v0.z /= spacingZ;
		
			//v0.x *= rescaleX;
			//v0.y *= rescaleY;
			//v0.z *= rescaleZ;
			float len = v0.length();

			if(len>max) max = len;
			if(len<min) min = len;
		}

		numb = setUnit(max);
		edges_max->setText(numb);
		numb = setUnit(min);
		edges_min->setText(numb);
		
		
		eval_length();
	}
	else
	{
		edges_count->setText("0");
		edges_sum->setText(setUnit(0));
		edges_cur->setText(setUnit(0));
		edges_min->setText(setUnit(0));
		edges_max->setText(setUnit(0));
	}

	//=========================
	//FACES
	//=========================
	if(!current_measure_object.faces.empty())
	{
		numb.setNum((int)current_measure_object.faces.size(),10);
		faces_count->setText(numb);

		if(current_face!=-1 && current_face<current_measure_object.faces.size())	
		{

			vector < Vector > tempverts;
			tempverts.resize(current_measure_object.vertices.size());

			for(int j=0; j<current_measure_object.vertices.size(); j++)
			{

				tempverts[j] = current_measure_object.vertices[j];
				tempverts[j].x *= unitX/voxX;
				tempverts[j].y *= unitY/voxY;
				tempverts[j].z *= unitZ/voxZ;

				tempverts[j].x /= spacingX;
				tempverts[j].y /= spacingY;
				tempverts[j].z /= spacingZ;
		
//				tempverts[j].x *= rescaleX;
//				tempverts[j].y *= rescaleY;
//				tempverts[j].z *= rescaleZ;
			}

			Vector centre;
			for(int i=0; i<current_measure_object.faces[current_face].size(); i++)
			{
				centre += tempverts[current_measure_object.faces[current_face][i]];
			}
			centre /= (float)current_measure_object.faces[current_face].size();

			Vector v;
			float area=0.0;
			float perim=0.0;
			Face f;
			for(int i=1; i<current_measure_object.faces[current_face].size(); i++)
			{
				f.vert[0] = &tempverts[current_measure_object.faces[current_face][i]];
				f.vert[1] = &tempverts[current_measure_object.faces[current_face][i-1]];
				f.vert[2] = &centre;
				area += f.area();

				v = *f.vert[0]-*f.vert[1];
				v.x *= unitX/voxX;
				v.y *= unitY/voxY;
				v.z *= unitZ/voxZ;

				v.x /= spacingX;
				v.y /= spacingY;
				v.z /= spacingZ;
		
//				v.x *= rescaleX;
//				v.y *= rescaleY;
//				v.z *= rescaleZ;

				perim += v.length();
			}
			f.vert[0] = &tempverts[current_measure_object.faces[current_face][0]];
			f.vert[1] = &tempverts[current_measure_object.faces[current_face][current_measure_object.faces[current_face].size()-1]];
			f.vert[2] = &centre;
			area += f.area();

			v = *f.vert[0]-*f.vert[1];
			v.x *= unitX/voxX;
			v.y *= unitY/voxY;
			v.z *= unitZ/voxZ;

			v.x /= spacingX;
			v.y /= spacingY;
			v.z /= spacingZ;
		
//			v.x *= rescaleX;
//			v.y *= rescaleY;
//			v.z *= rescaleZ;

			perim += v.length();
			
			QString areastr = setUnit(area);
			areastr += QChar(0x00B2); //QString::fromStdWString (L"\u00B2");
			faces_curarea->setText(areastr);
			
			QString perimstr = setUnit(perim);
			faces_curperim->setText(perimstr);
		}
		else
		{
			QString area = setUnit(0);
			area += QChar(0x00B2);//QString::fromStdWString (L"\u00B2");
			faces_curarea->setText(area);
			faces_curperim->setText(setUnit(0));
		}

		vector < Vector > tempverts;
		tempverts.resize(current_measure_object.vertices.size());

		for(int j=0; j<current_measure_object.vertices.size(); j++)
		{

			tempverts[j] = current_measure_object.vertices[j];
			tempverts[j].x *= unitX/voxX;
			tempverts[j].y *= unitY/voxY;
			tempverts[j].z *= unitZ/voxZ;

			tempverts[j].x /= spacingX;
			tempverts[j].y /= spacingY;
			tempverts[j].z /= spacingZ;
		
//			tempverts[j].x *= rescaleX;
//			tempverts[j].y *= rescaleY;
//			tempverts[j].z *= rescaleZ;
		}

		float sumarea=0.0;
		for(int j=0; j<current_measure_object.faces.size(); j++)
		{
			Vector centre;
			for(int i=0; i<current_measure_object.faces[j].size(); i++)
			{
				centre += tempverts[current_measure_object.faces[j][i]];
			}
			centre /= (float)current_measure_object.faces[j].size();

			Face f;
			for(int i=1; i<current_measure_object.faces[j].size(); i++)
			{
				f.vert[0] = &tempverts[current_measure_object.faces[j][i]];
				f.vert[1] = &tempverts[current_measure_object.faces[j][i-1]];
				f.vert[2] = &centre;
				
				sumarea += f.area();
			}
			f.vert[0] = &tempverts[current_measure_object.faces[j][0]];
			f.vert[1] = &tempverts[current_measure_object.faces[j][current_measure_object.faces[j].size()-1]];
			f.vert[2] = &centre;
			sumarea += f.area();
		}
		tempverts.clear();

		QString area = setUnit(sumarea);
		area += QChar(0x00B2);//QString::fromStdWString (L"\u00B2");
		faces_sumarea->setText(area);
	}
	else
	{
		faces_count->setText(0);

		QString area = setUnit(0);
		area += QChar(0x00B2);//QString::fromStdWString (L"\u00B2");
		faces_curarea->setText(area);
		faces_curperim->setText(setUnit(0));

		area = setUnit(0);
		area += QChar(0x00B2);//QString::fromStdWString (L"\u00B2");
		faces_sumarea->setText(area);
	}
}
void QtMeasuringWidget::okscalebutton_event(void)
{
	QString str;

	str = voxX_le->text();
	voxX = str.toFloat();

	str = voxY_le->text();
	voxY = str.toFloat();

	str = voxZ_le->text();
	voxZ = str.toFloat();

	str = unitX_le->text();
	unitX = str.toFloat();

	str = unitY_le->text();
	unitY = str.toFloat();

	str = unitZ_le->text();
	unitZ = str.toFloat();

//	add_dialogue->close();
	add_dialogue->accept();
	delete add_dialogue;
	add_dialogue = 0;
	vertices_listbox = 0;
	edges_listbox = 0;
	faces_listbox = 0;
	labels_listbox = 0;

	listbox->setCurrentCell(-1, -1);

	measure_objects.scale = Vector(unitX/voxX, unitY/voxY, unitZ/voxZ);
	emit measure_setscale_signal(unitX/voxX, unitY/voxY, unitZ/voxZ);
}
void QtMeasuringWidget::oktimebutton_event(void)
{
	QString str;

	str = time_le->text();
	time_value = str.toFloat();

	measure_objects.time = time_value;

//	add_dialogue->close();
	add_dialogue->accept();
	delete add_dialogue;
	add_dialogue = 0;
	vertices_listbox = 0;
	edges_listbox = 0;
	faces_listbox = 0;
	labels_listbox = 0;
}
void QtMeasuringWidget::settime_event(void)
{
	QString str;
	add_dialogue = new QDialog(this, "Time Editor", FALSE);

	Q3BoxLayout* time_layout = new Q3BoxLayout(add_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "time_dialogue_layout");

		Q3ButtonGroup *timeentrylabels_layout = new Q3ButtonGroup( 1, Qt::Vertical, "", this);
			QLabel* timeentrylabel = new QLabel(timeentrylabels_layout, 0);
			timeentrylabel->setText("Time in Hours: ");
	
			time_le = new QLineEdit(QString().setNum(time_value), timeentrylabels_layout);
			str.setNum(time_value);
			time_le->setText(str);

	QPushButton *okbutton		= new QPushButton("OK",	this);
	connect(okbutton, SIGNAL( clicked () ), this, SLOT( oktimebutton_event() ) );

	QPushButton *cancelbutton		= new QPushButton("Cancel",	this);
	connect(cancelbutton, SIGNAL( clicked () ), this, SLOT( cancbutton_event() ) );

	time_layout->addWidget(timeentrylabels_layout);
	time_layout->addWidget(okbutton);
	time_layout->addWidget(cancelbutton);

	add_dialogue->show();
}
void QtMeasuringWidget::setscale_event(void)
{
	QString str;
	add_dialogue = new QDialog(this, "Scale Editor", FALSE);

	Q3BoxLayout* scale_layout = new Q3BoxLayout(add_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "scale_dialogue_layout");

		Q3ButtonGroup *scalelabels_layout = new Q3ButtonGroup( 1, Qt::Vertical, "", this);
			QLabel* axislabel = new QLabel(scalelabels_layout, 0);
			axislabel->setText("");

			QLabel* voxlabel = new QLabel(scalelabels_layout, 0);
			voxlabel->setText("Voxel(s)");

			QLabel* unitlabel = new QLabel(scalelabels_layout, 0);
			unitlabel->setText("Unit(s) in microns");

		Q3ButtonGroup *scaleXlabels_layout = new Q3ButtonGroup( 1, Qt::Vertical, "", this);
			QLabel* axisXlabel = new QLabel(scaleXlabels_layout, 0);
			axisXlabel->setText("X: ");
	
			voxX_le = new QLineEdit(QString().setNum(voxX), scaleXlabels_layout);
			//voxX_le->setInputMask("9000.000");
			str.setNum(voxX);
			voxX_le->setText(str);

			unitX_le = new QLineEdit(QString().setNum(unitX), scaleXlabels_layout);
			//unitX_le->setInputMask("9000.000");
			str.setNum(unitX);
			unitX_le->setText(str);

		Q3ButtonGroup *scaleYlabels_layout = new Q3ButtonGroup( 1, Qt::Vertical, "", this);
			QLabel* axisYlabel = new QLabel(scaleYlabels_layout, 0);
			axisYlabel->setText("Y: ");

			voxY_le = new QLineEdit(QString().setNum(voxY), scaleYlabels_layout);
			//voxY_le->setInputMask("9000.000");
			str.setNum(voxY);
			voxY_le->setText(str);

			unitY_le = new QLineEdit(QString().setNum(unitY), scaleYlabels_layout);
			//unitY_le->setInputMask("9000.000");
			str.setNum(unitY);
			unitY_le->setText(str);

		Q3ButtonGroup *scaleZlabels_layout = new Q3ButtonGroup( 1, Qt::Vertical, "", this);
			QLabel* axisZlabel = new QLabel(scaleZlabels_layout, 0);
			axisZlabel->setText("Z: ");

			voxZ_le = new QLineEdit(QString().setNum(voxZ), scaleZlabels_layout);
			//voxZ_le->setInputMask("9000.000");
			str.setNum(voxZ);
			voxZ_le->setText(str);

			unitZ_le = new QLineEdit(QString().setNum(unitZ), scaleZlabels_layout);
			//unitZ_le->setInputMask("9000.000");
			str.setNum(unitZ);
			unitZ_le->setText(str);

	QPushButton *okbutton		= new QPushButton("OK",	this);
	connect(okbutton, SIGNAL( clicked () ), this, SLOT( okscalebutton_event() ) );

	QPushButton *cancelbutton		= new QPushButton("Cancel",	this);
	connect(cancelbutton, SIGNAL( clicked () ), this, SLOT( cancbutton_event() ) );

	scale_layout->addWidget(scalelabels_layout);
	scale_layout->addWidget(scaleXlabels_layout);
	scale_layout->addWidget(scaleYlabels_layout);	
	scale_layout->addWidget(scaleZlabels_layout);	
	scale_layout->addWidget(okbutton);
	scale_layout->addWidget(cancelbutton);

	add_dialogue->show();
}
void QtMeasuringWidget::tab_currentChanged_slot(int index)
{
	if(current_viewmode==MEASURE_VIEW_VERTS)
	{
		vertices_add_button->setOn(false);
		emit measure_update_current_editmode_signal(MEASURE_EDIT_NONE);
		emit measure_pause_signal();
	}
	else if(current_viewmode==MEASURE_VIEW_EDGES)
	{
		edges_add_button->setOn(false);
		emit measure_update_current_editmode_signal(MEASURE_EDIT_NONE);
		emit measure_pause_signal();
	}
	else if(current_viewmode==MEASURE_VIEW_FACES)
	{
		faces_add_button->setOn(false);
		emit measure_update_current_editmode_signal(MEASURE_EDIT_NONE);
		emit measure_pause_signal();
	}
	else
	{
		emit measure_update_current_editmode_signal(MEASURE_EDIT_NONE);
		emit measure_pause_signal();
	}

	if(index==0)
	{
		current_viewmode = MEASURE_VIEW_VERTS;
		emit measure_update_current_viewmode_signal(MEASURE_VIEW_VERTS);
	}
	else if(index==1)
	{
		current_viewmode = MEASURE_VIEW_EDGES;
		emit measure_update_current_viewmode_signal(MEASURE_VIEW_EDGES);
	}
	else if(index==2)
	{
		current_viewmode = MEASURE_VIEW_FACES;
		emit measure_update_current_viewmode_signal(MEASURE_VIEW_FACES);
	}
	else
	{
		current_viewmode = MEASURE_VIEW_NONE;
		emit measure_update_current_viewmode_signal(MEASURE_VIEW_NONE);
	}

	//printf("index: %d\n", index);
}
void QtMeasuringWidget::vertices_rmv_button_event(void)
{
//	printf("vertices_rmv_button_event\n");
//	printf("QtMeasuringWidget::vertices_rmv_button_event vert size: %d\n", current_measure_object.vertices.size());
//	printf("QtMeasuringWidget::vertices_rmv_button_event vertlabels size: %d\n", current_measure_object.vertex_labels.size());

	vertices_add_button->setOn(false);
	emit measure_pause_signal();

	//vertices_add_button_event();

	if(current_vert==-1) return;
	if(current_vert>=0 && current_vert<current_measure_object.vertices.size())
	{
		//remove vertex
		emit measure_rmv_currentvert_signal(current_measure_object, current_vert);
	}

	current_vert = current_measure_object.vertices.size()-1;
	vertices_listbox->setCurrentRow(current_vert);
}
void QtMeasuringWidget::vertices_add_button_event(void)
{
	printf("vertices_add_button_event\n");

	if(vertices_add_button->isOn())
	{
		printf("ON\n");
		emit measure_pause_signal();
		emit measure_update_current_editmode_signal(MEASURE_EDIT_VERTS);
		emit measure_start_signal();

		emit measureCursorOn_signal();
	}
	else
	{
		printf("OFF\n");
		emit measure_pause_signal();
		emit measure_update_current_editmode_signal(MEASURE_EDIT_NONE);

		emit measureCursorOff_signal();
	}
}
void QtMeasuringWidget::edges_add_button_event(void)
{
	if(edges_add_button->isOn())
	{
		emit measure_pause_signal();
		emit measure_update_current_editmode_signal(MEASURE_EDIT_EDGES);
		emit measure_start_signal();

		emit measureCursorOn_signal();
	}
	else
	{
		emit measure_pause_signal();
		emit measure_update_current_editmode_signal(MEASURE_EDIT_NONE);

		emit measureCursorOff_signal();
	}
}
void QtMeasuringWidget::edges_rmv_button_event(void)
{
	edges_add_button->setOn(false);
	emit measure_pause_signal();
	//edges_add_button_event();

	if(current_edge==-1) return;
	if(current_edge>=0 && current_edge<current_measure_object.edges.size())
	{
		//remove edge
		emit measure_rmv_currentedge_signal(current_edge);
	}

	current_edge = current_measure_object.edges.size()-1;
	edges_listbox->setCurrentRow(current_edge);
}
void QtMeasuringWidget::faces_add_button_event(void)
{
	if(faces_add_button->isOn())
	{
		//if(current_facemeasurement.size()>2)
		{
			emit measure_pause_signal();
			emit measure_update_current_editmode_signal(MEASURE_EDIT_FACES);
			emit measure_start_signal();

			emit measureCursorOn_signal();
		}
	}
	else
	{
		if(current_facemeasurement.size()<2) 
		{
			faces_add_button->setOn(true);
			return;
		}
		//if(current_facemeasurement.size()>2)
		{
			emit measure_pause_signal();
			emit measure_update_current_editmode_signal(MEASURE_EDIT_NONE);

			emit measureCursorOff_signal();
		}
	}
}
void QtMeasuringWidget::faces_rmv_button_event(void)
{
	//FACES
	printf("Face remove: %d , siz2: %d\n", current_face, current_facemeasurement.size());

	faces_add_button->setOn(false);
	emit measure_pause_signal();
	current_facemeasurement.clear();
	
	if(current_face==-1) return;

	current_measure_object.faces.erase(current_measure_object.faces.begin()+current_face);
	emit measure_rmv_currentface_signal(current_face);

	faces_listbox->takeItem(current_face);
	current_face = faces_listbox->count()-1;
	faces_listbox->setCurrentRow(current_face);
}
void QtMeasuringWidget::labels_doubleclicked_slot(const QModelIndex& index)
{
	//printf("BOH\n");
	
	int item = index.row();
	int lblindex = -1;
	int type = -1;

	//printf("LABEL INDEX: %d\n", item);

	list<string>	vertlist;
	list<string>	edgelist;
	list<string>	facelist;

	//===============================
	// VERTEX LABELS
	//===============================
	for(int i=0; i<current_measure_object.vertex_labels.size(); i++)
		for(int j=0; j<current_measure_object.vertex_labels[i].size(); j++)
			vertlist.push_back(current_measure_object.vertex_labels[i][j]);

	vertlist.sort();
	vertlist.unique();

	//===============================
	// EDGE LABELS
	//===============================
	for(int i=0; i<current_measure_object.edge_labels.size(); i++)
		for(int j=0; j<current_measure_object.edge_labels[i].size(); j++)
			edgelist.push_back(current_measure_object.edge_labels[i][j]);

	edgelist.sort();
	edgelist.unique();

	//===============================
	// FACE LABELS
	//===============================
	for(int i=0; i<current_measure_object.face_labels.size(); i++)
		for(int j=0; j<current_measure_object.face_labels[i].size(); j++)
			facelist.push_back(current_measure_object.face_labels[i][j]);

	facelist.sort();
	facelist.unique();

	if(item<vertlist.size())
	{
		//printf("VERTEXLABEL\n");
		lblindex = item;
		type = 0;
	}
	else if(item<vertlist.size()+edgelist.size())
	{
		//printf("EDGELABEL\n");
		lblindex = item-vertlist.size();
		type = 1;
	}
	else
	{
		//printf("FACELABEL\n");
		lblindex = item-vertlist.size()-edgelist.size();
		type = 2;
	}

//	printf("LABEL %d, TYPE: %d\n", lblindex, type);

	create_LabelGUI();
	label_listmode_event(type);
	mode_grp->setButton(type);
	mode_grp->setDisabled(true);

	if(type==0)
	{
		list<string>::iterator it=vertlist.begin();
		for(int i=0; i<lblindex; i++) ++it;
		
		QString txt = QString::fromStdString(*it);
		labelname_le->setText(txt);
		editlabels_oldname = labelname_le->text();
		
		QString lbl, numb, itemstr;
		for(int i=0; i<current_measure_object.vertex_labels.size(); i++)
			for(int j=0; j<current_measure_object.vertex_labels[i].size(); j++)
			{
				lbl = QString::fromStdString(current_measure_object.vertex_labels[i][j]);
				if(lbl==txt)
				{
					numb.setNum(i);
					itemstr = "p"+numb;
					editdstlabels_listbox->insertItem(editdstlabels_listbox->count(), itemstr);

					QListWidgetItem* listitm = editsrclabels_listbox->item(i);
					listitm->setFlags(Qt::NoItemFlags);
					listitm->setBackgroundColor(Qt::darkGray);
				}
			}

	}
	else if(type==1)
	{
		list<string>::iterator it=edgelist.begin();
		for(int i=0; i<lblindex; i++) ++it;

		QString txt = QString::fromStdString(*it);
		labelname_le->setText(txt);
		editlabels_oldname = labelname_le->text();

		QString lbl, numb, itemstr;
		for(int i=0; i<current_measure_object.edge_labels.size(); i++)
			for(int j=0; j<current_measure_object.edge_labels[i].size(); j++)
			{
				lbl = QString::fromStdString(current_measure_object.edge_labels[i][j]);
				if(lbl==txt)
				{
					numb.setNum(i);

					itemstr = "e" + numb + "\n   (";			
					numb.setNum(current_measure_object.edges[i].v0);
					itemstr += "p" + numb + ",";
					numb.setNum(current_measure_object.edges[i].v1);
					itemstr += "p" + numb + ")";

					editdstlabels_listbox->insertItem(editdstlabels_listbox->count(), itemstr);

					QListWidgetItem* listitm = editsrclabels_listbox->item(i);
					listitm->setFlags(Qt::NoItemFlags);
				}
			}
	}
	else
	{
		list<string>::iterator it=facelist.begin();
		for(int i=0; i<lblindex; i++) ++it;

		QString txt = QString::fromStdString(*it);
		labelname_le->setText(txt);
		editlabels_oldname = labelname_le->text();

		QString lbl, numb, itemstr;
		for(int i=0; i<current_measure_object.face_labels.size(); i++)
			for(int j=0; j<current_measure_object.face_labels[i].size(); j++)
			{
				lbl = QString::fromStdString(current_measure_object.face_labels[i][j]);
				if(lbl==txt)
				{
					numb.setNum(i);
					itemstr = "f"+numb;
					editdstlabels_listbox->insertItem(editdstlabels_listbox->count(), itemstr);

					QListWidgetItem* listitm = editsrclabels_listbox->item(i);
					listitm->setFlags(Qt::NoItemFlags);
				}
			}
	}

	connect(labelokbutton, SIGNAL( clicked () ), this, SLOT( labelokeditbutton_event() ) );
	connect(labelcancelbutton, SIGNAL( clicked () ), this, SLOT( labelcancbutton_event() ) );
	connect(label_dialogue, SIGNAL(rejected()), this, SLOT( labelcancbutton_event() ) );

	label_dialogue->show();
}
void QtMeasuringWidget::labels_add_button_event(void)
{
	create_LabelGUI();

	//populate vertices
	label_listmode_event(0);
	mode_grp->setDisabled(false);

	QString numb;
	numb.setNum(labels_listbox->count()-1);

	labelname_le->setText("NewLabel_"+numb);

	connect(labelokbutton, SIGNAL( clicked () ), this, SLOT( labelokbutton_event() ) );
	connect(labelcancelbutton, SIGNAL( clicked () ), this, SLOT( labelcancbutton_event() ) );
	connect(label_dialogue, SIGNAL(rejected()), this, SLOT( labelcancbutton_event() ) );

	label_dialogue->show();
}
void QtMeasuringWidget::labelokeditbutton_event(void) 
{
	//printf("labelokeditbutton_event\n");

	if(editlabels_mode==0)
	{
		//VERTS
		//printf("VERTS\n");

		//DELETE OLD LABEL
		//printf("DELETING\n");
		//printf("editlabels_oldname: %s\n", editlabels_oldname.toLatin1().data());
		QString existinglbl;
		int indexhlper;
		for(int i=0; i<current_measure_object.vertex_labels.size(); i++)
		{
			indexhlper = -1;
			for(int j=0; j<current_measure_object.vertex_labels[i].size(); j++)
			{
				//printf("existinglbl: %s\n", existinglbl.toLatin1().data());
				existinglbl = QString::fromStdString(current_measure_object.vertex_labels[i][j]);
				if(editlabels_oldname==existinglbl)
				{
					indexhlper = j;
				}
			}
			if(indexhlper!=-1)
			{
				current_measure_object.vertex_labels[i].erase(current_measure_object.vertex_labels[i].begin()+indexhlper);
			}
		}

		//CREATE THE UPDATED LABEL
		//printf("CREATING\n");
		for(int i=0; i<editdstlabels_listbox->count(); i++)
		{
			QString txt = editdstlabels_listbox->item(i)->text();
			QStringList list1 = txt.split(" ");
			txt = list1.at(0);
			txt = txt.remove(0,1);

			//printf("label: %s\n", txt.toLatin1().data());

			int index = txt.toInt()-1;
			
			//printf("point: %d , label: %s\n", index, labelname_le->text().toLatin1().data());

			if(current_measure_object.vertex_labels.empty()) current_measure_object.vertex_labels.resize(current_measure_object.vertices.size());			
			current_measure_object.vertex_labels[index].push_back(labelname_le->text().toLatin1().data());
		}
	}
	else if(editlabels_mode==1)
	{
		//VERTS
		//printf("EDGES\n");

		//DELETE OLD LABEL
	//	printf("DELETING\n");
	//	printf("editlabels_oldname: %s\n", editlabels_oldname.toLatin1().data());
		QString existinglbl;
		int indexhlper;
		for(int i=0; i<current_measure_object.edge_labels.size(); i++)
		{
			indexhlper = -1;
			for(int j=0; j<current_measure_object.edge_labels[i].size(); j++)
			{
		//		printf("existinglbl: %s\n", existinglbl.toLatin1().data());
				existinglbl = QString::fromStdString(current_measure_object.edge_labels[i][j]);
				if(editlabels_oldname==existinglbl)
				{
					indexhlper = j;
				}
			}
			if(indexhlper!=-1)
			{
				current_measure_object.edge_labels[i].erase(current_measure_object.edge_labels[i].begin()+indexhlper);
			}
		}

		//CREATE THE UPDATED LABEL
		//printf("CREATING\n");
		for(int i=0; i<editdstlabels_listbox->count(); i++)
		{
			QString txt = editdstlabels_listbox->item(i)->text();
			QStringList list1 = txt.split(" ");
			txt = list1.at(0);
			txt = txt.remove(0,1);

			//printf("label: %s\n", txt.toLatin1().data());

			int index = txt.toInt()-1;
			
		//	printf("point: %d , label: %s\n", index, labelname_le->text().toLatin1().data());

			if(current_measure_object.edge_labels.empty()) current_measure_object.edge_labels.resize(current_measure_object.vertices.size());			
			current_measure_object.edge_labels[index].push_back(labelname_le->text().toLatin1().data());
		}

	}
	else
	{
		//VERTS
		//printf("FACES\n");

		//DELETE OLD LABEL
		//printf("DELETING\n");
		//printf("editlabels_oldname: %s\n", editlabels_oldname.toLatin1().data());
		QString existinglbl;
		int indexhlper;
		for(int i=0; i<current_measure_object.face_labels.size(); i++)
		{
			indexhlper = -1;
			for(int j=0; j<current_measure_object.face_labels[i].size(); j++)
			{
			//	printf("existinglbl: %s\n", existinglbl.toLatin1().data());
				existinglbl = QString::fromStdString(current_measure_object.face_labels[i][j]);
				if(editlabels_oldname==existinglbl)
				{
					indexhlper = j;
				}
			}
			if(indexhlper!=-1)
			{
				current_measure_object.edge_labels[i].erase(current_measure_object.face_labels[i].begin()+indexhlper);
			}
		}

		//CREATE THE UPDATED LABEL
		//printf("CREATING\n");
		for(int i=0; i<editdstlabels_listbox->count(); i++)
		{
			QString txt = editdstlabels_listbox->item(i)->text();
			QStringList list1 = txt.split(" ");
			txt = list1.at(0);
			txt = txt.remove(0,1);

			//printf("label: %s\n", txt.toLatin1().data());

			int index = txt.toInt()-1;
			
			//printf("point: %d , label: %s\n", index, labelname_le->text().toLatin1().data());

			if(current_measure_object.face_labels.empty()) current_measure_object.face_labels.resize(current_measure_object.vertices.size());			
			current_measure_object.face_labels[index].push_back(labelname_le->text().toLatin1().data());
		}
	}

	emit measure_update_current_signal(current_measure_object);

	label_dialogue->accept();
	delete label_dialogue;
	editsrclabels_listbox = 0;
	editdstlabels_listbox = 0;
}
void QtMeasuringWidget::labelokbutton_event() 
{
	//=============================
	//check for unique label name
	//=============================
	QString label = labelname_le->text();
	if(editlabels_mode==0)
	{
		//VERTS
		QString existinglbl;
		//printf("NEW LABEL %s\n", label.toLatin1().data());
		for(int i=0; i<current_measure_object.vertex_labels.size(); i++)
			for(int j=0; j<current_measure_object.vertex_labels[i].size(); j++)
			{
				existinglbl = QString::fromStdString(current_measure_object.vertex_labels[i][j]);

				//printf("EXISTING LABELS %s\n", current_measure_object.vertex_labels[i][j]);

				if(label==existinglbl)
				{
					//printf("LABEL is not UNIQUE!\n");

					QMessageBox* mb = new QMessageBox( "WARNING",
					"Name is not unique!\nPlease rename your label.",QMessageBox::Warning,
					QMessageBox::Ok | QMessageBox::Default,0,0 );
					mb->exec();
					return;
				}
			}
	}
	else if(editlabels_mode==1)
	{
		//EDGES
		QString existinglbl;
		//printf("NEW LABEL %s\n", label.toLatin1().data());
		for(int i=0; i<current_measure_object.edge_labels.size(); i++)
			for(int j=0; j<current_measure_object.edge_labels[i].size(); j++)
			{
				existinglbl = QString::fromStdString(current_measure_object.edge_labels[i][j]);

				//printf("EXISTING LABELS %s\n", current_measure_object.vertex_labels[i][j]);

				if(label==existinglbl)
				{
					//printf("LABEL is not UNIQUE!\n");

					QMessageBox* mb = new QMessageBox( "WARNING",
					"Name is not unique!\nPlease rename your label.",QMessageBox::Warning,
					QMessageBox::Ok | QMessageBox::Default,0,0 );
					mb->exec();
					return;
				}
			}
	}
	else
	{
		//FACES
		QString existinglbl;
		//printf("NEW LABEL %s\n", label.toLatin1().data());
		for(int i=0; i<current_measure_object.face_labels.size(); i++)
			for(int j=0; j<current_measure_object.face_labels[i].size(); j++)
			{
				existinglbl = QString::fromStdString(current_measure_object.face_labels[i][j]);

				//printf("EXISTING LABELS %s\n", current_measure_object.vertex_labels[i][j]);

				if(label==existinglbl)
				{
					//printf("LABEL is not UNIQUE!\n");

					QMessageBox* mb = new QMessageBox( "WARNING",
					"Name is not unique!\nPlease rename your label.",QMessageBox::Warning,
					QMessageBox::Ok | QMessageBox::Default,0,0 );
					mb->exec();
					return;
				}
			}
	}

	//=============================
	//build label in current object
	//=============================
	if(editlabels_mode==0)
	{
		for(int i=0; i<editdstlabels_listbox->count(); i++)
		{
			QString txt = editdstlabels_listbox->item(i)->text();
			QStringList list1 = txt.split(" ");
			txt = list1.at(0);
			txt = txt.remove(0,1);

			//printf("label: %s\n", txt.toLatin1().data());

			int index = txt.toInt();
			
			//printf("point: %d , label: %s\n", index, labelname_le->text().toLatin1().data());

			if(current_measure_object.vertex_labels.empty()) current_measure_object.vertex_labels.resize(current_measure_object.vertices.size());			
			current_measure_object.vertex_labels[index].push_back(labelname_le->text().toLatin1().data());
		}
	}
	else if(editlabels_mode==1)
	{
		for(int i=0; i<editdstlabels_listbox->count(); i++)
		{
			QString txt = editdstlabels_listbox->item(i)->text();
			QStringList list1 = txt.split(" ");
			txt = list1.at(0);
			txt = txt.remove(0,1);

			//printf("label: %s\n", txt.toLatin1().data());

			int index = txt.toInt();
			
			//printf("point: %d , label: %s\n", index, labelname_le->text().toLatin1().data());

			if(current_measure_object.edge_labels.empty()) current_measure_object.edge_labels.resize(current_measure_object.edges.size());			
			current_measure_object.edge_labels[index].push_back(labelname_le->text().toLatin1().data());
		}
	}
	else 
	{
		for(int i=0; i<editdstlabels_listbox->count(); i++)
		{
			QString txt = editdstlabels_listbox->item(i)->text();
			QStringList list1 = txt.split(" ");
			txt = list1.at(0);
			//printf("label: %s\n", txt.toLatin1().data());
			txt = txt.remove(0,1);

			//printf("label: %s\n", txt.toLatin1().data());

			int index = txt.toInt();
			
			//printf("point: %d , label: %s\n", index, labelname_le->text().toLatin1().data());

			if(current_measure_object.face_labels.empty()) current_measure_object.face_labels.resize(current_measure_object.faces.size());			
			current_measure_object.face_labels[index].push_back(labelname_le->text().toLatin1().data());
		}
	}

	emit measure_update_current_signal(current_measure_object);

	label_dialogue->accept();
	delete label_dialogue;
	editsrclabels_listbox = 0;
	editdstlabels_listbox = 0;
}
void QtMeasuringWidget::labelcancbutton_event() 
{
	label_dialogue->accept();
	delete label_dialogue;
	editsrclabels_listbox = 0;
	editdstlabels_listbox = 0;
}
void QtMeasuringWidget::labels_rmv_button_event(void)
{
	//printf("REMOVE LABEL\n");
	if(current_label==-1) return;

	bool delete_geometry=false;
	switch( QMessageBox::warning(this, "Deleting...",
	"Would you like to delete the geometry linked with this label? \n",
	QMessageBox::Yes,
	QMessageBox::No | QMessageBox::Default )) 
	{
		case QMessageBox::Yes: 
			delete_geometry=true;
			break;
		case QMessageBox::No: // Abort or Cancel
			// abort
			break;
		default:
			break;
	}


	emit measure_rmv_currentlabel_signal(current_label, delete_geometry);

	labels_listbox->takeItem(current_label);
	current_label = labels_listbox->count()-1;
	labels_listbox->setCurrentRow(current_label);
}
void QtMeasuringWidget::measure_edge_select_slot(int index)
{
	current_edge = index;
	emit measure_selected_edgeindex_signal(index);

	if(index>=0 && index<current_measure_object.edges.size()) edges_listbox->setCurrentRow(index);
}
void QtMeasuringWidget::measure_face_select_slot(int index)
{
	current_face = index;
	emit measure_selected_faceindex_signal(index);

	if(index>=0 && index<current_measure_object.edges.size()) faces_listbox->setCurrentRow(index);
}
void QtMeasuringWidget::measure_editpoint_select_slot(int index)
{
	current_vert = index;
	measure_selected_vertindex_signal(index);

	if(index>=0 && index<current_measure_object.vertices.size()) vertices_listbox->setCurrentRow(index);
}
void QtMeasuringWidget::measure_editgrppoint_slot(int i, Vector v)
{
	if(i>=0 && i<=current_measure_object.vertices.size())
	{
		current_measure_object.vertices[i] = v;
		eval_length();
	}
}
void QtMeasuringWidget::measure_editpoint_slot(Vector v)
{	
	if(current_vert>=0 && current_vert<=current_measure_object.vertices.size())
	{
		current_measure_object.vertices[current_vert] = v;
		eval_length();
	}
}
void QtMeasuringWidget::measure_point_slot(Vector p)
{
	//printf("measure_point_slot\n");

	current_measure_object.addPoint(p);
	QString pointstr, numb;
	
	numb.setNum(current_measure_object.vertices.size()-1);
	pointstr = "p"+numb;

	vertices_listbox->insertItem(current_measure_object.vertices.size()-1, pointstr);

	current_vert = current_measure_object.vertices.size()-1;

	computeInfo();
}
void QtMeasuringWidget::measure_savecutplane_slot(int index, float translate, float rotang1, float rotang2)
{
//	printf("SAVING OK\n");	

	Vector v;
	v.x = index;
	v.y = translate;
	v.z = rotang1;
	v.w = rotang2;
	
//	printf("measure save slot: %d, %f, %f, %f\n", index, translate, rotang1, rotang2);
	
	measuring_planes.push_back(v);
}
void QtMeasuringWidget::measure_setspacing_slot(float x, float y, float z)
{
	spacingX = x;
	spacingY = y;
	spacingZ = z;

	printf("SPACING: %f %f %f\n", spacingX, spacingY, spacingZ);
}
void QtMeasuringWidget::measure_setscale2_slot(float x, float y, float z)
{
	//printf("QtMeasuringWidget::measure_setscale_slot\n");
	voxX = 1.0;
	voxY = 1.0;
	voxZ = 1.0;

	unitX = x*rescaleX;
	unitY = y*rescaleY;
	unitZ = z*rescaleZ;

	measure_objects.scale = Vector(unitX/voxX, unitY/voxY, unitZ/voxZ);

	printf("UNIT: %f %f %f\n", unitX, unitY, unitZ);
}
void QtMeasuringWidget::measure_setrescale_slot(float x, float y, float z)
{
	rescaleX = x;
	rescaleY = y;
	rescaleZ = z;
	
	printf("RESCALE: %f, %f, %f \n",x,y,z);	
}
void QtMeasuringWidget::measure_setoriginalfn_slot(QString fn)
{
	originalfilename = fn;
}
float QtMeasuringWidget::eval_area(void)
{
	int n = current_measure_object.vertices.size();
	float A=0.0f;
	return A;
	
	//-----------DEBUG
	//return A;

	int b = 0;	
	if(pointmode_button->isOn()) b = MEASURE_DRAWMODE_POINTS;
	else if(linemode_button->isOn()) b = MEASURE_DRAWMODE_LINES;
	else if(magnetline_button->isOn()) b = MEASURE_DRAWMODE_MAGNETLINE;

	if(b<4) return A;
	if(n<2) return A;

	if(b==5)
	{
//		printf("sphere\n");
		Vector v1 = current_measure_object.vertices[0];
		v1.x *= unitX/voxX;
		v1.y *= unitY/voxY;
		v1.z *= unitZ/voxZ;
		v1.x /= spacingX;
		v1.y /= spacingY;
		v1.z /= spacingZ;
//		v1.x *= rescaleX;
//		v1.y *= rescaleY;
//		v1.z *= rescaleZ;

		Vector v2 = current_measure_object.vertices[1];
		v2.x *= unitX/voxX;
		v2.y *= unitY/voxY;
		v2.z *= unitZ/voxZ;
		v2.x /= spacingX;
		v2.y /= spacingY;
		v2.z /= spacingZ;
//		v2.x *= rescaleX;
//		v2.y *= rescaleY;
//		v2.z *= rescaleZ;

		Vector v = v1-v2;
		float radius = v.length()*0.5f;
		A = (4.0)*(PI*pow(radius,2.0f));
		
		return A;
	}

	if(n<3) return A;

	PCA pca;
	int i;
	Vector v;
	for(i=0; i<current_measure_object.vertices.size(); i++)
	{
		v = current_measure_object.vertices[i];
		v.x *= unitX/voxX;
		v.y *= unitY/voxY;
		v.z *= unitZ/voxZ;
		v.x /= spacingX;
		v.y /= spacingY;
		v.z /= spacingZ;
//		v.x *= rescaleX;
//		v.y *= rescaleY;
//		v.z *= rescaleZ;

		pca.data.push_back(v);
	}

	pca.eval_PCA();
	pca.eval_aligned_data();
	
	float x, y, z;
	x=y=z=0;
	for (i=0; i<pca.aligned_data.size(); i++)
	{
		pca.aligned_data[i].round(4);
		x =+ pca.aligned_data[i].x;
		y =+ pca.aligned_data[i].y;
		z =+ pca.aligned_data[i].z;
	}

	int axis = -1;
	if(x==0) axis=0;
	else if(y==0) axis=1;
	else axis=2;

	vector < Vector > points2d;
	vector < Vector > result;

	for (i=0; i<pca.aligned_data.size(); i++)
	{
		if(axis==0) points2d.push_back(Vector(pca.aligned_data[i].y, pca.aligned_data[i].z,0,i));
		else if(axis==1) points2d.push_back(Vector(pca.aligned_data[i].x, pca.aligned_data[i].z,0,i));
		else if(axis==2) points2d.push_back(Vector(pca.aligned_data[i].x, pca.aligned_data[i].y,0,i));
	}


	for(int p=n-1,q=0; q<n; p=q++)
	{
		A+= points2d[p].x*points2d[q].y - points2d[q].x*points2d[p].y;
	}
	return abs(A*0.5f);
}

void QtMeasuringWidget::eval_length(void)
{
	float length = 0;

	for(int i=0; i<current_measure_object.edges.size(); i++)
	{
		//printf("edge: %d, %d\n", current_edgemeasurement[i].v0, current_edgemeasurement[i].v1);

		Vector v0 = current_measure_object.vertices[current_measure_object.edges[i].v0];
		Vector v1 = current_measure_object.vertices[current_measure_object.edges[i].v1];
		Vector temp = v0-v1;

		temp.x *= unitX/voxX;
		temp.y *= unitY/voxY;
		temp.z *= unitZ/voxZ;

		temp.x /= spacingX;
		temp.y /= spacingY;
		temp.z /= spacingZ;
		
//		temp.x *= rescaleX;
//		temp.y *= rescaleY;
//		temp.z *= rescaleZ;

		length += temp.length();
	}
	
	if(edges_sum!=NULL)
	{
		QString numb;
		numb = setUnit(length);
		edges_sum->setText(numb);
	}
}

void QtMeasuringWidget::measure_close_slot(void)
{
	//printf("close add measure slot\n");
//	add_dialogue->close();
	
	QMessageBox* mb = new QMessageBox( "WARNING",
			"No clipping plane is currently selected! \n",QMessageBox::Warning,
			QMessageBox::Ok | QMessageBox::Default,0,0 );

	mb->exec();
	delete mb;
	
	cancbutton_event();
}

void QtMeasuringWidget::measure_setcurrentworkingdir_slot(QString dn)
{
	currentworkingdir = dn;
}
void QtMeasuringWidget::vert_cb_event(void)
{
	if(vert_cb->isOn()) vert_label = 1;
	else vert_label = 0;
	
	emit measure_labels_signal(vert_label, edge_label, face_label, arrow_label, labels_label, lists_label, tris_label);
}
void QtMeasuringWidget::edge_cb_event(void)
{
	if(edge_cb->isOn()) edge_label = 1;
	else edge_label = 0;

	emit measure_labels_signal(vert_label, edge_label, face_label, arrow_label, labels_label, lists_label, tris_label);
}
void QtMeasuringWidget::face_cb_event(void)
{
	if(face_cb->isOn()) face_label = 1;
	else face_label = 0;

	emit measure_labels_signal(vert_label, edge_label, face_label, arrow_label, labels_label, lists_label, tris_label);
}
void QtMeasuringWidget::arrow_cb_event(void)
{
	if(arrow_cb->isOn()) arrow_label = 1;
	else arrow_label = 0;

	emit measure_labels_signal(vert_label, edge_label, face_label, arrow_label, labels_label, lists_label, tris_label);
}
void QtMeasuringWidget::labels_cb_event(void)
{
	if(labels_cb->isOn()) labels_label = 1;
	else labels_label = 0;

	emit measure_labels_signal(vert_label, edge_label, face_label, arrow_label, labels_label, lists_label, tris_label);
}
void QtMeasuringWidget::lists_cb_event(void)
{
	if(lists_cb->isOn()) lists_label = 1;
	else lists_label = 0;

	emit measure_labels_signal(vert_label, edge_label, face_label, arrow_label, labels_label, lists_label, tris_label);
}
void QtMeasuringWidget::tris_cb_event(void)
{
	if(tris_cb->isOn()) tris_label = 1;
	else tris_label = 0;

	emit measure_labels_signal(vert_label, edge_label, face_label, arrow_label, labels_label, lists_label, tris_label);

}
void QtMeasuringWidget::export_event(void)
{
	printf("EXPORT EVENT\n");
	QString fn = QFileDialog::getSaveFileName(currentworkingdir, "*.csv", this );

	if(current_viewmode==MEASURE_VIEW_VERTS)
	{
		printf("VERTS\n");

		if(!fn.isEmpty())
		{
			ofstream fout;
			fout.open(fn.latin1());
			fout<<"Object,X,Y,Z,R,G,B"<<endl;
			for(int i=0; i<current_measure_object.vertices.size(); i++)
			{
				Vector v = current_measure_object.vertices[i];
				v.x *= unitX/voxX;
				v.y *= unitY/voxY;
				v.z *= unitZ/voxZ;

				v.x /= spacingX;
				v.y /= spacingY;
				v.z /= spacingZ;
		
//				v.x *= rescaleX;
//				v.y *= rescaleY;
//				v.z *= rescaleZ;

				int index = volobj->get_index1D(v.x, v.y, v.z);
				int r = volobj->texture3d[3*index+0];
				int g = volobj->texture3d[3*index+0];
				int b = volobj->texture3d[3*index+0];

				QString label, indexstr;
				indexstr.setNum(i+1);
				label = "v"+ indexstr;

				fout<<label.latin1()<<","<<v.x<<","<<v.y<<","<<v.z<<","<<r<<","<<g<<","<<b<<endl;
			}
			fout.close();
		}
	}
	else if(current_viewmode==MEASURE_VIEW_EDGES)
	{
		printf("EDGES\n");
		if(!fn.isEmpty())
		{
			ofstream fout;
			fout.open(fn.latin1());
			fout<<"Object,Length,Unit"<<endl;
			for(int i=0; i<current_measure_object.edges.size(); i++)
			{
				Vector v0 = current_measure_object.vertices[current_measure_object.edges[i].v0];
				Vector v1 = current_measure_object.vertices[current_measure_object.edges[i].v1];
				Vector v = v1-v0;

				v.x *= unitX/voxX;
				v.y *= unitY/voxY;
				v.z *= unitZ/voxZ;

				v.x /= spacingX;
				v.y /= spacingY;
				v.z /= spacingZ;
		
//				v.x *= rescaleX;
//				v.y *= rescaleY;
//				v.z *= rescaleZ;

				float l = v.length();
				QString text = setUnit(l);
				text.replace(" ", ",");
				
				QString label, index;
				index.setNum(i+1);
				label = "e"+ index;
				
				fout<<label.latin1()<<","<<text.latin1()<<endl;
			}
			fout.close();
		}
	}
	else if(current_viewmode==MEASURE_VIEW_FACES)
	{
		printf("FACES\n");
		if(!fn.isEmpty())
		{
			ofstream fout;
			fout.open(fn.latin1());
			fout<<"Object,Perimeter,Unit,Area,Unit"<<endl;

			for(int j=0; j<current_measure_object.faces.size(); j++)
			{
				Vector centre;
				for(int i=0; i<current_measure_object.faces[j].size(); i++)
				{
					centre += current_measure_object.vertices[current_measure_object.faces[j][i]];
				}
				centre /= (float)current_measure_object.faces[j].size();

				Vector v;
				Face f;
				float area=0.0;
				float perim=0.0;
				for(int i=1; i<current_measure_object.faces[j].size(); i++)
				{
					f.vert[0] = &current_measure_object.vertices[current_measure_object.faces[j][i]];
					f.vert[1] = &current_measure_object.vertices[current_measure_object.faces[j][i-1]];
					f.vert[2] = &centre;

					area += f.area();

					v = *f.vert[0]-*f.vert[1];
					perim += v.length();
				}
				f.vert[0] = &current_measure_object.vertices[current_measure_object.faces[j][0]];
				f.vert[1] = &current_measure_object.vertices[current_measure_object.faces[j][current_measure_object.faces[j].size()-1]];
				f.vert[2] = &centre;
				area += f.area();

				v = *f.vert[0]-*f.vert[1];
				perim += v.length();

				QString perimtext = setUnit(perim);
				perimtext.replace(" ", ",");

				QString areatext = setUnit(area);
				areatext.replace(" ", ",");
				areatext += QChar(0x00B2); //QString::fromStdWString (L"\u00B2");

				QString label, indexstr;
				indexstr.setNum(j+1);
				label = "f"+ indexstr;

				fout<<label.latin1()<<","<<perimtext.latin1()<<","<<areatext.latin1()<<endl;

			}
			fout.close();
		}
	}
}
