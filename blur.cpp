#include "global.h"
#include <cstdio>


struct BlurVisitor : public StreamingVisitor
{
	BlurVisitor(int low, int high) : StreamingVisitor(low, high) {}
	BlurVisitor(int z_border) : StreamingVisitor(z_border) {}

	bool onVertex(TraversalData td[2][2][2]);
	bool onEdge(TraversalData td[2][2], int orientation);
	bool onFace(TraversalData td[2], int orientation);
	bool onNode(TraversalData &td);
};
struct BlurMemVisitor : public Visitor
{
	bool onVertex(TraversalData td[2][2][2]);
	bool onEdge(TraversalData td[2][2], int orientation);
	bool onFace(TraversalData td[2], int orientation);
	bool onNode(TraversalData &td);
};


// blur the things that aren't stored in memory
bool BlurVisitor::onVertex(TraversalData td[2][2][2])
{
	// boundary check
	if (!StreamingVisitor::onVertex(td))
		return false;

	for (int x1 = 0; x1 < 2; x1++)
	{
		int x2 = x1 ^ 1;
		for (int y1 = 0; y1 < 2; y1++)
		{
			int y2 = y1 ^ 1;
			for (int z1 = 0; z1 < 2; z1++)
			{
				int z2 = z1 ^ 1;

				if (td[x1][y1][z1].depth >= td[x2][y2][z2].depth && !td[x1][y1][z1].isCopy && td[x1][y1][z1].depth > g.maxDepthMem+1)
				{
					(*td[x1][y1][z1].blur_val) += td[x2][y2][z2].value / 64.0;
				}
			}
		}
	}

	return true;
}

bool BlurVisitor::onEdge(TraversalData td[2][2], int orientation)
{
	// boundary check
	if (!StreamingVisitor::onEdge(td, orientation))
		return false;

	for (int x1 = 0; x1 < 2; x1++)
	{
		int x2 = x1 ^ 1;
		for (int y1 = 0; y1 < 2; y1++)
		{
			int y2 = y1 ^ 1;

			if (td[x1][y1].depth >= td[x2][y2].depth && !td[x1][y1].isCopy && td[x1][y1].depth > g.maxDepthMem+1)
			{
				(*td[x1][y1].blur_val) += td[x2][y2].value / 32.0;
			}
		}
	}

	return true;
}

bool BlurVisitor::onFace(TraversalData td[2], int orientation)
{
	// boundary check
	if (!StreamingVisitor::onFace(td, orientation))
		return false;

	for (int x1 = 0; x1 < 2; x1++)
	{
		int x2 = x1 ^ 1;

		if (td[x1].depth >= td[x2].depth && td[x1].depth > g.maxDepthMem+1)
		{
			(*td[x1].blur_val) += td[x2].value / 16.0;
		}
	}

	return true;
}


bool BlurVisitor::onNode(TraversalData &td)
{
	// boundary check
	if (!StreamingVisitor::onNode(td))
		return false;

	if (td.depth > g.maxDepthMem+1)
		(*td.blur_val) += td.value / 8.0;

	return true;
}

/////////////////// blur what is in memory
bool BlurMemVisitor::onVertex(TraversalData td[2][2][2])
{
	bool recur = true;

	for (int x1 = 0; x1 < 2; x1++)
	{
		int x2 = x1 ^ 1;
		for (int y1 = 0; y1 < 2; y1++)
		{
			int y2 = y1 ^ 1;
			for (int z1 = 0; z1 < 2; z1++)
			{
				int z2 = z1 ^ 1;

				if (td[x1][y1][z1].depth >= td[x2][y2][z2].depth && !td[x1][y1][z1].isCopy)
				{
					(*td[x1][y1][z1].blur_val) += td[x2][y2][z2].value / 64.0;
				}
			}
		}
	}

	return recur;
}

bool BlurMemVisitor::onEdge(TraversalData td[2][2], int orientation)
{
	bool recur = true;

	for (int x1 = 0; x1 < 2; x1++)
	{
		int x2 = x1 ^ 1;
		for (int y1 = 0; y1 < 2; y1++)
		{
			int y2 = y1 ^ 1;

			if (td[x1][y1].depth >= td[x2][y2].depth && !td[x1][y1].isCopy)
			{
				(*td[x1][y1].blur_val) += td[x2][y2].value / 32.0;
			}
		}
	}

	return recur;
}

bool BlurMemVisitor::onFace(TraversalData td[2], int orientation)
{
	bool recur = true;

	for (int x1 = 0; x1 < 2; x1++)
	{
		int x2 = x1 ^ 1;

		if (td[x1].depth >= td[x2].depth)
		{
			(*td[x1].blur_val) += td[x2].value / 16.0;
		}
	}

	return recur;
}


bool BlurMemVisitor::onNode(TraversalData &td)
{
	(*td.blur_val) += td.value / 8.0;

	return true;
}




void blurAB(int start, int end, OctNode n, int size, int z)
{
	// recur
	int size2 = size >> 1;

	bool low = z < end && start < z + size2;
	bool high = z + size2 < end && start < z + size;

	if (low)
	{
		for (int child = 0; child < 4; child++)
		{
			if (n.children(child&1,child>>1,0).data)
				blurAB(start, end, n.children(child&1,child>>1,0), size2, z);
		}
	}
	if (high)
	{
		for (int child = 0; child < 4; child++)
		{
			if (n.children(child&1,child>>1,1).data)
				blurAB(start, end, n.children(child&1,child>>1,1), size2, z+size2);
		}
	}

	// crossing zero
	unsigned char &above_below_zero = n.above_below_zero();
	above_below_zero = 0;

	for (int i = 0; i < 8; i++)
	{
		if (n.children_linear(i).data)
			above_below_zero |= n.children_linear(i).above_below_zero();
		else if (n.blur_vals_linear(i) >= average_val)
			above_below_zero |= 1;
		else
			above_below_zero |= 2;
	}
}




void blur_second(int from, int to)
{
	Traversal trav;
	if (!g.do_eval_pass)
		trav.do_evaluation = true;

	// do the slab
	printf("blur slab %d/%d\n", from, streaminfo.cells_total);

	BlurVisitor visitorSlice(from, to);
	trav.traverse(g.tree, &visitorSlice);

	// do the border
	BlurVisitor visitorBorder(to);
	trav.traverse(g.tree, &visitorBorder);

	// get average value for above_below marking -- make sure not to calculate again later
	if (!g.do_streaming)
		average_nonstreaming(from, to);

	// mark above or below
	if (g.do_above_below)
		blurAB(from, to, g.tree.root, streaminfo.cells_total, 0);
}

void blur_in_memory()
{
	printf("blur in memory\n");

	Traversal trav;
	if (!g.do_eval_pass)
		trav.do_evaluation = true;

	BlurMemVisitor visitor;
	trav.traverse(g.tree, &visitor);
}
