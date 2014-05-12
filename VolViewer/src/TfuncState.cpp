#include "TfuncState.h"

TfuncState::TfuncState()
{
	brightness = 100;
	contrast = 100;
	threshold_min = 0;
	threshold_max = 255;
}
TfuncState::~TfuncState()
{
	clear();
}
void TfuncState::clear(void)
{
	functioneditor.clear();
}
void TfuncState::init(float s, int b, int o, int bright, int cont, int min, int max)
{
	functioneditor_size = s;
	functioneditor_border = b;
	functioneditor_offset = o;
	functioneditor.init(functioneditor_size, functioneditor_border, functioneditor_offset);

	brightness = bright;
	contrast = cont;
	threshold_min = min;
	threshold_max = max;
}
void TfuncState::reset(void)
{
	brightness = 100;
	contrast = 100;
	threshold_min = 0;
	threshold_max = 255;

	functioneditor.reset();
}
void TfuncState::save(ofstream& fout)
{
	fout.precision(10);
	fout.setf(ios::fixed); 

	fout<<brightness<<" "<<contrast<<" "<<threshold_min<<" "<<threshold_max<<" ";
	fout<<functioneditor.bs.control_points.size()<<" ";
	for(int i=0; i<functioneditor.bs.control_points.size(); i++)
	{
		fout<<functioneditor.bs.control_points[i].x<<" "<<functioneditor.bs.control_points[i].y<<" "<<functioneditor.bs.control_points[i].z<<" ";
	}
	fout<<endl;
}
const TfuncState& TfuncState::operator= (const TfuncState& right)
{
	brightness = right.brightness;
	contrast = right.contrast;
	threshold_min = right.threshold_min;
	threshold_max = right.threshold_max;

	functioneditor_size = right.functioneditor_size;
	functioneditor_border = right.functioneditor_border;
	functioneditor_offset = right.functioneditor_offset;

	functioneditor = right.functioneditor;
	return *this;
}
