#include "VolumeScript.h"
#include "QtMainWindow.h"

VolumeScript::VolumeScript()
{
	mw=NULL;
	image_batch=false;
	omero_batch=false;
	recursive_folders=false;
}
VolumeScript::~VolumeScript()
{
	mw=NULL;
	image_batch=false;
	omero_batch=false;
	recursive_folders=false;
}
void VolumeScript::parse_images_folder(QString path, bool recursive)
{
	QDir dir;
	dir.setPath(path);
	dir.setSorting(QDir::Name);
    dir.setFilter(QDir::Dirs);
    QStringList dirList = dir.entryList();

	//printf("path: %s\n", path.toLatin1().data());
	printf("SCRIPT: Parsing %d directories\n", dirList.size());

	QStringList valid_filter;
	valid_filter.push_back("voxelspacing.txt");
	valid_filter.push_back("voxelscale.txt");
	valid_filter.push_back("*.log");

	open_images_path.clear();
	open_images_path_only.clear();
	open_images_path_relative.clear();

	QDirIterator* it;

	if(recursive==true)
		it = new QDirIterator(path, QDir::Dirs|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	else
		it = new QDirIterator(path, QDir::Dirs|QDir::NoDotAndDotDot);

	//ofstream fout;
	//fout.open("Folder_paths.txt");

	while (it->hasNext())
	{
		QString current_dir = it->next();
//		QString current_dir = dirList[i];
		printf("current_dir: %s\n", current_dir.toLatin1().data());

		QDir subdir(current_dir);
		subdir.setSorting(QDir::Name);
		subdir.setFilter(QDir::Files);
		QStringList list = subdir.entryList(valid_filter);
		
		if(!list.empty())
		{
			//printf("VALID\n");
			open_images_path.push_back(current_dir+"/");

			//printf("current_dir: %s\n", current_dir.toLatin1().data());

			QString filename;
			QString pathname;
			QStringList list1 = current_dir.split("/");
			if(!list1.empty())
			{
				filename = list1[list1.size()-1];

				pathname="/";
				for(int i=1; i<list1.size(); i++)
				{
					pathname += list1[i]+"/";
				}
			}

			//printf("pathname: %s\n", pathname.toLatin1().data());	
			//system("pause");

			//TO DO!!!
			//fout<<filename.latin1()<<"\t"<<pathname.latin1()<<endl;
			open_images_path_only.push_back(filename);
			open_images_path_relative.push_back(pathname);
		}
	}

	//fout.close();
	/*for(int i=0; i<open_images_path.size(); i++)
	{
		QString current_dir = open_images_path[i];
		printf("valid_dir: %s\n", current_dir.toLatin1().data());
	}*/

	printf("SCRIPT: Found %d VALID directories\n", open_images_path.size());

	if(open_images_path.empty())
	{
		printf("WARNING: No Image paths have been found!\n"); 
	}
}
void VolumeScript::parse_script(QString commands)
{
	QStringList commandList = commands.split("\n");
	QString images_folder_path;

	now_commands.clear();
	atquit_commands.clear();

	bool atquit=false;
	image_batch=false;
	omero_batch=false;
	for(int i=0; i<commandList.size(); i++)
	{
		//printf("PARSING SCRIPT : %s\n", commandList[i].toLatin1().data());

		if(commandList[i].contains("wait_for_quit"))
		{
			atquit = true;
			i++;
			if(i>commandList.size()-1) break;
		}
		
		if(atquit==false)
		{
			if(commandList[i].contains("set_logfile"))
			{
				QString argstr = removeFunctionName(commandList[i],"set_logfile");
				QStringList funcargs = argstr.split(",");

				if(checkFuncArgs(&funcargs, 1, "set_logfile"))
				{
					QString logfilepath = funcargs[0];
					logfilepath = fixPath(logfilepath);

					QFile file(logfilepath);
					file.open(QIODevice::WriteOnly);
					file.close();
	 
					printf("LOGFILE: %s\n", logfilepath.latin1());

					freopen(logfilepath.latin1(), "w", stdout);
					freopen(logfilepath.latin1(), "w", stderr);

					#ifdef WIN32
						FreeConsole();
					#endif
				}
			}
			else if(commandList[i].contains("set_hide()"))
			{
				filter.showGUI = false;
				mw->glwindow->glvoxel->showGUI = false;
				mw->glwindow->glvoxel->volio.showGUI = false;
				mw->setVisible(false);
			}
#ifdef OMERO_SUPPORT
			else if(commandList[i].contains("open_omero_connection"))
			{
				QString argstr = removeFunctionName(commandList[i],"open_omero_connection");
				QStringList funcargs = argstr.split(",");

				printf("funcargs: %d\n", funcargs.size());

				if(checkFuncArgs(&funcargs, 3, "open_omero_connection"))
				{
					printf("OMERO: using sessionID\n");

					//via sessionID
					QString servername = funcargs[0];
					int serverport = funcargs[1].toInt();
					QString sessionID = funcargs[2];
					servername = servername.remove("'");
					sessionID = sessionID.remove("'");
				
					mw->omeroLoader.port = serverport;
					mw->omeroLoader.connect(servername.ascii(), sessionID.ascii());
				}
				else if(checkFuncArgs(&funcargs, 4, "open_omero_connection"))
				{
					printf("OMERO: using username/password\n");

					//via username/password
					QString servername = funcargs[0];
					int port = funcargs[1].toInt();
					QString username = funcargs[2];
					QString password = funcargs[3];
				
					servername = servername.remove("'");
					username = username.remove("'");
					password = password.remove("'");

					/*printf("servername: %s\n", servername.toLatin1().data());
					printf("port: %d\n", port);
					printf("username: %s\n", username.toLatin1().data());
					printf("password: %s\n", password.toLatin1().data());*/

					mw->omeroLoader.svrname = servername.ascii();
					mw->omeroLoader.username = username.ascii();
					mw->omeroLoader.psswd = password.ascii();					
					mw->omeroLoader.port = port;

					if(mw->omeroLoader.connect()==-1)
					{
						printf("ERROR: Failed to connect to OMERO server.\n");
						return;
					}	

				}
			}
#endif
		}
		if(commandList[i].contains("open_images_folder"))
		{
			image_batch = true;

			QString argstr = removeFunctionName(commandList[i],"open_images_folder");
			QStringList funcargs = argstr.split(",");

			if(checkFuncArgs(&funcargs, 3, "open_images_folder"))
			{
				images_folder_path = funcargs[1]; 
				images_folder_path = fixPath(images_folder_path);

				if(funcargs[2]=="TRUE" || funcargs[2]=="true" || funcargs[2]=="1") recursive_folders = true;
				else recursive_folders = false;
			}

			printf("SCRIPT : %s\n", images_folder_path.toLatin1().data());
			current_image_index = 0;
		}
#ifdef OMERO_SUPPORT
		if(commandList[i].contains("open_omero_project"))
		{
			omero_batch= true;
			current_omeroimage_index=0;

			QString argstr = removeFunctionName(commandList[i],"open_omero_project");
			QStringList funcargs = argstr.split(",");

			if(checkFuncArgs(&funcargs, 1, "open_omero_project"))
			{
				int long projID = funcargs[0].toInt(); 
				mw->omeroLoader.getProjectImages(projID, &omero_image_ids);
			}
		}
		if(commandList[i].contains("open_omero_dataset"))
		{
			omero_batch= true;
			current_omeroimage_index=0;

			QString argstr = removeFunctionName(commandList[i],"open_omero_dataset");
			QStringList funcargs = argstr.split(",");

			if(checkFuncArgs(&funcargs, 1, "open_omero_dataset"))
			{
				int long dsID = funcargs[0].toInt(); 
				mw->omeroLoader.getDatasetImages(dsID, &omero_image_ids);
			}
		}
#endif
		if(atquit==false)
		{
			//printf("SCRIPT NOW: %s\n", commandList[i].toLatin1().data());
			now_commands.push_back(commandList[i]);
		}
		else
		{
			//printf("SCRIPT ATQUIT: %s\n", commandList[i].toLatin1().data());
			atquit_commands.push_back(commandList[i]);
		}
	}

	if(image_batch)
	{
		parse_images_folder(images_folder_path, recursive_folders);
	}
//	mw->close();
}
bool VolumeScript::checkFile(QString filepath, QString extension)
{
	if(filepath.endsWith(extension))
	{
		QFile file(filepath);

		if(file.exists()) return true;
		else
		{
			printf("WARNING: File: %s\n", filepath.toLatin1().data());
			printf("WARNING: Does not exist!\n");
			return false;
		}
	}
	else
	{
		printf("WARNING: Incorect file extension: %s\n", filepath.toLatin1().data());
		printf("WARNING: Was expecting: %s.\n", extension.toLatin1().data());
		return false;
	}
}
bool VolumeScript::createPath(QString path)
{
	QFileInfo fileinfo(path);
	
	QDir dir(fileinfo.absolutePath());
	if(!dir.exists()) dir.mkpath(fileinfo.absolutePath());

	if(dir.exists()) return true;
	else
	{
		printf("WARNING: Could not create path: %s\n", path.toLatin1().data());
		return false;
	}
}
bool VolumeScript::checkPath(QString path)
{
	QFileInfo fileinfo(path);
	QDir dir(fileinfo.absolutePath());
	if(!dir.exists())
	{
		printf("WARNING: Could not find the path: %s\n", path.toLatin1().data());
		return false;
	}
	else
	{
		return true;
	}
}
QString VolumeScript::fixPath(QString path)
{
	path = path.remove("'");
	path = path.replace('\\', '/');

	QDir dir(path);

	if(dir.isRelative())
	{
		//printf("isRelative\n");
		path = current_working_folder + path;
	}
	
	if(path.contains("##ORIGINALNAME##", true) && !open_images_path_only.empty())
	{
		path = path.replace("##ORIGINALNAME##", open_images_path_only[current_image_index]);
	}

	if(path.contains("##ORIGINALPATH##", true) && !open_images_path_relative.empty())
	{
		path = path.replace("##ORIGINALPATH##", open_images_path_relative[current_image_index]);
	}
	
	if(path.contains("."))
	{
	}
	else path += '/';

	//printf("SCRIPT: fixed path = %s\n", path.latin1());
	return path;
}
QString VolumeScript::removeFunctionName(QString str, QString funcname)
{
//	QString arguments = str.remove(funcname+"(");
//	arguments.remove(")");
	
	int func_length = funcname.length()+1;
	int start = func_length + str.find(funcname+"(");
	int end = str.lastIndexOf(")") - func_length;
	QString arguments = str.mid(start, end);

	//printf("SCRIPT: start: %d, end: %d\n", start, end);
	//printf("SCRIPT: %s: %s\n", funcname.toLatin1().data(), arguments.toLatin1().data());
	return arguments;
}
bool VolumeScript::checkFuncArgs(QStringList* argus, int expected_size, QString funcname)
{
	for(int i=0; i<argus->size(); i++)
	{
		if(argus->at(i).contains("'")==false)
		{
				//printf("SCRIPT: argument[%d], removing spaces\n", i);
				argus->replace(i, argus->at(i).stripWhiteSpace());
		}
		else
		{
				//printf("SCRIPT: argument[%d], trimming spaces\n", i);
				argus->replace(i, argus->at(i).trimmed());			
		}
		//printf("SCRIPT: argument[%d], %s\n", i, argus->at(i).toLatin1().data());
	}
	if(argus->size()==expected_size)
	{
		return true;
	}
	else
	{
		printf("WARNING: Incorrect number of arguments to funstion: %s\n", funcname.toLatin1().data());
		return false;
	}
}
void VolumeScript::interpret_script(QStringList commandList)
{
	for(int i=0; i<commandList.size(); i++)
	{
		printf("SCRIPT: %s \n", commandList[i].toLatin1().data());

		open_events(commandList[i]);
		close_events(commandList[i]);
		save_events(commandList[i]);
		set_events(commandList[i]);
		movie_events(commandList[i]);
		commpute_events(commandList[i]);
		wait_events(commandList[i]);
	}
}
void VolumeScript::execute_script_now(void)
{
	interpret_script(now_commands);
	if(image_batch==true && atquit_commands.empty()==true) next_image(0);
	if(omero_batch==true && atquit_commands.empty()==true) next_omeroimage(0);
}
void VolumeScript::execute_script_at_quit(void)
{
	interpret_script(atquit_commands);
	if(image_batch==true  && atquit_commands.empty()==false) next_image(1);
	if(omero_batch==true && atquit_commands.empty()==false) next_omeroimage(1);
}
void VolumeScript::next_image(int mode)
{
	if(current_image_index<open_images_path.size()-1)
	{
		current_image_index++;
		if(mode==0) execute_script_now();
		else execute_script_at_quit();
	}
	else
	{
		image_batch = false;
	}
}
void VolumeScript::next_omeroimage(int mode)
{
	if(current_omeroimage_index<omero_image_ids.size()-1)
	{
		current_omeroimage_index++;
		if(mode==0) execute_script_now();
		else execute_script_at_quit();
	}
	else
	{
		omero_batch = false;
	}
}

void VolumeScript::wait_events(QString command)
{
	if(command.contains("wait_for_opengl()"))
	{
		glFinish();
	}
}
void VolumeScript::open_events(QString command)
{
		//OPEN_IMAGE_STACK
		if(command.contains("open_image_stack"))
		{			
			QString argstr = removeFunctionName(command,"open_image_stack");
			QStringList funcargs = argstr.split(",");

			if(checkFuncArgs(&funcargs, 2, "open_image_stack"))
			{
				int channelmode = funcargs[0].toInt();
				QString path = fixPath(funcargs[1]);

				mw->openImageChannelMode(channelmode+1);
				if(checkPath(path)) mw->openImageDir(path);
			}
		}
		//OPEN_IMAGE_RAW
		else if(command.contains("open_image_raw"))
		{
			QString argstr = removeFunctionName(command,"open_image_raw");
			QStringList funcargs = argstr.split(",");

			if(checkFuncArgs(&funcargs, 2, "open_image_raw"))
			{
				int channelmode = funcargs[0].toInt();
				QString path = fixPath(funcargs[1]);

				mw->openImageChannelMode(channelmode+1);
				if(checkFile(path, ".raw") || checkFile(path, ".dat")) mw->openRawFile(path);
			}
		}
		//OPEN_IMAGE_FOLDER
		else if(command.contains("open_images_folder"))
		{
			QString path = open_images_path[current_image_index];
			path = fixPath(path);
			current_working_folder = path;

			printf("SCRIPT: Loading %s \n", path.latin1());

			//printf("SCRIPT: path = %s\n", path.toLatin1().data()); 

			QString argstr = removeFunctionName(command,"open_images_folder");
			QStringList funcargs = argstr.split(",");

			if(checkFuncArgs(&funcargs, 3, "open_images_folder"))
			{
				int channelmode = funcargs[0].toInt();
				//QString path = fixPath(funcargs[1]);

				mw->openImageChannelMode(channelmode+1);
				if(checkPath(path)) mw->openImageDir(path);
			}
		}
		//OPEN_INI
		else if(command.contains("open_ini"))
		{
			QString argstr = removeFunctionName(command,"open_ini");
			QStringList funcargs = argstr.split(",");

			if(checkFuncArgs(&funcargs, 1, "open_ini"))
			{
				QString path = funcargs[0];
				path = fixPath(path);
			
				if(checkFile(path, ".ini")) mw->parse_inifile(path);
			}
		}
		//OPEN_MSR
		else if(command.contains("open_msr"))
		{
			QString argstr = removeFunctionName(command,"open_msr");
			QStringList funcargs = argstr.split(",");

			if(checkFuncArgs(&funcargs, 1, "open_msr"))
			{
				QString path = funcargs[0];
				path = fixPath(path);
				if(checkFile(path, ".msr")) mw->measure_editor_tabdlg->measuring_tab->open_file(path);
			}
		}
		//OPEN_SLC
		else if(command.contains("open_slc"))
		{
			QString argstr = removeFunctionName(command,"open_slc");
			QStringList funcargs = argstr.split(",");

			if(checkFuncArgs(&funcargs, 1, "open_slc"))
			{
				QString path = funcargs[0];
				path = fixPath(path);

				if(checkFile(path, ".slc")) mw->glwindow->glvoxel->cutplane_load_slot(path);
			}
		}		
		//OPEN_TFN
		else if(command.contains("open_tfn"))
		{
			QString argstr = removeFunctionName(command,"open_tfn");
			QStringList funcargs = argstr.split(",");

			if(checkFuncArgs(&funcargs, 1, "open_tfn"))
			{
				QString path = funcargs[0];
				path = fixPath(path);

				if(checkFile(path, ".tfn")) mw->transferfunction_editor_tabdlg->tranfer_function_tab->hist->loadfile(path);
			}
		}
		//OPEN_VRV
		else if(command.contains("open_vrv"))
		{
			QString argstr = removeFunctionName(command,"open_vrv");
			QStringList funcargs = argstr.split(",");

			if(checkFuncArgs(&funcargs, 1, "open_vrv"))
			{
				QString path = funcargs[0];
				path = fixPath(path);

				if(checkFile(path, ".vrv")) mw->renderview_editor_tabdlg->renviewtab->load_file(path);
			}
		}
		//OPEN_OBJ
		else if(command.contains("open_obj"))
		{
			QString argstr = removeFunctionName(command,"open_obj");
			QStringList funcargs = argstr.split(",");

			if(checkFuncArgs(&funcargs, 1, "open_obj"))
			{
				QString path = funcargs[0];
				path = fixPath(path);

				if(checkFile(path, ".obj")) mw->glwindow->glvoxel->isosurface_load_slot(path.latin1());
			}
		}
		//OPEN_SCRIPT
		else if(command.contains("open_script"))
		{
			QString argstr = removeFunctionName(command,"open_script");
			QStringList funcargs = argstr.split(",");

			if(checkFuncArgs(&funcargs, 1, "open_script"))
			{
				QString path = funcargs[0];
				path = fixPath(path);

				if(checkFile(path, ".txt")) mw->runScript(path.latin1());
			}
		}
#ifdef OMERO_SUPPORT
		else if(command.contains("open_omero_connection"))
		{
			QString argstr = removeFunctionName(command, "open_omero_connection");
			QStringList funcargs = argstr.split(",");

			printf("funcargs: %d\n", funcargs.size());

			if(checkFuncArgs(&funcargs, 3, "open_omero_connection"))
			{
				printf("OMERO: using sessionID\n");

				//via sessionID
				QString servername = funcargs[0];
				int serverport = funcargs[1].toInt();
				QString sessionID = funcargs[2];
				servername = servername.remove("'");
				sessionID = sessionID.remove("'");
			
				mw->omeroLoader.port = serverport;
				mw->omeroLoader.connect(servername.ascii(), sessionID.ascii());
			}
			else if(checkFuncArgs(&funcargs, 4, "open_omero_connection"))
			{
				printf("OMERO: using username/password\n");

				//via username/password
				QString servername = funcargs[0];
				int port = funcargs[1].toInt();
				QString username = funcargs[2];
				QString password = funcargs[3];
			
				servername = servername.remove("'");
				username = username.remove("'");
				password = password.remove("'");

				/*printf("servername: %s\n", servername.toLatin1().data());
				printf("port: %d\n", port);
				printf("username: %s\n", username.toLatin1().data());
				printf("password: %s\n", password.toLatin1().data());*/

				mw->omeroLoader.svrname = servername.ascii();
				mw->omeroLoader.username = username.ascii();
				mw->omeroLoader.psswd = password.ascii();					
				mw->omeroLoader.port = port;

				if(mw->omeroLoader.connect()==-1)
				{
					printf("ERROR: Failed to connect to OMERO server.\n");
					return;
				}	

			}
		}
		else if(command.contains("open_omero_project"))
		{
			int long imgID = omero_image_ids[current_omeroimage_index];
			mw->omeroLoader.imageID = imgID;
			mw->openOmeroImage_Load();
		}
		else if(command.contains("open_omero_dataset"))
		{
			int long imgID = omero_image_ids[current_omeroimage_index];
			mw->omeroLoader.imageID = imgID;
			mw->openOmeroImage_Load();
		}
		else if(command.contains("open_omero_image"))
		{
			printf("open_omero_image\n");

			QString argstr = removeFunctionName(command,"open_omero_image");
			QStringList funcargs = argstr.split(",");

			if(checkFuncArgs(&funcargs, 1, "open_omero_image"))
			{
				int long imgID = funcargs[0].toInt();

				mw->omeroLoader.imageID = imgID;
				mw->openOmeroImage_Load();
			}
		}
#endif
}
void VolumeScript::close_events(QString command)
{
	//CLOSE_OMERO_CONNECTION
	if(command.contains("close_omero_connection"))
	{			
#ifdef OMERO_SUPPORT
		mw->omeroLoader.disconnect();
#endif
	}
}

void VolumeScript::save_events(QString command)
{
	//SAVE_IMAGE_STACK
	if(command.contains("save_image_stack"))
	{
		QString argstr = removeFunctionName(command,"save_image_stack");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "save_image_stack"))
		{
			QString path = funcargs[0];
			path = fixPath(path);

			if(createPath(path)) mw->glwindow->glvoxel->volio.save_PNG(path);
		}
	}
	//SAVE_IMAGE_RAW
	else if(command.contains("save_image_raw"))
	{
		QString argstr = removeFunctionName(command,"save_image_raw");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "save_image_raw"))
		{
			QString path = funcargs[0];
			path = fixPath(path);

			if(createPath(path)) mw->glwindow->glvoxel->volio.save_RAW(path);
		}
	}
	//SAVE_MSR
	else if(command.contains("save_msr"))
	{
		QString argstr = removeFunctionName(command,"save_msr");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "save_msr"))
		{
			QString path = funcargs[0];
			path = fixPath(path);

			if(createPath(path)) mw->measure_editor_tabdlg->measuring_tab->save_file(path);
		}
	}		
	//SAVE_TFN
	else if(command.contains("save_tfn"))
	{
		QString argstr = removeFunctionName(command,"save_tfn");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "save_tfn"))
		{
			QString path = funcargs[0];
			path = fixPath(path);

			if(createPath(path)) mw->transferfunction_editor_tabdlg->tranfer_function_tab->hist->savefile(path);
		}
	}		
	//SAVE_VVR
	else if(command.contains("save_vrv"))
	{
		QString argstr = removeFunctionName(command,"save_vrv");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "save_vrv"))
		{
			QString path = funcargs[0];
			path = fixPath(path);

			if(createPath(path)) mw->renderview_editor_tabdlg->renviewtab->save_file(path);
		}
	}
	//SAVE_OBJ
	else if(command.contains("save_obj"))
	{
		QString argstr = removeFunctionName(command,"save_obj");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "save_obj"))
		{
			QString path = funcargs[0];
			path = fixPath(path);

			if(createPath(path)) mw->glwindow->glvoxel->volrender.meshlist.save(path);
		}
	}		
	//SAVE_SLC
	else if(command.contains("save_slc"))
	{
		QString argstr = removeFunctionName(command,"save_slc");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "save_slc"))
		{
			QString path = funcargs[0];
			path = fixPath(path);

			if(createPath(path)) mw->glwindow->glvoxel->cutplane_save_slot(path);
		}
	}
	//SAVE_PROJECTION
	else if(command.contains("save_projection"))
	{
		QString argstr = removeFunctionName(command,"save_projection");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "save_projection"))
		{
			QString path = funcargs[0];
			path = fixPath(path);

			if(createPath(path)) mw->glwindow->saveimageEvent(path, 1);
		}
	}
	else if(command.contains("save_text"))
	{		
		QString argstr = removeFunctionName(command,"save_text");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 2, "save_text"))
		{
			QString txt = funcargs[0];
			txt = txt.remove("'");

			QString path = funcargs[1];
			path = fixPath(path);

			if(createPath(path))
			{
				ofstream fout;
				fout.open(path.latin1());
				fout<<txt.latin1()<<endl;
				fout.close();
			}
		}
		
	}
#ifdef OMERO_SUPPORT
	else if(command.contains("save_omero_image"))
	{
		QString argstr = removeFunctionName(command,"save_omero_image");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "save_omero_image"))
		{
				printf("save without dataset\n");
				//QString name = funcargs[0];
				QString descrip = funcargs[0];
			
				//name = name.remove("'");
				descrip = descrip.remove("'");
				
				//string name_str = name.ascii();
				string descrip_str = descrip.ascii();

				mw->saveOmeroImage_Save("", descrip_str);
		}
		if(checkFuncArgs(&funcargs, 2, "save_omero_image"))
		{
				printf("save with dataset\n");
				int long dsid = funcargs[0].toInt();
				//QString name = funcargs[1];
				QString descrip = funcargs[1];
			
				//name = name.remove("'");
				descrip = descrip.remove("'");
				
				//string name_str = name.ascii();
				string descrip_str = descrip.ascii();

				mw->saveOmeroImage_Save(dsid, "", descrip_str);
		}
	}
#endif
}
void VolumeScript::set_events(QString command)
{
	//SET_WATCHFILE
	if(command.contains("set_watchfile"))
	{
		QString argstr = removeFunctionName(command,"set_watchfile");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 1, "set_watchfile"))
		{
			QString path = funcargs[0];
			path = fixPath(path);

			mw->setWatchFile(path);
		}
	}
	else if(command.contains("set_logfile"))
	{
		QString argstr = removeFunctionName(command,"set_logfile");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "set_logfile"))
		{
			QString logfilepath = funcargs[0];
			logfilepath = fixPath(logfilepath);

			QFile file(logfilepath);
			file.open(QIODevice::WriteOnly);
			file.close();
	 
			freopen(logfilepath.latin1(), "w", stdout);
			freopen(logfilepath.latin1(), "w", stderr);

			#ifdef WIN32
				FreeConsole();
			#endif

		}
	}
	//SET_ORIENTATION
	else if(command.contains("set_orientation"))
	{
		QString argstr = removeFunctionName(command,"set_orientation");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 3, "set_orientation"))
		{
			mw->glwindow->glvoxel->volrender.setViewfromEuler(funcargs[0].toFloat(), funcargs[1].toFloat(), funcargs[2].toFloat()); 
			mw->glwindow->glvoxel->redraw_event();
		}
	}
	//SET_PROJECTION_MODE
	else if(command.contains("set_projection_mode"))
	{
		QString argstr = removeFunctionName(command,"set_projection_mode");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "set_projection_mode"))
		{
			if(funcargs[0]=="'SUM'") mw->glwindow->glvoxel->volrender.blend_mode(1);
			else if(funcargs[0]=="'MIP'") mw->glwindow->glvoxel->volrender.blend_mode(2);

			mw->glwindow->glvoxel->redraw_event();
		}
	}
	else if(command.contains("set_shader"))
	{
		QString argstr = removeFunctionName(command,"set_shader");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "set_shader"))
		{
			int shader_mode = funcargs[0].toInt();

			if(shader_mode==0) mw->glwindow->glvoxel->light_volshader_slot(SHADER_NOLIGHT);
			if(shader_mode==1) mw->glwindow->glvoxel->light_volshader_slot(SHADER_PHOTOREAL);
			if(shader_mode==2) mw->glwindow->glvoxel->light_volshader_slot(SHADER_NONPHOTOREAL);
			if(shader_mode==3) mw->glwindow->glvoxel->light_volshader_slot(SHADER_DEPTH);
			mw->glwindow->glvoxel->redraw_event();			
		}
	}
	else if(command.contains("set_view"))
	{
		QString argstr = removeFunctionName(command,"set_view");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "set_view"))
		{
			printf("SET VIEW: %s\n", funcargs[0].latin1());

			if(funcargs[0]=="'TOP'")
			{
				mw->top_event();
			}
			if(funcargs[0]=="'BOTTOM'")
			{
				mw->bottom_event();
			}
			if(funcargs[0]=="'RIGHT'")
			{
				mw->right_event();
			}
			if(funcargs[0]=="'LEFT'")
			{
				mw->left_event();
			}
			if(funcargs[0]=="'FRONT'")
			{
				mw->front_event();
			}
			if(funcargs[0]=="'BACK'")
			{
				mw->back_event();
			}

			mw->glwindow->glvoxel->redraw_event();
		}
	}
	else if(command.contains("set_depthpeal"))
	{
		QString argstr = removeFunctionName(command,"set_depthpeal");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 2, "set_depthpeal"))
		{
			int offset = funcargs[0].toInt();
			int thickness = funcargs[1].toInt();

			mw->glwindow->glvoxel->light_depthpeal_slot(offset, thickness);
			mw->glwindow->glvoxel->redraw_event();			
		}
	}
	else if(command.contains("set_zoom"))
	{
		QString argstr = removeFunctionName(command,"set_zoom");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "set_zoom"))
		{
			float zoomamount = funcargs[0].toFloat();

			mw->glwindow->glvoxel->volrender.volrenderview.zoom = zoomamount;
			mw->glwindow->glvoxel->redraw_event();
		}
	}
	else if(command.contains("set_current_visual_bookmark"))
	{
		QString argstr = removeFunctionName(command,"set_current_visual_bookmark");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "set_current_visual_bookmark"))
		{
			mw->renderview_editor_tabdlg->renviewtab->renderview_selectItem(funcargs[0].toInt());
			mw->glwindow->glvoxel->redraw_event();
		}
	}	
	else if(command.contains("set_background_colour"))
	{
		QString argstr = removeFunctionName(command,"set_background_colour");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 3, "set_background_colour"))
		{
			mw->setBG_event(funcargs[0].toInt(), funcargs[1].toInt(), funcargs[2].toInt());
			//mw->renderview_editor_tabdlg->renviewtab->renderview_selectItem(funcargs[0].toInt());
			mw->glwindow->glvoxel->redraw_event();
		}
	}	
	//SET_CLOSE
	else if(command.contains("set_close"))
	{
		if(image_batch)
		{
			if(current_image_index==open_images_path.size()-1) exit(1);
		}
		else
		{
			exit(1);
		}
	}
	//SET_HIDE
	else if(command.contains("set_hide()"))
	{
		filter.showGUI = false;
		mw->glwindow->glvoxel->showGUI = false;
		mw->glwindow->glvoxel->volio.showGUI = false;
#ifdef OMERO_SUPPORT
		mw->omeroLoader.showGUI = false;
#endif
		mw->setVisible(false);
	}
	//SET_SHOW
	else if(command.contains("set_show()"))
	{
		filter.showGUI = true;
		mw->glwindow->glvoxel->showGUI = true;
		mw->glwindow->glvoxel->volio.showGUI = true;
#ifdef OMERO_SUPPORT
		mw->omeroLoader.showGUI = true;
#endif
		mw->setVisible(true);
	}
	else if(command.contains("set_depthcue"))
	{
		QString argstr = removeFunctionName(command,"set_depthcue");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 4, "set_depthcue"))
		{
			Vector col;
			col.x = funcargs[0].toFloat()/255.0;
			col.y = funcargs[1].toFloat()/255.0;
			col.z = funcargs[2].toFloat()/255.0;
			float scale = 8.0*(funcargs[3].toFloat()/100.0);
			mw->glwindow->glvoxel->volrender.shaderDepthCueEvent(col, scale);
			mw->glwindow->glvoxel->redraw_event();
		}
	}
	else if(command.contains("set_gooch_lighting"))
	{
		QString argstr = removeFunctionName(command,"set_gooch_lighting");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 9, "set_gooch_lighting"))
		{
			Vector warm, cool;
			warm.x = funcargs[0].toFloat()/255.0;
			warm.y = funcargs[1].toFloat()/255.0;
			warm.z = funcargs[2].toFloat()/255.0;

			cool.x = funcargs[3].toFloat()/255.0;
			cool.y = funcargs[4].toFloat()/255.0;
			cool.z = funcargs[5].toFloat()/255.0;

			mw->glwindow->glvoxel->volrender.shaderGoochEvent(warm, cool, funcargs[6].toInt(), funcargs[7].toInt(), 100, funcargs[8].toInt());
			mw->glwindow->glvoxel->redraw_event();
		}
	}
	else if(command.contains("set_light_colour"))
	{
		QString argstr = removeFunctionName(command,"set_light_colour");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 9, "set_light_colour"))
		{
			Vector amb, diff, spec;

			amb.x = funcargs[0].toFloat()/255.0;
			amb.y = funcargs[1].toFloat()/255.0;
			amb.z = funcargs[2].toFloat()/255.0;

			diff.x = funcargs[3].toFloat()/255.0;
			diff.y = funcargs[4].toFloat()/255.0;
			diff.z = funcargs[5].toFloat()/255.0;

			spec.x = funcargs[6].toFloat()/255.0;
			spec.y = funcargs[7].toFloat()/255.0;
			spec.z = funcargs[8].toFloat()/255.0;

			mw->glwindow->glvoxel->volrender.light_colours(amb, diff, spec);
			mw->glwindow->glvoxel->redraw_event();
		}
	}
	else if(command.contains("set_hide_menubars"))
	{
		mw->ui_toggle_menubars(false);
	}
	else if(command.contains("set_hide_orthosections"))
	{
		mw->ui_toggle_orthosections(false);
	}
	else if(command.contains("set_show_menubars"))
	{
		mw->ui_toggle_menubars(true);
	}
	else if(command.contains("set_show_orthosections"))
	{
		mw->ui_toggle_orthosections(true);
	}
	else if(command.contains("set_channels"))
	{
		QString argstr = removeFunctionName(command,"set_channels");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 3, "set_channels"))
		{
			int red = funcargs[0].toInt();
			int green = funcargs[0].toInt();
			int blue = funcargs[0].toInt();

			mw->glwindow->glvoxel->volobject.is_greyscale = false;

			if(red)	mw->glwindow->glvoxel->volobject.has_red = true;
			else mw->glwindow->glvoxel->volobject.has_red = false;

			if(red)	mw->glwindow->glvoxel->volobject.has_green = true;
			else mw->glwindow->glvoxel->volobject.has_green = false;

			if(red)	mw->glwindow->glvoxel->volobject.has_blue = true;
			else mw->glwindow->glvoxel->volobject.has_blue = false;
			
			mw->update_channels_slot();
			mw->glwindow->glvoxel->redraw_event();
		}
	}
	else if(command.contains("set_clear_all_MSR"))
	{
		mw->measure_editor_tabdlg->measuring_tab->clear_all();
		mw->glwindow->glvoxel->redraw_event();
	}
}
void VolumeScript::movie_events(QString command)
{
	//MOVIE_ROCK
	if(command.contains("movie_rock"))
	{
		QString argstr = removeFunctionName(command,"movie_rock");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 3, "movie_rock"))
		{
			funcargs[1] = fixPath(funcargs[1]);
			if(createPath(funcargs[1])) mw->glwindow->glvoxel->movie_rockmovie_slot(mw, funcargs[0].toInt(), 0, funcargs[1], funcargs[2].toFloat());
		}
	}	
	//MOVIE_ROTATE
	else if(command.contains("movie_rotate"))
	{
		QString argstr = removeFunctionName(command,"movie_rotate");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 6, "movie_rotate"))
		{
			funcargs[1] = fixPath(funcargs[1]);
			if(createPath(funcargs[1])) mw->glwindow->glvoxel->movie_rotmovie_slot(mw, funcargs[0].toInt(), 0, funcargs[1], funcargs[2].toInt(), funcargs[3].toInt(), funcargs[4].toInt(), funcargs[5].toFloat());
		}
	}	
	//MOVIE_ORTHOSECTIONS
	else if(command.contains("movie_orthosections"))
	{
		QString argstr = removeFunctionName(command,"movie_orthosections");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 2, "movie_orthosections"))
		{
			funcargs[1] = fixPath(funcargs[1]);
			if(createPath(funcargs[1])) mw->glwindow->glvoxel->movie_orthosectmovie_slot(mw, funcargs[0].toInt(), 0, funcargs[1]);
		}
	}	
	//MOVIE_VISUALBOOKMARKS
	else if(command.contains("movie_visualbookmarks"))
	{
		QString argstr = removeFunctionName(command,"movie_visualbookmarks");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 2, "movie_visualbookmarks"))
		{
			funcargs[1] = fixPath(funcargs[1]);
			if(createPath(funcargs[1])) mw->glwindow->glvoxel->movie_renderviewmovie_slot(mw, funcargs[0].toInt(), 0, funcargs[1]);
		}
	}	
}
void VolumeScript::filter_start(void)
{
	printf("SCRIPT: Filter Start\n");
	uploadTexture = true;
	filter.volobj = &mw->glwindow->glvoxel->volobject;
	
#ifdef OPENCL_SUPPORT
	filter.ocl_gaussian_ptr = &mw->glwindow->glvoxel->ocl_gaussian;
	filter.ocl_dilate_ptr = &mw->glwindow->glvoxel->ocl_dilate;
	filter.ocl_erode_ptr = &mw->glwindow->glvoxel->ocl_erode;
	filter.ocl_median_ptr = &mw->glwindow->glvoxel->ocl_median;
	filter.ocl_peronamalik_ptr = &mw->glwindow->glvoxel->ocl_peronamalik;
	filter.ocl_bilateral_ptr = &mw->glwindow->glvoxel->ocl_bilateral;
#endif
}
bool VolumeScript::setupchannels(int offset)
{
	channels.clear();
	for(int i=0; i<3; i++) channels.push_back(false);
	if(offset<channels.size()) channels[offset] = true;
	else
	{
		printf("WARNING: Channel index is out of range!!\n");
		return false;
	}
	return true;
}
void VolumeScript::filter_end(void)
{
	printf("SCRIPT: Filter End\n");
	if(uploadTexture) mw->glwindow->glvoxel->volrender.load_TextureMap();

	mw->glwindow->frames_redraw_slot();
	mw->glwindow->glvoxel->redraw_event();
	mw->update_channels_slot();

	filter.volobj = NULL;

#ifdef OPENCL_SUPPORT
	filter.ocl_gaussian_ptr = NULL;
	filter.ocl_dilate_ptr = NULL;
	filter.ocl_erode_ptr = NULL;
	filter.ocl_peronamalik_ptr = NULL;
#endif

}
void VolumeScript::commpute_events(QString command)
{
	if(command.contains("compute_filter_Collapse"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Collapse");

		filter_start();
		filter.apply_collapse();
		filter_end();
	}
	else if(command.contains("compute_filter_Invert"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Invert");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "compute_filter_Invert"))
		{
			int offset = funcargs[0].toInt();

			filter_start();
			filter.apply_invert(offset);
			filter_end();
		}
	}
	else if(command.contains("compute_filter_Threshold"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Threshold");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 2, "compute_filter_Threshold"))
		{
			int offset = funcargs[0].toInt();
			int threshval = funcargs[1].toInt();

			filter_start();
			filter.apply_threshold(offset, threshval);
			filter_end();
		}
	}
	else if(command.contains("compute_filter_Binarize"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Binarize");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "compute_filter_Binarize"))
		{
			int offset = funcargs[0].toInt();

			filter_start();
			filter.apply_binarize(offset);
			filter_end();
		}
	}
	else if(command.contains("compute_filter_Clear"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Clear");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 1, "compute_filter_Clear"))
		{
			int offset = funcargs[0].toInt();

			filter_start();
			filter.apply_clear(offset);
			filter_end();
		}
	}
	else if(command.contains("compute_filter_CopyTo"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_CopyTo");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 2, "compute_filter_CopyTo"))
		{
			int srcchan = funcargs[0].toInt();
			int dstchan = funcargs[1].toInt();

			filter_start();
			filter.apply_copyto(srcchan, dstchan);
			filter_end();
		}
	}
	else if(command.contains("compute_filter_AddTo"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_AddTo");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 2, "compute_filter_AddTo"))
		{
			int srcchan = funcargs[0].toInt();
			int dstchan = funcargs[1].toInt();

			filter_start();
			filter.apply_addto(srcchan, dstchan);
			filter_end();
		}
	}			
	else if(command.contains("compute_filter_ReplaceWith"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_ReplaceWith");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 4, "compute_filter_ReplaceWith"))
		{
			int src_chan = funcargs[0].toInt();
			int dst_chan = funcargs[1].toInt();
			int test_chan = funcargs[2].toInt();
			int test_op = funcargs[3].toInt();

			filter_start();
			filter.apply_replace(test_chan, test_op, src_chan, dst_chan);
			filter_end(); 
		}
	}			
	else if(command.contains("compute_filter_SetOperator"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_SetOperator");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 3, "compute_filter_SetOperator"))
		{
			int a_chan = funcargs[0].toInt();
			int b_chan = funcargs[1].toInt();
			int operatormode = funcargs[2].toInt();

			filter_start();
			filter.apply_setoperator(a_chan, b_chan, operatormode);
			filter_end();
		}
	}		
	else if(command.contains("compute_filter_ArithmeticOperator"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_ArithmeticOperator");
		QStringList funcargs = argstr.split(",");

		if(checkFuncArgs(&funcargs, 3, "compute_filter_ArithmeticOperator"))
		{
			int a_chan = funcargs[0].toInt();
			int b_chan = funcargs[1].toInt();
			int operatormode = funcargs[2].toInt();

			filter_start();
			filter.apply_arithmoperator(a_chan, b_chan, operatormode);
			filter_end();
		}
	}		
	else if(command.contains("compute_filter_Dilate"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Dilate");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 1, "compute_filter_Dilate"))
		{
			int offset = funcargs[0].toInt();

			if(setupchannels(offset))
			{
				mw->glwindow->glvoxel->volobject.compute_boundingbox();

				filter_start();
				filter.apply_dilate_filter(1,0,channels);
				filter_end();
			}
		}	
	}
	else if(command.contains("compute_filter_Erode"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Erode");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 1, "compute_filter_Erode"))
		{
			int offset = funcargs[0].toInt();

			mw->glwindow->glvoxel->volobject.compute_boundingbox();

			if(setupchannels(offset))
			{
				mw->glwindow->glvoxel->volobject.compute_boundingbox();

				filter_start();
				filter.apply_erode_filter(1,0,channels);
				filter_end();
			}
		}	
	}
	else if(command.contains("compute_filter_Open"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Open");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 1, "compute_filter_Dilate"))
		{
			int offset = funcargs[0].toInt();

			if(setupchannels(offset))
			{
				mw->glwindow->glvoxel->volobject.compute_boundingbox();

				filter_start();
				filter.apply_erode_filter(1,0,channels);
				filter.apply_dilate_filter(1,0,channels);
				filter_end();
			}
		}	
	}
	else if(command.contains("compute_filter_Close"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Close");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 1, "compute_filter_Dilate"))
		{
			int offset = funcargs[0].toInt();

			if(setupchannels(offset))
			{
				mw->glwindow->glvoxel->volobject.compute_boundingbox();

				filter_start();
				filter.apply_dilate_filter(1,0,channels);
				filter.apply_erode_filter(1,0,channels);
				filter_end();
			}
		}	
	}
	else if(command.contains("compute_filter_Median"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Median");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 2, "compute_filter_Median"))
		{
			int offset = funcargs[0].toInt();
			int size = funcargs[1].toInt();

			if(setupchannels(offset))
			{
				mw->glwindow->glvoxel->volobject.compute_boundingbox();

				filter_start();
				filter.apply_median_filter(size, 1, channels);
				filter_end();
			}
		}	
	}			
	else if(command.contains("compute_filter_AnisotropicDiffusion"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_AnisotropicDiffusion");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 4, "compute_filter_AnisotropicDiffusion"))
		{
			int offset = funcargs[0].toInt();
			double l = funcargs[1].toDouble();
			double dt = funcargs[2].toDouble();
			int iters = funcargs[3].toInt();

			printf("SCRIPT: %d, %f, %f, %d\n", offset, l, dt, iters);

			if(setupchannels(offset))
			{
				mw->glwindow->glvoxel->volobject.compute_boundingbox();

				filter_start();
				filter.apply_peronamalik(1, l, dt, iters, channels);
				filter_end();
			}
		}	
	}	
	else if(command.contains("compute_filter_Gaussian"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Gaussian");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 3, "compute_filter_Gaussian"))
		{
			int offset = funcargs[0].toInt();
			double sigma = funcargs[1].toDouble();
			int size = funcargs[2].toInt();

			if(setupchannels(offset))
			{
				mw->glwindow->glvoxel->volobject.compute_boundingbox();

				filter_start();
				filter.gaussian_filter_decomp_init(size, sigma);
				filter.apply_gaussian_filter_decomp(size, sigma, 1, channels);
				filter.gaussian_filter_decomp_dealloc();
				filter_end();
			}
		}	
	}	
	else if(command.contains("compute_filter_Bilateral"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Bilateral");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 3, "compute_filter_Bilateral"))
		{
			int offset = funcargs[0].toInt();
			int spatial = funcargs[1].toInt();
			float range = funcargs[2].toDouble();

			if(setupchannels(offset))
			{
				mw->glwindow->glvoxel->volobject.compute_boundingbox();

				filter_start();
				filter.apply_bilateral_filter(spatial, range, channels);
				filter_end();
			}
		}	
	}	
	else if(command.contains("compute_filter_UnsharpMask"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_UnsharpMask");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 3, "compute_filter_UnsharpMask"))
		{
			int offset = funcargs[0].toInt();
			double sigma = funcargs[1].toDouble();
			int size = funcargs[2].toInt();

			if(setupchannels(offset))
			{
				mw->glwindow->glvoxel->volobject.compute_boundingbox();

				filter_start();
				filter.apply_unsharpmask_filter(size, sigma, 1, channels);	
				filter_end();
			}
		}	
	}
	else if(command.contains("compute_filter_Sobel"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Sobel");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 1, "compute_filter_Sobel"))
		{
			int offset = funcargs[0].toInt();

			if(setupchannels(offset))
			{
				mw->glwindow->glvoxel->volobject.compute_boundingbox();

				filter_start();
				filter.apply_sobel3D_filter(channels);	
				filter_end();
			}
		}	
	}
	else if(command.contains("compute_filter_Sieve"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Sieve");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 2, "compute_filter_Sieve"))
		{
			int src = funcargs[0].toInt();
			//int dst = funcargs[1].toInt();
			int size = funcargs[1].toInt();

			mw->glwindow->glvoxel->volobject.compute_boundingbox();

			filter_start();
			filter.apply_sieve(src, src, size);
			filter_end();
		}	
	}			
	else if(command.contains("compute_filter_FloodFill"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_FloodFill");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 6, "compute_filter_FloodFill"))
		{
			int src = funcargs[0].toInt();
			int dst = funcargs[1].toInt();
			int size = funcargs[2].toInt();
			int x = funcargs[3].toInt();
			int y = funcargs[4].toInt();
			int z = funcargs[5].toInt();

			mw->glwindow->glvoxel->volobject.compute_boundingbox();

			filter_start();
			filter.apply_floodfill(src, dst, size, x,y,z);
			filter_end();
		}	
	}		
	else if(command.contains("compute_filter_SmoothedDilate"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_SmoothedDilate");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 7, "compute_filter_SmoothedDilate"))
		{
			int srcchan = funcargs[0].toInt();
			int dstchan = funcargs[1].toInt();
			int seed_window = funcargs[2].toInt();
			int minsize = funcargs[3].toInt();
			int gaussian_width = funcargs[4].toInt();
			float gaussian_strength = funcargs[5].toFloat();
			int gaussian_window = funcargs[6].toInt();

			mw->glwindow->glvoxel->filter_smthdilate_slot(srcchan, dstchan, seed_window, minsize, gaussian_width, gaussian_strength, gaussian_window,1,0);
		}	
	}		
	else if(command.contains("compute_filter_MaxFlow"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_MaxFlow");
		QStringList funcargs = argstr.split(",");

		//TO DO
	}
#ifdef OPENCL_SUPPORT
	else if(command.contains("compute_GPUfilter_SmoothedDilate"))
	{
		QString argstr = removeFunctionName(command,"compute_GPUfilter_SmoothedDilate");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 7, "compute_GPUfilter_SmoothedDilate"))
		{
			int srcchan = funcargs[0].toInt();
			int dstchan = funcargs[1].toInt();
			int seed_window = funcargs[2].toInt();
			int minsize = funcargs[3].toInt();
			int gaussian_width = funcargs[4].toInt();
			float gaussian_strength = funcargs[5].toFloat();
			int gaussian_window = funcargs[6].toInt();

			mw->glwindow->glvoxel->filter_gpu_smthdilate_slot(srcchan, dstchan, seed_window, minsize, gaussian_width, gaussian_strength, gaussian_window,1,0);
		}	
	}
	else if(command.contains("compute_GPUfilter_Median"))
	{
		QString argstr = removeFunctionName(command,"compute_GPUfilter_Median");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 2, "compute_GPUfilter_Median"))
		{
			int offset = funcargs[0].toInt();
			int size = funcargs[1].toInt();
			
			if(size>1) printf("WARNING: Only size = 1 is currently supported on GPU\n");

			if(setupchannels(offset))
			{
				filter_start();
				filter.apply_median_filter_gpu(1, 1, channels);
				filter_end();
			}
		}	
	}			
	else if(command.contains("compute_GPUfilter_AnisotropicDiffusion"))
	{
		QString argstr = removeFunctionName(command,"compute_GPUfilter_AnisotropicDiffusion");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 4, "compute_GPUfilter_AnisotropicDiffusion"))
		{
			int offset = funcargs[0].toInt();
			double l = funcargs[1].toDouble();
			double dt = funcargs[2].toDouble();
			int iters = funcargs[3].toInt();

			if(setupchannels(offset))
			{
				filter_start();
				filter.apply_peronamalik_gpu(1, l, dt, iters, channels);
				filter_end();
			}
		}	
	}	
	else if(command.contains("compute_GPUfilter_Bilateral"))
	{
		QString argstr = removeFunctionName(command,"compute_GPUfilter_Bilateral");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 3, "compute_GPUfilter_Bilateral"))
		{
			int offset = funcargs[0].toInt();
			int spatial = funcargs[1].toInt();
			float range = funcargs[2].toDouble();

			if(setupchannels(offset))
			{
				mw->glwindow->glvoxel->volobject.compute_boundingbox();

				filter_start();
				filter.apply_bilateral_filter_gpu(spatial, range, channels);
				filter_end();
			}
		}	
	}
	else if(command.contains("compute_GPUfilter_Gaussian"))
	{
		QString argstr = removeFunctionName(command,"compute_GPUfilter_Gaussian");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 3, "compute_GPUfilter_Gaussian"))
		{
			int offset = funcargs[0].toInt();
			int size = funcargs[1].toInt();
			double sigma = funcargs[2].toDouble();

			if(setupchannels(offset))
			{
				filter_start();
				filter.apply_gaussian_filter_gpu(size, sigma, 1, channels);
				filter_end();
			}
		}	
	}	
	else if(command.contains("compute_filter_Dilate"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Dilate");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 1, "compute_filter_Dilate"))
		{
			int offset = funcargs[0].toInt();

			if(setupchannels(offset))
			{
				filter_start();
				filter.apply_dilate_filter_gpu(1, 1, channels);
				filter_end();
			}
		}	
	}
	else if(command.contains("compute_filter_Erode"))
	{
		QString argstr = removeFunctionName(command,"compute_filter_Erode");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 1, "compute_filter_Erode"))
		{
			int offset = funcargs[0].toInt();

			if(setupchannels(offset))
			{
				filter_start();
				filter.apply_erode_filter_gpu(1, 1, channels);
				filter_end();
			}
		}	
	}
#endif
	else if(command.contains("compute_isosurface"))
	{
		QString argstr = removeFunctionName(command,"compute_isosurface");
		QStringList funcargs = argstr.split(",");
		
		if(checkFuncArgs(&funcargs, 1, "compute_isosurface"))
		{
			int offset = funcargs[0].toInt();
			int qual = funcargs[1].toInt();
			int value = funcargs[2].toInt();

			if(setupchannels(offset))
			{
				bool chans[3];
				for(int i=0; i<3; i++) chans[i] = channels[i];

				mw->glwindow->glvoxel->isosurface_new_slot("iso-surface", qual, value, 0, Vector(1,1,1), chans, 1);
			}
		}
	}
}
