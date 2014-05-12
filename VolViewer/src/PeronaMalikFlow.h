#ifndef PERONAMALIK_H
#define PERONAMALIK_H

#include <QProgressDialog>

#include <math.h>
#include <vector>
using namespace std;

class PeronaMalikFlow
{
	public:

		double dx, dy, dz, dd, dh, dc;

		float lambda;
		float lambda2;

		int offset;

		unsigned char* data;
		int width;
		int height;
		int depth;

		PeronaMalikFlow();
		~PeronaMalikFlow();

		void init_data2d(int w, int h);
		void set_dataValue(int index, float val);

		void init_data3d(int w, int h, int d, unsigned char* imgdata);

		float computeDiffusion(float v);

		float compute2d_4connect(int x, int y);
		float compute2d_8connect(int x, int y);
		
		float compute3d_26connect(int x, int y, int z);
		float compute3d_6connect(int x, int y, int z);

		void peronaMalikOperator2d(float dt, int nbiter);
		void peronaMalikOperator3d(float dt, int nbiter, int chan, int dim);
};

#endif //PERONAMALIK_H
