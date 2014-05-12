/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#define max(a,b) (((a)>(b))?(a):(b)) 

#include "QtHistoTfuncPixmap.h"
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <Q3Frame>
#include <QPixmap>
#include <QLabel>
#include <QMouseEvent>
#include <QEvent>

QtHistoTfuncPixmap::QtHistoTfuncPixmap(float size)
{
	currentworkingdir="";
	mode = 0;	
	hist_enable=1;
	tfunc_enable=1;
	logscale=1;
	cumfreq_enable=0;
	remaponfly = 1;
	
	graph_size = size;
	canvas_size = size*1.25;
	pensize = canvas_size/200.0;

	border = (canvas_size-graph_size)/2.0;

	maxthr=graph_size;
	minthr=0;
	max_histogram = 0;

	data=NULL;
	data = new double[(int)graph_size];
	for(int i=0; i<graph_size; i++) data[i]=0;

	setMouseTracking(true);

	/*QSizePolicy sizepolicy;
	sizepolicy.setHeightForWidth(true);
	sizepolicy.setVerticalPolicy(QSizePolicy::Expanding);
	sizepolicy.setHorizontalPolicy(QSizePolicy::Expanding);
	setSizePolicy(sizepolicy);*/

	brightness = 100;
	contrast = 100;
/*	QSizePolicy sizepolicy;
	sizepolicy.setVerticalPolicy(QSizePolicy::Expanding);
	sizepolicy.setHorizontalPolicy(QSizePolicy::Expanding);
	setSizePolicy(sizepolicy);
*/

        create();
}
QtHistoTfuncPixmap::~QtHistoTfuncPixmap()
{
	data=NULL;
}

void QtHistoTfuncPixmap::reset(void)
{
	max_histogram = 0;
	
	logscale=1;
	hist_enable=1;
	tfunc_enable=1;
	cumfreq_enable=0;
	maxthr=graph_size;
	minthr=0;

	funceditor.reset();
	for(int i=0; i<graph_size; i++) data[i]=0;
}
void QtHistoTfuncPixmap::create(void)
{
//	printf("graph_size: %d\n", (int)graph_size);
	funceditor.init(graph_size, 0, graph_size/50.0);
}
void QtHistoTfuncPixmap::tfunc_remap16bit(void)
{
	if(remaponfly==1)
	{
		emit tfunc_remapsignal();
//		printf("REMAP YES\n");
//		emit tfunc_remap16bit_signal(minthr, maxthr, funceditor.bs.curve_points);
	}
	else
	{
//			printf("REMAP NO\n");
	}
}

void QtHistoTfuncPixmap::draw_functioneditoronpixmap(QPainter* p, FunctionEditor* func, QColor col)
{
	//draw our control points line
	QPainterPath cplinepath;
	cplinepath.moveTo(func->bs.control_points[0].x, func->bs.control_points[0].y);
	for(int i=0; i<func->bs.control_points.size(); i++)
	{		 
		cplinepath.lineTo(func->bs.control_points[i].x, func->bs.control_points[i].y);		
	}
    
	p->setPen( QPen(col, pensize, Qt::DashLine, Qt::FlatCap, Qt::RoundJoin) );
	p->drawPath(cplinepath);
	
	//draw our control points
	QPainterPath cppath;
	cppath.moveTo(func->bs.control_points[0].x, func->bs.control_points[0].y);
	for(int i=0; i<func->bs.control_points.size(); i++)
	{		 
                cppath.addRect( func->bs.control_points[i].x-(pensize*0.5), func->bs.control_points[i].y-(pensize*0.5), pensize, pensize);
	}
	p->setBrush(col);
    p->setPen( QPen(Qt::black, pensize*2.0, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin) );
	p->drawPath(cppath);
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
	curvepath.lineTo(func->bs.curve_points[func->bs.curve_points.size()-1].x, func->bs.curve_points[func->bs.curve_points.size()-1].y);

	//printf("%f\n", func->bs.curve_points[0].y);

    p->setPen( QPen(Qt::black, pensize*0.5f, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin) );
	p->drawPath(curvepath);
}
void QtHistoTfuncPixmap::draw_functioneditor(QPainter* p)
{
	//draw our histogram
	draw_histogram(p);

	if(tfunc_enable)
	{
            draw_functioneditoronpixmap(p, &funceditor, Qt::darkGray);
	}
}
void QtHistoTfuncPixmap::draw_histogram(QPainter* p)
{	
	//find the max value in our histogram
	max_histogram=0;
	float max_loghisto =0;
	float min_loghisto = log(1024.0*1024.0*1024.0);
	for(int iii=1; iii<graph_size; iii++)
	{
		max_histogram = max(max_histogram, data[iii]);

		if(data[iii]!=0)
		{
			max_loghisto = max((float)max_loghisto, (float)log(data[iii]));
			min_loghisto= min((float)min_loghisto, (float)log(data[iii]));
		}
	}

	//draw a background rect
	p->setPen(Qt::NoPen);
	p->setBrush(Qt::NoBrush);

	p->setPen(Qt::black);
	p->setBrush(Qt::white);
	p->drawRect(0,0,graph_size, graph_size);

	//reset our painter's brush
	p->setBrush(Qt::NoBrush);

	//draw a grid
	p->setPen(Qt::black);
	float fontsize = pensize*2.0;
	QFont serifFont("Arial", fontsize, QFont::Bold);
	p->setFont(serifFont);
	float grid_steps = (graph_size)/8.0;
		
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
			p->drawLine(grid_step, 0, grid_step, graph_size+border*0.25);
			p->drawLine(-border*0.25, grid_step, graph_size, grid_step);

			//x,y labels
			label.setNum((int)(grid_step),10);
                        p->drawText( QRectF(grid_step-grid_steps/2.0, graph_size+border*0.4, grid_steps, fontsize*1.25), Qt::AlignCenter, label);
                        p->drawText( QRectF(-border, graph_size-grid_step-4, grid_steps, fontsize*1.25), Qt::AlignCenter, label);
		}
		else
		{	
			//x,y grid lines

			//Y AXIS LINES
			p->drawLine(grid_step, 0, grid_step, graph_size+border*0.25);
			label.setNum((int)(grid_step),10);
                        p->drawText( QRectF(grid_step-grid_steps/2.0, graph_size+border*0.4, grid_steps, fontsize*2.0), Qt::AlignCenter, label);

                        p->drawText( QRectF(-border, graph_size-grid_step-4, grid_steps, fontsize*2.0), Qt::AlignCenter, label);
			p->drawLine(-border*0.25, grid_step, graph_size, grid_step);
		}
		grid_step+=grid_steps;
	}

	if(hist_enable==1)
	{
		//dray our histogram bars
		//editor->fe_seleted
		// 1 = RGB// 2 = RGBA// 3 = R// 4 = G// 5 = B// 6 = A
		p->setBrush(Qt::NoBrush);
		p->setPen(Qt::NoPen);

		if(mode==0) p->setBrush( QColor(32,32,32, 255) );
		else p->setBrush( QColor(64,64,64, 255) );
		
		double height;
		int i;
		float xpos=0.0;
	
		//QPolygonF polygon;
		float iterp=0;

		if(max_histogram==0) max_histogram=1;

		for(i=0 ; i<graph_size; i++)
		{
			//linear scale
			if(logscale==1)
			{
				if(data[i]!=0) iterp = (double)(log(data[i])-min_loghisto)/(double)(max_loghisto-min_loghisto);
				else iterp = 0;
			}
			else
			{
				iterp = ((double)data[i]/(double)max_histogram);
			}

			if(iterp>1.0) iterp=1.0;
			if(iterp<0.0) iterp=0.0;

			height = (graph_size)*iterp;
			height = (int)(height+0.5);

			xpos = i;

			//polygon << QPointF(xpos, graph_size-height) ;
			QRectF rectangle((float)xpos-0.5, graph_size, 1.0, -height);
			p->drawRect(rectangle);
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
        for(int i=0 ; i<graph_size; i++)
		{
			maxsum += data[i];
		}

		if(maxsum)
		{
			QPainterPath cumpath;
			cumpath.moveTo(0,0);
                        for(int i=0 ; i<graph_size; i++)
			{
				sum += data[i];
				height = (graph_size)*(((double)sum/(double)maxsum));
                                height = graph_size-(int)(height+0.5);
				cumpath.lineTo (i, height );
			}
			p->setPen(Qt::red);
			p->setBrush(Qt::NoBrush);

			//p->setBrush(Qt::red);
			p->drawPath(cumpath);
		}
	}

	//draw our thresholding...
	p->setPen(Qt::black);
	p->setBrush(QBrush ( QColor(128,128,128,128), Qt::SolidPattern ));
	QRectF rectangle(0.0, 0.0, minthr, graph_size);
	p->drawRect(rectangle);
	QRectF rectangle2(maxthr, 0.0, graph_size-maxthr, graph_size);
	p->drawRect(rectangle2);
	p->setBrush(Qt::NoBrush);
}

void QtHistoTfuncPixmap::paintEvent( QPaintEvent *e )
{	
	QPainter pp(this);
	pp.setRenderHint(QPainter::Antialiasing, false);	
	pp.scale((float)width()/(float)canvas_size, (float)height()/(float)canvas_size);
	
	if(mode==0) pp.fillRect(QRect(0, 0, canvas_size, canvas_size), QBrush ( QColor(128,128,128,255), Qt::SolidPattern ));
	else pp.fillRect(QRect(0, 0, canvas_size, canvas_size), QBrush ( QColor(128,128,128,255), Qt::SolidPattern ));
	
	pp.translate(border,border);

	draw_functioneditor(&pp);
}
void QtHistoTfuncPixmap::resizeEvent ( QResizeEvent *e )
{
//        setMinimumWidth(e->size().height());
}
void QtHistoTfuncPixmap::mouseMoveEvent  ( QMouseEvent * e )
{
	if(!tfunc_enable) return;

	float x = (((float)(e->x())/(float)width())*canvas_size)-border;
	float y = (((float)(e->y())/(float)height())*canvas_size)-border;
	
	if(funceditor.selected!=-1 || funceditor.delete_event==1) funceditor.movePoint(x, y);

	update();
}
void QtHistoTfuncPixmap::mousePressEvent  ( QMouseEvent * e )
{
	if(!tfunc_enable) return;
	
	float x = (((float)(e->x())/(float)width())*canvas_size)-border;
	float y = (((float)(e->y())/(float)height())*canvas_size)-border;

	if ((e->button()==Qt::LeftButton)) 
	{
                funceditor.selectPoint(x, y);
	}
	else if ((e->button()==Qt::MidButton)) 
	{
            if(funceditor.selectPoint(x, y)) funceditor.deletePoint();
        }
	else if ((e->button()==Qt::RightButton)) 
	{
            funceditor.addPoint(x,y);
	}
}
void QtHistoTfuncPixmap::mouseReleaseEvent(QMouseEvent * e )
{
	if(!tfunc_enable) return;
	
	if ((e->state()&QEvent::MouseButtonRelease))
	{
            //setCursor(QCursor(Qt::waitCursor));
			
			printf("QtHistoTfuncPixmap::mouseReleaseEvent\n");

			tfunc_remap16bit();
			
            funceditor.selected = -1;
            funceditor.delete_event=0;

            //emit_tfunc_current();
            //emit hist_redraw_signal();

            //setCursor(QCursor(Qt::arrowCursor));
	}	
}

void QtHistoTfuncPixmap::save(void)
{
	printf("QtHistoTfuncPixmap save()\n");
	int i;
	
	QFileDialog* fd = new QFileDialog(this);
	QString fn = fd->getSaveFileName(currentworkingdir, "*.tf2", this );

	if(fn.isEmpty()) return;

	//check the user entered the extension, otherwise add it to the filename
	if(fn[int(fn.length()-4)]!='.') fn += ".tfn";

	if (fn.isEmpty()) return;

	ofstream fout;												
	fout.open(fn.latin1());

	fout<<"// Transfer Function File"<<endl<<endl;
	fout<<"//=========================================================="<<endl;

	fout<<"// Threshold levels:"<<endl;
	fout<<"// min, max"<<endl;
	fout<<"// int, int"<<endl;
	fout<<"//=========================================================="<<endl;
	fout<<"THRESHOLD_LEVELS"<<endl;
	fout<<minthr<<"	"<<maxthr<<endl;
	fout<<"// Trasnfer Functions"<<endl;
	fout<<"// size, cp.x, cp.y, cp.z ....."<<endl;
	fout<<"// int, float, float, float ...."<<endl;
	fout<<"//=========================================================="<<endl;

	fout<<"TF_CONTROL_POINTS"<<endl;
	fout<<funceditor.bs.control_points.size()<<endl;
	for(i=0; i<funceditor.bs.control_points.size(); i++)
	{
		fout<<funceditor.bs.control_points[i].x<<"	"<<funceditor.bs.control_points[i].y<<"	"<<funceditor.bs.control_points[i].z<<endl;
	}
	fout<<endl;
	
	fout.close();

	delete fd;
}
void QtHistoTfuncPixmap::loadfile(QString fn)
{
	if(fn.isEmpty()) return;

	//temp storage for param parsing
	string label;
	int i;
	int size;
	Vector v;

	//clear our BSs
	funceditor.bs.clear();

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
			
			if(label=="THRESHOLD_LEVELS")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) minthr = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) maxthr = token.nval();
				token.nextToken();
			}			
			else if(label=="TF_CONTROL_POINTS")
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

					funceditor.bs.control_points.push_back(v);
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
	funceditor.bs.compute();
}
void QtHistoTfuncPixmap::tfunc16_setcurrentworkingdir_slot(QString dn)
{
	currentworkingdir = dn;
}

void QtHistoTfuncPixmap::load(void)
{
	QFileDialog* fd = new QFileDialog(0);

	QString fn = fd->getOpenFileName(currentworkingdir, "*.tf2", this );
	loadfile(fn);
	delete fd;
}
void QtHistoTfuncPixmap::sethistogramdata(double* histodata)
{
	for(int i=0; i<graph_size; i++) 
	{
	//	printf("i:%d\n", i);
		data[i]=histodata[i];
	}
	
	update();
}

