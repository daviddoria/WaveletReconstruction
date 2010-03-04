#include "global.h"

const int prune_threshold = 2;

struct PruneMarkVisitor : public StreamingVisitor
{
	PruneMarkVisitor(int low, int high) : StreamingVisitor(low, high) {}
	PruneMarkVisitor(int z_border) : StreamingVisitor(z_border) {}

	bool onVertex(TraversalData td[2][2][2]);
	bool onEdge(TraversalData td[2][2], int orientation);
	bool onFace(TraversalData td[2], int orientation);
	bool onNode(TraversalData &td);
};

bool PruneMarkVisitor::onNode(TraversalData &td)
{
	if (!StreamingVisitor::onNode(td))
		return false;

	if (td.pt_num > prune_threshold && !td.isCopy)
		if (!td.isCopy)
			(*td.neighborCells) += 26;

	return true;
}

bool PruneMarkVisitor::onFace(TraversalData td[2], int orientation) 
{
	if (!StreamingVisitor::onFace(td, orientation))
		return false;

	bool rvalue = false;

	if (td[0].pt_num >= prune_threshold && td[1].pt_num >= prune_threshold && !td[0].isCopy && !td[1].isCopy)
	{
		(*td[0].neighborCells)++;
		(*td[1].neighborCells)++;
		rvalue = true;
	}

	return rvalue;
}

bool PruneMarkVisitor::onEdge(TraversalData td[2][2], int orientation) 
{
	if (!StreamingVisitor::onEdge(td, orientation))
		return false;

	bool rvalue = false;
	
	if (td[0][0].pt_num >= prune_threshold && td[1][1].pt_num >= prune_threshold && !td[0][0].isCopy && !td[1][1].isCopy)
	{
		(*td[0][0].neighborCells)++;
		(*td[1][1].neighborCells)++;
		rvalue = true;
	}
	if (td[0][1].pt_num >= prune_threshold && td[1][0].pt_num >= prune_threshold && !td[0][1].isCopy && !td[1][0].isCopy)
	{
		(*td[0][1].neighborCells)++;
		(*td[1][0].neighborCells)++;
		rvalue = true;
	}

	return rvalue;
}

bool PruneMarkVisitor::onVertex(TraversalData td[2][2][2]) 
{
	if (!StreamingVisitor::onVertex(td))
		return false;

	bool rvalue = false;

	if (td[0][0][0].pt_num >= prune_threshold && td[1][1][1].pt_num >= prune_threshold && !td[0][0][0].isCopy && !td[1][1][1].isCopy)
	{
		(*td[0][0][0].neighborCells)++;
		(*td[1][1][1].neighborCells)++;
		rvalue = true;
	}
	if (td[0][0][1].pt_num >= prune_threshold && td[1][1][0].pt_num >= prune_threshold && !td[0][0][1].isCopy && !td[1][1][0].isCopy)
	{
		(*td[0][0][1].neighborCells)++;
		(*td[1][1][0].neighborCells)++;
		rvalue = true;
	}
	if (td[0][1][0].pt_num >= prune_threshold && td[1][0][1].pt_num >= prune_threshold && !td[0][1][0].isCopy && !td[1][0][1].isCopy)
	{
		(*td[0][1][0].neighborCells)++;
		(*td[1][0][1].neighborCells)++;
		rvalue = true;
	}
	if (td[0][1][1].pt_num >= prune_threshold && td[1][0][0].pt_num >= prune_threshold && !td[0][1][1].isCopy && !td[1][0][0].isCopy)
	{
		(*td[0][1][1].neighborCells)++;
		(*td[1][0][0].neighborCells)++;
		rvalue = true;
	}

	return rvalue;
}


int pruned = 0;
int prune_depth = -1;
bool OctNode::prune(int from, int to, int minz, int extz)
{
	bool remove = true;

	if (g.do_prune)
	{
		prune_depth++;
		for (int z = 0; z < 2; z++)
		{
			int extz2 = extz >> 1;
			int minz2 = minz + z*extz2;
			if (minz2 >= to || minz2 + extz2 <= from)
				continue;

			for (int x = 0; x < 2; x++)
			{
				for (int y = 0; y < 2; y++)
				{
					// don't remove if artificial
					if (g.do_pts_node)
					{
						if (pts_node() == 0)
							remove = false;
					}
					else
					{
						if (pt_num(0,0,0) + pt_num(0,0,1) + pt_num(0,1,0) + pt_num(0,1,1) +
							pt_num(1,0,0) + pt_num(1,0,1) + pt_num(1,1,0) + pt_num(1,1,1) == 0)
							remove = false;
					}

					// don't remove if enough neighbors
					if (neighbor_cells(x,y,z) >= 3)
						remove = false;

					if (children(x,y,z).data)
					{
						bool remove_it = children(x,y,z).prune(from, to, minz2, extz2);
						if (remove_it && prune_depth >= g.maxDepthMem)
						{
							pruned++;
							children(x,y,z).detachFromNeighbors();
							children(x,y,z).clear();
							//children(x,y,z).data = 0;
						}
						else
							remove = false;
					}
				}
			}
		}

		prune_depth--;
	}

	return remove;
}

void prune(int from, int to)                                                                            
{
	// prune
	Traversal trav; 
	PruneMarkVisitor visitor(from, to);
	trav.traverse(g.tree, &visitor);

	PruneMarkVisitor visitor_border(to);
	trav.traverse(g.tree, &visitor_border);
	
	g.tree.root.prune(from, to, 0, streaminfo.cells_total);
}
