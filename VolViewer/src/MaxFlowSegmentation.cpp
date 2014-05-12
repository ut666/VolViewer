#include "MaxFlowSegmentation.h"

MaxFlowSegmentation::MaxFlowSegmentation()
{
	image_width=0;
	image_height=0;
	image_depth=0;
	image_data=0;
	numnodes=0;
	g=0;
	edgesbuilt=false;

	w_weight = 1.0;
	v_weight = 0.45;
	k_weight = 1000.0;
}
MaxFlowSegmentation::~MaxFlowSegmentation()
{

}
void MaxFlowSegmentation::init_data2d(int w, int h)
{
	threeD=false;
	image_width = w;
	image_height = h;

	if(image_data) delete[] image_data ;
	image_data = new unsigned char[image_width*image_height];

	numnodes = (image_width*image_height);
}
void MaxFlowSegmentation::init_data3d(int _w, int _h, int _d, unsigned char* data)
{
	threeD=true;
	image_data = data;
	image_width = _w;
	image_height = _h;
	image_depth = _d;
	numnodes = (image_width*image_height*image_depth);
}
void error_func(char* msg)
{
	printf("error: %s\n", msg);
	system("pause");
}
void MaxFlowSegmentation::init_graph(void)
{
	if(g) delete g;
	g = new GraphType(numnodes,  numnodes*2, &error_func); //nodes, edges... estimated
	for(int i=0; i<numnodes; i++) g->add_node(); 
}
void MaxFlowSegmentation::del_graph(void)
{
	if(g) delete g;
	g=0;
}
void MaxFlowSegmentation::clear_histograms(void)
{
	histogram_bg.clear();
	histogram_fg.clear();

	histogramindices_bg.clear();
	histogramindices_fg.clear();
}
double MaxFlowSegmentation::compute_gradient3d(int x, int y, int z)
{
	if(x==0 || y==0 || z==0 || x==image_width-1 || y==image_height-1 || z==image_depth-1) return 0.00001;

	double filtered_valuex = 0.0;
	double filtered_valuey = 0.0;
	double filtered_valuez = 0.0;

	int xval, yval, zval;
	int index;

	int x_iter, y_iter, z_iter;
	x_iter = y_iter = z_iter = 0;

	int offset = 0;

	for(int i=-1; i<=1; i++)
	{
		for(int j=-1; j<=1; j++)
		{
			for(int k=-1; k<=1; k++)
			{
				zval = z+i;
				yval = y+j;
				xval = x+k;

				if(xval>image_width-1) xval = image_width-1;
				if(xval<0) xval=0;
				if(yval>image_height-1) yval = image_height-1;
				if(yval<0) yval=0;
				if(zval>image_depth-1) zval = image_depth-1;
				if(zval<0) zval=0;
			
				index = 3*((((zval*image_height)+(yval))*image_width)+(xval));	

				filtered_valuex += (double)(image_data[index+offset])*sobel.sobelX3d[x_iter][y_iter][z_iter];
				filtered_valuey += (double)(image_data[index+offset])*sobel.sobelY3d[x_iter][y_iter][z_iter];
				filtered_valuez += (double)(image_data[index+offset])*sobel.sobelZ3d[x_iter][y_iter][z_iter];

				z_iter++;
			}
			
			y_iter++;
			z_iter=0;
		}

		x_iter++;
		y_iter=0;
	}

	double filtered_value =sqrt((filtered_valuex*filtered_valuex)+(filtered_valuey*filtered_valuey)+(filtered_valuez*filtered_valuez));
	return filtered_value;
}

double MaxFlowSegmentation::compute_gradient2d(int x, int y)
{
	if(x==0 || y==0 || x==image_width-1 || y==image_height-1) return 0.00001;

	double filtered_valuex = 0.0;
	double filtered_valuey = 0.0;

	int xval, yval;
	int index;

	int x_iter, y_iter;
	x_iter = y_iter = 0;

	for(int j=-1; j<=1; j++)
	{
		for(int k=-1; k<=1; k++)
		{
			yval = y+j;
			xval = x+k;

			if(xval>image_width-1) xval = image_width-1;
			if(xval<0) xval=0;
			if(yval>image_height-1) yval = image_height-1;
			if(yval<0) yval=0;

			index = (xval)*image_height+(yval);		

			filtered_valuex += (double)(image_data[index])*sobel.sobelX2d[x_iter][y_iter];
			filtered_valuey += (double)(image_data[index])*sobel.sobelY2d[x_iter][y_iter];
			
			x_iter++;
		}
		
		y_iter++;
		x_iter=0;
	}

	double filtered_value =sqrt((filtered_valuex*filtered_valuex)+(filtered_valuey*filtered_valuey));
	return filtered_value;
}
double MaxFlowSegmentation::get_gradient(int index0)
{
	double gradient = (gradientvalues[index0] / maxgrad);
	return gradient;
}
double MaxFlowSegmentation::get_histoprob(int index0)
{
	int pix = 0;
	if(threeD==true)	pix = image_data[3*index0+0];
	else pix = image_data[index0];

	double probA = compute_prob(mean_histogram_bg, stdv_histogram_bg, normal_mean_histogram_bg, normal_stdv_histogram_bg, pix);
	double probB = compute_prob(mean_histogram_fg, stdv_histogram_fg, normal_mean_histogram_fg, normal_stdv_histogram_fg, pix);

	if(probB==0.0) probB = 0.000000001;

	//double log_like = -2*(log(w_weight*probA)/log(probB));
	double log_like = (w_weight*probA)/probB;

	return log_like;
}
double MaxFlowSegmentation::get_totalprob(int index0)
{
	double gradient = get_gradient(index0);
	double histo = get_histoprob(index0);

	//double cost = k_weight*exp( -(v_weight*log_like+(1-v_weight)*gradient));
	double cost = pow(k_weight, -(v_weight*histo+(1-v_weight)*gradient) );
	return cost;
}
void MaxFlowSegmentation::build_edges2d(void)
{
	//build graph
	printf("Building Graph\n");

	int index0 = 0;
	int index1 = 0;

	//max gradient
	gradientvalues.clear();
	gradientvalues.resize(image_width*image_height);
		
	printf("Building Gradient data...\n");
	int index;
	maxhist = maxprob = maxgrad = -999999999;
	for(int i=1; i<image_width-1; i++)
	{
		for(int j=1; j<image_height-1; j++)
		{
			index = i*image_height+j;
			
			double gradient = compute_gradient2d(i,j);
			maxgrad = max(maxgrad, gradient);
			gradientvalues[index] = gradient;

			double hist = get_histoprob(index);
			maxhist = max(maxhist, hist);

			double prob = get_totalprob(index);
			maxprob = max(maxprob, prob);
		}
	}

	printf("Building Edge Capacities...\n");
	for(int i=bb_minX; i<bb_maxX; i++)
	{
		for(int j=bb_minY; j<bb_maxY; j++)
		{
			//printf("ADDING EDGES\n");

			index0 = i*image_height+j;		
			int pix = image_data[index0];

			double cost = get_totalprob(index0);

			if(j<image_height-1)
			{
				index1 = i*image_height+(j+1);	
				g->add_edge(index0, index1, cost, cost);
			}
			if(i<image_width-1)
			{
				index1 = (i+1)*image_height+j;
				g->add_edge(index0, index1, cost, cost);
			}
		}
	}
}
void MaxFlowSegmentation::build_edges3d(int src)
{
	//build graph
	printf("Building Graph\n");

	int index0 = 0;
	int index1 = 0;

	//max gradient
	gradientvalues.clear();
	gradientvalues.resize(image_width*image_height*image_depth);	
	for(int i=0; i<gradientvalues.size(); i++) gradientvalues[i]=0.0;

	printf("Building Gradient data...\n");
	int index;
	maxgrad = -999999999;
	for(int kk=1; kk<image_depth-1; kk++)
	{
		for(int j=1; j<image_height-1; j++)
		{
			for(int i=1; i<image_width-1; i++)
			{
				index = ((((kk*image_height)+j)*image_width)+i);	

				double gradient = compute_gradient3d(i,j, kk);
				maxgrad = max(maxgrad, gradient);

				gradientvalues[index] = gradient;

				double hist = get_histoprob(index);
				maxhist = max(maxhist, hist);

				double prob = get_totalprob(index);
				maxprob = max(maxprob, prob);
			}
		}
	}

	printf("Building Edge Capacities...\n");
	for(int kk=bb_minZ; kk<bb_maxZ; kk++)
	{
		for(int j=bb_minY; j<bb_maxY; j++)
		{
			for(int i=bb_minX; i<bb_maxX; i++)
			{
				index0 = ((((kk*image_height)+j)*image_width)+i);	
				int pix = image_data[3*index0+src];

				double cost = get_totalprob(index0);

				//image_data[3*index0+src] = get_gradient(index0)*255.0;
				//image_data[3*index0+src] = (get_histoprob(index0)/maxhist)*255.0;

				if(kk<image_depth-1)
				{
					index1 = (((((kk+1)*image_height)+j)*image_width)+i);	

					//add edge kk -> kk+1
					g->add_edge(index0, index1, cost, cost);
				}
				if(j<image_height-1)
				{
					index1 = ((((kk*image_height)+(j+1))*image_width)+i);	
					
					//add edge j -> j+1
					g->add_edge(index0, index1, cost, cost);
				}
				if(i<image_width-1)
				{
					index1 = (((((kk)*image_height)+j)*image_width)+(i+1));	
					//add edge i -> i+1
					g->add_edge(index0, index1, cost, cost);
				}
			}
		}
	}
}
double MaxFlowSegmentation::compute_prob(double mean, double stdv, double norm_mean, double norm_stdv, double sample)
{
	double prob = 0.0;
	double norm_sample = (sample-mean)/stdv;
	prob = ( 1.0 / sqrt(2*PI*norm_stdv*norm_stdv) ) * exp( -((norm_sample-norm_mean)*(norm_sample-norm_mean)) / (2*norm_stdv*norm_stdv) );

	//prob = ((-mean+sample)*(-mean+sample))/ (2*stdv*stdv);

	return prob;
}
void MaxFlowSegmentation::compute3d(int src)
{
	if(edgesbuilt==false)
	{
		init_graph();
		build_terminals();
		build_statistics();
		build_edges3d(src);
		//edgesbuilt = true;
	}

	double flow = g->maxflow();
//	printf("Flow = %f\n", flow);

	//printf("Minimum cut:\n");
	//delete[] image_data;
	//delete g;
}

void MaxFlowSegmentation::compute2d(void)
{
	if(edgesbuilt==false)
	{
		init_graph();
		build_statistics();
		build_terminals();
		build_edges2d();
		//edgesbuilt = true;
	}

	double flow = g->maxflow();
	//printf("Flow = %f\n", flow);

	//printf("Minimum cut:\n");
	//delete[] image_data;
	//delete g;
}
void MaxFlowSegmentation::build_statistics(void)
{
	double sum_values;
	double varSum;
	double x;
	double variance;

	printf("histogram_bg.size(): %d\n", histogram_bg.size());
	printf("histogram_fg.size(): %d\n", histogram_fg.size());

	//------------------------
	//Evaluate the means...
	//------------------------

	//sum the values up
	sum_values = 0;
	for(unsigned int i=0; i<histogram_bg.size(); i++)
	{
		sum_values +=  histogram_bg[i];
	}
	mean_histogram_bg = sum_values / (double)(histogram_bg.size());
	if(mean_histogram_bg==0) mean_histogram_bg=1;

	//sum the values up
	sum_values = 0;
	for(unsigned int i=0; i<histogram_fg.size(); i++)
	{
		sum_values +=  histogram_fg[i];
	}
	mean_histogram_fg = sum_values / (double)(histogram_fg.size());
	if(mean_histogram_fg==0) mean_histogram_fg=1;

	//------------------------
	//Evaluate the stdv...
	//------------------------

	// calculate the standard deviation
	varSum = 0;
	for (unsigned int i=0; i<histogram_bg.size(); i++) 
	{
		x = histogram_bg[i] - mean_histogram_bg;
		varSum += (x * x);
	}
	variance = varSum / (double)(histogram_bg.size()-1);
	stdv_histogram_bg = sqrt(variance);
	if(stdv_histogram_bg==0.0) stdv_histogram_bg=2.0;

	// calculate the standard deviation sum
	varSum = 0;
	for (unsigned int i=0; i<histogram_fg.size(); i++) 
	{
		x = histogram_fg[i] - mean_histogram_fg;
		varSum += (x * x);
	}
	variance = varSum / (double)(histogram_fg.size()-1);
	stdv_histogram_fg = sqrt(variance);
	if(stdv_histogram_fg==0.0) stdv_histogram_fg=2.0;

	printf("mean: %f, stdv: %f\n",mean_histogram_fg, stdv_histogram_fg);
	printf("mean: %f, stdv: %f\n",mean_histogram_bg, stdv_histogram_bg);

	//------------------------------
	//Evaluate the normed means...
	//------------------------------
	//sum the values up
	sum_values = 0;
	for(unsigned int i=0; i<histogram_bg.size(); i++)
	{
		sum_values +=  (histogram_bg[i]-mean_histogram_bg)/stdv_histogram_bg;
	}
	normal_mean_histogram_bg = sum_values / (double)(histogram_bg.size());

	//sum the values up
	sum_values = 0;
	for(unsigned int i=0; i<histogram_fg.size(); i++)
	{
		sum_values +=  (histogram_fg[i]-mean_histogram_fg)/stdv_histogram_fg;
	}
	normal_mean_histogram_fg = sum_values / (double)(histogram_fg.size());

	//------------------------
	//Evaluate the normed stdv...
	//------------------------

	// calculate the standard deviation
	varSum = 0;
	for (unsigned int i=0; i<histogram_bg.size(); i++) 
	{
		x = ((histogram_bg[i]-mean_histogram_bg)/stdv_histogram_bg) - normal_mean_histogram_bg;
		varSum += (x * x);
	}
	variance = varSum / (double)(histogram_bg.size()-1);
	normal_stdv_histogram_bg = sqrt(variance);
	if(normal_stdv_histogram_bg==0.0) normal_stdv_histogram_bg=0.01;

	// calculate the standard deviation sum
	varSum = 0;
	for (unsigned int i=0; i<histogram_fg.size(); i++) 
	{
		x = (histogram_fg[i]-mean_histogram_fg)/stdv_histogram_fg - normal_mean_histogram_fg;
		varSum += (x * x);
	}
	variance = varSum / (double)(histogram_fg.size()-1);
	normal_stdv_histogram_fg = sqrt(variance);
	if(normal_stdv_histogram_fg==0.0) normal_stdv_histogram_fg=0.01;

	/*normal_mean_histogram_fg = sqrt(normal_mean_histogram_fg*normal_mean_histogram_fg);
	normal_stdv_histogram_fg = sqrt(normal_stdv_histogram_fg*normal_stdv_histogram_fg);

	normal_mean_histogram_bg = sqrt(normal_mean_histogram_bg*normal_mean_histogram_bg);
	normal_stdv_histogram_bg = sqrt(normal_stdv_histogram_bg*normal_stdv_histogram_bg);*/

	printf("normal_mean: %e, normal_stdv: %e\n",normal_mean_histogram_fg, normal_stdv_histogram_fg);
	printf("normal_mean: %e, normal_stdv: %e\n",normal_mean_histogram_bg, normal_stdv_histogram_bg);
}
void MaxFlowSegmentation::set_dataValue(int index, double value)
{
	image_data[index] = (unsigned char) value;
}
double MaxFlowSegmentation::get_dataValue(int index)
{
	double value = image_data[index];
	return value;
}
/*void MaxFlowSegmentation::add_terminal(int node_mode, int index, double weight)
{
	if(node_mode==1) g->add_tweights(index, weight, 0.0);
	else if(node_mode==2) g->add_tweights(index, 0.0, weight);
}*/
void MaxFlowSegmentation::build_terminals(void)
{
	for(int i=0; i<histogramindices_bg.size(); i++)
	{
		g->add_tweights(histogramindices_bg[i], 1000.0, 0.0);
	}
	for(int i=0; i<histogramindices_fg.size(); i++)
	{
		g->add_tweights(histogramindices_fg[i], 0.0, 1000.0);
	}
}
void MaxFlowSegmentation::add_background(int index, double value)
{
	//printf("bg_value: %f\n", value);
	histogram_bg.push_back(value);
	histogramindices_bg.push_back(index);
}
void MaxFlowSegmentation::add_foreground(int index, double value)
{
	//printf("fg_value: %f\n", value);
	histogram_fg.push_back(value);
	histogramindices_fg.push_back(index);
}
bool MaxFlowSegmentation::belongsToSource(int index)
{
	if(g->what_segment(index) == GraphType::SOURCE) return true;
	else return false;
}
