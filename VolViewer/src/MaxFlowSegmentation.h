#ifndef MAXFLOWSEGM_H
#define MAXFLOWSEGM_H

#include <math.h>
#include <vector>
#include <stdio.h>

using namespace std;

#include "Sobel.h"
#include "maxflow_graph.h"

//template class Graph<float,float,float>;
typedef Graph<float, float, float> GraphType;

#define PI 			3.141592653589793238462643383279f
#define TWOPI 		6.283185307179586476925286766558f
#define RAD_TO_DEG (180.0f / PI)
#define DEG_TO_RAD (PI / 180.0f)

class MaxFlowSegmentation
{
	public:

		int bb_sizeX;
		int bb_sizeY;
		int bb_sizeZ;

		int bb_minX, bb_maxX;
		int bb_minY, bb_maxY;
		int bb_minZ, bb_maxZ;

		bool threeD;

		bool firstrun;
	
		double maxprob;
		double maxhist;

		double maxgrad;
		vector< double > gradientvalues;

		Sobel sobel;

		//bias background vs foreground
		double w_weight;
		//edges vs intensity
		double v_weight;
		//exponent
		double k_weight;

		int image_width;
		int image_height;
		int image_depth;
		unsigned char* image_data;

		double normal_stdv_histogram_bg;
		double normal_mean_histogram_bg;
		double stdv_histogram_bg;
		double mean_histogram_bg;
		vector< double > histogram_bg;
		vector< int > histogramindices_bg;

		double normal_stdv_histogram_fg;
		double normal_mean_histogram_fg;
		double stdv_histogram_fg;
		double mean_histogram_fg;
		vector< double > histogram_fg;
		vector< int > histogramindices_fg;
		
		int numnodes;
		GraphType* g;

		bool edgesbuilt;

		vector< double > delta;
	
		MaxFlowSegmentation();
		~MaxFlowSegmentation();

		void init_data2d(int w, int h);
		void init_data3d(int w, int h, int d, unsigned char* data);

		void compute2d(void);
		void build_edges2d(void);
		double compute_gradient2d(int i, int j);

		void compute3d(int src);
		void build_edges3d(int src);
		double compute_gradient3d(int i, int j, int k);

		void init_graph(void);
		void del_graph(void);
		void build_statistics(void);
		double compute_prob(double mean, double stdv, double sample, double norm_stdv, double norm_sample);

		void clear_histograms(void);

		double get_totalprob(int index0);
		double get_gradient(int index0);
		double get_histoprob(int index0);

		double get_dataValue(int index);
		void set_dataValue(int index, double value);

		//void add_terminal(int node_mode, int index, double weight);
		void build_terminals(void);

		void add_background(int index, double value);
		void add_foreground(int index, double value);
		bool belongsToSource(int index);
};

#endif //MAXFLOWSEGM_H
