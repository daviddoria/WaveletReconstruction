#include "global.h"
#include <float.h>
#include <cstring>

int OctNode::data_size = 0;
int OctNode::node_count = 0;

int OctNode::o_children = -100000000;
int OctNode::o_coeffs = -100000000;
int OctNode::o_pt_num = -100000000;
int OctNode::o_pts_node = -100000000;
int OctNode::o_func_vals = -100000000;
int OctNode::o_blur_vals = -100000000;
int OctNode::o_neighbor_cells = -100000000;
int OctNode::o_neighbors = -100000000;
int OctNode::o_above_below_zero = -100000000;

void OctNode::calc_offsets()
{
	data_size = 0;
	{o_children = data_size; data_size += 8*sizeof(OctNode*);}
	{o_coeffs = data_size; data_size += 8*sizeof(float);}

	if (g.do_pts_node)
		{o_pts_node = data_size; data_size += sizeof(int);}
	else
		{o_pt_num = data_size; data_size += 8*sizeof(int);}

	if (g.do_eval_pass) {o_func_vals = data_size; data_size += 8*sizeof(float);}
	if (g.do_blur) {o_blur_vals = data_size; data_size += 8*sizeof(float);}
	if (g.do_prune) {o_neighbor_cells = data_size; data_size += 8*sizeof(unsigned char);}
	if (g.wavelet != haar) {o_neighbors = data_size; data_size += 6*sizeof(OctNode);}
	if (g.do_above_below) {o_above_below_zero = data_size; data_size += sizeof(unsigned char);}
}

void OctNode::allocate()
{
	//calc_offsets();
	assert(data_size > 0);

	// allocate and clear the data
	data = new unsigned char [data_size];
	memset(data, 0, data_size);
	
	node_count++;
}

void OctNode::clear()
{
	removeChildren();
	
	delete[] data;
	data = 0;

	node_count--;
}


void OctNode::detachFromNeighbors()
{
	if (g.wavelet == haar)
		return;

	for (int d = 0; d < 3; d++)
	{
		OctNode n0 = neighbors(d, 0);
		OctNode n1 = neighbors(d, 1);
		if (n0.data)
		{
			n0.neighbors(d, 1).data = 0;
			n0.data = 0;
		}
		
		if (n1.data)
		{
			n1.neighbors(d, 0).data = 0;
			n1.data = 0;
		}
	}
}

void OctNode::removeChildren()
{
	for (int m = 0; m < 8; m++)
	{
		const int i = m&1;
		const int j = (m>>1)&1;
		const int k = (m>>2)&1;

		if (children(i,j,k).data)
		{
			children(i,j,k).detachFromNeighbors();
			children(i,j,k).clear();
			//children(i,j,k).data = 0;
		}
	}
}

void OctNode::countPoints(vect3f &pt, int maxDepth, int depth, vect3f &mine, vect3f &ext, int num)
{
	vect3f ext2 = ext*.5;
	vect3f center = mine + ext2;

	// split them
	int ix = 0, iy = 0, iz = 0;

	if (pt[0] > center[0])
		ix = 1;
	if (pt[1] > center[1])
		iy = 1;
	if (pt[2] > center[2])
		iz = 1;

	// split the node and push down point
	if (depth < maxDepth)
	{
		// push down
		if (children(ix,iy,iz).data == 0)
			children(ix,iy,iz).allocate();
		vect3f corner2(mine[0] + ext2[0]*ix, mine[1] + ext2[1]*iy, mine[2] + ext2[2]*iz);
		children(ix,iy,iz).countPoints(pt, maxDepth, depth+1, corner2, ext2, num);
	}

	// it's a leaf
	if (g.do_pts_node)
		pts_node() += num;
	else
		pt_num(ix,iy,iz) += num;
}

OctNode OctNode::getNeighbor(int x, int y, int z)
{
	OctNode n = *this;

	if (x)
	{
		n = n.neighbors(0, (x+1)>>1);
		if (n.data == 0)
			return n;
	}

	if (y)
	{
		n = n.neighbors(1, (y+1)>>1);
		if (n.data == 0)
			return n;
	}

	if (z)
	{
		n = n.neighbors(2, (z+1)>>1);
		if (n.data == 0)
			return n;
	}

	return n;
}


OctNode OctNode::getNeighborUnsafe(int x, int y, int z)
{
	OctNode n = *this;

	if (x)
		n = n.neighbors(0, (x+1)>>1);

	assert(n.data != 0);

	if (y)
		n = n.neighbors(1, (y+1)>>1);
	
	assert(n.data != 0);

	if (z)
		n = n.neighbors(2, (z+1)>>1);

	return n;
}


float OctNode::getValueAtPoint(vect3f &pos, vect3f mine, vect3f ext)
{
	// split the node and push down point
	vect3f ext2 = ext*.5;
	vect3f center = mine + ext2;

	// split them
	int ix = 0, iy = 0, iz = 0;

	if (pos[0] > center[0])
		ix = 1;
	if (pos[1] > center[1])
		iy = 1;
	if (pos[2] > center[2])
		iz = 1;

	// push down
	if (children(ix, iy, iz).data)
	{
		vect3f corner2(mine[0] + ext2[0]*ix, mine[1] + ext2[1]*iy, mine[2] + ext2[2]*iz);
		return children(ix, iy, iz).getValueAtPoint(pos, corner2, ext2);
	}
	else
	{
		if (g.do_blur)
			return blur_vals(ix, iy, iz);
		else
			return func_vals(ix, iy, iz);
	}
}
