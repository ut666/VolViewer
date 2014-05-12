#include "maxflow_graph.h"

#ifndef GRAPHTEMPLATES_INC
#define GRAPHTEMPLATES_INC

#ifdef _MSC_VER
#pragma warning(disable: 4661)
#endif

// Instantiations: <captype, tcaptype, flowtype>
// IMPORTANT: 
//    flowtype should be 'larger' than tcaptype 
//    tcaptype should be 'larger' than captype

template class Graph<int,int,int>;
template class Graph<float,float,float>;
template class Graph<double,double,double>;

#endif
