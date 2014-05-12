/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#define max(a,b) (((a)>(b))?(a):(b)) 

#include "QtHistogram.h"
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <Q3Frame>
#include <QPixmap>
#include <QLabel>
#include <QMouseEvent>
#include <QEvent>

QtHistogram::QtHistogram(QWidget* parent, const char* name, float size): QLabel( parent, name )
{
	currentworkingdir="";
	
	max_histogram = 0;
	data=NULL;
	
	hist_enable=1;
	cumfreq_enable=0;
	tfunc_enable=1;
	maxthr=minthr=0;

	logscale=0;

//	if(width()<height()) size = width();
//		else size = height();

	graph_size = size;
	canvas_size = size*1.25;
	border = (canvas_size-graph_size)/2.0;

	setFrameStyle(QFrame::Panel | QFrame::Sunken);
	setAlignment(Qt::AlignCenter);
	setMouseTracking(true);
	//setScaledContents(false);

	/*QSizePolicy sizepolicy;
	sizepolicy.setVerticalPolicy(QSizePolicy::Expanding);
	sizepolicy.setHorizontalPolicy(QSizePolicy::Expanding);
	sizepolicy.setHeightForWidth(true);
	setSizePolicy(sizepolicy);

	resize(384,384);*/
	//setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

	slice_toggle = 0;
	create();
	
}

QtHistogram::~QtHistogram()
{
	data=NULL;
}
void QtHistogram::reset(void)
{
	max_histogram = 0;
	
	hist_enable=1;
	cumfreq_enable=0;
	tfunc_enable=1;
	maxthr=minthr=0;

	luminance.reset();
	alpha.reset();
	redluminance.reset();
	redalpha.reset();
	greenluminance.reset();
	greenalpha.reset();
	blueluminance.reset();
	bluealpha.reset();

	for(int i=0; i<256; i++) data[i]=0;
}
void QtHistogram::reload_transfer_function_parameters(void)
{
	printf("reload_transfer_function_parameters\n");
/*	//CHANGE!
	emit hist_reset_transfer_function_parameters_signal(redluminance.threshold_min, redluminance.threshold_max, greenluminance.threshold_min, greenluminance.threshold_max, blueluminance.threshold_min, blueluminance.threshold_max, redalpha.threshold_min, redalpha.threshold_max, greenalpha.threshold_min, greenalpha.threshold_max, bluealpha.threshold_min, bluealpha.threshold_max,
													   redluminance.brightness, redluminance.contrast, greenluminance.brightness, greenluminance.contrast, blueluminance.brightness, blueluminance.contrast, redalpha.brightness, redalpha.contrast, greenalpha.brightness, greenalpha.contrast, bluealpha.brightness, bluealpha.contrast,
													   false, false, false, false, false, false);
*/
}

void QtHistogram::reset_l(void)
{
	max_histogram = 0;
	
	hist_enable=1;
	cumfreq_enable=0;
	tfunc_enable=1;
	maxthr=minthr=0;

	luminance.reset();
	redluminance.reset();
	blueluminance.reset();
	greenluminance.reset();
}
void QtHistogram::reset_la(void)
{
	max_histogram = 0;
	
	hist_enable=1;
	cumfreq_enable=0;
	tfunc_enable=1;
	maxthr=minthr=0;

	alpha.reset();
	redalpha.reset();
	bluealpha.reset();
	greenalpha.reset();
}
void QtHistogram::reset_r(void)
{
	max_histogram = 0;
	
	hist_enable=1;
	cumfreq_enable=0;
	tfunc_enable=1;
	maxthr=minthr=0;

	redluminance.reset();
}
void QtHistogram::reset_g(void)
{
	max_histogram = 0;
	
	hist_enable=1;
	cumfreq_enable=0;
	tfunc_enable=1;
	maxthr=minthr=0;

	greenluminance.reset();
}
void QtHistogram::reset_b(void)
{
	max_histogram = 0;
	
	hist_enable=1;
	cumfreq_enable=0;
	tfunc_enable=1;
	maxthr=minthr=0;

	blueluminance.reset();
}
void QtHistogram::reset_ra(void)
{
	max_histogram = 0;
	
	hist_enable=1;
	cumfreq_enable=0;
	tfunc_enable=1;
	maxthr=minthr=0;

	redalpha.reset();
}
void QtHistogram::reset_ga(void)
{
	max_histogram = 0;
	
	hist_enable=1;
	cumfreq_enable=0;
	tfunc_enable=1;
	maxthr=minthr=0;

	greenalpha.reset();
}
void QtHistogram::reset_ba(void)
{
	max_histogram = 0;
	
	hist_enable=1;
	cumfreq_enable=0;
	tfunc_enable=1;
	maxthr=minthr=0;

	bluealpha.reset();
}

void QtHistogram::create(void)
{
/*    setFrameStyle( Q3Frame::Panel );
	setLineWidth( 2 );
	setAlignment( Qt::AlignCenter );
	setMargin(0);
	setScaledContents(TRUE);
 */
	//printf("graph_size: %d\n", (int)graph_size);

	luminance.init(graph_size, 0, 10, 100, 100, 0, 255);
	redluminance.init(graph_size, 0, 10, 100, 100, 0, 255);
	greenluminance.init(graph_size, 0, 10, 100, 100, 0, 255);
	blueluminance.init(graph_size, 0, 10, 100, 100, 0, 255);

	alpha.init(graph_size, 0, 10, 100, 100, 0, 255);
	redalpha.init(graph_size, 0, 10, 100, 100, 0, 255);
	greenalpha.init(graph_size, 0, 10, 100, 100, 0, 255);
	bluealpha.init(graph_size, 0, 10, 100, 100, 0, 255);
}
void QtHistogram::set_transfer_mode(int mode)
{
	fe_seleted=mode;
	//draw_transfer();
}
void QtHistogram::draw_functioneditoronpixmap(QPainter* p, FunctionEditor* func, QColor col)
{
	//Input image map
	/*
	QImage strip_img = QImage(255, 10, QImage::Format_RGB32);
	for(int i=0; i<strip_img.width(); i++)
	{
		for(int j=0; j<strip_img.height(); j++)
		{
			strip_img.setPixel(i, j, qRgb(i,i,i));
		}
	}
	p->drawImage(QRectF(0,-border*0.5,255, 10), strip_img);
	*/
/*
	QImage strip_img = QImage(255, 10, QImage::Format_RGB32);
	for(int i=0; i<strip_img.width(); i++)
	{
		for(int j=0; j<strip_img.height(); j++)
		{
			int p = 256-(float)func->bs.curve_points[i].y;

			QColor c = QColor(p,p,p);
			c.setRed(c.red() * (col.red()/255.0));
			c.setGreen(c.green() * (col.green()/255.0));
			c.setBlue(c.blue() * (col.blue()/255.0));

			strip_img.setPixel(i, j, qRgb(c.red(), c.green(), c.blue()));
		}
	}
	p->drawImage(QRectF(0,-border*0.5,255, 10), strip_img);
	//p->drawImage(QRectF(255+border*0.125,0,10, 255), strip_img);
*/
	if(col==QColor(Qt::white))
	{
		col=QColor(Qt::darkGray);
	}
	//draw our control points line
	QPainterPath cplinepath;
	cplinepath.moveTo(func->bs.control_points[0].x, func->bs.control_points[0].y);
	for(int i=0; i<func->bs.control_points.size(); i++)
	{		 
		cplinepath.lineTo(func->bs.control_points[i].x, func->bs.control_points[i].y);		
	}
	p->setPen( QPen(col, 1, Qt::DashLine, Qt::FlatCap, Qt::RoundJoin) );
	p->drawPath(cplinepath);
	
	//draw our control points
	QPainterPath cppath;
	cppath.moveTo(func->bs.control_points[0].x, func->bs.control_points[0].y);
	for(int i=0; i<func->bs.control_points.size(); i++)
	{	
		cppath.moveTo(func->bs.control_points[i].x, func->bs.control_points[i].y);
		//cppath.addRect( func->bs.control_points[i].x-2.5, func->bs.control_points[i].y-2.5, 5, 5);
	}
	p->setBrush(col);
	p->setPen( QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin) );
	p->drawPath(cppath);
	p->setBrush(Qt::NoBrush);

	for(int i=0; i<func->bs.control_points.size(); i++)
	{
		QColor c = col;
/*			QColor(255-func->bs.control_points_colours[i].x, 255-func->bs.control_points_colours[i].y, 255-func->bs.control_points_colours[i].z);
		c.setRed(c.red() * (col.red()/255.0));
		c.setGreen(c.green() * (col.green()/255.0));
		c.setBlue(c.blue() * (col.blue()/255.0));
*/		
		p->setBrush(c);
		p->setPen(c);
		p->drawRect(func->bs.control_points[i].x-2.5, func->bs.control_points[i].y-2.5, 5, 5);
	}
	p->setBrush(col);
	p->setPen( QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin) );
	p->setBrush(Qt::NoBrush);

	//if(i==func->selected)

	if(func->delete_event==1)
	{
		p->setPen(Qt::yellow);
		p->drawRect( func->v.x-2.5, func->v.y-2.5, 5, 5);		
	}

	//draw our curve as line segments
	QPainterPath curvepath;
	curvepath.moveTo(func->bs.curve_points[0].x, func->bs.curve_points[0].y);
	for(int ij=0; ij<func->bs.curve_points.size(); ij++)
	{		 
		curvepath.lineTo(func->bs.curve_points[ij].x, func->bs.curve_points[ij].y);
	}

	//printf("%f\n", func->bs.curve_points[0].y);

	p->setPen( QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin) );
	p->drawPath(curvepath);
}
void QtHistogram::draw_functioneditor(QPainter* p)
{
	//draw our histogram
	draw_histogram(p);

	if(tfunc_enable)
	{
		if(fe_seleted==1)
		{
			draw_functioneditoronpixmap(p, &luminance.functioneditor, Qt::white);
		}
		else if(fe_seleted==2)
		{
			draw_functioneditoronpixmap(p, &alpha.functioneditor, Qt::white);
		}
		else if(fe_seleted==3)
		{
			draw_functioneditoronpixmap(p, &redluminance.functioneditor, QColor(100,0,0, 255));
		}
		else if(fe_seleted==4)
		{
			draw_functioneditoronpixmap(p, &greenluminance.functioneditor, QColor(0,100,0, 255));
		}
		else if(fe_seleted==5)
		{
			draw_functioneditoronpixmap(p, &blueluminance.functioneditor, QColor(0,0,100, 255));
		}
		else if(fe_seleted==6)
		{
			draw_functioneditoronpixmap(p, &redalpha.functioneditor, Qt::red);
		}
		else if(fe_seleted==7)
		{
			draw_functioneditoronpixmap(p, &greenalpha.functioneditor, Qt::green);
		}
		else if(fe_seleted==8)
		{
			draw_functioneditoronpixmap(p, &bluealpha.functioneditor, Qt::blue);
		}	
	}
}
void QtHistogram::draw_histogram(QPainter* p)
{	
	//printf("DRAW HISTO\n");
	//find the max value in our histogram
	max_histogram=0;
	float max_loghisto =0;
	float min_loghisto = log(1024.0*1024.0*1024.0);
	for(int iii=1; iii<255; iii++) 
	{
		max_histogram = max(max_histogram, data[iii]);
		if(data[iii]!=0)
		{
			max_loghisto = max((float)max_loghisto, (float)log(data[iii]));
			min_loghisto= min((float)min_loghisto, (float)log(data[iii]));
		}
		//printf("%d\n", data[iii]);
	}

	//draw a background rect
	p->setPen(Qt::NoPen);

	p->setBrush(Qt::white);
	p->drawRect(0,0,graph_size-1, graph_size-1);

	//reset our painter's brush
	p->setBrush(Qt::NoBrush);

	//draw a grid
	p->setPen(Qt::black);
	QFont serifFont("Arial", 6, QFont::Bold);
	p->setFont(serifFont);
	float grid_steps = (graph_size-1)/8.0;
		
	float grid_step=0;
	float loggrid_step=0;
	float logrid_stepincr=max_histogram/8.0;

	QString label;
	for(int ii=0; ii<9; ii++)
	{
		//linear scale
		if(logscale==0)
		{
			//x,y grid lines
			p->drawLine(grid_step, 0, grid_step, graph_size-1+border*0.25);
			p->drawLine(-border*0.25, grid_step, graph_size-1, grid_step);

			//x,y labels
			label.setNum((int)(grid_step),10);
			p->drawText( QRectF(grid_step-grid_steps/2.0, graph_size-1+border*0.4, grid_steps, 8), Qt::AlignCenter, label);
			p->drawText( QRectF(-border, graph_size-1-grid_step-4, grid_steps, 8), Qt::AlignCenter, label);
		}
		else
		{	
			//x,y grid lines

			//Y AXIS LINES
			p->drawLine(grid_step, 0, grid_step, graph_size+border*0.25);
			label.setNum((int)(grid_step),10);
			p->drawText( QRectF(grid_step-grid_steps/2.0, graph_size-1+border*0.4, grid_steps, 8), Qt::AlignCenter, label);

			p->drawText( QRectF(-border, graph_size-1-grid_step-4, grid_steps, 8), Qt::AlignCenter, label);
			p->drawLine(-border*0.25, grid_step, graph_size-1, grid_step);
			/*//X AXIS LINES
			double height;
			int i;
			float iterp=0;

			if(grid_step!=0) iterp = (double)(log(loggrid_step)-min_loghisto)/(double)(max_loghisto-min_loghisto);
			else iterp = 0;

			if(iterp>1.0) iterp=1.0;
			if(iterp<0.0) iterp=0.0;

			height = (graph_size)*iterp;
			height = (int)(height+0.5);

			p->drawLine(-border*0.25, height, graph_size, height);
						
			label.setNum((int)loggrid_step,10);

			p->drawText( QRectF(-border, graph_size-height-4, logrid_stepincr, 8), Qt::AlignCenter, label);*/
		}
//		p->drawRect(QRectF(grid_step-grid_steps/2.0, graph_size+border*0.4, grid_steps, 8));
//		p->drawRect(QRectF(-border, graph_size-grid_step-4, grid_steps, 8));

		grid_step+=grid_steps;
		//loggrid_step+=logrid_stepincr;
	}

	if(hist_enable==1)
	{
		//dray our histogram bars
		//editor->fe_seleted
		// 1 = RGB// 2 = RGBA// 3 = R// 4 = G// 5 = B// 6 = A
		p->setBrush(Qt::NoBrush);
		p->setPen(Qt::NoPen);

		if(fe_seleted==1) p->setBrush( QColor(32,32,32, 255) );
		else if(fe_seleted==2) p->setBrush( QColor(64,64,64, 255) );
		else if(fe_seleted==3) p->setBrush( QColor(255,0,0, 255) );
		else if(fe_seleted==4) p->setBrush( QColor(0,200,0, 255) );
		else if(fe_seleted==5) p->setBrush( QColor(0,0,255, 255) );
		else if(fe_seleted==6) p->setBrush( QColor(100,0,0, 255) );
		else if(fe_seleted==7) p->setBrush( QColor(0,100,0, 255) );
		else if(fe_seleted==8) p->setBrush( QColor(0,0,100, 255) );
		
		double height;
		int i;
		float xpos=0.0;
	
		//QPolygonF polygon;
		float iterp=0;

		if(max_histogram==0) max_histogram=1;

		for(i=0 ; i<256; i++)
		{
			//linear scale
			if(logscale==0) iterp = ((double)data[i]/(double)max_histogram);
			else
			{
				if(data[i]!=0) iterp = (double)(log(data[i])-min_loghisto)/(double)(max_loghisto-min_loghisto);
				else iterp = 0;
			}

//			printf("data[i]: %f\n", (float)data[i]);
//			printf("max_histogram: %f\n", (float)max_histogram);
//			printf("LOGdata[i]: %f\n", (float)log(data[i]));
//			printf("max_loghisto: %f\n", (float)max_loghisto);
//			printf("min_loghisto: %f\n", (float)min_loghisto);
//			printf("iterp: %f\n", (float)iterp);

			if(iterp>1.0) iterp=1.0;
			if(iterp<0.0) iterp=0.0;

			height = (graph_size)*iterp;
			height = (int)(height+0.5);
			
			//printf("%f\n", (float)height);
			//if(height>graph_size) height=graph_size;
			//if(height<0.0) height=0.0;
			//if(i==0) height=0.0;
			//if(i==255) height=0.0;

			xpos = i;

			//polygon << QPointF(xpos, graph_size-height) ;
			QRectF rectangle((float)xpos-0.5, graph_size, 1.0, -height);
			p->drawRect(rectangle);

			//p->drawRoundRect(rectangle, 25, 25);

			//p->drawRect(xpos, 0.0, 1.0, 128);
			//p->drawLine(xpos, graph_size, xpos, graph_size-height);
		}

		//p->drawPolygon(polygon, Qt::OddEvenFill);
	}
	if(cumfreq_enable==1)
	{
		//draw our cummulatative frequency
		//dray our histogram bars
		double sum =0;
		double maxsum =0;
		double height;
		for(int i=0 ; i<256; i++)
		{
			maxsum += data[i];
		}

		if(maxsum)
		{
			QPainterPath cumpath;
			cumpath.moveTo(0,0);
			for(int i=0 ; i<256; i++)
			{
				sum += data[i];
				height = (graph_size)*(((double)sum/(double)maxsum));
				height = 255-(int)(height+0.5);
				cumpath.lineTo (i, height );
			}
			p->setPen(Qt::red);
			p->setBrush(Qt::NoBrush);

			//p->setBrush(Qt::red);
			p->drawPath(cumpath);
		}
	}

	//draw our thresholding...
	if(fe_seleted==1)
	{
		p->setPen(Qt::black);
		p->setBrush(QBrush ( QColor(128,128,128,128), Qt::SolidPattern ));
		QRectF rectangle(0.0, 0.0, luminance.threshold_min, graph_size-1);
		p->drawRect(rectangle);

		QRectF rectangle2(luminance.threshold_max, 0.0, graph_size-1-luminance.threshold_max, graph_size-1);
		p->drawRect(rectangle2);
	}
	else if(fe_seleted==2)
	{
		p->setPen(Qt::black);
		p->setBrush(QBrush ( QColor(128,128,128,128), Qt::SolidPattern ));
		QRectF rectangle(0.0, 0.0, alpha.threshold_min, graph_size-1);
		p->drawRect(rectangle);

		QRectF rectangle2(alpha.threshold_max, 0.0, graph_size-1-alpha.threshold_max, graph_size-1);
		p->drawRect(rectangle2);
	}
	else if(fe_seleted==3)
	{
		p->setPen(Qt::red);
		p->setBrush(QBrush ( QColor(255,0,0,128), Qt::SolidPattern ));
		QRectF rectangle(0.0, 0.0, redluminance.threshold_min, graph_size-1);
		p->drawRect(rectangle);

		QRectF rectangle2(redluminance.threshold_max, 0.0, graph_size-1-redluminance.threshold_max, graph_size-1);
		p->drawRect(rectangle2);
	}
	else if(fe_seleted==4)
	{
		p->setPen(Qt::green);
		p->setBrush(QBrush ( QColor(0,255,0,128), Qt::SolidPattern ));
		QRectF rectangle(0.0, 0.0, greenluminance.threshold_min, graph_size-1);
		p->drawRect(rectangle);

		QRectF rectangle2(greenluminance.threshold_max, 0.0, graph_size-1-greenluminance.threshold_max, graph_size-1);
		p->drawRect(rectangle2);
	}
	else if(fe_seleted==5)
	{
		p->setPen(Qt::blue);
		p->setBrush(QBrush ( QColor(0,0,255,128), Qt::SolidPattern ));
		QRectF rectangle(0.0, 0.0, blueluminance.threshold_min, graph_size-1);
		p->drawRect(rectangle);

		QRectF rectangle2(blueluminance.threshold_max, 0.0, graph_size-1-blueluminance.threshold_max, graph_size-1);
		p->drawRect(rectangle2);	
	}
	else if(fe_seleted==6)
	{
		p->setPen(Qt::red);
		p->setBrush(QBrush ( QColor(255,0,0,128), Qt::SolidPattern ));
		QRectF rectangle(0.0, 0.0, redalpha.threshold_min, graph_size-1);
		p->drawRect(rectangle);

		QRectF rectangle2(redalpha.threshold_max, 0.0, graph_size-1-redalpha.threshold_max, graph_size-1);
		p->drawRect(rectangle2);
	}
	else if(fe_seleted==7)
	{
		p->setPen(Qt::green);
		p->setBrush(QBrush ( QColor(0,255,0,128), Qt::SolidPattern ));
		QRectF rectangle(0.0, 0.0, greenalpha.threshold_min, graph_size-1);
		p->drawRect(rectangle);

		QRectF rectangle2(greenalpha.threshold_max, 0.0, graph_size-1-greenalpha.threshold_max, graph_size-1);
		p->drawRect(rectangle2);
	}
	else if(fe_seleted==8)
	{
		p->setPen(Qt::blue);
		p->setBrush(QBrush ( QColor(0,0,255,128), Qt::SolidPattern ));
		QRectF rectangle(0.0, 0.0, bluealpha.threshold_min, graph_size-1);
		p->drawRect(rectangle);

		QRectF rectangle2(bluealpha.threshold_max, 0.0, graph_size-1-bluealpha.threshold_max, graph_size-1);
		p->drawRect(rectangle2);
	}

	p->setBrush(Qt::NoBrush);
}
void QtHistogram::draw_transfer(void)
{
	//setPixmap(histogram_pm);
	update();
}
void QtHistogram::paintEvent( QPaintEvent *e )
{	
	QPainter pp(this);
	pp.setRenderHint(QPainter::Antialiasing, false);	

	//pp.scale((float)width()/(float)graph_size, (float)height()/(float)graph_size);

	pp.scale((float)width()/(float)canvas_size, (float)height()/(float)canvas_size);
	
	//Change to constructor, and QPallete SetBrush BackgroundRole
	pp.fillRect(QRect(0, 0, canvas_size, canvas_size), QBrush ( QColor(128,128,128,255), Qt::SolidPattern ));

	pp.translate(border,border);
	draw_functioneditor(&pp);
}
void QtHistogram::resizeEvent ( QResizeEvent *e )
{
//	setMinimumHeight(e->size().height());

//	setMinimumWidth(e->size().height());

//	setMaximumHeight(e->size().height());
//	setMaximumWidth(e->size().height());

//	printf("W, H: %d, %d\n", width(), height());

//	histogram_pm.resize(1, 1);
//	float oldsize = size;

/*	if(width()<height()) size = width();
		else size = height();
	
	graph_size=size;
*/
//	graph_size = size*0.7;
//	border = (size-graph_size)/2.0;

	//histogram_pm = histogram_pm.scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	//setPixmap(histogram_pm);
//	maxthr= ((float)maxthr/oldsize) * size;
//	minthr= ((float)minthr/oldsize) * size;;*/
}

void QtHistogram::mouseMoveEvent  ( QMouseEvent * e )
{
	if(!tfunc_enable) return;

	float x = (((float)(e->x())/(float)width())*canvas_size)-border;
	float y = (((float)(e->y())/(float)height())*canvas_size)-border;
	
	if(fe_seleted==1) if(luminance.functioneditor.selected!=-1|| luminance.functioneditor.delete_event==1) luminance.functioneditor.movePoint(x, y);
	if(fe_seleted==2) if(alpha.functioneditor.selected!=-1|| alpha.functioneditor.delete_event==1) alpha.functioneditor.movePoint(x, y);
	if(fe_seleted==3) if(redluminance.functioneditor.selected!=-1|| redluminance.functioneditor.delete_event==1) redluminance.functioneditor.movePoint(x, y);
	if(fe_seleted==4) if(greenluminance.functioneditor.selected!=-1|| greenluminance.functioneditor.delete_event==1) greenluminance.functioneditor.movePoint(x, y);
	if(fe_seleted==5) if(blueluminance.functioneditor.selected!=-1|| blueluminance.functioneditor.delete_event==1) blueluminance.functioneditor.movePoint(x, y);
	if(fe_seleted==6) if(redalpha.functioneditor.selected!=-1|| redalpha.functioneditor.delete_event==1) redalpha.functioneditor.movePoint(x, y);
	if(fe_seleted==7) if(greenalpha.functioneditor.selected!=-1|| greenalpha.functioneditor.delete_event==1) greenalpha.functioneditor.movePoint(x, y);
	if(fe_seleted==8) if(bluealpha.functioneditor.selected!=-1|| bluealpha.functioneditor.delete_event==1) bluealpha.functioneditor.movePoint(x, y);

	//for(int i=0; i<256; i++) 
	//printf("fe: %d \n", (int)alpha.functioneditor.bs.curve_points[0].y);

	//draw_transfer();
	update();
}
void QtHistogram::mouseDoubleClickEvent(QMouseEvent * e)
{
	if(!tfunc_enable) return;
	
	float x = (((float)(e->x())/(float)width())*canvas_size)-border;
	float y = (((float)(e->y())/(float)height())*canvas_size)-border;

	if ((e->button()==Qt::LeftButton)) 
	{
		if(fe_seleted==1) luminance.functioneditor.selectPoint(x, y);	
		else if(fe_seleted==2) alpha.functioneditor.selectPoint(x, y);
		else if(fe_seleted==3) redluminance.functioneditor.selectPoint(x, y);
		else if(fe_seleted==4) greenluminance.functioneditor.selectPoint(x, y);
		else if(fe_seleted==5) blueluminance.functioneditor.selectPoint(x, y);
		else if(fe_seleted==6) redalpha.functioneditor.selectPoint(x, y);
		else if(fe_seleted==7) greenalpha.functioneditor.selectPoint(x, y);
		else if(fe_seleted==8) bluealpha.functioneditor.selectPoint(x, y);
	}
}
void QtHistogram::mousePressEvent  ( QMouseEvent * e )
{
	if(!tfunc_enable) return;
	
	float x = (((float)(e->x())/(float)width())*canvas_size)-border;
	float y = (((float)(e->y())/(float)height())*canvas_size)-border;

	if ((e->button()==Qt::LeftButton)) 
	{
		if(fe_seleted==1) luminance.functioneditor.selectPoint(x, y);	
		else if(fe_seleted==2) alpha.functioneditor.selectPoint(x, y);
		else if(fe_seleted==3) redluminance.functioneditor.selectPoint(x, y);
		else if(fe_seleted==4) greenluminance.functioneditor.selectPoint(x, y);
		else if(fe_seleted==5) blueluminance.functioneditor.selectPoint(x, y);
		else if(fe_seleted==6) redalpha.functioneditor.selectPoint(x, y);
		else if(fe_seleted==7) greenalpha.functioneditor.selectPoint(x, y);
		else if(fe_seleted==8) bluealpha.functioneditor.selectPoint(x, y);
	}
	else if ((e->button()==Qt::MidButton)) 
	{
		if(fe_seleted==1)
		{
			if(luminance.functioneditor.selectPoint(x, y)) luminance.functioneditor.deletePoint();
		}
		if(fe_seleted==2)
		{
			if(alpha.functioneditor.selectPoint(x, y)) alpha.functioneditor.deletePoint();
		}
		if(fe_seleted==3)
		{
			if(redluminance.functioneditor.selectPoint(x, y)) redluminance.functioneditor.deletePoint();
		}
		if(fe_seleted==4)
		{
			if(greenluminance.functioneditor.selectPoint(x, y)) greenluminance.functioneditor.deletePoint();
		}
		if(fe_seleted==5)
		{
			if(blueluminance.functioneditor.selectPoint(x, y)) blueluminance.functioneditor.deletePoint();
		}
		if(fe_seleted==6)
		{
			if(redalpha.functioneditor.selectPoint(x, y)) redalpha.functioneditor.deletePoint();
		}
		if(fe_seleted==7)
		{
			if(greenalpha.functioneditor.selectPoint(x, y)) greenalpha.functioneditor.deletePoint();
		}		
		if(fe_seleted==8)
		{
			if(bluealpha.functioneditor.selectPoint(x, y)) bluealpha.functioneditor.deletePoint();
		}
	}
	else if ((e->button()==Qt::RightButton)) 
	{
		if(fe_seleted==1)
		{
			luminance.functioneditor.addPoint(x,y);
		}
		if(fe_seleted==2)
		{
			alpha.functioneditor.addPoint(x,y);
		}
		if(fe_seleted==3)
		{
			redluminance.functioneditor.addPoint(x,y);
		}
		if(fe_seleted==4)
		{
			greenluminance.functioneditor.addPoint(x,y);
		}
		if(fe_seleted==5)
		{
			blueluminance.functioneditor.addPoint(x,y);
		}
		if(fe_seleted==6)
		{
			redalpha.functioneditor.addPoint(x,y);
		}
		if(fe_seleted==7)
		{
			greenalpha.functioneditor.addPoint(x,y);
		}		
		if(fe_seleted==8)
		{
			bluealpha.functioneditor.addPoint(x,y);
		}	
		
		//draw_transfer();
	}
}
void QtHistogram::mouseReleaseEvent(QMouseEvent * e )
{
	if(!tfunc_enable) return;
	
	if ((e->state()&QEvent::MouseButtonRelease))
	{
		mouse_release();
		//draw_transfer();
	}	
}
void QtHistogram::mouse_release(void)
{
	setCursor(QCursor(Qt::waitCursor));

	if(fe_seleted==1)
	{
		luminance.functioneditor.selected = -1;
		luminance.functioneditor.delete_event=0;
	}
	if(fe_seleted==2)
	{
		alpha.functioneditor.selected = -1;
		alpha.functioneditor.delete_event=0;
	}
	if(fe_seleted==3)
	{
		redluminance.functioneditor.selected = -1;
		redluminance.functioneditor.delete_event=0;
	}
	if(fe_seleted==4)
	{
		greenluminance.functioneditor.selected = -1;
		greenluminance.functioneditor.delete_event=0;
	}
	if(fe_seleted==5)
	{
		blueluminance.functioneditor.selected = -1;
		blueluminance.functioneditor.delete_event=0;
	}
	if(fe_seleted==6)
	{
		redalpha.functioneditor.selected = -1;
		redalpha.functioneditor.delete_event=0;
	}
	if(fe_seleted==7)
	{
		greenalpha.functioneditor.selected = -1;
		greenalpha.functioneditor.delete_event=0;
	}		
	if(fe_seleted==8)
	{
		bluealpha.functioneditor.selected = -1;
		bluealpha.functioneditor.delete_event=0;
	}

	emit_tfunc_current();
	emit hist_redraw_signal();

	setCursor(QCursor(Qt::arrowCursor));
}
void QtHistogram::emit_redraw(void)
{
	emit hist_redraw_signal();	
}
void QtHistogram::emit_tfunc_reloadall(void)
{
	//now just let the shader know about the new lookup table
	//we generated.
	emit hist_tfunc_signal(1, luminance);
	emit hist_tfunc_signal(2, alpha);
	emit hist_tfunc_signal(3, redluminance);
	emit hist_tfunc_signal(4, greenluminance);
	emit hist_tfunc_signal(5, blueluminance);
	emit hist_tfunc_signal(6, redalpha);
	emit hist_tfunc_signal(7, greenalpha);
	emit hist_tfunc_signal(8, bluealpha);
}
void QtHistogram::emit_tfunc_all(void)
{
	//if we are in intensity mode, then set all othe
	//lookups to that of L & I
	redluminance = greenluminance = blueluminance = luminance;
	redalpha = greenalpha = bluealpha = alpha;

	emit hist_tfunc_signal(1, luminance);
	emit hist_tfunc_signal(2, alpha);
	emit hist_tfunc_signal(3, redluminance);
	emit hist_tfunc_signal(4, greenluminance);
	emit hist_tfunc_signal(5, blueluminance);
	emit hist_tfunc_signal(6, redalpha);
	emit hist_tfunc_signal(7, greenalpha);
	emit hist_tfunc_signal(8, bluealpha);
}
void QtHistogram::emit_tfunc_current(void)
{
	//now just let the shader know about the new lookup table
	//we generated.
	if(fe_seleted==1)
	{
		redluminance = greenluminance = blueluminance = luminance;
		emit hist_tfunc_signal(fe_seleted, luminance);
		emit hist_tfunc_signal(3, luminance);
		emit hist_tfunc_signal(4, luminance);
		emit hist_tfunc_signal(5, luminance);
	}
	else if(fe_seleted==2)
	{
		redalpha = greenalpha = bluealpha = alpha;
		emit hist_tfunc_signal(fe_seleted, alpha);
		emit hist_tfunc_signal(6, redalpha);
		emit hist_tfunc_signal(7, greenalpha);
		emit hist_tfunc_signal(8, bluealpha);
	}
	else if(fe_seleted==3) emit hist_tfunc_signal(fe_seleted, redluminance);
	else if(fe_seleted==4) emit hist_tfunc_signal(fe_seleted, greenluminance);
	else if(fe_seleted==5) emit hist_tfunc_signal(fe_seleted, blueluminance);
	else if(fe_seleted==6) emit hist_tfunc_signal(fe_seleted, redalpha);
	else if(fe_seleted==7) emit hist_tfunc_signal(fe_seleted, greenalpha);
	else if(fe_seleted==8) emit hist_tfunc_signal(fe_seleted, bluealpha);
}
void QtHistogram::savefile(QString fn)
{
	if(fn.isEmpty()) return;

	//check the user entered the extension, otherwise add it to the filename
	if(fn[int(fn.length()-4)]!='.') fn += ".tfn";

	if (fn.isEmpty()) return;
	int i;
	ofstream fout;												
	fout.open(fn.latin1());

	fout<<"// Transfer Function File"<<endl<<endl;
	
	fout<<"// brightness, contrast, threshmax, threshmin, invert"<<endl;
	fout<<"// int, int, int, int, bool"<<endl;
	fout<<"//=========================================================="<<endl;
	fout<<"L	"<<luminance.brightness<<"	"<<luminance.contrast<<"	"<<luminance.threshold_max<<"	"<<luminance.threshold_min<<endl;
	fout<<"LA	"<<alpha.brightness<<"	"<<alpha.contrast<<"	"<<alpha.threshold_max<<"	"<<alpha.threshold_min<<endl;
	fout<<"R	"<<redluminance.brightness<<"	"<<redluminance.contrast<<"	"<<redluminance.threshold_max<<"	"<<redluminance.threshold_min<<endl;
	fout<<"G	"<<greenluminance.brightness<<"	"<<greenluminance.contrast<<"	"<<greenluminance.threshold_max<<"	"<<greenluminance.threshold_min<<endl;
	fout<<"B	"<<blueluminance.brightness<<"	"<<blueluminance.contrast<<"	"<<blueluminance.threshold_max<<"	"<<blueluminance.threshold_min<<endl<<endl;
	fout<<"RA	"<<redalpha.brightness<<"	"<<redalpha.contrast<<"	"<<redalpha.threshold_max<<"	"<<redalpha.threshold_min<<endl;
	fout<<"GA	"<<greenalpha.brightness<<"	"<<greenalpha.contrast<<"	"<<greenalpha.threshold_max<<"	"<<greenalpha.threshold_min<<endl;
	fout<<"BA	"<<bluealpha.brightness<<"	"<<bluealpha.contrast<<"	"<<bluealpha.threshold_max<<"	"<<bluealpha.threshold_min<<endl<<endl;

	fout<<"// Trasnfer Functions"<<endl;
	fout<<"// size, cp.x, cp.y, cp.z ....."<<endl;
	fout<<"// int, float, float, float ...."<<endl;
	fout<<"//=========================================================="<<endl;

	fout<<"TF_L_CONTROL_POINTS"<<endl;
	fout<<luminance.functioneditor.bs.control_points.size()<<endl;
	for(i=0; i<luminance.functioneditor.bs.control_points.size(); i++)
	{
		fout<<luminance.functioneditor.bs.control_points[i].x<<"	"<<luminance.functioneditor.bs.control_points[i].y<<"	"<<luminance.functioneditor.bs.control_points[i].z<<endl;
	}
	fout<<endl;
	fout<<"TF_LA_CONTROL_POINTS"<<endl;
	fout<<alpha.functioneditor.bs.control_points.size()<<endl;
	for(i=0; i<alpha.functioneditor.bs.control_points.size(); i++)
	{
		fout<<alpha.functioneditor.bs.control_points[i].x<<"	"<<alpha.functioneditor.bs.control_points[i].y<<"	"<<alpha.functioneditor.bs.control_points[i].z<<endl;
	}
	fout<<endl;	
	fout<<"TF_R_CONTROL_POINTS"<<endl;
	fout<<redluminance.functioneditor.bs.control_points.size()<<endl;
	for(i=0; i<redluminance.functioneditor.bs.control_points.size(); i++)
	{
		fout<<redluminance.functioneditor.bs.control_points[i].x<<"	"<<redluminance.functioneditor.bs.control_points[i].y<<"	"<<redluminance.functioneditor.bs.control_points[i].z<<endl;
	}
	fout<<endl;
	fout<<"TF_RA_CONTROL_POINTS"<<endl;
	fout<<redalpha.functioneditor.bs.control_points.size()<<endl;
	for(i=0; i<redalpha.functioneditor.bs.control_points.size(); i++)
	{
		fout<<redalpha.functioneditor.bs.control_points[i].x<<"	"<<redalpha.functioneditor.bs.control_points[i].y<<"	"<<redalpha.functioneditor.bs.control_points[i].z<<endl;
	}
	fout<<endl;
	fout<<"TF_G_CONTROL_POINTS"<<endl;
	fout<<greenluminance.functioneditor.bs.control_points.size()<<endl;
	for(i=0; i<greenluminance.functioneditor.bs.control_points.size(); i++)
	{
		fout<<greenluminance.functioneditor.bs.control_points[i].x<<"	"<<greenluminance.functioneditor.bs.control_points[i].y<<"	"<<greenluminance.functioneditor.bs.control_points[i].z<<endl;
	}
	fout<<endl;
	fout<<"TF_GA_CONTROL_POINTS"<<endl;
	fout<<greenalpha.functioneditor.bs.control_points.size()<<endl;
	for(i=0; i<greenalpha.functioneditor.bs.control_points.size(); i++)
	{
		fout<<greenalpha.functioneditor.bs.control_points[i].x<<"	"<<greenalpha.functioneditor.bs.control_points[i].y<<"	"<<greenalpha.functioneditor.bs.control_points[i].z<<endl;
	}
	fout<<endl;
	fout<<"TF_B_CONTROL_POINTS"<<endl;
	fout<<blueluminance.functioneditor.bs.control_points.size()<<endl;
	for(i=0; i<blueluminance.functioneditor.bs.control_points.size(); i++)
	{
		fout<<blueluminance.functioneditor.bs.control_points[i].x<<"	"<<blueluminance.functioneditor.bs.control_points[i].y<<"	"<<blueluminance.functioneditor.bs.control_points[i].z<<endl;
	}
	fout<<endl;
	fout<<"TF_BA_CONTROL_POINTS"<<endl;
	fout<<bluealpha.functioneditor.bs.control_points.size()<<endl;
	for(i=0; i<bluealpha.functioneditor.bs.control_points.size(); i++)
	{
		fout<<bluealpha.functioneditor.bs.control_points[i].x<<"	"<<bluealpha.functioneditor.bs.control_points[i].y<<"	"<<bluealpha.functioneditor.bs.control_points[i].z<<endl;
	}
	fout<<endl;

	fout.close();
}

void QtHistogram::save(void)
{
	printf("tfunc_save_slot\n");
	int i;
	
	QFileDialog* fd = new QFileDialog(this);
	QString fn = fd->getSaveFileName(currentworkingdir, "*.tfn", this );

	savefile(fn);

	delete fd;
}
void QtHistogram::loadfile(QString fn)
{
	if(fn.isEmpty()) return;

	//temp storage for param parsing
	string label;
	int i;
	int size;
	Vector v;

	//clear our BSs
	luminance.functioneditor.bs.clear();
	alpha.functioneditor.bs.clear();
	redluminance.functioneditor.bs.clear();
	redalpha.functioneditor.bs.clear();
	greenluminance.functioneditor.bs.clear();
	greenalpha.functioneditor.bs.clear();
	blueluminance.functioneditor.bs.clear();
	bluealpha.functioneditor.bs.clear();

	//input file for reading in parameter ranges
	ifstream input_file; 
	input_file.open(fn.latin1());

	//tokenize our input file
	Tokenizer token(input_file);

	//until oef is reached step through each token
	while(!input_file.eof()){
		if(token.ttype()==TT_WORD)
		{
			//get the first string lable
			label = token.sval();
			
			if(label=="L")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) luminance.brightness = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) luminance.contrast = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) luminance.threshold_max = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) luminance.threshold_min = token.nval();
				token.nextToken();
			}
			else if(label=="LA")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) alpha.brightness = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) alpha.contrast = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) alpha.threshold_max = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) alpha.threshold_min = token.nval();
				token.nextToken();
			}
			else if(label=="R")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) redluminance.brightness = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) redluminance.contrast = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) redluminance.threshold_max = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) redluminance.threshold_min = token.nval();
				token.nextToken();
			}
			else if(label=="G")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) greenluminance.brightness = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) greenluminance.contrast = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) greenluminance.threshold_max = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) greenluminance.threshold_min = token.nval();
				token.nextToken();
			}			
			else if(label=="B")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) blueluminance.brightness = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) blueluminance.contrast = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) blueluminance.threshold_max = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) blueluminance.threshold_min = token.nval();
				token.nextToken();
			}			
			else if(label=="RA")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) redalpha.brightness = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) redalpha.contrast = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) redalpha.threshold_max = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) redalpha.threshold_min = token.nval();
				token.nextToken();
			}
			else if(label=="GA")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) greenalpha.brightness = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) greenalpha.contrast = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) greenalpha.threshold_max = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) greenalpha.threshold_min = token.nval();
				token.nextToken();
			}			
			else if(label=="BA")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) bluealpha.brightness = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) bluealpha.contrast = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) bluealpha.threshold_max = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) bluealpha.threshold_min = token.nval();
				token.nextToken();
			}			
			else if(label=="TF_L_CONTROL_POINTS")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) size = token.nval();
				
				for(i=0; i<size; i++)
				{
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();

					luminance.functioneditor.bs.control_points.push_back(v);
				}
			}			
			else if(label=="TF_LA_CONTROL_POINTS")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) size = token.nval();
				
				for(i=0; i<size; i++)
				{
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();

					alpha.functioneditor.bs.control_points.push_back(v);
				}
			}
			else if(label=="TF_R_CONTROL_POINTS")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) size = token.nval();
				
				for(i=0; i<size; i++)
				{
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();

					redluminance.functioneditor.bs.control_points.push_back(v);
				}
			}
			else if(label=="TF_RA_CONTROL_POINTS")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) size = token.nval();
				
				for(i=0; i<size; i++)
				{
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();

					redalpha.functioneditor.bs.control_points.push_back(v);
				}
			}
			else if(label=="TF_G_CONTROL_POINTS")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) size = token.nval();
				
				for(i=0; i<size; i++)
				{
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();

					greenluminance.functioneditor.bs.control_points.push_back(v);
				}
			}
			else if(label=="TF_GA_CONTROL_POINTS")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) size = token.nval();
				
				for(i=0; i<size; i++)
				{
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();

					greenalpha.functioneditor.bs.control_points.push_back(v);
				}
			}
			else if(label=="TF_B_CONTROL_POINTS")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) size = token.nval();
				
				for(i=0; i<size; i++)
				{
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();

					blueluminance.functioneditor.bs.control_points.push_back(v);
				}
			}
			else if(label=="TF_BA_CONTROL_POINTS")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) size = token.nval();
				
				for(i=0; i<size; i++)
				{
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();

					bluealpha.functioneditor.bs.control_points.push_back(v);
				}
			}	
			//else if we dont know what it is, go to next label
			else
			{
				//f<<"WARNING: Unrecognized Label..."<<endl;
				token.nextToken();
			}
		}
		else token.nextToken();
	}

	input_file.close();

	//compute our BS curve from our loaded control points
	luminance.functioneditor.bs.compute();
	alpha.functioneditor.bs.compute();
	redluminance.functioneditor.bs.compute();
	redalpha.functioneditor.bs.compute();
	greenluminance.functioneditor.bs.compute();
	greenalpha.functioneditor.bs.compute();
	blueluminance.functioneditor.bs.compute();
	bluealpha.functioneditor.bs.compute();
}
void QtHistogram::load(void)
{
	QFileDialog* fd = new QFileDialog( this );

	QString fn = fd->getOpenFileName(currentworkingdir, "*.tfn", this );
	loadfile(fn);
	delete fd;
}
void QtHistogram::hist_setcurrentworkingdir_slot(QString dn)
{
	currentworkingdir = dn;
}