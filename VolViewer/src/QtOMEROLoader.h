/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTOMEROLOADER_H
#define QTOMEROLOADER_H

#include "MyDefines.h"

#ifdef OMERO_SUPPORT

#include <QtGui/QMainWindow>
#include <QPixmap>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>

#include "OMEROLoader.h"

class QtOMEROLoader : public QDialog
{
	Q_OBJECT

public:

	QtOMEROLoader(QWidget *parent = 0, Qt::WFlags flags = 0, OMEROLoader* omerol = NULL, QString appPath = "");
	~QtOMEROLoader();

	QPushButton* connect_button;
	QLineEdit* imgID_lineedit;

private:
	OMEROLoader* omeroloader;

	QLineEdit* serveraddress_lineedit;
	QLineEdit* serverport_lineedit;
	QLineEdit* username_lineedit;
	QLineEdit* passwd_lineedit;

	QPushButton* cancel_button;

private slots:

	void serveraddress_event(QString);
	void serverport_event(QString);
	void username_event(QString);
	void passwd_event(QString);
	void imgID_event(QString);
	void okbutton_event(void);
	void cancelbutton_event(void);

signals:

};

#endif // OMERO_SUPPORT

#endif // QTNETWORKCLIENTC_H
