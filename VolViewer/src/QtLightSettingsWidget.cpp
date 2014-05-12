/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtLightSettingsWidget.h"
#include "ShaderDefinitions.h"

//Added by qt3to4:
#include <QPixmap>
#include <QLabel>
#include <Q3Frame>


QtLightSettingsWidget::QtLightSettingsWidget(QWidget* parent, const char* name)
: QWidget( parent, name )
{
	l_altitude = 0;
	l_azimuth = 0;
	amb = Vector(0.0, 0.0, 0.0);
	diff = Vector(0.25, 0.25, 0.25);
	spec = Vector(0.25, 0.25, 0.25);
	depthcol = Vector(0,0,0);

	amb_colour = QColor(0,0,0);
	dif_colour = QColor(64,64,64);
	spec_colour = QColor(64,64,64);

	depth_colour = QColor(0,0,0);

	warm_colour = QColor(255,255,0);
	warm = Vector(1.0,1.0,0.0);

	cool_colour = QColor(0,0,255);
	cool = Vector(0.0,0.0,1.0);

	depthshade_contrib = 0;

	silh_contrib = 1;
	silh_exp = 4;

	semsilh_contrib = 8;
	semsilh_exp = 1;

	degenthresh = 0.25;
	alphathresh = 100;

	depthoffset=1;
	depthpeal=1;
}
QtLightSettingsWidget::~QtLightSettingsWidget()
{
}
void QtLightSettingsWidget::reset(void)
{
}
void QtLightSettingsWidget::create(void)
{
	//LOAD/SAVE
	Q3ButtonGroup *loadsave_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "File:", this );

		load = new QPushButton(QIcon(QPixmap("Icons/fileopen.xpm")), "Open", loadsave_grp, "open");
		connect( load, SIGNAL( clicked () ), this, SLOT( load_event() ) );

		save = new QPushButton(QIcon(QPixmap("Icons/filesave.xpm")), "Save", loadsave_grp, "save");
		connect( save, SIGNAL( clicked () ), this, SLOT( save_event() ) );

	//LIGHT PREFERENCES
	Q3ButtonGroup *light_grp = new Q3ButtonGroup( 8, Qt::Vertical, "Lighting Preferences:", this );
//	light_grp->setLineWidth(2);
//	light_grp->setFrameStyle( QFrame::Box | QFrame::Plain );
		//vol shader prog combo box

		Q3ButtonGroup *shaderprog_grp = new Q3ButtonGroup( 3, Qt::Vertical, "Shader Programs:", light_grp );

			QComboBox* volshader_combobox = new QComboBox(shaderprog_grp, "volshader_combobox");
/*			static const char* volshader_items[] = { "Single Channel NOLIGHT", 
													 "Single Channel FULL", 
													 "Multi Channel NOLIGHT", 
													 "Multi Channel LAMBERTIAN", 
													 "Multi Channel BLING-PHONG", 
													 "Multi Channel COOK-TORRANCE", 
													 "Multi Channel GOOCH", 
													 "Multi Channel BLING-PHONG 2SP", 
													  0};*/
			QStringList volshader_items;
			volshader_items += "No Lighting (FASTEST)";
			//volshader_items += "Lambertian Lighting";
			volshader_items += "Photo-Realistic Lighting";
			volshader_items += "Non Photo-Realistic Lighting";
			volshader_items += "Depth-Peal Shader";
			//volshader_items += "SEM Lighting";
			//volshader_items += "Clipper";
		
			volshader_combobox->insertItems(0, volshader_items);
			volshader_combobox->setCurrentItem(0);
			connect(volshader_combobox, SIGNAL(activated(int)), this, SLOT(volshader_combobox_event(int)));

		goochlight_grid = new Q3Grid(4, light_grp);
		
				Q3ButtonGroup *warm_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Warm Colour:", goochlight_grid );

					warm_lb = new QLabel("", warm_grp);
					warm_lb->setFrameStyle(Q3Frame::Panel | Q3Frame::Raised);
					warm_lb->setFixedSize(32, 32);
					QPixmap pixmap = QPixmap(32, 32);
					pixmap.fill(warm_colour);
					warm_lb->setPixmap(pixmap);

					QPushButton* warm_btn = new QPushButton("Change", warm_grp);
					connect( warm_btn, SIGNAL( clicked() ), this, SLOT(warm_colourdlg_event() ) );
		
				Q3ButtonGroup *cool_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Cool Colour:", goochlight_grid );

					cool_lb = new QLabel("", cool_grp);
					cool_lb->setFrameStyle(Q3Frame::Panel | Q3Frame::Raised);
					cool_lb->setFixedSize(32, 32);
					pixmap.fill(cool_colour);
					cool_lb->setPixmap(pixmap);

					QPushButton* cool_btn = new QPushButton("Change", cool_grp);
					connect( cool_btn, SIGNAL( clicked() ), this, SLOT(cool_colourdlg_event() ) );

				Q3ButtonGroup *spec_grp2 = new Q3ButtonGroup( 1, Qt::Vertical, "Specular Colour:", goochlight_grid );

					goochspec_lb = new QLabel("", spec_grp2);
					goochspec_lb->setFrameStyle(Q3Frame::Panel | Q3Frame::Raised);
					goochspec_lb->setFixedSize(32, 32);
					goochspec_lb->setBackgroundColor(spec_colour);
					pixmap.fill(spec_colour);
					goochspec_lb->setPixmap(pixmap);

					QPushButton* spec_btn2 = new QPushButton("Change", spec_grp2);
					connect( spec_btn2, SIGNAL( clicked() ), this, SLOT(spec_colourdlg_event() ) );

		goochlight_grid->hide();

		depth_grp = new Q3ButtonGroup( 2, Qt::Horizontal, "Depth Cues:", light_grp );

			//our depthshade colour
			Q3ButtonGroup *depthcol_grp = new Q3ButtonGroup( 1, Qt::Vertical, "", depth_grp );
				depthcol_lb = new QLabel("", depthcol_grp);
				depthcol_lb->setFrameStyle(Q3Frame::Panel | Q3Frame::Raised);
				depthcol_lb->setFixedSize(32, 32);
				pixmap.fill(depth_colour);
				depthcol_lb->setPixmap(pixmap);
				QPushButton* depth_btn = new QPushButton("Change", depthcol_grp);
				connect( depth_btn, SIGNAL( clicked() ), this, SLOT(depth_colourdlg_event() ) );

			//our depthshade slider
			Q3ButtonGroup* depthshade_contrib_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", depth_grp );
				depthshade_contrib_lb = new QLabel( "Scale: 0", depthshade_contrib_grp );
				depthshade_contrib_slider = new QSlider ( 0, 100, 1, 0, Qt::Horizontal, depthshade_contrib_grp, "depthshade_contrib_slider" );
				depthshade_contrib_slider->setTickmarks( QSlider::Below );
				depthshade_contrib_slider->setTracking(FALSE);
				depthshade_contrib_slider->setTickInterval(10);
				connect(depthshade_contrib_slider, SIGNAL(valueChanged(int)), this, SLOT(depthshade_contrib_event(int)) );
				depthshade_contrib_event(depthshade_contrib);

		depth_grp->hide();

		depthpeal_grp = new Q3ButtonGroup( 2, Qt::Horizontal, "Depth Peal:", light_grp );

			//our depthoffset slider
			Q3ButtonGroup* depthoffset_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", depthpeal_grp );
				depthoffset_lb = new QLabel( "Offset: 0", depthoffset_grp );
				depthoffset_slider = new QSlider ( 0, 100, 1, 0, Qt::Horizontal, depthoffset_grp, "depthoffset_slider" );
				depthoffset_slider->setTickmarks( QSlider::Below );
				depthoffset_slider->setTracking(FALSE);
				depthoffset_slider->setTickInterval(10);
				connect(depthoffset_slider, SIGNAL(valueChanged(int)), this, SLOT(depthoffset_event(int)) );
				depthoffset_event(depthoffset);

			//our depthpeal slider
			Q3ButtonGroup* depthpealoff_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", depthpeal_grp );
				depthpeal_lb = new QLabel( "Thickness: 0", depthpealoff_grp );
				depthpeal_slider = new QSlider ( 0, 100, 1, 0, Qt::Horizontal, depthpealoff_grp, "depthpeal_slider" );
				depthpeal_slider->setTickmarks( QSlider::Below );
				depthpeal_slider->setTracking(FALSE);
				depthpeal_slider->setTickInterval(10);
				connect(depthpeal_slider, SIGNAL(valueChanged(int)), this, SLOT(depthpeal_event(int)) );
				depthpeal_event(depthpeal);
		depthpeal_grp->hide();

		silhoutte_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Silhouttes:", light_grp );

			//our silh_contrib slider
			Q3ButtonGroup* silh_contrib_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", silhoutte_grp );
			silh_contrib_lb = new QLabel( "Contribution: 0", silh_contrib_grp );
			silh_contrib_slider = new QSlider ( 0, 16, 1, 0, Qt::Horizontal, silh_contrib_grp, "silh_contrib_slider" );
			silh_contrib_slider->setTickmarks( QSlider::Below );
			silh_contrib_slider->setTracking(FALSE);
			silh_contrib_slider->setTickInterval(2);
			connect(silh_contrib_slider, SIGNAL(valueChanged(int)), this, SLOT(silh_contrib_event(int)) );
			silh_contrib_event(silh_contrib);

			//our silh_exp slider
			Q3ButtonGroup* silh_exp_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", silhoutte_grp );
			silh_exp_lb = new QLabel( "Exponent: 0", silh_exp_grp );
			silh_exp_slider = new QSlider ( 0, 32, 1, 0, Qt::Horizontal, silh_exp_grp, "silh_exp_slider" );
			silh_exp_slider->setTickmarks( QSlider::Below );
			silh_exp_slider->setTracking(FALSE);
			silh_exp_slider->setTickInterval(4);
			connect(silh_exp_slider, SIGNAL(valueChanged(int)), this, SLOT(silh_exp_event(int)) );
			silh_exp_event(silh_exp);

			//our degenthresh slider
			Q3ButtonGroup* degenthresh_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", silhoutte_grp );
			degenthresh_lb = new QLabel( "Threshold: 0", degenthresh_grp );
			degenthresh_slider = new QSlider ( 0, 100, 1, 0, Qt::Horizontal, degenthresh_grp, "degenthresh_slider" );
			degenthresh_slider->setTickmarks( QSlider::Below );
			degenthresh_slider->setTracking(FALSE);
			degenthresh_slider->setTickInterval(12.5);
			connect(degenthresh_slider, SIGNAL(valueChanged(int)), this, SLOT(degenthresh_event(int)) );
			degenthresh_event(degenthresh);

		semsilhoutte_grp = new Q3ButtonGroup( 1, Qt::Vertical, "SEM Lighting:", light_grp );

			//our silh_contrib slider
			Q3ButtonGroup* semsilh_contrib_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", semsilhoutte_grp );
			semsilh_contrib_lb = new QLabel( "Contribution: 0", semsilh_contrib_grp );
			semsilh_contrib_slider = new QSlider ( 0, 8, 1, 0, Qt::Horizontal, semsilh_contrib_grp, "semsilh_contrib_slider" );
			semsilh_contrib_slider->setTickmarks( QSlider::Below );
			semsilh_contrib_slider->setTracking(FALSE);
			semsilh_contrib_slider->setTickInterval(1);
			connect(semsilh_contrib_slider, SIGNAL(valueChanged(int)), this, SLOT(semsilh_contrib_event(int)) );
			semsilh_contrib_event(semsilh_contrib);

			//our silh_exp slider
			Q3ButtonGroup* semsilh_exp_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", semsilhoutte_grp );
			semsilh_exp_lb = new QLabel( "Exponent: 0", semsilh_exp_grp );
			semsilh_exp_slider = new QSlider ( 0, 32, 1, 0, Qt::Horizontal, semsilh_exp_grp, "semsilh_exp_slider" );
			semsilh_exp_slider->setTickmarks( QSlider::Below );
			semsilh_exp_slider->setTracking(FALSE);
			semsilh_exp_slider->setTickInterval(1);
			connect(semsilh_exp_slider, SIGNAL(valueChanged(int)), this, SLOT(semsilh_exp_event(int)) );
			semsilh_exp_event(semsilh_exp);
/*
			//our degenthresh slider
			Q3ButtonGroup* degenthresh_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", semsilhoutte_grp );
			degenthresh_lb = new QLabel( "Degenerate Threshold: 0", degenthresh_grp );
			degenthresh_slider = new QSlider ( 0, 100, 1, 0, Qt::Horizontal, degenthresh_grp, "degenthresh_slider" );
			degenthresh_slider->setTickmarks( QSlider::Below );
			degenthresh_slider->setTracking(FALSE);
			degenthresh_slider->setTickInterval(10);
			connect(degenthresh_slider, SIGNAL(valueChanged(int)), this, SLOT(degenthresh_event(int)) );
			degenthresh_event(degenthresh);
	*/		
		/*silhoutte2_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Silhouttes:", light_grp );

			//our alphathresh slider
			Q3ButtonGroup* alphathresh_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", silhoutte2_grp );
			alphathresh_lb = new QLabel( "Alpha Threshold: 0", alphathresh_grp );
			alphathresh_slider = new QSlider ( 0, 10, 1, 0, Qt::Horizontal, alphathresh_grp, "alphathresh_slider" );
			alphathresh_slider->setTickmarks( QSlider::Below );
			alphathresh_slider->setTracking(FALSE);
			alphathresh_slider->setTickInterval(1);
			connect(alphathresh_slider, SIGNAL(valueChanged(int)), this, SLOT(alphathresh_event(int)) );
			alphathresh_event(alphathresh);

*/

	silhoutte_grp->hide();
	semsilhoutte_grp->hide();
	//silhoutte2_grp->hide();

		lightcolour_grid = new Q3Grid( 3, light_grp );
		lightcolour_grid->setSpacing(5);

				Q3ButtonGroup *amb_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Ambient Colour:", lightcolour_grid );

					amb_lb = new QLabel("", amb_grp);
					amb_lb->setFrameStyle(Q3Frame::Panel | Q3Frame::Raised);
					amb_lb->setFixedSize(32, 32);
					pixmap.fill(amb_colour);
					amb_lb->setPixmap(pixmap);

					QPushButton* amb_btn = new QPushButton("Change", amb_grp);
					connect( amb_btn, SIGNAL( clicked() ), this, SLOT(amb_colourdlg_event() ) );

				Q3ButtonGroup *dif_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Diffuse Colour:", lightcolour_grid );

					dif_lb = new QLabel("", dif_grp);
					dif_lb->setFrameStyle(Q3Frame::Panel | Q3Frame::Raised);
					dif_lb->setFixedSize(32, 32);
					pixmap.fill(dif_colour);
					dif_lb->setPixmap(pixmap);

					QPushButton* dif_btn = new QPushButton("Change", dif_grp);
					connect( dif_btn, SIGNAL( clicked() ), this, SLOT(dif_colourdlg_event() ) );

				Q3ButtonGroup *spec_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Specular Colour:", lightcolour_grid );

					spec_lb = new QLabel("", spec_grp);
					spec_lb->setFrameStyle(Q3Frame::Panel | Q3Frame::Raised);
					spec_lb->setFixedSize(32, 32);
					spec_lb->setBackgroundColor(spec_colour);
					pixmap.fill(spec_colour);
					spec_lb->setPixmap(pixmap);

					QPushButton* spec_btn = new QPushButton("Change", spec_grp);
					connect( spec_btn, SIGNAL( clicked() ), this, SLOT(spec_colourdlg_event() ) );

				Q3ButtonGroup *lightpos_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Light Position:", light_grp );

					//our l_altitude slider
					Q3ButtonGroup* l_altitude_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", lightpos_grp );
					l_altitude_lb = new QLabel( "Altitude: 0", l_altitude_grp );
					l_altitude_slider = new QSlider ( -90, 90, 10, 0, Qt::Horizontal, l_altitude_grp, "l_altitude_slider" );
					l_altitude_slider->setTickmarks( QSlider::Below );
					l_altitude_slider->setTracking(FALSE);
					l_altitude_slider->setTickInterval(18);
					connect(l_altitude_slider, SIGNAL(valueChanged(int)), this, SLOT(l_altitude_event(int)) );
					l_altitude_event(l_altitude);

					//our l_azimuth slider
					Q3ButtonGroup* l_azimuth_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", lightpos_grp );
					l_azimuth_lb = new QLabel( "Azimuth: 0", l_azimuth_grp );
					l_azimuth_slider = new QSlider ( -180, 180, 10, 0, Qt::Horizontal, l_azimuth_grp, "l_azimuth_slider" );
					l_azimuth_slider->setTickmarks( QSlider::Below );
					l_azimuth_slider->setTracking(FALSE);
					l_azimuth_slider->setTickInterval(36);
					connect(l_azimuth_slider, SIGNAL(valueChanged(int)), this, SLOT(l_azimuth_event(int)) );
					l_azimuth_event(l_azimuth);

	//TOPMOST LAYOUT
	Q3BoxLayout *topmost= new Q3BoxLayout ( this, Q3BoxLayout::LeftToRight, 10, 5, "topmost");
	topmost->setAlignment(Qt::AlignTop);
	topmost->addWidget(loadsave_grp, 0, Qt::AlignTop);
	topmost->addWidget(light_grp, 0, Qt::AlignTop);

	loadsave_grp->hide();

	emit light_colour_signal(amb, diff, spec);
	emit light_gooch_lighting_signal(warm, cool, silh_contrib, silh_exp, alphathresh, degenthresh);
}
void QtLightSettingsWidget::apply(void)
{

}

void QtLightSettingsWidget::rebuild(void)
{
	l_altitude_event(l_altitude);
	l_azimuth_event(l_azimuth);

}
void QtLightSettingsWidget::load_event(void)
{

}
void QtLightSettingsWidget::save_event(void)
{

}
void QtLightSettingsWidget::warm_colourdlg_event(void)
{
	QColorDialog* coldialogue=0;
	warm_colour = coldialogue->getColor(warm_colour, this, "Choose a colour");
	QPixmap pixmap = QPixmap(32, 32);
	pixmap.fill(warm_colour);
	warm_lb->setPixmap(pixmap);
	
	warm.x = warm_colour.red();
	warm.y = warm_colour.green();
	warm.z = warm_colour.blue();
	warm /= 255.0;

	emit light_gooch_lighting_signal(warm, cool, silh_contrib, silh_exp, alphathresh, degenthresh);
}
void QtLightSettingsWidget::cool_colourdlg_event(void)
{
	QColorDialog* coldialogue=0;
	cool_colour = coldialogue->getColor(cool_colour, this, "Choose a colour");
	QPixmap pixmap = QPixmap(32, 32);
	pixmap.fill(cool_colour);
	cool_lb->setPixmap(pixmap);
	
	cool.x = cool_colour.red();
	cool.y = cool_colour.green();
	cool.z = cool_colour.blue();
	cool /= 255.0;

	emit light_gooch_lighting_signal(warm, cool, silh_contrib, silh_exp, alphathresh, degenthresh);
}

void QtLightSettingsWidget::amb_colourdlg_event(void)
{
	QColorDialog* coldialogue=0;
	amb_colour = coldialogue->getColor(amb_colour, this, "Choose a colour");
	QPixmap pixmap = QPixmap(32, 32);
	pixmap.fill(amb_colour);
	amb_lb->setPixmap(pixmap);
	
	amb.x = amb_colour.red();
	amb.y = amb_colour.green();
	amb.z = amb_colour.blue();
	amb /= 255.0;

	emit light_colour_signal(amb, diff, spec);
}
void QtLightSettingsWidget::dif_colourdlg_event(void)
{
	QColorDialog* coldialogue=0;
	dif_colour = coldialogue->getColor(dif_colour, this, "Choose a colour");
	QPixmap pixmap = QPixmap(32, 32);
	pixmap.fill(dif_colour);
	dif_lb->setPixmap(pixmap);

	diff.x = dif_colour.red();
	diff.y = dif_colour.green();
	diff.z = dif_colour.blue();
	diff /= 255.0;

	emit light_colour_signal(amb, diff, spec);
}
void QtLightSettingsWidget::spec_colourdlg_event(void)
{
	QColorDialog* coldialogue=0;
	spec_colour = coldialogue->getColor(spec_colour, this, "Choose a colour");
	QPixmap pixmap = QPixmap(32, 32);
	pixmap.fill(spec_colour);
	
	spec_lb->setPixmap(pixmap);
	goochspec_lb->setPixmap(pixmap);

	spec.x = spec_colour.red();
	spec.y = spec_colour.green();
	spec.z = spec_colour.blue();
	spec /= 255.0;

	emit light_colour_signal(amb, diff, spec);
}

void QtLightSettingsWidget::silh_contrib_event(int j)
{
	int temp = j;
/*	temp = temp%10;
	temp = j-temp;
	j = temp;*/

	silh_contrib_slider->setValue(j);
	silh_contrib=j;

	//update our slider label
	QString s;
	QString text;
	text = "Contribution: ";
	s.setNum(j);
	text += s;
	silh_contrib_lb->setText (text);

	emit light_gooch_lighting_signal(warm, cool, silh_contrib, silh_exp, alphathresh, degenthresh);
}
void QtLightSettingsWidget::alphathresh_event(int j)
{
	int temp = j;
	alphathresh_slider->setValue(j);
	alphathresh=j;

	//update our slider label
	QString s;
	QString text;
	text = "Alpha Threshold: ";
	s.setNum(j);
	text += s;
	alphathresh_lb->setText (text);

	emit light_gooch_lighting_signal(warm, cool, silh_contrib, silh_exp, alphathresh, degenthresh);
}
void QtLightSettingsWidget::degenthresh_event(int j)
{
	int temp = j;
	degenthresh_slider->setValue(j);
	degenthresh=j;

	//update our slider label
	QString s;
	QString text;
	text = "Threshold: ";
	s.setNum(j);
	text += s;
	degenthresh_lb->setText (text);

	emit light_gooch_lighting_signal(warm, cool, silh_contrib, silh_exp, alphathresh, degenthresh);
}

void QtLightSettingsWidget::silh_exp_event(int j)
{
	int temp = j;
/*	temp = temp%10;
	temp = j-temp;
	j = temp;*/

	silh_exp_slider->setValue(j);
	silh_exp=j;

	//update our slider label
	QString s;
	QString text;
	text = "Exponent: ";
	s.setNum(j);
	text += s;
	silh_exp_lb->setText (text);

	emit light_gooch_lighting_signal(warm, cool, silh_contrib, silh_exp, alphathresh, degenthresh);
}
void QtLightSettingsWidget::l_altitude_event(int j)
{
	int temp = j;
/*	temp = temp%10;
	temp = j-temp;
	j = temp;*/

	l_altitude_slider->setValue(j);
	l_altitude=j;

	//update our slider label
	QString s;
	QString text;
	text = "Altitude: ";
	s.setNum(j);
	text += s;
	l_altitude_lb->setText (text);

	emit light_altitude_signal(-j);
}
void QtLightSettingsWidget::l_azimuth_event(int j)
{
	int temp = j;
/*	temp = temp%10;
	temp = j-temp;
	j = temp;*/

	l_azimuth_slider->setValue(j);
	l_azimuth=j;

	//update our slider label
	QString s;
	QString text;
	text = "Azimuth: ";
	s.setNum(j);
	text += s;
	l_azimuth_lb->setText (text);

	emit light_azimuth_signal(j);
}
void QtLightSettingsWidget::volshader_combobox_event(int id)
{
	if(id==0)
	{
		lightcolour_grid->show();
		goochlight_grid->hide();
		silhoutte_grp->hide();
		semsilhoutte_grp->hide();
		depth_grp->hide();
		depthpeal_grp->hide();

		curr_shader = SHADER_NOLIGHT;
		emit light_volshader_signal(SHADER_NOLIGHT);
	}
	else if(id==1)
	{
		lightcolour_grid->show();
		depth_grp->show();
		goochlight_grid->hide();
		silhoutte_grp->hide();
		semsilhoutte_grp->hide();
		depthpeal_grp->hide();

		curr_shader = SHADER_PHOTOREAL;
		emit light_volshader_signal(SHADER_PHOTOREAL);
	}
	else if(id==2)
	{
		lightcolour_grid->hide();
		goochlight_grid->show();
		silhoutte_grp->show();
		depth_grp->show();
		semsilhoutte_grp->hide();
		depthpeal_grp->hide();

		curr_shader = SHADER_NONPHOTOREAL;
		emit light_volshader_signal(SHADER_NONPHOTOREAL);
		emit light_gooch_lighting_signal(warm, cool, silh_contrib, silh_exp, alphathresh, degenthresh);
	}
	else if(id==3)
	{
		
		
		lightcolour_grid->show();
		goochlight_grid->hide();
		silhoutte_grp->hide();
		semsilhoutte_grp->hide();
		depth_grp->hide();
		depthpeal_grp->show();

		curr_shader = SHADER_DEPTH;
		emit light_volshader_signal(SHADER_DEPTH);

	}
}

void QtLightSettingsWidget::semsilh_exp_event(int j)
{
	int temp = j;
/*	temp = temp%10;
	temp = j-temp;
	j = temp;*/

	semsilh_exp_slider->setValue(j);
	semsilh_exp=j;

	//update our slider label
	QString s;
	QString text;
	text = "Exponent: ";
	s.setNum(j);
	text += s;
	semsilh_exp_lb->setText (text);

	emit light_gooch_lighting_signal(warm, cool, semsilh_contrib, semsilh_exp, alphathresh, degenthresh);
}

void QtLightSettingsWidget::semsilh_contrib_event(int j)
{
	int temp = j;
/*	temp = temp%10;
	temp = j-temp;
	j = temp;*/

	semsilh_contrib_slider->setValue(j);
	semsilh_contrib=j;

	//update our slider label
	QString s;
	QString text;
	text = "Contribution: ";
	s.setNum(j);
	text += s;
	semsilh_contrib_lb->setText (text);

	emit light_gooch_lighting_signal(warm, cool, semsilh_contrib, semsilh_exp, alphathresh, degenthresh);
}
void QtLightSettingsWidget::depthshade_contrib_event(int value)
{
	depthshade_contrib_slider->setValue(value);
	depthshade_contrib=value;

	//update our slider label
	QString s;
	QString text;
	text = "Scale: ";
	s.setNum(value);
	text += s;
	depthshade_contrib_lb->setText (text);

	emit light_depthcue_signal(depthcol, depthshade_contrib);
}
void QtLightSettingsWidget::depthoffset_event(int value)
{
	depthoffset_slider->setValue(value);
	depthoffset=value;

	//update our slider label
	QString s;
	QString text;
	text = "Offset: ";
	s.setNum(value);
	text += s;
	depthoffset_lb->setText (text);

	emit light_depthpeal_signal(depthoffset, depthpeal);
}
void QtLightSettingsWidget::depthpeal_event(int value)
{
	depthpeal_slider->setValue(value);
	depthpeal=value;

	//update our slider label
	QString s;
	QString text;
	text = "Thickness: ";
	s.setNum(value);
	text += s;
	depthpeal_lb->setText (text);

	emit light_depthpeal_signal(depthoffset, depthpeal);
}

void QtLightSettingsWidget::depth_colourdlg_event(void)
{
	QColorDialog* coldialogue=0;
	depth_colour = coldialogue->getColor(depth_colour, this, "Choose a colour");
	QPixmap pixmap = QPixmap(32, 32);
	pixmap.fill(depth_colour);
	depthcol_lb->setPixmap(pixmap);

	depthcol.x = depth_colour.red();
	depthcol.y = depth_colour.green();
	depthcol.z = depth_colour.blue();
	depthcol /= 255.0;

	emit light_depthcue_signal(depthcol, depthshade_contrib);
}