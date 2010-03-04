#include "global.h"

struct TraversalNeighborGen
{
	void traverse(OctTree &tree, Visitor *v); // copy from Traversal

	void genNodeChild(TraversalData &ctd, TraversalData &td, int x, int y, int z);

	void traverseNode(Visitor *v, TraversalData &td); // copy from Traversal
	void traverseFace(Visitor *v, TraversalData td[2], int orientation);
	void traverseEdge(Visitor *v, TraversalData td[2][2], int orientation);
	void traverseVertex(Visitor *v, TraversalData td[2][2][2]);
	
	void recFace0(Visitor *v, TraversalData ch[2][2][2]); // copy from Traversal
	void recFace1(Visitor *v, TraversalData ch[2][2][2]); // copy from Traversal
	void recFace2(Visitor *v, TraversalData ch[2][2][2]); // copy from Traversal

	void recEdge0(Visitor *v, TraversalData ch[2][2][2]); // copy from Traversal
	void recEdge1(Visitor *v, TraversalData ch[2][2][2]); // copy from Traversal
	void recEdge2(Visitor *v, TraversalData ch[2][2][2]); // copy from Traversal
};

void TraversalNeighborGen::traverse(OctTree &tree, Visitor *v)
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


struct StreamingVisitorNeighbor : public StreamingVisitor
{
	StreamingVisitorNeighbor(int low, int high) : StreamingVisitor(low, high) {}
	StreamingVisitorNeighbor(int z_border) : StreamingVisitor(z_border) {}

	TraversalData *minSize(TraversalData *a, TraversalData *b)
	{
		if (a->cellsize < b->cellsize)
			return a;
		else
			return b;
	}

	bool insideOrCrossing(int zl, int zm, int zh, int cellsize)
	{
		return zl < z_high+cellsize && zh > z_low-cellsize && (zl == zm || (zm > z_low-cellsize && zm < z_high+cellsize)); 
	}

	virtual bool onNode(TraversalData &td)
	{
		return insideOrCrossing(td.z, td.z, td.z + td.cellsize, td.cellsize);
	}

	virtual bool onFace(TraversalData td[2], int orientation)
	{
		if (orientation == 2)
			return insideOrCrossing(td[0].z, td[1].z, td[1].z + td[1].cellsize, td[1].cellsize);
		else
		{
			TraversalData *m = minSize(&td[0], &td[1]);
			return insideOrCrossing(m->z, m->z, m->z + m->cellsize, m->cellsize);
		}
	}

	virtual bool onEdge(TraversalData td[2][2], int orientation)
	{
		if (orientation == 2)
		{
			TraversalData *m = minSize( minSize(&td[0][0], &td[1][0]), minSize(&td[0][1], &td[1][1]) );
			return insideOrCrossing(m->z, m->z, m->z + m->cellsize, m->cellsize);
		}
		else
		{
			TraversalData *l = minSize(&td[0][0], &td[1][0]);
			TraversalData *h = minSize(&td[0][1], &td[1][1]);
			return insideOrCrossing(l->z, h->z, h->z + h->cellsize, h->cellsize);
		}
	}

	virtual bool onVertex(TraversalData td[2][2][2])
	{
		TraversalData *l = minSize( minSize(&td[0][0][0], &td[1][0][0]), minSize(&td[0][1][0], &td[1][1][0]) );
		TraversalData *h = minSize( minSize(&td[0][0][1], &td[1][0][1]), minSize(&td[0][1][1], &td[1][1][1]) );
		return insideOrCrossing(l->z, h->z, h->z + h->cellsize, h->cellsize);
	}
};

struct ConnectNeighborsVisitor : public StreamingVisitorNeighbor
{
	int maxDepth;

	ConnectNeighborsVisitor(int low, int high) : StreamingVisitorNeighbor(low, high) {maxDepth = 100;}

	virtual bool onVertex(TraversalData td[2][2][2])
	{
		// boundary check
		if (!StreamingVisitorNeighbor::onVertex(td))
			return false;

		// connect neighbors
		if (td[0][0][1].node.data)
		{
			td[0][0][0].node.neighbors(0, 1) = td[1][0][0].node;
			td[0][0][1].node.neighbors(0, 1) = td[1][0][1].node;
			td[0][1][0].node.neighbors(0, 1) = td[1][1][0].node;
			td[0][1][1].node.neighbors(0, 1) = td[1][1][1].node;

			td[1][0][0].node.neighbors(0, 0) = td[0][0][0].node;
			td[1][0][1].node.neighbors(0, 0) = td[0][0][1].node;
			td[1][1][0].node.neighbors(0, 0) = td[0][1][0].node;
			td[1][1][1].node.neighbors(0, 0) = td[0][1][1].node;

			td[0][0][0].node.neighbors(1, 1) = td[0][1][0].node;
			td[0][0][1].node.neighbors(1, 1) = td[0][1][1].node;
			td[1][0][0].node.neighbors(1, 1) = td[1][1][0].node;
			td[1][0][1].node.neighbors(1, 1) = td[1][1][1].node;

			td[0][1][0].node.neighbors(1, 0) = td[0][0][0].node;
			td[0][1][1].node.neighbors(1, 0) = td[0][0][1].node;
			td[1][1][0].node.neighbors(1, 0) = td[1][0][0].node;
			td[1][1][1].node.neighbors(1, 0) = td[1][0][1].node;

			td[0][0][0].node.neighbors(2, 1) = td[0][0][1].node;
			td[0][1][0].node.neighbors(2, 1) = td[0][1][1].node;
			td[1][0][0].node.neighbors(2, 1) = td[1][0][1].node;
			td[1][1][0].node.neighbors(2, 1) = td[1][1][1].node;

			td[0][0][1].node.neighbors(2, 0) = td[0][0][0].node;
			td[0][1][1].node.neighbors(2, 0) = td[0][1][0].node;
			td[1][0][1].node.neighbors(2, 0) = td[1][0][0].node;
			td[1][1][1].node.neighbors(2, 0) = td[1][1][0].node;

			bool child = false;
			if (g.do_pts_node)
			{
				for (int x = 0; x < 2; x++)
				{
					for (int y = 0; y < 2; y++)
						for (int z = 0; z < 2; z++)
						{
							if (td[x][y][z].node.pts_node())
							{
								for (int i = 0; i < 8; i++)
									if (td[x][y][z].node.children_linear(i).data)
									{
										child = true;
										goto done;
									}
							}
						}
				}
			}
			else
			{
				for (int x = 0; x < 2; x++)
				{
					for (int y = 0; y < 2; y++)
						for (int z = 0; z < 2; z++)
							for (int i = 0; i < 8; i++)
								if (td[x][y][z].node.children_linear(i).data && td[x][y][z].node.pt_num_linear(i))
								{
									child = true;
									goto done;
								}
				}
			}
done:
			return 
				(td[0][0][0].pt_num) + 
				(td[0][0][1].pt_num) + 
				(td[0][1][0].pt_num) + 
				(td[0][1][1].pt_num) +
				(td[1][0][0].pt_num) + 
				(td[1][0][1].pt_num) + 
				(td[1][1][0].pt_num) + 
				(td[1][1][1].pt_num) != 0 && child;
		}

		return false;
	}

	virtual bool onEdge(TraversalData td[2][2], int orientation)
	{
		// boundary check
		if (!StreamingVisitorNeighbor::onEdge(td, orientation))
			return false;

		//for (int i = 0; i < 4; i++)
		//	checkIfContainsPoint(td[(i >> 1) & 1][i&1]);

		if (g.do_pts_node)
		{
			if ( td[0][0].node.data )
			{
				bool child = false;
				for (int x = 0; x < 2; x++)
				{
					for (int y = 0; y < 2; y++)
					{
						if (td[x][y].node.pts_node())
						{
							for (int i = 0; i < 8; i++)
								if (td[x][y].node.children_linear(i).data)
								{
									child = true;
									goto donea;
								}
						}
					}
				}
	donea:

				return	 (td[0][0].pt_num) + 
					(td[0][1].pt_num) + 
					(td[1][0].pt_num) + 
					(td[1][1].pt_num) != 0 && child;
			}
		}
		else
		{
			if ( td[0][0].node.data )
			{
				bool child = false;
				for (int x = 0; x < 2; x++)
				{
					for (int y = 0; y < 2; y++)
					{
						for (int i = 0; i < 8; i++)
							if (td[x][y].node.children_linear(i).data && td[x][y].node.pt_num_linear(i))
							{
								child = true;
								goto done;
							}
					}
				}
	done:

				return	 (td[0][0].pt_num) + 
					(td[0][1].pt_num) + 
					(td[1][0].pt_num) + 
					(td[1][1].pt_num) != 0 && child;
			}
		}
		return false;
	}

	virtual bool onFace(TraversalData td[2], int orientation) 
	{
		// boundary check
		if (!StreamingVisitorNeighbor::onFace(td, orientation))
			return false;

		//for (int i = 0; i < 2; i++)
		//	checkIfContainsPoint(td[i&1]);

		if (g.do_pts_node)
		{
			if ( td[0].node.data )
			{
				bool child = false;
				for (int x = 0; x < 2; x++)
				{
					if (td[x].node.pts_node())
					{
						for (int i = 0; i < 8; i++)
							if (td[x].node.children_linear(i).data)
							{
								child = true;
								goto donea;
							}
					}
				}
	donea:
				return (td[0].pt_num) + (td[1].pt_num) != 0 && child;
			}
		}
		else
		{
			if ( td[0].node.data )
			{
				bool child = false;
				for (int x = 0; x < 2; x++)
				{
					for (int i = 0; i < 8; i++)
						if (td[x].node.children_linear(i).data && td[x].node.pt_num_linear(i))
						{
							child = true;
							goto done;
						}
				}
	done:
				return (td[0].pt_num) + (td[1].pt_num) != 0 && child;
			}
		}
		return false;
	}

	virtual bool onNode(TraversalData &td)
	{
		// boundary check
		if (!StreamingVisitorNeighbor::onNode(td))
			return false;

		//checkIfContainsPoint(td);

		if (g.do_pts_node)
		{
			if (td.node.data && td.node.pts_node())
			{
				bool child = false;
				for (int i = 0; i < 8; i++)
				{
					if (td.node.children_linear(i).data)
					{
						child = true;
						break;
					}
				}

				return (td.pt_num) != 0 && child;
			}
		}
		else
		{
			if (td.node.data)
			{
				bool child = false;
				for (int i = 0; i < 8; i++)
				{
					if (td.node.children_linear(i).data && td.node.pt_num_linear(i))
					{
						child = true;
						break;
					}
				}

				return (td.pt_num) != 0 && child;
			}
		}
		return false;
	}
};


void TraversalNeighborGen::genNodeChild(TraversalData &ctd, TraversalData &td, int x, int y, int z)
{
	ctd.depth = td.depth + 1;
	ctd.cellsize = td.cellsize >> 1;
	ctd.x = td.x + x*ctd.cellsize;
	ctd.y = td.y + y*ctd.cellsize;
	ctd.z = td.z + z*ctd.cellsize;

//	ctd.parent = td.node;

	if (td.node.children(x,y,z).data == 0)
		td.node.children(x,y,z).allocate();

	ctd.node = td.node.children(x,y,z);

	if (g.do_pts_node)
		ctd.pt_num = td.node.pts_node();
	else
		ctd.pt_num = td.node.pt_num(x,y,z);
}


void TraversalNeighborGen::recFace0(Visitor *v, TraversalData ch[2][2][2])
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

void TraversalNeighborGen::recFace1(Visitor *v, TraversalData ch[2][2][2])
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

void TraversalNeighborGen::recFace2(Visitor *v, TraversalData ch[2][2][2])
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


void TraversalNeighborGen::recEdge0(Visitor *v, TraversalData ch[2][2][2])
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

void TraversalNeighborGen::recEdge1(Visitor *v, TraversalData ch[2][2][2])
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

void TraversalNeighborGen::recEdge2(Visitor *v, TraversalData ch[2][2][2])
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


int ptsChild(TraversalData &td, int x, int y, int z)
{
	if (td.node.data)
	{
		if (td.node.children(x,y,z).data)
		{
			if (g.do_pts_node)
				return td.node.pts_node();
			else
				return td.node.pt_num(x,y,z);
		}
		else
			return 0;
	}
	else
		return 0;
}


void TraversalNeighborGen::traverseNode(Visitor *v, TraversalData &td)
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

void TraversalNeighborGen::traverseFace(Visitor *v, TraversalData td[2], int orientation)
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
		if (
			ptsChild(td[0], 1, 0, 0) +
			ptsChild(td[0], 1, 0, 1) +
			ptsChild(td[0], 1, 1, 0) +
			ptsChild(td[0], 1, 1, 1) +
			ptsChild(td[1], 0, 0, 0) +
			ptsChild(td[1], 0, 0, 1) +
			ptsChild(td[1], 0, 1, 0) +
			ptsChild(td[1], 0, 1, 1) == 0) 
			return;


		genNodeChild(ch[0][0][0], td[0], 1, 0, 0);
		genNodeChild(ch[0][0][1], td[0], 1, 0, 1);
		genNodeChild(ch[0][1][0], td[0], 1, 1, 0);
		genNodeChild(ch[0][1][1], td[0], 1, 1, 1);
		genNodeChild(ch[1][0][0], td[1], 0, 0, 0);
		genNodeChild(ch[1][0][1], td[1], 0, 0, 1);
		genNodeChild(ch[1][1][0], td[1], 0, 1, 0);
		genNodeChild(ch[1][1][1], td[1], 0, 1, 1);

		recFace0(v, ch);
		recEdge1(v, ch);
		recEdge2(v, ch);
		traverseVertex(v, ch);
	}
	else if (orientation == 1)
	{
		if (
			ptsChild(td[0], 0, 1, 0) +
			ptsChild(td[0], 0, 1, 1) +
			ptsChild(td[0], 1, 1, 0) +
			ptsChild(td[0], 1, 1, 1) +
			ptsChild(td[1], 0, 0, 0) +
			ptsChild(td[1], 0, 0, 1) +
			ptsChild(td[1], 1, 0, 0) +
			ptsChild(td[1], 1, 0, 1) == 0)
			return;


		genNodeChild(ch[0][0][0], td[0], 0, 1, 0);
		genNodeChild(ch[0][0][1], td[0], 0, 1, 1);
		genNodeChild(ch[1][0][0], td[0], 1, 1, 0);
		genNodeChild(ch[1][0][1], td[0], 1, 1, 1);
		genNodeChild(ch[0][1][0], td[1], 0, 0, 0);
		genNodeChild(ch[0][1][1], td[1], 0, 0, 1);
		genNodeChild(ch[1][1][0], td[1], 1, 0, 0);
		genNodeChild(ch[1][1][1], td[1], 1, 0, 1);

		recEdge0(v, ch);
		recFace1(v, ch);
		recEdge2(v, ch);
		traverseVertex(v, ch);
	}
	else if (orientation == 2)
	{
		if (
			ptsChild(td[0], 0, 0, 1) +
			ptsChild(td[0], 0, 1, 1) +
			ptsChild(td[0], 1, 0, 1) +
			ptsChild(td[0], 1, 1, 1) +
			ptsChild(td[1], 0, 0, 0) +
			ptsChild(td[1], 0, 1, 0) +
			ptsChild(td[1], 1, 0, 0) +
			ptsChild(td[1], 1, 1, 0) == 0)
			return;

		genNodeChild(ch[0][0][0], td[0], 0, 0, 1);
		genNodeChild(ch[0][1][0], td[0], 0, 1, 1);
		genNodeChild(ch[1][0][0], td[0], 1, 0, 1);
		genNodeChild(ch[1][1][0], td[0], 1, 1, 1);
		genNodeChild(ch[0][0][1], td[1], 0, 0, 0);
		genNodeChild(ch[0][1][1], td[1], 0, 1, 0);
		genNodeChild(ch[1][0][1], td[1], 1, 0, 0);
		genNodeChild(ch[1][1][1], td[1], 1, 1, 0);

		recEdge0(v, ch);
		recEdge1(v, ch);
		recFace2(v, ch);
		traverseVertex(v, ch);
	}
}


void TraversalNeighborGen::traverseEdge(Visitor *v, TraversalData td[2][2], int orientation)
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
		if (
			ptsChild(td[0][0], 0, 1, 1) +
			ptsChild(td[0][0], 1, 1, 1) +
			ptsChild(td[0][1], 0, 1, 0) +
			ptsChild(td[0][1], 1, 1, 0) +
			ptsChild(td[1][0], 0, 0, 1) +
			ptsChild(td[1][0], 1, 0, 1) +
			ptsChild(td[1][1], 0, 0, 0) +
			ptsChild(td[1][1], 1, 0, 0) == 0)
			return;

		genNodeChild(ch[0][0][0], td[0][0], 0, 1, 1);
		genNodeChild(ch[1][0][0], td[0][0], 1, 1, 1);
		genNodeChild(ch[0][0][1], td[0][1], 0, 1, 0);
		genNodeChild(ch[1][0][1], td[0][1], 1, 1, 0);
		genNodeChild(ch[0][1][0], td[1][0], 0, 0, 1);
		genNodeChild(ch[1][1][0], td[1][0], 1, 0, 1);
		genNodeChild(ch[0][1][1], td[1][1], 0, 0, 0);
		genNodeChild(ch[1][1][1], td[1][1], 1, 0, 0);

		recEdge0(v, ch);
		traverseVertex(v, ch);
	}
	else if (orientation == 1)
	{
		if (
			ptsChild(td[0][0], 1, 0, 1) +
			ptsChild(td[0][0], 1, 1, 1) +
			ptsChild(td[0][1], 1, 0, 0) +
			ptsChild(td[0][1], 1, 1, 0) +
			ptsChild(td[1][0], 0, 0, 1) +
			ptsChild(td[1][0], 0, 1, 1) +
			ptsChild(td[1][1], 0, 0, 0) +
			ptsChild(td[1][1], 0, 1, 0) == 0)
			return;

		genNodeChild(ch[0][0][0], td[0][0], 1, 0, 1);
		genNodeChild(ch[0][1][0], td[0][0], 1, 1, 1);
		genNodeChild(ch[0][0][1], td[0][1], 1, 0, 0);
		genNodeChild(ch[0][1][1], td[0][1], 1, 1, 0);
		genNodeChild(ch[1][0][0], td[1][0], 0, 0, 1);
		genNodeChild(ch[1][1][0], td[1][0], 0, 1, 1);
		genNodeChild(ch[1][0][1], td[1][1], 0, 0, 0);
		genNodeChild(ch[1][1][1], td[1][1], 0, 1, 0);

		recEdge1(v, ch);
		traverseVertex(v, ch);
	}
	else if (orientation == 2)
	{
		if (
			ptsChild(td[0][0], 1, 1, 0) +
			ptsChild(td[0][0], 1, 1, 1) +
			ptsChild(td[0][1], 1, 0, 0) +
			ptsChild(td[0][1], 1, 0, 1) +
			ptsChild(td[1][0], 0, 1, 0) +
			ptsChild(td[1][0], 0, 1, 1) +
			ptsChild(td[1][1], 0, 0, 0) +
			ptsChild(td[1][1], 0, 0, 1) == 0)
			return;

		genNodeChild(ch[0][0][0], td[0][0], 1, 1, 0);
		genNodeChild(ch[0][0][1], td[0][0], 1, 1, 1);
		genNodeChild(ch[0][1][0], td[0][1], 1, 0, 0);
		genNodeChild(ch[0][1][1], td[0][1], 1, 0, 1);
		genNodeChild(ch[1][0][0], td[1][0], 0, 1, 0);
		genNodeChild(ch[1][0][1], td[1][0], 0, 1, 1);
		genNodeChild(ch[1][1][0], td[1][1], 0, 0, 0);
		genNodeChild(ch[1][1][1], td[1][1], 0, 0, 1);

		recEdge2(v, ch);
		traverseVertex(v, ch);
	}
}


void TraversalNeighborGen::traverseVertex(Visitor *v, TraversalData td[2][2][2])
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

	// get points
	int pts = 0;
	for (int i = 0; i < 8; i++)
	{
		const int x = i&1;
		const int y = (i>>1)&1;
		const int z = (i>>2)&1;
		pts += ptsChild(td[x][y][z], x^1, y^1, z^1);
	}
	if (pts == 0)
		return;

	// get children (in standard orientation)
	TraversalData ch[2][2][2];
	for (int i = 0; i < 8; i++)
	{
		const int x = i&1;
		const int y = (i>>1)&1;
		const int z = (i>>2)&1;
		genNodeChild(ch[x][y][z], td[x][y][z], x^1, y^1, z^1);
	}

	// recurse
	traverseVertex(v, ch);
}


void connect_neighbors_first(int from, int to)
{
	// connect neighbors
	TraversalNeighborGen trav;

	ConnectNeighborsVisitor visitora(0, streaminfo.cells_total);
	visitora.maxDepth = 100;
	trav.traverse(g.tree, &visitora);
}


void connect_neighbors_second(int from, int to)
{
	// connect neighbors
	TraversalNeighborGen trav;
	
	ConnectNeighborsVisitor visitor(from, to);
	visitor.maxDepth = 100;
	trav.traverse(g.tree, &visitor);
	
	ConnectNeighborsVisitor visitorb(to, to);
	visitorb.maxDepth = 100;
	trav.traverse(g.tree, &visitorb);
}


