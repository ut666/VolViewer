/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtDatasetViewWidget.h"
//Added by qt3to4:
#include <QPixmap>

	
QtDatasetViewWidget::QtDatasetViewWidget(QWidget* parent, const char* name)
: QWidget( parent, name )
{
	currentworkingdir="";
	setAcceptDrops(true);
}
QtDatasetViewWidget::~QtDatasetViewWidget()
{
}
void QtDatasetViewWidget::clear_all(void)
{
	tablewidget->clearContents();
	tablewidget->clear();
	
	for(int i=tablewidget->rowCount()-1; i>=0; i--)
		tablewidget->removeRow(i);
	
	dsview.clear();
}
void QtDatasetViewWidget::open(void)
{
	QFileDialog* fd = new QFileDialog( this );
	fd->setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	//Load our folder structure.
	QString dn = fd->getExistingDirectory(this, "Please select a Directory", currentworkingdir);

	if(dn.isEmpty())
	{
		return;
	}

	clear_all();

	//now populate the ListWidgetItems
	dsview.datafolder = dn;

	QPixmap pixmap =  QPixmap(128, 128);
	pixmap.fill(QColor(0,0,0));

	QDir dirs = QDir(dn);
	dirs.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	dirs.setSorting(QDir::Name | QDir::LocaleAware);

	QStringList stringdirlist = dirs.entryList();
	for(int i=0; i<stringdirlist.size(); i++)
	{
		if(stringdirlist[i]!="VVData")
		{
			dsview.datasubfolders.push_back(stringdirlist[i]);
			dsview.datathumbnails.push_back(pixmap);
		}
	}
	for(int i=0; i<stringdirlist.size(); i++)
	{
		QString savedir = dn + "/" + "VVData/";
		QString savesubdir = dn + "/" + "VVData/" + stringdirlist[i] + "/";

		QDir dir(savesubdir);
		if (!dir.exists())
		{
			dir.mkpath(savesubdir);
		}
	}
	for(int i=0; i<dsview.datasubfolders.size(); i++)
	{
		QString thumbname = dsview.datafolder + "/VVData/Thumbs/";		

		QDir dir;
		dir.mkdir(thumbname);
		thumbname += dsview.datasubfolders[i] + ".png";
		printf("thumbname: %s\n", thumbname.latin1());
		dsview.datathumbnails[i].load(thumbname, "PNG");
	}

	int thumb_size=64;
	tablewidget->setIconSize(QSize(thumb_size, thumb_size));

	QTableWidgetItem *newitem;
	for(int i=0; i<dsview.datasubfolders.size(); i++)
	{
		QIcon viewIcon(dsview.datathumbnails[i]);

		printf("ADDING ITEM\n");

		tablewidget->setRowCount(tablewidget->rowCount()+1);
		tablewidget->setRowHeight(tablewidget->rowCount()-1, thumb_size);

		newitem = new QTableWidgetItem(viewIcon,"",0);
		tablewidget->setItem(tablewidget->rowCount()-1, 0, newitem);

		newitem = new QTableWidgetItem(dsview.datasubfolders[i],0);
		tablewidget->setItem(tablewidget->rowCount()-1, 1, newitem);

		newitem = new QTableWidgetItem("align.msr",0);
		tablewidget->setItem(tablewidget->rowCount()-1, 2, newitem);
	}

	dsview.numbimages = dsview.datasubfolders.size();
	dsview.current_image = 0;
}

void QtDatasetViewWidget::rebuild(void)
{
}
void QtDatasetViewWidget::reset(void)
{

}
void QtDatasetViewWidget::create(void)
{
	tablewidget = new QTableWidget(this);
    tablewidget->setColumnCount(3);	
	QStringList headerlist;
	headerlist << ""<<"Name" << "MSR";
	tablewidget->setHorizontalHeaderLabels(headerlist);
	tablewidget->verticalHeader()->hide();
	tablewidget->hideColumn(2);
	tablewidget->setAlternatingRowColors(true);

	connect(tablewidget,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
		     this,SLOT(itemDoublClicked_event(QTableWidgetItem*)));

	Q3ButtonGroup* opensave_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "File:", this );
    opensave_grp->setExclusive( FALSE );
	opensave_grp->setAlignment(Qt::AlignLeft);
	//opensave_grp->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));

		QPushButton* openbtn = new QPushButton(QIcon(QPixmap("icons/fileopen.xpm")), "Open", opensave_grp, "open");
		connect( openbtn, SIGNAL( clicked () ), this, SLOT( open() ) );
		//open->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		
		//QPushButton* savebtn = new QPushButton(QIcon(QPixmap("icons/filesave.xpm")), "Save", opensave_grp, "save");
//		connect( save, SIGNAL( clicked () ), this, SLOT( save_event() ) );
		//save->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
	
	Q3ButtonGroup *act_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Actions:", this );

		QPushButton* genthumbs_but = new QPushButton("Compute Projections", act_grp);
		connect(genthumbs_but, SIGNAL( clicked () ), this, SLOT( generatethumbs_event() ) );

		QPushButton* openmovie_but = new QPushButton("Open Movie Player", act_grp);
		connect(openmovie_but, SIGNAL( clicked () ), this, SLOT( openmovieplayer_event() ) );

	Q3ButtonGroup *align_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Alignment:", this );

		QPushButton* align_but = new QPushButton("Align Data", align_grp);
		connect(align_but, SIGNAL( clicked () ), this, SLOT( align_event() ) );

		QPushButton* stacks_but = new QPushButton("Compute Stacks", align_grp);
		connect(stacks_but, SIGNAL( clicked () ), this, SLOT( computestacks_event() ) );

	Q3BoxLayout * left_blayout = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "left_blayout");
	left_blayout->setAlignment(Qt::AlignTop);
    left_blayout->addWidget( opensave_grp, 0, Qt::AlignTop);
    left_blayout->addWidget( act_grp, 0, Qt::AlignTop);
	align_grp->hide();
    //left_blayout->addWidget( align_grp, 0, Qt::AlignTop);
	

	Q3BoxLayout * rigth_blayout = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "rigth_blayout");
    rigth_blayout->addWidget( tablewidget, 10);
	
	//our transfertab top level layout
	Q3BoxLayout * topmost = new Q3BoxLayout ( this, Q3BoxLayout::LeftToRight, 10, 5, "topmost_blayout");
    topmost->addLayout( left_blayout, 1);
    topmost->addLayout( rigth_blayout, 10);
}
void QtDatasetViewWidget::align_event(void)
{
	emit datasetview_align_signal(dsview);
}
void QtDatasetViewWidget::openmovieplayer_event(void)
{
	QDialog* movie_dialogue = new QDialog(this, "Movie Player", 0);
	movie_dialogue->setModal(false);

	movieplayer = new QtMoviePlayer(0, &dsview.datathumbnails);

	Q3BoxLayout * boxlayout = new Q3BoxLayout ( movie_dialogue, Q3BoxLayout::TopToBottom, 0, 5, "rigth_blayout");
    boxlayout->addWidget( movieplayer, 10);

	movie_dialogue->resize(512,512);
	movie_dialogue->show();
}

void QtDatasetViewWidget::generatethumbs_event(void)
{
//	emit datasetview_align_signal(dsview);

	QProgressDialog progress("Updating Thumbnails", "Cancel", 0, dsview.datasubfolders.size(), this);
	progress.setMinimumDuration(10);

	for(int i=0; i<dsview.datasubfolders.size(); i++)
	{
		progress.setValue(i);
        if (progress.wasCanceled()) return;

		QString datadn = dsview.datafolder + "/" + dsview.datasubfolders[i] + "/";

		emit datasetview_loadFolder_signal(datadn);
		emit datasetview_loadSettings_signal(dsview.datafolder, dsview.datasubfolders[i]);
		emit datasetview_updatethumbnail_signal(i);

		QString thumbname = dsview.datafolder + "/VVData/Thumbs/";		
		//printf("save dir: %s\n", thumbname.latin1());

		QDir dir;
		dir.mkdir(thumbname);
		thumbname += dsview.datasubfolders[i] + ".png";
		dsview.datathumbnails[i].save(thumbname, "PNG");
	}
	progress.setValue(dsview.datasubfolders.size());
}
void QtDatasetViewWidget::exit(void)
{
}
void QtDatasetViewWidget::dragndrop_slot(QString sFileName)
{	
}
void QtDatasetViewWidget::dragEnterEvent(QDragEnterEvent* event)
{
}
void QtDatasetViewWidget::dropEvent(QDropEvent* event)
{
	Q3StrList URIs;

	if ( Q3UriDrag::decode(event, URIs) ) 
	{
		QString text = Q3UriDrag::uriToLocalFile(URIs.getFirst());
		dragndrop_slot(text);
	}
}
void QtDatasetViewWidget::saveview_event(void)
{
	//printf("boh\n");
	//emit renderview_saveview_signal();
}

void QtDatasetViewWidget::itemDoublClicked_event(QTableWidgetItem* item)
{
	//item we double clicked
	int row = item->row();

	dsview.current_image = row;
	QString datadn = dsview.datafolder + "/" + dsview.datasubfolders[row] + "/";
	QString saveddatadn = dsview.datafolder + "/" + "VVData/" + dsview.datasubfolders[row] + "/";
	QString aligndatafn = dsview.datafolder + "/" + "VVData/" + dsview.datasubfolders[row] + "/align.msr";

	if(item->column()==0 || item->column()==1)
	{
		//first we save settings from the old image.
		//emit datasetview_saveSettings_signal(dsview.datafolder, dsview.datasubfolders[dsview.current_image]);
		
		emit datasetview_loadFolder_signal(datadn);
		emit datasetview_loadSettings_signal(dsview.datafolder, dsview.datasubfolders[dsview.current_image]);
	}
	else if(item->column()==2)
	{
		emit datasetview_loadFolder_signal(datadn);
	
		printf("ALIGN clicked\n");
		printf("volume data: %s\n", datadn.latin1());
		printf("saved data: %s\n", saveddatadn.latin1());

		if(row==0)
		{
			printf("Timepoint 0\n");
			QFile f(aligndatafn);
			if( !f.exists() )
			{
				printf("no align.msr found: CREATE NEW\n");
				emit datasetview_edit_MSR_signal(aligndatafn, aligndatafn, row);
			}
			else
			{
				printf("align.msr found: LOAD + EDIT\n");
				emit datasetview_edit_MSR_signal(aligndatafn, aligndatafn, row);
			}
		}
		else
		{
			QString aligndatafn_src = dsview.datafolder + "/" + "VVData/" + dsview.datasubfolders[row-1] + "/align.msr";

			QFile f(aligndatafn);
			printf("Timepoint X\n");
			if( !f.exists() )
			{
				printf("no align.msr found: LOAD (X-1) + EDIT\n");
				emit datasetview_edit_MSR_signal(aligndatafn_src, aligndatafn, row);
			}
			else
			{
				printf("align.msr found: LOAD + EDIT\n");
				emit datasetview_edit_MSR_signal(aligndatafn, aligndatafn, row);
			}
		}
	}
}

void QtDatasetViewWidget::datasetview_setThumbnail_slot(int i, QPixmap thumbnail)
{
	dsview.datathumbnails[i] = thumbnail;

	QIcon viewIcon(dsview.datathumbnails[i]);
	QTableWidgetItem* newitem = new QTableWidgetItem(viewIcon,"",0);
	tablewidget->setItem(i, 0, newitem);
}
void QtDatasetViewWidget::computestacks_event(void)
{
	QFileDialog* fd = new QFileDialog( this );
	fd->setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	//Load our folder structure.
	QString dn = fd->getExistingDirectory(this, "Please select a Directory", currentworkingdir);

	if(dn.isEmpty())
	{
		return;
	}

	printf("Computing stacks...\n");
	emit datasetview_computestacks_signal(dn, dsview);	
}