// Author: Josiah Manson

#include "global.h"
#include "matrix.h"
#include "parse.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <float.h>
//#include <GL/glut.h>

using namespace std;

extern double average_val;


volatile __int32 pts_num;
vect3f ext; 
float scale_factor;
vect3f center;

ifstream file;
ifstream file2;

void count_points_second(int from, int to)
{
	static int pts_index = 0;
	OrientedPoint pt_read;

	// report progress
	printf("read slab %d/%d\n", from, streaminfo.cells_total);

	// count the points
	srand(1010101);
	while (readPointUpTo(pt_read, file, center, scale_factor, g.tree, pts_index, pts_num, 0, to))
	{
		g.tree.countPoints(pt_read.pos);
		pts_index++;
	}
}

void count_points_first(int from, int to)
{
	static int pts_index = 0;
	OrientedPoint pt_read;

	// report progress
	printf("read slab %d/%d\n", from, streaminfo.cells_total);

	// count the points
	srand(1010101);
	while (readPointUpTo(pt_read, file, center, scale_factor, g.tree, pts_index, pts_num, 0, to))
	{
		g.tree.countPoints(pt_read.pos);
		pts_index++;
	}
}

void calc_coeffs_daub4_second(int from, int to)
{
	static int pts_index2 = 0;
	OrientedPoint pt_read;

	connect_neighbors_second(from, to);

	// calc coeffs
	srand(1010101);
	while (readPointUpTo(pt_read, file2, center, scale_factor, g.tree, pts_index2, pts_num, 1, to))
	{
		pt_read.ds = 0;
		g.tree.coeffs_daub4(pt_read, g.maxDepthMem);
		pts_index2++;
	}
}

void calc_coeffs_daub4_first(int from, int to)
{
	static int pts_index2 = 0;
	OrientedPoint pt_read;

	connect_neighbors_first(from, to);

	// calc coeffs
	srand(1010101);
	while (readPointUpTo(pt_read, file2, center, scale_factor, g.tree, pts_index2, pts_num, 1, to))
	{
		pt_read.ds = 0;
		g.tree.coeffs_daub4(pt_read, 1, g.maxDepthMem);
		pts_index2++;
	}

	// clean up the fine resolution
	g.tree.removeHighRes(to);
}


void calc_coeffs_haar_second(int from, int to)
{
	static int pts_index2 = 0;
	OrientedPoint pt_read;

	// calc coeffs
	srand(1010101);
	while (readPointUpTo(pt_read, file2, center, scale_factor, g.tree, pts_index2, pts_num, 1, to))
	{
		pt_read.ds = 0;
		g.tree.coeffs_haar(pt_read, g.maxDepthMem);
		pts_index2++;
	}
}

void calc_coeffs_haar_first(int from, int to)
{
	static int pts_index2 = 0;
	OrientedPoint pt_read;

	// calc coeffs
	srand(1010101);
	while (readPointUpTo(pt_read, file2, center, scale_factor, g.tree, pts_index2, pts_num, 1, to))
	{
		pt_read.ds = 0;
		g.tree.coeffs_haar(pt_read, -1, g.maxDepthMem);
		pts_index2++;
	}

	// clean up the fine resolution
	g.tree.removeHighRes(to);
}

void count_and_coeffs_haar_first(int from, int to)
{
	count_points_first(from, to);
	calc_coeffs_haar_first(from, to);
}

void count_and_coeffs_haar_second(int from, int to)
{
	count_points_second(from, to);
	calc_coeffs_haar_second(from, to);
}

void fill_in_mem(OctNode n, int d = 0)
{
	if (d >= g.maxDepthMem)
		return;

	for (int i = 0; i < 8; i++)
	{
		if (n.children_linear(i).data == 0)
			n.children_linear(i).allocate();
			
		fill_in_mem(n.children_linear(i), d+1);
	}
}

void average_nonstreaming(int start, int end)
{
	average_val = calc_average();
}
