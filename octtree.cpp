#include "global.h"

void OctTree::countPoints(vect3f &pt, int num) 
{
	vect3f ext = maxe-mine;
	root.countPoints(pt, g.maxDepth, 0, mine, ext, num);
}
void OctTree::coeffs_daub4(OrientedPoint &pt, int minDepth, int maxDepthRecur) 
{
	vect3f ext = maxe-mine;
	root.coeffs_daub4(pt, maxDepthRecur, 0, mine, ext, minDepth);
}
void OctTree::coeffs_haar(OrientedPoint &pt, int minDepth, int maxDepthRecur) 
{
	vect3f ext = maxe-mine;
	root.coeffs_haar(pt, maxDepthRecur, 0, mine, ext, minDepth);
}

// removal of used nodes
void OctTree::removeUpTo(int zvalue)
{
	removeUpTo(root, 1 << (g.maxDepth+1), 0, zvalue);
}

void OctTree::removeUpTo(OctNode node, const int size, const int z, const int zvalue)
{
	if (!node.data)
		return;

	const int size2 = size >> 1;

	if (z < zvalue)
	{
		if (z + size <= zvalue)
		{
			// remove low children
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					if (node.children(i,j,0).data)
					{
						node.children(i,j,0).detachFromNeighbors();
						node.children(i,j,0).clear();
						//node.children(i,j,0).data = 0;
					}
				}
			}
		}
		else
		{
			// recurse low
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					removeUpTo(node.children(i,j,0), size2, z, zvalue);
			}
		}
	}

	if (z + size2 < zvalue)
	{
		if (z + size + size2 <= zvalue)
		{
			// remove high children
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					if (node.children(i,j,1).data)
					{
						node.children(i,j,1).detachFromNeighbors();
						node.children(i,j,1).clear();
						//node.children(i,j,1).data = 0;
					}
				}
			}
		}
		else
		{
			// recurse high
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					removeUpTo(node.children(i,j,1), size2, z+size2, zvalue);
			}
		}
	}
}


// removal of high resolution nodes
void OctTree::removeHighRes(int zvalue)
{
	removeHighRes(root, 1 << (g.maxDepth+1), 0, zvalue, 0);
}

void OctTree::removeHighRes(OctNode node, int size, int z, int zvalue, int depth)
{
	if (!node.data)
		return;

	if (depth >= g.maxDepthMem)
	{
		if (z + size <= zvalue)
		{
			// remove children
			node.removeChildren();
		}

		return;
	}

	// recurse
	int size2 = size / 2;

	if (z < zvalue)
	{
		// recurse low
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
				removeHighRes(node.children(i,j,0), size2, z, zvalue, depth+1);
		}
	}
	
	if (z + size2 < zvalue)
	{
		// recurse high
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
				removeHighRes(node.children(i,j,1), size2, z+size2, zvalue, depth+1);
		}
	}
}

void removeHighResAll(OctNode node, int depth)
{
	if (depth >= g.maxDepthMem)
		node.removeChildren();

	// recur
	for (int i = 0; i < 8; i++)
		if (node.children_linear(i).data)
			removeHighResAll(node.children_linear(i), depth+1);
}
