/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef EDGE_H
#define EDGE_H

class Edge
{
public:
	int v0;
	int v1;

	Edge()
	{
		v0=0;
		v1=0;
	}
	Edge(int v0_, int v1_)
	{
		v0 = v0_;
		v1 = v1_;
	}

	Edge& operator=(const Edge& rhs)
	{
		v0 = rhs.v0;
		v1 = rhs.v1;

		return *this;
	}
};

#endif //EDGE_H
