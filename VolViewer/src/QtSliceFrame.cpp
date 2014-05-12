/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtSliceFrame.h"
//Added by qt3to4:
#include <QPixmap>
#include <QResizeEvent>
#include <QEvent>
#include <QLabel>
#include <QMouseEvent>
#include <Q3Frame>
#include <QPaintEvent>

QtSliceFrame::QtSliceFrame(QWidget* parent, const char* name): QGraphicsView( parent )
{
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
 
    //Set-up the scene
    scene = new QGraphicsScene(this);
	scene->setBackgroundBrush(Qt::black);
    setScene(scene);

	x = y = z = 0;
	selected = 0;
	slices=1;
	dimension_mode = 1;
	draw_mode=-1;
	click_on = 0;

	zoomlevel=1.0;

	cursor_on = true;
	invert_on = false;

	resizeX = resizeY = 1.0;
	brushsize = 1.0;

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	create();
	show();
}

QtSliceFrame::~QtSliceFrame()
{
}
void QtSliceFrame::create(void)
{
	slice_pixmap = new QPixmap(128, 128);
	slice_pixmap->fill(Qt::black);

	pixmapitem = scene->addPixmap(*slice_pixmap);
	lineitem1 = scene->addLine(0, 0, 0, height(), QPen(Qt::black));
	lineitem2 = scene->addLine(0, 0, 0, height(), QPen(Qt::black));

	scene->setSceneRect(pixmapitem->sceneBoundingRect());

	centerOn(pixmapitem);
	fitInView(pixmapitem, Qt::KeepAspectRatio);

	setResizeAnchor(QGraphicsView::AnchorViewCenter);
	setTransformationAnchor(QGraphicsView::AnchorViewCenter);
	setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);

	draw();
	update();
}
void QtSliceFrame::initPixmap(QPixmap* pixmap)
{
	//printf("INIT PIXMAP\n");

	resetTransform();

	slice_pixmap = pixmap;

	if(invert_on)
	{
		QImage img = slice_pixmap->convertToImage();
		img.invertPixels();

		slice_pixmap->convertFromImage(img);
	}

	pixmapitem->setPixmap(*slice_pixmap);

	scene->setSceneRect(pixmapitem->sceneBoundingRect());
	centerOn(pixmapitem);
	fitInView(pixmapitem, Qt::KeepAspectRatio);
	draw();
	
	update();
}
void QtSliceFrame::updatePixmap(QPixmap* pixmap)
{
	//printf("UPDATE PIXMAP\n");
	updateLabels();

	slice_pixmap = pixmap;

	if(invert_on)
	{
		QImage img = slice_pixmap->convertToImage();
		img.invertPixels();

		slice_pixmap->convertFromImage(img);
	}
	//slice_pixmap->resize(pixmapitem->pixmap().width(), pixmapitem->pixmap().height());
	//fitInView(pixmapitem, Qt::KeepAspectRatio);

	pixmapitem->setPixmap(*pixmap);
	//centreonView();
	//draw();
}
void QtSliceFrame::sliceframe_drawmode_slot(int mode)
{
	draw_mode = mode;
	//printf("mode: %d\n", mode);
}
void QtSliceFrame::draw(void)
{
	QRectF rect = scene->sceneRect();

	if(mode==0)
	{
		if(cursor_on==true)
		{
			//printf("mode: %d\n", mode);
			lineitem1->setPen(QPen(Qt::green));
			lineitem1->setLine(0*resizeX, y*resizeY, rect.width(), y*resizeY);
			lineitem2->setPen(QPen(Qt::blue));
			lineitem2->setLine(x*resizeX, 0*resizeY, x*resizeX, rect.height());
		}
	}
	if(mode==1 && cursor_on==true)
	{
		if(cursor_on==true)
		{
			//printf("mode: %d\n", mode);
			lineitem1->setPen(QPen(Qt::red));
			lineitem1->setLine(0*resizeX, z*resizeY, rect.width(),z*resizeY);
			lineitem2->setPen(QPen(Qt::blue));
			lineitem2->setLine(x*resizeX, 0*resizeY, x*resizeX, rect.height());
		}
	}
	if(mode==2 && cursor_on==true)			
	{
		if(cursor_on==true)
		{
			//printf("mode: %d\n", mode);
			lineitem1->setPen(QPen(Qt::red));
			lineitem1->setLine(0*resizeX, z*resizeY, rect.width(),z*resizeY);
			lineitem2->setPen(QPen(Qt::green));
			lineitem2->setLine(y*resizeX, 0*resizeY, y*resizeX, rect.height());
		}
	}
	//update();
}
void QtSliceFrame::centreonView(void)
{
	QPointF p0;

	if(mode==0)
	{
		p0.setX(x);
		p0.setY(y);
	}
	if(mode==1)
	{
		p0.setX(x);
		p0.setY(z);
	}
	if(mode==2)
	{
		p0.setX(y);
		p0.setY(z);
	}

	//QPointF p = mapFromScene(p0);
	centerOn(p0);
}
void QtSliceFrame::sliceframe_addlabel_slot(Vector v)
{
	brushsize = v.w;

	if(draw_mode==1)
	{
		int index=0;
		if(mode==0) index = v.z;
		else if(mode==1) index = v.y; 
		else if(mode==2) index = v.x; 
	
		for(int i=0; i<brushsize; i++)
		{
			if(i==0)
			{
				bg_mflowLabels[index].push_back(Vector(v.x,v.y,v.z,brushsize+i));
			}
			else
			{
				if(dimension_mode==1)
				{
					if(mode==0)
					{
						bg_mflowLabels[index-i].push_back(Vector(v.x,v.y,v.z-i,brushsize-i));
						bg_mflowLabels[index+i].push_back(Vector(v.x,v.y,v.z+i,brushsize-i));
					}
					else if(mode==1)
					{
						bg_mflowLabels[index-i].push_back(Vector(v.x,v.y-i,v.z,brushsize-i));
						bg_mflowLabels[index+i].push_back(Vector(v.x,v.y+i,v.z,brushsize-i));
					}
					else if(mode==2)
					{
						bg_mflowLabels[index-i].push_back(Vector(v.x-i,v.y,v.z,brushsize-i));
						bg_mflowLabels[index+i].push_back(Vector(v.x+i,v.y,v.z,brushsize-i));
					}
				}
			}
		}
	}
	else if(draw_mode==2)
	{
		
		int index=0;
		if(mode==0) index = v.z;
		else if(mode==1) index = v.y; 
		else if(mode==2) index = v.x; 
		
		for(int i=0; i<brushsize; i++)
		{
			if(i==0)
			{
				fg_mflowLabels[index].push_back(Vector(v.x,v.y,v.z,brushsize+i));
			}
			else
			{
				if(dimension_mode==1)
				{
					if(mode==0)
					{
						fg_mflowLabels[index-i].push_back(Vector(v.x,v.y,v.z-i,brushsize-i));
						fg_mflowLabels[index+i].push_back(Vector(v.x,v.y,v.z+i,brushsize-i));
					}
					else if(mode==1)
					{
						fg_mflowLabels[index-i].push_back(Vector(v.x,v.y-i,v.z,brushsize-i));
						fg_mflowLabels[index+i].push_back(Vector(v.x,v.y+i,v.z,brushsize-i));
					}
					else if(mode==2)
					{
						fg_mflowLabels[index-i].push_back(Vector(v.x-i,v.y,v.z,brushsize-i));
						fg_mflowLabels[index+i].push_back(Vector(v.x+i,v.y,v.z,brushsize-i));
					}
				}
			}
		}
	}
}
void QtSliceFrame::computeXYZ(int xmouse, int ymouse)
{
	QPointF p0 = QPointF(xmouse, ymouse);
	QPointF p = mapToScene(p0.toPoint());

	if(mode==0)
	{
		x = p.x()/resizeX;
		y = p.y()/resizeY;
	}
	if(mode==1)
	{
		x = p.x()/resizeX;
		z = p.y()/resizeY;
	}
	if(mode==2)
	{
		y = p.x()/resizeX;
		z = p.y()/resizeY;
	}

	if(x<0) x = 0;
	if(y<0) y = 0;
	if(z<0) z = 0;
	if(x>xres-1) x = xres-1;
	if(y>yres-1) y = yres-1;
	if(z>zres-1) z = zres-1;

	Vector v = Vector(x,y,z, brushsize);

	emit sliceframe_addlabel_signal(draw_mode, v);
	emit sliceframe_pixel_signal(x,y,z, mode);
}
void QtSliceFrame::clearLabels(void)
{
	printf("clearLabels\n");

	printf("fg_mflowLabels.size(): %d\n", fg_mflowLabels.size());

	for( Labels::iterator ii=fg_mflowLabels.begin(); ii!=fg_mflowLabels.end(); ++ii)
	{
		printf("clear: %d\n", (*ii).second.size());
		(*ii).second.clear();
		printf("clear: %d\n", (*ii).second.size());
	}
	fg_mflowLabels.clear();

	for( Labels::iterator ii=bg_mflowLabels.begin(); ii!=bg_mflowLabels.end(); ++ii)
	{
		(*ii).second.clear();
	}
	bg_mflowLabels.clear();

	printf("done\n");
//	updatePixmap(slice_pixmap);
}
void QtSliceFrame::updateLabels(void)
{
	if(fg_mflowLabels.empty()) return;

	if(mode==0)
	{
		QPointF p0;
		selection_painter.begin(slice_pixmap);
		selection_painter.setPen(QPen(Qt::green));
		selection_painter.setBrush(QBrush(Qt::green));
		for(int i=0; i<fg_mflowLabels[z].size(); i++)
		{
			Vector v = fg_mflowLabels[z][i];
			p0.setX(v.x*resizeX);
			p0.setY(v.y*resizeY);		
			//selection_painter.drawPoint(p0);
			selection_painter.drawRect(p0.x()-(v.w/2.0f), p0.y()-(v.w/2.0f), v.w, v.w);
		}
   		selection_painter.end();
	}
	else if(mode==1)
	{
		QPointF p0;
		selection_painter.begin(slice_pixmap);
		selection_painter.setPen(QPen(Qt::green));
		selection_painter.setBrush(QBrush(Qt::green));		
		for(int i=0; i<fg_mflowLabels[y].size(); i++)
		{
			Vector v = fg_mflowLabels[y][i];
			p0.setX(v.x*resizeX);
			p0.setY(v.z*resizeY);			
			//selection_painter.drawPoint(p0);
			selection_painter.drawRect(p0.x()-(v.w/2.0f), p0.y()-(v.w/2.0f), v.w, v.w);
		}
   		selection_painter.end();
	}
	else if(mode==2)
	{
		QPointF p0;
		selection_painter.begin(slice_pixmap);
		selection_painter.setPen(QPen(Qt::green));
		selection_painter.setBrush(QBrush(Qt::green));
		for(int i=0; i<fg_mflowLabels[x].size(); i++)
		{
			Vector v = fg_mflowLabels[x][i];
			p0.setX(v.y*resizeX);
			p0.setY(v.z*resizeY);
			//selection_painter.drawPoint(p0);
			selection_painter.drawRect(p0.x()-(v.w/2.0f), p0.y()-(v.w/2.0f), v.w, v.w);
		}
   		selection_painter.end();
	}

	if(bg_mflowLabels.empty()) return;

	if(mode==0)
	{
		QPointF p0;
		selection_painter.begin(slice_pixmap);
		selection_painter.setPen(QPen(Qt::red));
		selection_painter.setBrush(QBrush(Qt::red));
		for(int i=0; i<bg_mflowLabels[z].size(); i++)
		{
			Vector v = bg_mflowLabels[z][i];
			p0.setX(v.x*resizeX);
			p0.setY(v.y*resizeY);		
			//selection_painter.drawPoint(p0);
			selection_painter.drawRect(p0.x()-(v.w/2.0f), p0.y()-(v.w/2.0f), v.w, v.w);
		}
   		selection_painter.end();
	}
	else if(mode==1)
	{
		QPointF p0;
		selection_painter.begin(slice_pixmap);
		selection_painter.setPen(QPen(Qt::red));
		selection_painter.setBrush(QBrush(Qt::red));
		for(int i=0; i<bg_mflowLabels[y].size(); i++)
		{
			Vector v = bg_mflowLabels[y][i];
			p0.setX(v.x*resizeX);
			p0.setY(v.z*resizeY);			
			//selection_painter.drawPoint(p0);
			selection_painter.drawRect(p0.x()-(v.w/2.0f), p0.y()-(v.w/2.0f), v.w, v.w);

		}
   		selection_painter.end();
	}
	else if(mode==2)
	{
		QPointF p0;
		selection_painter.begin(slice_pixmap);
		selection_painter.setPen(QPen(Qt::red));
		selection_painter.setBrush(QBrush(Qt::red));
		for(int i=0; i<bg_mflowLabels[x].size(); i++)
		{
			Vector v = bg_mflowLabels[x][i];
			p0.setX(v.y*resizeX);
			p0.setY(v.z*resizeY);
			//selection_painter.drawPoint(p0);
			selection_painter.drawRect(p0.x()-(v.w/2.0f), p0.y()-(v.w/2.0f), v.w, v.w);
		}
   		selection_painter.end();
	}
	//updatePixmap(slice_pixmap);
}
void QtSliceFrame::mouseMoveEvent  ( QMouseEvent * e )
{
	mouse_x = e->x();
	mouse_y = e->y();

	if(draw_mode==-1)
	{
		return;
	}

	//if ((e->button()&Qt::LeftButton)) 
	if(click_on)
	{ 
		computeXYZ(mouse_x, mouse_y);

		QPointF p0;

		if(mode==0)
		{
			p0.setX(x*resizeX);
			p0.setY(y*resizeY);
		}
		if(mode==1)
		{
			p0.setX(x*resizeX);
			p0.setY(z*resizeY);
		}
		if(mode==2)
		{
			p0.setX(y*resizeX);
			p0.setY(z*resizeY);
		}

		selection_painter.begin(slice_pixmap);
		if(draw_mode==1)
		{
			selection_painter.setPen(QPen(Qt::red));
			selection_painter.setBrush(QBrush(Qt::red));
		}
		else
		{
			selection_painter.setPen(QPen(Qt::green));
			selection_painter.setBrush(QBrush(Qt::green));
		}
		//selection_painter.drawPoint(p0);
		selection_painter.drawRect(p0.x()-(brushsize/2.0f), p0.y()-(brushsize/2.0f), brushsize, brushsize);
 		selection_painter.end();

		updatePixmap(slice_pixmap);
/*
		float imgX, imgY;
		int r,g,b;
		imgX = (float)mouse_x/width() * (float)slice_pixmap->width();
		imgY = (float)mouse_y/height() * (float)slice_pixmap->height();
		QImage img = slice_pixmap->convertToImage();
		QRgb pixel = img.pixel((int)imgX+0.5,(int)imgY+0.5);	
		r = qRed(pixel);
		g = qGreen(pixel);
		b = qBlue(pixel);
		printf("pixel value: %d\n", (int)r);
		img.reset();
*/
/*
		oldx = x;
		oldy = y;
		oldz = z;

		//let frame know its the seleceted one
		selected = 1;
		//and tell all others about it
		emit sliceframe_selected_signal(mode);
*/
		update();
	}
}

void QtSliceFrame::mousePressEvent  ( QMouseEvent * e )
{	
	mouse_x = e->x();
	mouse_y = e->y();

	//printf("mouse: %d, %d\n", mouse_x, mouse_y);
	
	float r,g,b;

	if ((e->button()&Qt::LeftButton)) 
	{ 
		click_on=1;
		computeXYZ(mouse_x, mouse_y);

	/*	float imgX, imgY;
		int r,g,b;
		imgX = (float)mouse_x/width() * (float)slice_pixmap->width();
		imgY = (float)mouse_y/height() * (float)slice_pixmap->height();
		QImage img = slice_pixmap->convertToImage();
		QRgb pixel = img.pixel((int)imgX+0.5,(int)imgY+0.5);	
		r = qRed(pixel);
		g = qGreen(pixel);
		b = qBlue(pixel);
		printf("pixel value: %d\n", (int)r);
		img.reset();*/

/*
		oldx = x;
		oldy = y;
		oldz = z;

		//let frame know its the seleceted one
		selected = 1;
		//and tell all others about it
		emit sliceframe_selected_signal(mode);
*/
		update();
	}
	else if ((e->button()&Qt::MidButton)) 
	{
	}
	else if ((e->button()&Qt::RightButton)) 
	{
	}
}
void QtSliceFrame::resizeEvent(QResizeEvent* e)
{
	/*
    //Get the rectangle of the visible area in scene coords
    QRectF visibleArea = mapToScene(rect()).boundingRect();   

	const QSize& oldSize = e->oldSize();
	const QSize& newSize = e->size();

	qreal scaleFactorX;
	qreal scaleFactorY;
	
	if( oldSize.width()*oldSize.height() < newSize.width()*newSize.height())
	{
		//be aware of integer division and division by zero
		scaleFactorX = (oldSize.width ()>0) ? ((double)newSize.width())/oldSize.width ():1;
		scaleFactorY = (oldSize.height()>0) ?((double)newSize.height())/oldSize.height():1;
		scaleFactorX = scaleFactorY = qMax(scaleFactorX, scaleFactorY);
	}
	else
	{
		//be aware of integer division and division by zero
		scaleFactorX = (oldSize.width ()>0) ? ((double)newSize.width())/oldSize.width ():1;
		scaleFactorY = (oldSize.height()>0) ?((double)newSize.height())/oldSize.height():1;
		scaleFactorX = scaleFactorY = qMin(scaleFactorX, scaleFactorY);
	}

	/*if(scaleFactorX==1.0)
	{
		scaleFactorX = scaleFactorY;
	}
	else if(scaleFactorY==1.0)
	{
		scaleFactorY = scaleFactorX;
	}*/
	/*else
	{
		scaleFactorX = scaleFactorY = qMin(scaleFactorX, scaleFactorY);
	}
*/
	//printf("scaleFactorX: %f\n", (float)scaleFactorX);

	//scale the view
//	scale(scaleFactorX, scaleFactorY);

	fitInView(pixmapitem, Qt::KeepAspectRatio);
	scale(zoomlevel, zoomlevel);
	centreonView();

	//centreonView();
	//SetCenter(visibleArea.center());
 
    //Call the subclass resize so the scrollbars are updated correctly
	//QGraphicsView::resizeEvent(e);
}
void QtSliceFrame::mouseReleaseEvent(QMouseEvent * e )
{	
	if(click_on==1) click_on=0;
}

void QtSliceFrame::sliceframe_selected_slot(int current)
{
	selected = 0;
	oldx = -1;
	oldy = -1;
	oldz = -1;
	update();
}
void QtSliceFrame::sliceframe_zoomin_slot(void)
{
	//printf("zoomlevel: %d\n",zoomlevel);
	//zoomlevel++;
	zoomlevel*=1.2;

	fitInView(pixmapitem, Qt::KeepAspectRatio);
	scale(zoomlevel, zoomlevel);
	centreonView();
	update();
}
void QtSliceFrame::sliceframe_zoomout_slot(void)
{
	//printf("zoomlevel: %d\n",zoomlevel);
	//zoomlevel--;

	zoomlevel*=0.8;
	if(zoomlevel<0.2) zoomlevel=0.2;

	fitInView(pixmapitem, Qt::KeepAspectRatio);
	scale(zoomlevel, zoomlevel);
	centreonView();
	update();
}

void QtSliceFrame::sliceframe_zoomreset_slot(void)
{
	zoomlevel=1.0;
	scale(1.0, 1.0);
	centerOn(pixmapitem);
	fitInView(pixmapitem, Qt::KeepAspectRatio);
	//update();
}
void QtSliceFrame::sliceframe_sectioncursor_slot(void)
{
//	printf("CURSOR TOGGLE\n");
	if(cursor_on)
	{
		cursor_on = false;
		lineitem1->hide();
		lineitem2->hide();
	}
	else
	{
		cursor_on = true;
		lineitem1->show();
		lineitem2->show();
	}

	update();
}
void QtSliceFrame::sliceframe_sectioninvert_slot(void)
{
	invert_on = !invert_on;

	QImage img = slice_pixmap->convertToImage();
	img.invertPixels();

	slice_pixmap->convertFromImage(img);
	pixmapitem->setPixmap(*slice_pixmap);
	update();

	//	pixmapitem->pixmap().convertToImage().invertPixels();
}
