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

void execPipeline(vector<Process> &pipeline)
{
	srand(1010101);
	
	printf("init marching cubes table\n");
	initMCTable();

	// open the stream
	file.open(g.pointsfile.c_str(), ios::binary);
	if (file.fail())
		return;
	file2.open(g.pointsfile.c_str(), ios::binary);
	if (file2.fail())
		return;

	readBBox(g.tree.mine, g.tree.maxe, file);
	readBBox(g.tree.mine, g.tree.maxe, file2);

	ext = g.tree.maxe - g.tree.mine; 
	scale_factor = 1.0 / max(max(ext[0], ext[1]), ext[2]);
	center = (g.tree.mine + g.tree.maxe) * 0.5;
	if (g.wavelet == daub4)
	{
		g.tree.maxe(3,3,3);
		g.tree.mine(-1,-1,-1);
	}
	else if (g.wavelet == haar)
	{
		g.tree.maxe(1,1,1);
		g.tree.mine(0,0,0);
	}
	ext = g.tree.maxe - g.tree.mine;

	readInline(file, pts_num);
	readInline(file2, pts_num);

	// run the pipeline
	while (!pipeline[pipeline.size()-1].finished)
	{
		for (int i = 0; i < pipeline.size(); i++)
		{
			Process &proc = pipeline[i];

			if (!proc.finished)
			{
				// wait ahead
				if (i == 0 || pipeline[i-1].completed > proc.completed+streaminfo.stride || pipeline[i-1].finished)
				{
					proc.func(proc.completed, proc.completed+streaminfo.stride);

					// increase the advancement counter
					proc.completed += streaminfo.stride;
					if (proc.completed >= streaminfo.cells_total)
						proc.finished = true;
				}
			}
		}
	}

	// close the stream
	file.close();
	file2.close();
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
