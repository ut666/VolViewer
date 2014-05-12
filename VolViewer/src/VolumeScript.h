#ifndef VOLUMESCRIPT_H
#define VOLUMESCRIPT_H

#include <QString>
#include <QStringList>

#include <stdio.h>
#include <stdlib.h>

#include "MyDefines.h"
#include "ShaderDefinitions.h"
#include "Filter.h"

class QtMainWindow;

class VolumeScript
{
public:

	vector< bool > channels;
	bool uploadTexture;
	Filter filter;
	
	bool omero_batch;
	bool image_batch;
	
	bool recursive_folders;

	QtMainWindow* mw;

	QString current_working_folder;

	QStringList now_commands;
	QStringList atquit_commands;

	int current_image_index;
	QStringList open_images_path;
	QStringList open_images_path_only;
	QStringList open_images_path_relative;

	int current_omeroimage_index;
	vector<int long> omero_image_ids;

	VolumeScript();
	~VolumeScript();

	bool checkPath(QString path);
	QString fixPath(QString path);
	bool createPath(QString path);
	bool checkFile(QString filepath, QString extension);
	QString removeFunctionName(QString str, QString funcname);
	bool checkFuncArgs(QStringList* argus, int expected_siz, QString funcname);

	void parse_script(QString commands);
	void execute_script_now(void);
	void execute_script_at_quit(void);
	bool setupchannels(int offset);

	void interpret_script(QStringList commandList);

	void parse_images_folder(QString path, bool recurs);

	void open_events(QString command);
	void close_events(QString command);
	void save_events(QString command);
	void wait_events(QString command);
	void set_events(QString command);
	void movie_events(QString command);
	void commpute_events(QString command);

	void filter_start(void);
	void filter_end(void);

	void next_image(int mode);
	void next_omeroimage(int mode);
};

#endif //VOLUMESCRIPT_H