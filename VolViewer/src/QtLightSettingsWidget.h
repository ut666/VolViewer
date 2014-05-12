/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QtLightSettingsWidget_H
#define QtLightSettingsWidget_H

#include <qpainter.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qslider.h>
#include <qstring.h>
#include <q3buttongroup.h>
#include <qcheckbox.h>
#include <qimage.h>
#include <qpushbutton.h> 
#include <qcolordialog.h>
#include <qcombobox.h>
#include <q3grid.h>
#include <q3boxlayout.h>

#include "Vector.h"
#include "ColourFun.h"

class QtLightSettingsWidget: public QWidget
{
	Q_OBJECT
		
public:

	QtLightSettingsWidget(QWidget* parent, const char* name);
	~QtLightSettingsWidget();

	QPushButton* load;
	QPushButton* save;

	int curr_shader;

	Q3Grid *goochlight_grid;

	QLabel* warm_lb;
	QColor warm_colour;
	Vector warm;
	
	QLabel* cool_lb;
	QColor cool_colour;
	Vector cool;

	Q3ButtonGroup *depth_grp;

	QLabel* depthcol_lb;
	QColor depth_colour;
	Vector depthcol;

	QLabel* depthshade_contrib_lb;
	QSlider* depthshade_contrib_slider;
	int depthshade_contrib;

	Q3ButtonGroup *silhoutte_grp;

	QLabel* silh_contrib_lb;
	QSlider* silh_contrib_slider;
	int silh_contrib;

	QLabel* silh_exp_lb;
	QSlider* silh_exp_slider;
	int silh_exp;

	Q3ButtonGroup *semsilhoutte_grp;

	QLabel* semsilh_contrib_lb;
	QSlider* semsilh_contrib_slider;
	int semsilh_contrib;
	QLabel* semsilh_exp_lb;
	QSlider* semsilh_exp_slider;
	int semsilh_exp;

	Q3ButtonGroup *silhoutte2_grp;

	QLabel* degenthresh_lb;
	QSlider* degenthresh_slider;
	int degenthresh;
	QLabel* alphathresh_lb;
	QSlider* alphathresh_slider;
	int alphathresh;

	//lighting label & colour
	Q3Grid *lightcolour_grid;

	QLabel* amb_lb;
	QColor amb_colour;
	Vector amb;

	QLabel* dif_lb;
	QColor dif_colour;
	Vector diff;

	QLabel* goochspec_lb;
	QLabel* spec_lb;
	QColor spec_colour;
	Vector spec;

	//lighting position
	QLabel* l_altitude_lb;
	QSlider* l_altitude_slider;
	int l_altitude;
	QLabel* l_azimuth_lb;
	QSlider* l_azimuth_slider;
	int l_azimuth;

	QLabel* depthoffset_lb;
	QSlider* depthoffset_slider;
	int depthoffset;

	QLabel* depthpeal_lb;
	QSlider* depthpeal_slider;
	int depthpeal;

	Q3ButtonGroup* depthpeal_grp;

	void create(void);
	void reset(void);
	void rebuild(void);
	void apply(void);

private slots:
	
	void load_event(void);
	void save_event(void);
	void warm_colourdlg_event(void);
	void cool_colourdlg_event(void);
	void amb_colourdlg_event(void);
	void dif_colourdlg_event(void);
	void spec_colourdlg_event(void);
	void l_altitude_event(int value);
	void l_azimuth_event(int value);
	void volshader_combobox_event(int id);
	void silh_contrib_event(int value);
	void silh_exp_event(int value);
	void alphathresh_event(int value);
	void degenthresh_event(int value);
	void semsilh_contrib_event(int value);
	void semsilh_exp_event(int value);
	void depthshade_contrib_event(int value);
	void depth_colourdlg_event(void);
	void depthoffset_event(int value);
	void depthpeal_event(int value);

signals:
	void light_altitude_signal(float);
	void light_azimuth_signal(float);
	void light_colour_signal(Vector, Vector, Vector);
	void light_gooch_lighting_signal(Vector, Vector, int, int, int, int);
	void light_volshader_signal(int);
	void light_depthcue_signal(Vector, int);
	void light_depthpeal_signal(int,int);
};

#endif // QtLightSettingsWidget_H
