#include "common.h"

StreamingVisitor::StreamingVisitor(int low, int high)
{
	z_low = low;
	z_high = high;
}

StreamingVisitor::StreamingVisitor(int z_border)
{
	z_high = z_low = z_border;
}

TraversalData *StreamingVisitor::minSize(TraversalData *a, TraversalData *b)
{
	if (a->cellsize < b->cellsize)
		return a;
	else
		return b;
}

bool StreamingVisitor::insideOrCrossing(int zl, int zm, int zh)
{
	if (z_low < z_high) // slab
		return zl < z_high && zh > z_low && (zl == zm || (zm > z_low && zm < z_high)); 
	else // border
		return zl < z_high && z_low < zh;
}

bool StreamingVisitor::onNode(TraversalData &td)
{
	return insideOrCrossing(td.z, td.z, td.z + td.cellsize);
}

bool StreamingVisitor::onFace(TraversalData td[2], int orientation)
{
	if (orientation == 2)
		return insideOrCrossing(td[0].z, td[1].z, td[1].z + td[1].cellsize);
	else
	{
		TraversalData *m = minSize(&td[0], &td[1]);
		return insideOrCrossing(m->z, m->z, m->z + m->cellsize);
	}
}

bool StreamingVisitor::onEdge(TraversalData td[2][2], int orientation)
{
	if (orientation == 2)
	{
		TraversalData *m = minSize( minSize(&td[0][0], &td[1][0]), minSize(&td[0][1], &td[1][1]) );
		return insideOrCrossing(m->z, m->z, m->z + m->cellsize);
	}
	else
	{
		TraversalData *l = minSize(&td[0][0], &td[1][0]);
		TraversalData *h = minSize(&td[0][1], &td[1][1]);
		return insideOrCrossing(l->z, h->z, h->z + h->cellsize);
	}
}

bool StreamingVisitor::onVertex(TraversalData td[2][2][2])
{
	TraversalData *l = minSize( minSize(&td[0][0][0], &td[1][0][0]), minSize(&td[0][1][0], &td[1][1][0]) );
	TraversalData *h = minSize( minSize(&td[0][0][1], &td[1][0][1]), minSize(&td[0][1][1], &td[1][1][1]) );
	return insideOrCrossing(l->z, h->z, h->z + h->cellsize);
}
