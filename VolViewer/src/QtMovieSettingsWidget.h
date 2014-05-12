/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QtMovieSettingsWidget_H
#define QtMovieSettingsWidget_H

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
#include <QFileDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QMessageBox>

#include "Vector.h"
#include "ColourFun.h"

class QtMovieSettingsWidget: public QWidget
{
	Q_OBJECT
		
public:

	QtMovieSettingsWidget(QWidget* parent, const char* name);
	~QtMovieSettingsWidget();

	QPushButton* load;
	QPushButton* save;

	QPushButton* choosedir;
	QTextEdit* choosedir_te;
	QString defaul_movie_dir;

	QLabel* moviesampling_lb;
	QSlider* moviesampling_slider;
	int moviesampling;
	
	QPushButton *preview;
	
	QPushButton *xbutton;
	QPushButton *ybutton;
	QPushButton *zbutton;
	QLineEdit	*angle_le;

	QDialog* rotationaxis_dlg;
	float rotangle;

	float rockangle;

	float faderotangle;
	QComboBox* channelOne_combobox;
	int channelOne_selected;
	QComboBox* channelTwo_combobox;
	int channelTwo_selected;
	QComboBox* channelThree_combobox;
	int channelThree_selected;

	float cliprotang;
	QPushButton *clipafterrotationbutton;
	QPushButton *clipvbutton;
	QPushButton *clipxbutton;
	QPushButton *clipybutton;
	QPushButton *clipzbutton;

	void reset(void);
	void create(void);
	void apply(void);
	void rebuild(void);

private slots:
	
	void load_event(void);
	void save_event(void);
	void choosedir_event(void);
	void moviesampling_event(int);
	void movie_rock_rotate_event(void);
	void movie_orthosect_event(void);
	void movie_rotate_event(void);
	void movie_crosschanfade_event(void);
	void movie_chanfade_event(void);
	void movie_clip_event(void);
	void movie_renderview_event(void);

	void okbuttonrot_event(void);
	void okbuttonrock_event(void);
	void okbuttonfade_event(void);
	void okbuttonclip_event(void);
	void okbuttoncrossfade_event(void);
	void okbuttonrenderview_event(void);

	void cancbuttonrot_event(void);

	void channelOne_combobox_event(int);
	void channelTwo_combobox_event(int);
	void channelThree_combobox_event(int);

signals:
	void movie_orthosectmovie_signal(QWidget* parent, int sampling, int preview, QString dn);
	void movie_rockmovie_signal(QWidget* parent, int sampling, int preview, QString dn, float ang);
	void movie_rotmovie_signal(QWidget* parent, int sampling, int preview, QString dn, int x, int y, int z, float ang);
	void movie_fademovie_signal(QWidget* parent, int sampling, int preview, QString dn, int x, int y, int z, float ang, int ch1, int ch2, int ch3);
	void movie_clipmovie_signal(QWidget* parent, int sampling, int preview, QString dn, int x, int y, int z, float ang, int clipmode, int clipafterrot);
	void movie_crossfademovie_signal(QWidget* parent, int sampling, int preview, QString dn, int x, int y, int z, float ang, int ch1, int ch2, int ch3);
	void movie_renderviewmovie_signal(QWidget* parent, int sampling, int preview, QString dn);

};

#endif // QtMovieSettingsWidget_H
