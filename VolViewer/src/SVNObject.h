#ifndef SVNOBJECT_H
#define SVNOBJECT_H

#include <qstring.h>

class SVNObject
{

public:
	QString revision;
	QString modified;
	QString date;
	QString range;
	QString uRL;

	SVNObject();
};

#endif //SVNOBJECT_H