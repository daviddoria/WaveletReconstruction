#include "global.h"
#include "parse.h"

void squareExtents(vect3f &mine, vect3f &maxe)
{
	// increase size a little bit
	float border = .1f;
	vect3f dif = maxe - mine;
	maxe += dif*border;
	mine -= dif*border;

	// make square
	dif = maxe - mine;
	float mdif = max(dif[0], max(dif[1], dif[2]));

	for (int i = 0; i < 3; i++)
	{
		if (dif[i] < mdif)
		{
			float fix = (mdif - dif[i])*0.5f;
			maxe[i] += fix;
			mine[i] -= fix;
		}
	}
}

void readBBox(vect3f &mine, vect3f &maxe, ifstream &file)
{
	// read
	vect3f n, p; // must be floats
	readInline(file, n);
	readInline(file, p);

	mine(n[0], n[1], n[2]);
	maxe(p[0], p[1], p[2]);

	squareExtents(mine, maxe);
}

OrientedPoint pt_buffer[2];
bool read_from_buffer[2] = {false, false};

void readPoint(OrientedPoint &pt_read, ifstream &file, const vect3f& center, const float scale_factor, int which_stream)
{
	if (read_from_buffer[which_stream])
	{
		pt_read = pt_buffer[which_stream];
		read_from_buffer[which_stream] = false;
	}
	else
	{
		// read
		vect3f n, p; // must be floats
		readInline(file, n);
		readInline(file, p);
		
		pt_read.norm(n[0], n[1], n[2]);
		pt_read.pos(p[0], p[1], p[2]);

		// fit points in [0, 1] box
		pt_read.pos -= center;
		pt_read.pos *= scale_factor;
		pt_read.pos += vect3f(.5,.5,.5);

		assert(0 <= pt_read.pos[0] && pt_read.pos[0] <= 1);
		assert(0 <= pt_read.pos[1] && pt_read.pos[1] <= 1);
		assert(0 <= pt_read.pos[2] && pt_read.pos[2] <= 1);
	
		pt_buffer[which_stream] = pt_read;
	}
}


bool readPointUpTo(OrientedPoint &pt_read, ifstream &file, const vect3f& center, const float scale_factor, OctTree &tree, int &pts_index, int pts_num, int which_stream, int upto)
{
	if (pts_index < pts_num)
	{
		readPoint(pt_read, file, center, scale_factor, which_stream);

		// if we go past the slice boundary, put the point back and return false
		if ((pt_read.pos[2] - tree.mine[2])*streaminfo.cells_total / (tree.maxe[2] - tree.mine[2]) >= upto)
		{
			read_from_buffer[which_stream] = true;
			return false; // we went past the slice boundary
		}
		else
			return true; // everything worked
	}
	else
		return false; // we are past the end of the file
}


void readPointsForDisplay(string filename)
{
	srand(1010101);

	// open the stream
	ifstream file(filename.c_str(), ios_base::binary);
	if (file.fail())
		return;

	vect3f maxe, mine;
	readBBox(mine, maxe, file);

	printf ( "mine = %f,%f,%f  maxe = %f,%f,%f\n", mine [ 0 ], mine [ 1 ], mine [ 2 ], maxe [ 0 ], maxe [ 1 ], maxe [ 2 ] );
	float scale_factor = 1.0 / (maxe[0] - mine[0]);
	vect3f center = (mine + maxe) * 0.5;

	volatile __int32 pts_num = 0;
	readInline(file, pts_num);

	g.points.reserve(pts_num);

	printf("reading %d points\n", pts_num);

	for (int pts_index = 0; pts_index < pts_num; pts_index++)
	{
		OrientedPoint pt_read;
		readPoint(pt_read, file, center, scale_factor, 0);
		g.points.push_back(pt_read);
	}

	file.close();
}
