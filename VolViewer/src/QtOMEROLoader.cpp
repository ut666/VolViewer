/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtOMEROLoader.h"
#include "MyDefines.h"

#ifdef OMERO_SUPPORT

QtOMEROLoader::QtOMEROLoader(QWidget *parent, Qt::WFlags flags, OMEROLoader* omerol, QString appPath)
{
//	QWidget* widg = new QWidget();
//	setCentralWidget(widg);

	omeroloader = omerol;
	QString linevalue;

	QLabel* logo_label = new QLabel( this, "logo", Qt::Widget );
	logo_label->setPixmap(QPixmap(appPath+"/Icons/OME.png"));
	logo_label->setBackgroundColor(Qt::black);
	logo_label->setAutoFillBackground(true);
	logo_label->setAlignment(Qt::AlignCenter);
	logo_label->setFixedHeight(QPixmap(appPath+"/Icons/OME.png").height()*1.2);

	QLabel* loginlabel = new QLabel(this, "loginlabel" );
	loginlabel->setText("Server:");
	loginlabel->setFont( QFont( "System", 12 ) );

	serveraddress_lineedit = new QLineEdit(this);
	serveraddress_lineedit->insert(omeroloader->svrname.data());
	serveraddress_lineedit->setFixedWidth (300);
	connect(serveraddress_lineedit, SIGNAL(textChanged(QString)), this, SLOT(serveraddress_event(QString)));

	serverport_lineedit = new QLineEdit(this);
	serverport_lineedit->insert(linevalue.setNum(omeroloader->port));
	serverport_lineedit->setFixedWidth(100);
	connect(serverport_lineedit, SIGNAL(textChanged(QString)), this, SLOT(serverport_event(QString)));

	QLabel* usernamelabel = new QLabel(this, "usernamelabel" );
	usernamelabel->setText("Username:");
	usernamelabel->setFont( QFont( "System", 12 ) );

	username_lineedit = new QLineEdit(this);
	username_lineedit->insert(omeroloader->username.data());
	connect(username_lineedit, SIGNAL(textChanged(QString)), this, SLOT(username_event(QString)));

	QLabel* passwdlabel = new QLabel(this, "passwdlabel" );
	passwdlabel->setText("Password:");
	passwdlabel->setFont( QFont( "System", 12 ) );

	passwd_lineedit = new QLineEdit(this);
	passwd_lineedit->setEchoMode(QLineEdit::Password);
	passwd_lineedit->insert(omeroloader->psswd.data());
	connect(passwd_lineedit, SIGNAL(textChanged(QString)), this, SLOT(passwd_event(QString)));

	QLabel* imgidlabel = new QLabel(this, "imgidlabel" );
	imgidlabel->setText("ImageID:");
	imgidlabel->setFont( QFont( "System", 12 ) );

	imgID_lineedit = new QLineEdit(this);
	imgID_lineedit->insert(linevalue.setNum(omeroloader->imageID));
	connect(imgID_lineedit, SIGNAL(textChanged(QString)), this, SLOT(imgID_event(QString)));
	
	connect_button = new QPushButton("Connect", this);
	connect(connect_button, SIGNAL(pressed()), this, SLOT(okbutton_event()));
	cancel_button = new QPushButton("Cancel", this);
	connect(cancel_button, SIGNAL(pressed()), this, SLOT(cancelbutton_event()));

	QHBoxLayout* serverhboxlayout = new QHBoxLayout();
	serverhboxlayout->addWidget(loginlabel, 0, Qt::AlignJustify );
	serverhboxlayout->addWidget(serveraddress_lineedit, 0, Qt::AlignJustify );
	serverhboxlayout->addWidget(serverport_lineedit, 0, Qt::AlignJustify );

	QHBoxLayout* accounthboxlayout = new QHBoxLayout();
	accounthboxlayout->addWidget(usernamelabel, 0, Qt::AlignJustify );
	accounthboxlayout->addWidget(username_lineedit, 0, Qt::AlignJustify );
	accounthboxlayout->addWidget(passwdlabel, 0, Qt::AlignJustify );
	accounthboxlayout->addWidget(passwd_lineedit, 0, Qt::AlignJustify );

	QHBoxLayout* imgboxlayout = new QHBoxLayout();
	imgboxlayout->addWidget(imgidlabel, 0, Qt::AlignJustify);
	imgboxlayout->addWidget(imgID_lineedit, 0, Qt::AlignJustify);
	imgboxlayout->addWidget(cancel_button, 0, Qt::AlignJustify);
	imgboxlayout->addWidget(connect_button, 0, Qt::AlignJustify);

	QVBoxLayout* vboxlayout = new QVBoxLayout(this);
	vboxlayout->addWidget(logo_label);
	vboxlayout->addLayout(serverhboxlayout);
	vboxlayout->addLayout(accounthboxlayout);
	vboxlayout->addLayout(imgboxlayout);

	setFixedSize(0, 425);
}

QtOMEROLoader::~QtOMEROLoader()
{
}
void QtOMEROLoader::serveraddress_event(QString le)
{
	omeroloader->svrname = serveraddress_lineedit->text().latin1();;
}
void QtOMEROLoader::serverport_event(QString le)
{
	omeroloader->port = serverport_lineedit->text().toInt();
}
void QtOMEROLoader::username_event(QString le)
{
	omeroloader->username = username_lineedit->text().latin1();;
}
void QtOMEROLoader::passwd_event(QString le)
{
	omeroloader->psswd = passwd_lineedit->text().latin1();
}
void QtOMEROLoader::imgID_event(QString le)
{
	omeroloader->imageID = imgID_lineedit->text().toInt();
}
void QtOMEROLoader::cancelbutton_event(void)
{
	close();
}
void QtOMEROLoader::okbutton_event(void)
{
	close();
}
#endif //OMERO_SUPPORT