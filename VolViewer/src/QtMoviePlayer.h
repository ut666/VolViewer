/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
 #ifndef QTMOVIEPLAYER_H
 #define QTMOVIEPLAYER_H

#include <QWidget>
#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>
#include <QPixmap>
#include <QTimeLine>

#include <vector>
using namespace std;

class QtMoviePlayer : public QWidget
{
     Q_OBJECT

 public:
     QtMoviePlayer(QWidget* parent, vector<QPixmap>* frames);
	 int heightForWidth ( int w );
	 void	resizeEvent ( QResizeEvent * event );
 private slots:

	 void goToFrame(int);
	 void setFrameRate(int);
	 void updateButtons();
	 void saveButton_event(void);

 private:
     void createControls();
     void createButtons();

	 int current_index;
	vector<QPixmap>* movieframes;
	QLabel *movieLabel;

	 QTimeLine	*timeLine;

     QToolButton *playButton;
     QToolButton *pauseButton;
     QToolButton *stopButton;
     QToolButton *saveButton;

	 QCheckBox *fitCheckBox;
     QSlider *frameSlider;
     QSpinBox *speedSpinBox;
     QLabel *frameLabel;
     QLabel *speedLabel;

     QGridLayout *controlsLayout;
     QHBoxLayout *buttonsLayout;
     QVBoxLayout *mainLayout;
};

#endif