#include "global.h"

#include <cmath>

void Traversal::traverse(OctTree &tree, Visitor *v)
{
	TraversalData td;
	td.node = tree.root;

	td.depth = 0;
	td.value = 0;
	td.isCopy = false;

	td.cellsize = 1 << (g.maxDepth+1);
	td.x = td.y = td.z = 0;

	static unsigned char root_neighborCells = 0;
	td.neighborCells = &root_neighborCells;

	static float blur_val = 0;
	td.blur_val = &blur_val;

	td.pt_num = 10000;

	traverseNode(v, td);
}


void Traversal::genNodeChild(TraversalData &ctd, TraversalData &td, int x, int y, int z)
{
	if (td.node.data)
	{
		ctd.isCopy = false;
		ctd.depth = td.depth + 1;
		ctd.cellsize = td.cellsize >> 1;
		ctd.x = td.x + x*ctd.cellsize;
		ctd.y = td.y + y*ctd.cellsize;
		ctd.z = td.z + z*ctd.cellsize;

		ctd.node = td.node.children(x,y,z);
		ctd.blur_val = &td.node.blur_vals(x,y,z);
		
		if (g.do_pts_node)
			ctd.pt_num = td.node.pts_node();
		else
			ctd.pt_num = td.node.pt_num(x,y,z);

		ctd.neighborCells = &td.node.neighbor_cells(x,y,z);

		if (g.do_eval_pass)
		{
			// the value was already calculated in a previous pass
			ctd.value = td.node.func_vals(x,y,z);
		}
		else if (do_evaluation && g.wavelet == haar)
		{
			// calculate the value on the fly using haar basis functions
			ctd.value = td.value;
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						ctd.value += td.node.coeffs(i,j,k) * ((x&i)^(y&j)^(z&k) ? -1 : 1);
		}
	}
	else
	{
		ctd = td;
		ctd.isCopy = true;
	}
}

void Traversal::recFace0(Visitor *v, TraversalData ch[2][2][2])
{
	TraversalData face[2];
	face[0] = ch[0][0][0]; 
	face[1] = ch[1][0][0]; 
	traverseFace(v, face, 0);

	face[0] = ch[0][1][0]; 
	face[1] = ch[1][1][0]; 
	traverseFace(v, face, 0);
	
	face[0] = ch[0][0][1]; 
	face[1] = ch[1][0][1]; 
	traverseFace(v, face, 0);

	face[0] = ch[0][1][1]; 
	face[1] = ch[1][1][1]; 
	traverseFace(v, face, 0);
}

void Traversal::recFace1(Visitor *v, TraversalData ch[2][2][2])
{
	TraversalData face[2];
	face[0] = ch[0][0][0]; 
	face[1] = ch[0][1][0]; 
	traverseFace(v, face, 1);

	face[0] = ch[1][0][0]; 
	face[1] = ch[1][1][0]; 
	traverseFace(v, face, 1);
	
	face[0] = ch[0][0][1]; 
	face[1] = ch[0][1][1];
	traverseFace(v, face, 1);

	face[0] = ch[1][0][1];
	face[1] = ch[1][1][1];
	traverseFace(v, face, 1);
}

void Traversal::recFace2(Visitor *v, TraversalData ch[2][2][2])
{
	TraversalData face[2];
	face[0] = ch[0][0][0];
	face[1] = ch[0][0][1];
	traverseFace(v, face, 2);

	face[0] = ch[0][1][0];
	face[1] = ch[0][1][1];
	traverseFace(v, face, 2);

	face[0] = ch[1][0][0];
	face[1] = ch[1][0][1];
	traverseFace(v, face, 2);

	face[0] = ch[1][1][0];
	face[1] = ch[1][1][1];
	traverseFace(v, face, 2);
}


void Traversal::recEdge0(Visitor *v, TraversalData ch[2][2][2])
{
	TraversalData edge[2][2];

	edge[0][0] = ch[0][0][0];
	edge[0][1] = ch[0][0][1]; 
	edge[1][0] = ch[0][1][0]; 
	edge[1][1] = ch[0][1][1]; 
	traverseEdge(v, edge, 0);

	edge[0][0] = ch[1][0][0]; 
	edge[0][1] = ch[1][0][1]; 
	edge[1][0] = ch[1][1][0]; 
	edge[1][1] = ch[1][1][1]; 
	traverseEdge(v, edge, 0);
}

void Traversal::recEdge1(Visitor *v, TraversalData ch[2][2][2])
{
	TraversalData edge[2][2];

	edge[0][0] = ch[0][0][0]; 
	edge[0][1] = ch[0][0][1]; 
	edge[1][0] = ch[1][0][0]; 
	edge[1][1] = ch[1][0][1]; 
	traverseEdge(v, edge, 1);

	edge[0][0] = ch[0][1][0]; 
	edge[0][1] = ch[0][1][1]; 
	edge[1][0] = ch[1][1][0]; 
	edge[1][1] = ch[1][1][1]; 
	traverseEdge(v, edge, 1);
}

void Traversal::recEdge2(Visitor *v, TraversalData ch[2][2][2])
{
	TraversalData edge[2][2];

	edge[0][0] = ch[0][0][0]; 
	edge[0][1] = ch[0][1][0]; 
	edge[1][0] = ch[1][0][0]; 
	edge[1][1] = ch[1][1][0]; 
	traverseEdge(v, edge, 2);
	
	edge[0][0] = ch[0][0][1]; 
	edge[0][1] = ch[0][1][1];
	edge[1][0] = ch[1][0][1];
	edge[1][1] = ch[1][1][1]; 
	traverseEdge(v, edge, 2);
}

void Traversal::traverseNode(Visitor *v, TraversalData &td)
{
	if (!v->onNode(td))
		return;

	// stop if all leaves
	if (!td.node.data)
		return;

	// get children (in standard orientation)
	TraversalData ch[2][2][2];

	for (int i = 0; i < 8; i++)
	{
		const int x = i&1;
		const int y = (i>>1)&1;
		const int z = (i>>2)&1;
		genNodeChild(ch[x][y][z], td, x, y, z);
	}

	traverseNode(v, ch[0][0][0]);
	traverseNode(v, ch[0][1][0]);
	traverseNode(v, ch[1][0][0]);
	traverseNode(v, ch[1][1][0]);
	traverseNode(v, ch[0][0][1]);
	traverseNode(v, ch[0][1][1]);
	traverseNode(v, ch[1][0][1]);
	traverseNode(v, ch[1][1][1]);

	recFace0(v, ch);
	recFace1(v, ch);
	recFace2(v, ch);

	recEdge0(v, ch);
	recEdge1(v, ch);
	recEdge2(v, ch);

	traverseVertex(v, ch);
}


void Traversal::traverseFace(Visitor *v, TraversalData td[2], int orientation)
{
	if (!v->onFace(td, orientation))
		return;

	// stop if all leaves
	for (int x = 0; x < 2; x++)
	{
		if (td[x].node.data)
			goto not_all_leaves;
	}
	return;
not_all_leaves:;

	// get children (in standard orientation) and recurse
	TraversalData ch[2][2][2];

	if (orientation == 0)
	{
		genNodeChild(ch[0][0][0], td[0], 1, 0, 0);
		genNodeChild(ch[0][0][1], td[0], 1, 0, 1);
		genNodeChild(ch[0][1][0], td[0], 1, 1, 0);
		genNodeChild(ch[0][1][1], td[0], 1, 1, 1);

		genNodeChild(ch[1][0][0], td[1], 0, 0, 0);
		genNodeChild(ch[1][0][1], td[1], 0, 0, 1);
		genNodeChild(ch[1][1][0], td[1], 0, 1, 0);
		genNodeChild(ch[1][1][1], td[1], 0, 1, 1);

		// low z order
		recFace0(v, ch);
		recEdge2(v, ch);

		// mid z order
		recEdge1(v, ch);
		traverseVertex(v, ch);
	}
	else if (orientation == 1)
	{
		genNodeChild(ch[0][0][0], td[0], 0, 1, 0);
		genNodeChild(ch[0][0][1], td[0], 0, 1, 1);
		genNodeChild(ch[1][0][0], td[0], 1, 1, 0);
		genNodeChild(ch[1][0][1], td[0], 1, 1, 1);

		genNodeChild(ch[0][1][0], td[1], 0, 0, 0);
		genNodeChild(ch[0][1][1], td[1], 0, 0, 1);
		genNodeChild(ch[1][1][0], td[1], 1, 0, 0);
		genNodeChild(ch[1][1][1], td[1], 1, 0, 1);

		// low z order
		recFace1(v, ch);
		recEdge2(v, ch);

		// mid z order
		recEdge0(v, ch);
		traverseVertex(v, ch);
	}
	else if (orientation == 2)
	{
		genNodeChild(ch[0][0][0], td[0], 0, 0, 1);
		genNodeChild(ch[0][1][0], td[0], 0, 1, 1);
		genNodeChild(ch[1][0][0], td[0], 1, 0, 1);
		genNodeChild(ch[1][1][0], td[0], 1, 1, 1);

		genNodeChild(ch[0][0][1], td[1], 0, 0, 0);
		genNodeChild(ch[0][1][1], td[1], 0, 1, 0);
		genNodeChild(ch[1][0][1], td[1], 1, 0, 0);
		genNodeChild(ch[1][1][1], td[1], 1, 1, 0);

		// mid z order
		recEdge0(v, ch);
		recEdge1(v, ch);
		recFace2(v, ch);
		traverseVertex(v, ch);
	}
}


void Traversal::traverseEdge(Visitor *v, TraversalData td[2][2], int orientation)
{
	if (!v->onEdge(td, orientation))
		return;

	// stop if all leaves
	for (int i = 0; i < 4; i++)
	{
		const int x = i&1;
		const int y = (i>>1)&1;
		if (td[x][y].node.data)
			goto not_all_leaves;
	}
	return;
not_all_leaves:;

	// get children (in standard orientation) and recurse
	TraversalData ch[2][2][2];

	if (orientation == 0)
	{
		genNodeChild(ch[0][0][0], td[0][0], 0, 1, 1);
		genNodeChild(ch[1][0][0], td[0][0], 1, 1, 1);

		genNodeChild(ch[0][0][1], td[0][1], 0, 1, 0);
		genNodeChild(ch[1][0][1], td[0][1], 1, 1, 0);

		genNodeChild(ch[0][1][0], td[1][0], 0, 0, 1);
		genNodeChild(ch[1][1][0], td[1][0], 1, 0, 1);

		genNodeChild(ch[0][1][1], td[1][1], 0, 0, 0);
		genNodeChild(ch[1][1][1], td[1][1], 1, 0, 0);

		// mid z order
		recEdge0(v, ch);
		traverseVertex(v, ch);
	}
	else if (orientation == 1)
	{
		genNodeChild(ch[0][0][0], td[0][0], 1, 0, 1);
		genNodeChild(ch[0][1][0], td[0][0], 1, 1, 1);

		genNodeChild(ch[0][0][1], td[0][1], 1, 0, 0);
		genNodeChild(ch[0][1][1], td[0][1], 1, 1, 0);

		genNodeChild(ch[1][0][0], td[1][0], 0, 0, 1);
		genNodeChild(ch[1][1][0], td[1][0], 0, 1, 1);

		genNodeChild(ch[1][0][1], td[1][1], 0, 0, 0);
		genNodeChild(ch[1][1][1], td[1][1], 0, 1, 0);

		// mid z order
		recEdge1(v, ch);
		traverseVertex(v, ch);
	}
	else if (orientation == 2)
	{
		genNodeChild(ch[0][0][0], td[0][0], 1, 1, 0);
		genNodeChild(ch[0][0][1], td[0][0], 1, 1, 1);

		genNodeChild(ch[0][1][0], td[0][1], 1, 0, 0);
		genNodeChild(ch[0][1][1], td[0][1], 1, 0, 1);

		genNodeChild(ch[1][0][0], td[1][0], 0, 1, 0);
		genNodeChild(ch[1][0][1], td[1][0], 0, 1, 1);

		genNodeChild(ch[1][1][0], td[1][1], 0, 0, 0);
		genNodeChild(ch[1][1][1], td[1][1], 0, 0, 1);

		// low z order
		recEdge2(v, ch);

		// mid z order
		traverseVertex(v, ch);
	}
}


void Traversal::traverseVertex(Visitor *v, TraversalData td[2][2][2])
{
	if (!v->onVertex(td))
		return;

	// stop if all leaves
	for (int i = 0; i < 8; i++)
	{
		const int x = i&1;
		const int y = (i>>1)&1;
		const int z = (i>>2)&1;
		if (td[x][y][z].node.data)
			goto not_all_leaves;
	}
	return;
not_all_leaves:;

	// get children (in standard orientation)
	TraversalData ch[2][2][2];
	for (int i = 0; i < 8; i++)
	{
		const int x = i&1;
		const int y = (i>>1)&1;
		const int z = (i>>2)&1;
		genNodeChild(ch[x][y][z], td[x][y][z], x^1, y^1, z^1);
	}

	// mid z order
	traverseVertex(v, ch);
}

