#include "global.h"
#include <cstdio>

struct MCVisitor : public StreamingVisitor
{
	FILE *fptr;

	MCVisitor(int low, int high, FILE *out) : StreamingVisitor(low, high) { fptr = out;}
	MCVisitor(int z_border, FILE *out) : StreamingVisitor(z_border) { fptr = out;}

	virtual bool onNode (TraversalData &td);
	virtual bool onVertex(TraversalData td[2][2][2]);
};

struct MCEntry
{
	vector<vect3ub> tris;

	MCEntry()
	{
	}
	MCEntry(vect3ub a)
	{
		tris.push_back(a);
	}
	MCEntry(vect3ub a, vect3ub b)
	{
		tris.push_back(a);
		tris.push_back(b);
	}
	MCEntry(vect3ub a, vect3ub b, vect3ub c)
	{
		tris.push_back(a);
		tris.push_back(b);
		tris.push_back(c);
	}
	MCEntry(vect3ub a, vect3ub b, vect3ub c, vect3ub d)
	{
		tris.push_back(a);
		tris.push_back(b);
		tris.push_back(c);
		tris.push_back(d);
	}
	MCEntry(vect3ub a, vect3ub b, vect3ub c, vect3ub d, vect3ub e)
	{
		tris.push_back(a);
		tris.push_back(b);
		tris.push_back(c);
		tris.push_back(d);
		tris.push_back(e);
	}
};

vector<MCEntry> mctable;

void initMCTable()
{
	mctable.clear();

	mctable.push_back(MCEntry());
	mctable.push_back(MCEntry(vect3ub(0,1,4)));
	mctable.push_back(MCEntry(vect3ub(2,0,5)));
	mctable.push_back(MCEntry(vect3ub(2,1,4), vect3ub(4,5,2)));
	mctable.push_back(MCEntry(vect3ub(1,3,6)));
	mctable.push_back(MCEntry(vect3ub(0,3,6), vect3ub(6,4,0)));
	mctable.push_back(MCEntry(vect3ub(2,3,6), vect3ub(6,1,0), vect3ub(0,5,2), vect3ub(2,6,0)));
	mctable.push_back(MCEntry(vect3ub(2,3,6), vect3ub(6,4,5), vect3ub(5,2,6)));
	mctable.push_back(MCEntry(vect3ub(3,2,7)));
	mctable.push_back(MCEntry(vect3ub(0,2,7), vect3ub(7,3,1), vect3ub(1,4,0), vect3ub(0,7,1)));
	mctable.push_back(MCEntry(vect3ub(3,0,5), vect3ub(5,7,3)));
	mctable.push_back(MCEntry(vect3ub(3,1,4), vect3ub(4,5,7), vect3ub(7,3,4)));
	mctable.push_back(MCEntry(vect3ub(1,2,7), vect3ub(7,6,1)));
	mctable.push_back(MCEntry(vect3ub(0,2,7), vect3ub(7,6,4), vect3ub(4,0,7)));
	mctable.push_back(MCEntry(vect3ub(1,0,5), vect3ub(5,7,6), vect3ub(6,1,5)));
	mctable.push_back(MCEntry(vect3ub(4,5,7), vect3ub(7,6,4)));
	mctable.push_back(MCEntry(vect3ub(9,8,4)));
	mctable.push_back(MCEntry(vect3ub(0,1,9), vect3ub(9,8,0)));
	mctable.push_back(MCEntry(vect3ub(2,0,4), vect3ub(4,9,8), vect3ub(8,5,2), vect3ub(2,4,8)));
	mctable.push_back(MCEntry(vect3ub(2,1,9), vect3ub(9,8,5), vect3ub(5,2,9)));
	mctable.push_back(MCEntry(vect3ub(1,3,6), vect3ub(6,9,8), vect3ub(8,4,1), vect3ub(1,6,8)));
	mctable.push_back(MCEntry(vect3ub(0,3,6), vect3ub(6,9,8), vect3ub(8,0,6)));
	mctable.push_back(MCEntry(vect3ub(2,3,6), vect3ub(6,9,8), vect3ub(8,5,2), vect3ub(2,6,8), vect3ub(1,0,4)));
	mctable.push_back(MCEntry(vect3ub(2,3,6), vect3ub(6,9,8), vect3ub(8,5,2), vect3ub(2,6,8)));
	mctable.push_back(MCEntry(vect3ub(3,2,7), vect3ub(9,8,4)));
	mctable.push_back(MCEntry(vect3ub(0,2,7), vect3ub(7,3,1), vect3ub(1,9,8), vect3ub(8,0,7), vect3ub(7,1,8)));
	mctable.push_back(MCEntry(vect3ub(3,0,4), vect3ub(4,9,8), vect3ub(8,5,7), vect3ub(7,3,4), vect3ub(4,8,7)));
	mctable.push_back(MCEntry(vect3ub(3,1,9), vect3ub(9,8,5), vect3ub(5,7,3), vect3ub(3,9,5)));
	mctable.push_back(MCEntry(vect3ub(1,2,7), vect3ub(7,6,9), vect3ub(9,8,4), vect3ub(4,1,7), vect3ub(7,9,4)));
	mctable.push_back(MCEntry(vect3ub(0,2,7), vect3ub(7,6,9), vect3ub(9,8,0), vect3ub(0,7,9)));
	mctable.push_back(MCEntry(vect3ub(1,0,4), vect3ub(9,8,5), vect3ub(5,7,6), vect3ub(6,9,5)));
	mctable.push_back(MCEntry(vect3ub(9,8,5), vect3ub(5,7,6), vect3ub(6,9,5)));
	mctable.push_back(MCEntry(vect3ub(8,10,5)));
	mctable.push_back(MCEntry(vect3ub(0,1,4), vect3ub(4,8,10), vect3ub(10,5,0), vect3ub(0,4,10)));
	mctable.push_back(MCEntry(vect3ub(2,0,8), vect3ub(8,10,2)));
	mctable.push_back(MCEntry(vect3ub(2,1,4), vect3ub(4,8,10), vect3ub(10,2,4)));
	mctable.push_back(MCEntry(vect3ub(1,3,6), vect3ub(8,10,5)));
	mctable.push_back(MCEntry(vect3ub(0,3,6), vect3ub(6,4,8), vect3ub(8,10,5), vect3ub(5,0,6), vect3ub(6,8,5)));
	mctable.push_back(MCEntry(vect3ub(2,3,6), vect3ub(6,1,0), vect3ub(0,8,10), vect3ub(10,2,6), vect3ub(6,0,10)));
	mctable.push_back(MCEntry(vect3ub(2,3,6), vect3ub(6,4,8), vect3ub(8,10,2), vect3ub(2,6,8)));
	mctable.push_back(MCEntry(vect3ub(3,2,5), vect3ub(5,8,10), vect3ub(10,7,3), vect3ub(3,5,10)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(3,1,4), vect3ub(4,8,10), vect3ub(10,7,3), vect3ub(3,4,10)));
	mctable.push_back(MCEntry(vect3ub(3,0,8), vect3ub(8,10,7), vect3ub(7,3,8)));
	mctable.push_back(MCEntry(vect3ub(3,1,4), vect3ub(4,8,10), vect3ub(10,7,3), vect3ub(3,4,10)));
	mctable.push_back(MCEntry(vect3ub(1,2,5), vect3ub(5,8,10), vect3ub(10,7,6), vect3ub(6,1,5), vect3ub(5,10,6)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(8,10,7), vect3ub(7,6,4), vect3ub(4,8,7)));
	mctable.push_back(MCEntry(vect3ub(1,0,8), vect3ub(8,10,7), vect3ub(7,6,1), vect3ub(1,8,7)));
	mctable.push_back(MCEntry(vect3ub(8,10,7), vect3ub(7,6,4), vect3ub(4,8,7)));
	mctable.push_back(MCEntry(vect3ub(9,10,5), vect3ub(5,4,9)));
	mctable.push_back(MCEntry(vect3ub(0,1,9), vect3ub(9,10,5), vect3ub(5,0,9)));
	mctable.push_back(MCEntry(vect3ub(2,0,4), vect3ub(4,9,10), vect3ub(10,2,4)));
	mctable.push_back(MCEntry(vect3ub(2,1,9), vect3ub(9,10,2)));
	mctable.push_back(MCEntry(vect3ub(1,3,6), vect3ub(6,9,10), vect3ub(10,5,4), vect3ub(4,1,6), vect3ub(6,10,4)));
	mctable.push_back(MCEntry(vect3ub(0,3,6), vect3ub(6,9,10), vect3ub(10,5,0), vect3ub(0,6,10)));
	mctable.push_back(MCEntry(vect3ub(2,3,6), vect3ub(6,9,10), vect3ub(10,2,6), vect3ub(1,0,4)));
	mctable.push_back(MCEntry(vect3ub(2,3,6), vect3ub(6,9,10), vect3ub(10,2,6)));
	mctable.push_back(MCEntry(vect3ub(3,2,5), vect3ub(5,4,9), vect3ub(9,10,7), vect3ub(7,3,5), vect3ub(5,9,7)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(3,1,9), vect3ub(9,10,7), vect3ub(7,3,9)));
	mctable.push_back(MCEntry(vect3ub(3,0,4), vect3ub(4,9,10), vect3ub(10,7,3), vect3ub(3,4,10)));
	mctable.push_back(MCEntry(vect3ub(3,1,9), vect3ub(9,10,7), vect3ub(7,3,9)));
	mctable.push_back(MCEntry(vect3ub(1,2,5), vect3ub(5,4,1), vect3ub(9,10,7), vect3ub(7,6,9)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(9,10,7), vect3ub(7,6,9)));
	mctable.push_back(MCEntry(vect3ub(1,0,4), vect3ub(9,10,7), vect3ub(7,6,9)));
	mctable.push_back(MCEntry(vect3ub(9,10,7), vect3ub(7,6,9)));
	mctable.push_back(MCEntry(vect3ub(11,9,6)));
	mctable.push_back(MCEntry(vect3ub(0,1,6), vect3ub(6,11,9), vect3ub(9,4,0), vect3ub(0,6,9)));
	mctable.push_back(MCEntry(vect3ub(2,0,5), vect3ub(11,9,6)));
	mctable.push_back(MCEntry(vect3ub(2,1,6), vect3ub(6,11,9), vect3ub(9,4,5), vect3ub(5,2,6), vect3ub(6,9,5)));
	mctable.push_back(MCEntry(vect3ub(1,3,11), vect3ub(11,9,1)));
	mctable.push_back(MCEntry(vect3ub(0,3,11), vect3ub(11,9,4), vect3ub(4,0,11)));
	mctable.push_back(MCEntry(vect3ub(2,3,11), vect3ub(11,9,1), vect3ub(1,0,5), vect3ub(5,2,11), vect3ub(11,1,5)));
	mctable.push_back(MCEntry(vect3ub(2,3,11), vect3ub(11,9,4), vect3ub(4,5,2), vect3ub(2,11,4)));
	mctable.push_back(MCEntry(vect3ub(3,2,7), vect3ub(7,11,9), vect3ub(9,6,3), vect3ub(3,7,9)));
	mctable.push_back(MCEntry(vect3ub(0,2,7), vect3ub(7,11,9), vect3ub(9,4,0), vect3ub(0,7,9), vect3ub(3,1,6)));
	mctable.push_back(MCEntry(vect3ub(3,0,5), vect3ub(5,7,11), vect3ub(11,9,6), vect3ub(6,3,5), vect3ub(5,11,6)));
	mctable.push_back(MCEntry(vect3ub(3,1,6), vect3ub(11,9,4), vect3ub(4,5,7), vect3ub(7,11,4)));
	mctable.push_back(MCEntry(vect3ub(1,2,7), vect3ub(7,11,9), vect3ub(9,1,7)));
	mctable.push_back(MCEntry(vect3ub(0,2,7), vect3ub(7,11,9), vect3ub(9,4,0), vect3ub(0,7,9)));
	mctable.push_back(MCEntry(vect3ub(1,0,5), vect3ub(5,7,11), vect3ub(11,9,1), vect3ub(1,5,11)));
	mctable.push_back(MCEntry(vect3ub(11,9,4), vect3ub(4,5,7), vect3ub(7,11,4)));
	mctable.push_back(MCEntry(vect3ub(11,8,4), vect3ub(4,6,11)));
	mctable.push_back(MCEntry(vect3ub(0,1,6), vect3ub(6,11,8), vect3ub(8,0,6)));
	mctable.push_back(MCEntry(vect3ub(2,0,4), vect3ub(4,6,11), vect3ub(11,8,5), vect3ub(5,2,4), vect3ub(4,11,5)));
	mctable.push_back(MCEntry(vect3ub(2,1,6), vect3ub(6,11,8), vect3ub(8,5,2), vect3ub(2,6,8)));
	mctable.push_back(MCEntry(vect3ub(1,3,11), vect3ub(11,8,4), vect3ub(4,1,11)));
	mctable.push_back(MCEntry(vect3ub(0,3,11), vect3ub(11,8,0)));
	mctable.push_back(MCEntry(vect3ub(2,3,11), vect3ub(11,8,5), vect3ub(5,2,11), vect3ub(1,0,4)));
	mctable.push_back(MCEntry(vect3ub(2,3,11), vect3ub(11,8,5), vect3ub(5,2,11)));
	mctable.push_back(MCEntry(vect3ub(3,2,7), vect3ub(7,11,8), vect3ub(8,4,6), vect3ub(6,3,7), vect3ub(7,8,6)));
	mctable.push_back(MCEntry(vect3ub(0,2,7), vect3ub(7,11,8), vect3ub(8,0,7), vect3ub(3,1,6)));
	mctable.push_back(MCEntry(vect3ub(3,0,4), vect3ub(4,6,3), vect3ub(11,8,5), vect3ub(5,7,11)));
	mctable.push_back(MCEntry(vect3ub(3,1,6), vect3ub(11,8,5), vect3ub(5,7,11)));
	mctable.push_back(MCEntry(vect3ub(1,2,7), vect3ub(7,11,8), vect3ub(8,4,1), vect3ub(1,7,8)));
	mctable.push_back(MCEntry(vect3ub(0,2,7), vect3ub(7,11,8), vect3ub(8,0,7)));
	mctable.push_back(MCEntry(vect3ub(1,0,4), vect3ub(11,8,5), vect3ub(5,7,11)));
	mctable.push_back(MCEntry(vect3ub(11,8,5), vect3ub(5,7,11)));
	mctable.push_back(MCEntry(vect3ub(11,10,5), vect3ub(5,8,9), vect3ub(9,6,11), vect3ub(11,5,9)));
	mctable.push_back(MCEntry(vect3ub(0,1,6), vect3ub(6,11,10), vect3ub(10,5,0), vect3ub(0,6,10), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(2,0,8), vect3ub(8,9,6), vect3ub(6,11,10), vect3ub(10,2,8), vect3ub(8,6,10)));
	mctable.push_back(MCEntry(vect3ub(2,1,6), vect3ub(6,11,10), vect3ub(10,2,6), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(1,3,11), vect3ub(11,10,5), vect3ub(5,8,9), vect3ub(9,1,11), vect3ub(11,5,9)));
	mctable.push_back(MCEntry(vect3ub(0,3,11), vect3ub(11,10,5), vect3ub(5,0,11), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(2,3,11), vect3ub(11,10,2), vect3ub(1,0,8), vect3ub(8,9,1)));
	mctable.push_back(MCEntry(vect3ub(2,3,11), vect3ub(11,10,2), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(3,2,5), vect3ub(5,8,9), vect3ub(9,6,3), vect3ub(3,5,9), vect3ub(11,10,7)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(3,1,6), vect3ub(11,10,7), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(3,0,8), vect3ub(8,9,6), vect3ub(6,3,8), vect3ub(11,10,7)));
	mctable.push_back(MCEntry(vect3ub(3,1,6), vect3ub(11,10,7), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(1,2,5), vect3ub(5,8,9), vect3ub(9,1,5), vect3ub(11,10,7)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(11,10,7), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(1,0,8), vect3ub(8,9,1), vect3ub(11,10,7)));
	mctable.push_back(MCEntry(vect3ub(11,10,7), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(11,10,5), vect3ub(5,4,6), vect3ub(6,11,5)));
	mctable.push_back(MCEntry(vect3ub(0,1,6), vect3ub(6,11,10), vect3ub(10,5,0), vect3ub(0,6,10)));
	mctable.push_back(MCEntry(vect3ub(2,0,4), vect3ub(4,6,11), vect3ub(11,10,2), vect3ub(2,4,11)));
	mctable.push_back(MCEntry(vect3ub(2,1,6), vect3ub(6,11,10), vect3ub(10,2,6)));
	mctable.push_back(MCEntry(vect3ub(1,3,11), vect3ub(11,10,5), vect3ub(5,4,1), vect3ub(1,11,5)));
	mctable.push_back(MCEntry(vect3ub(0,3,11), vect3ub(11,10,5), vect3ub(5,0,11)));
	mctable.push_back(MCEntry(vect3ub(2,3,11), vect3ub(11,10,2), vect3ub(1,0,4)));
	mctable.push_back(MCEntry(vect3ub(2,3,11), vect3ub(11,10,2)));
	mctable.push_back(MCEntry(vect3ub(3,2,5), vect3ub(5,4,6), vect3ub(6,3,5), vect3ub(11,10,7)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(3,1,6), vect3ub(11,10,7)));
	mctable.push_back(MCEntry(vect3ub(3,0,4), vect3ub(4,6,3), vect3ub(11,10,7)));
	mctable.push_back(MCEntry(vect3ub(3,1,6), vect3ub(11,10,7)));
	mctable.push_back(MCEntry(vect3ub(1,2,5), vect3ub(5,4,1), vect3ub(11,10,7)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(11,10,7)));
	mctable.push_back(MCEntry(vect3ub(1,0,4), vect3ub(11,10,7)));
	mctable.push_back(MCEntry(vect3ub(11,10,7)));
	mctable.push_back(MCEntry(vect3ub(10,11,7)));
	mctable.push_back(MCEntry(vect3ub(0,1,4), vect3ub(10,11,7)));
	mctable.push_back(MCEntry(vect3ub(2,0,5), vect3ub(5,10,11), vect3ub(11,7,2), vect3ub(2,5,11)));
	mctable.push_back(MCEntry(vect3ub(2,1,4), vect3ub(4,5,10), vect3ub(10,11,7), vect3ub(7,2,4), vect3ub(4,10,7)));
	mctable.push_back(MCEntry(vect3ub(1,3,7), vect3ub(7,10,11), vect3ub(11,6,1), vect3ub(1,7,11)));
	mctable.push_back(MCEntry(vect3ub(0,3,7), vect3ub(7,10,11), vect3ub(11,6,4), vect3ub(4,0,7), vect3ub(7,11,4)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(1,0,5), vect3ub(5,10,11), vect3ub(11,6,1), vect3ub(1,5,11)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(10,11,6), vect3ub(6,4,5), vect3ub(5,10,6)));
	mctable.push_back(MCEntry(vect3ub(3,2,10), vect3ub(10,11,3)));
	mctable.push_back(MCEntry(vect3ub(0,2,10), vect3ub(10,11,3), vect3ub(3,1,4), vect3ub(4,0,10), vect3ub(10,3,4)));
	mctable.push_back(MCEntry(vect3ub(3,0,5), vect3ub(5,10,11), vect3ub(11,3,5)));
	mctable.push_back(MCEntry(vect3ub(3,1,4), vect3ub(4,5,10), vect3ub(10,11,3), vect3ub(3,4,10)));
	mctable.push_back(MCEntry(vect3ub(1,2,10), vect3ub(10,11,6), vect3ub(6,1,10)));
	mctable.push_back(MCEntry(vect3ub(0,2,10), vect3ub(10,11,6), vect3ub(6,4,0), vect3ub(0,10,6)));
	mctable.push_back(MCEntry(vect3ub(1,0,5), vect3ub(5,10,11), vect3ub(11,6,1), vect3ub(1,5,11)));
	mctable.push_back(MCEntry(vect3ub(10,11,6), vect3ub(6,4,5), vect3ub(5,10,6)));
	mctable.push_back(MCEntry(vect3ub(9,11,7), vect3ub(7,10,8), vect3ub(8,4,9), vect3ub(9,7,8)));
	mctable.push_back(MCEntry(vect3ub(0,1,9), vect3ub(9,11,7), vect3ub(7,10,8), vect3ub(8,0,9), vect3ub(9,7,8)));
	mctable.push_back(MCEntry(vect3ub(2,0,4), vect3ub(4,9,11), vect3ub(11,7,2), vect3ub(2,4,11), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(2,1,9), vect3ub(9,11,7), vect3ub(7,2,9), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(1,3,7), vect3ub(7,10,8), vect3ub(8,4,1), vect3ub(1,7,8), vect3ub(9,11,6)));
	mctable.push_back(MCEntry(vect3ub(0,3,7), vect3ub(7,10,8), vect3ub(8,0,7), vect3ub(9,11,6)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(1,0,4), vect3ub(9,11,6), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(9,11,6), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(3,2,10), vect3ub(10,8,4), vect3ub(4,9,11), vect3ub(11,3,10), vect3ub(10,4,11)));
	mctable.push_back(MCEntry(vect3ub(0,2,10), vect3ub(10,8,0), vect3ub(3,1,9), vect3ub(9,11,3)));
	mctable.push_back(MCEntry(vect3ub(3,0,4), vect3ub(4,9,11), vect3ub(11,3,4), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(3,1,9), vect3ub(9,11,3), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(1,2,10), vect3ub(10,8,4), vect3ub(4,1,10), vect3ub(9,11,6)));
	mctable.push_back(MCEntry(vect3ub(0,2,10), vect3ub(10,8,0), vect3ub(9,11,6)));
	mctable.push_back(MCEntry(vect3ub(1,0,4), vect3ub(9,11,6), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(9,11,6), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(8,11,7), vect3ub(7,5,8)));
	mctable.push_back(MCEntry(vect3ub(0,1,4), vect3ub(4,8,11), vect3ub(11,7,5), vect3ub(5,0,4), vect3ub(4,11,5)));
	mctable.push_back(MCEntry(vect3ub(2,0,8), vect3ub(8,11,7), vect3ub(7,2,8)));
	mctable.push_back(MCEntry(vect3ub(2,1,4), vect3ub(4,8,11), vect3ub(11,7,2), vect3ub(2,4,11)));
	mctable.push_back(MCEntry(vect3ub(1,3,7), vect3ub(7,5,8), vect3ub(8,11,6), vect3ub(6,1,7), vect3ub(7,8,6)));
	mctable.push_back(MCEntry(vect3ub(0,3,7), vect3ub(7,5,0), vect3ub(8,11,6), vect3ub(6,4,8)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(1,0,8), vect3ub(8,11,6), vect3ub(6,1,8)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(8,11,6), vect3ub(6,4,8)));
	mctable.push_back(MCEntry(vect3ub(3,2,5), vect3ub(5,8,11), vect3ub(11,3,5)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(3,1,4), vect3ub(4,8,11), vect3ub(11,3,4)));
	mctable.push_back(MCEntry(vect3ub(3,0,8), vect3ub(8,11,3)));
	mctable.push_back(MCEntry(vect3ub(3,1,4), vect3ub(4,8,11), vect3ub(11,3,4)));
	mctable.push_back(MCEntry(vect3ub(1,2,5), vect3ub(5,8,11), vect3ub(11,6,1), vect3ub(1,5,11)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(8,11,6), vect3ub(6,4,8)));
	mctable.push_back(MCEntry(vect3ub(1,0,8), vect3ub(8,11,6), vect3ub(6,1,8)));
	mctable.push_back(MCEntry(vect3ub(8,11,6), vect3ub(6,4,8)));
	mctable.push_back(MCEntry(vect3ub(9,11,7), vect3ub(7,5,4), vect3ub(4,9,7)));
	mctable.push_back(MCEntry(vect3ub(0,1,9), vect3ub(9,11,7), vect3ub(7,5,0), vect3ub(0,9,7)));
	mctable.push_back(MCEntry(vect3ub(2,0,4), vect3ub(4,9,11), vect3ub(11,7,2), vect3ub(2,4,11)));
	mctable.push_back(MCEntry(vect3ub(2,1,9), vect3ub(9,11,7), vect3ub(7,2,9)));
	mctable.push_back(MCEntry(vect3ub(1,3,7), vect3ub(7,5,4), vect3ub(4,1,7), vect3ub(9,11,6)));
	mctable.push_back(MCEntry(vect3ub(0,3,7), vect3ub(7,5,0), vect3ub(9,11,6)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(1,0,4), vect3ub(9,11,6)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(9,11,6)));
	mctable.push_back(MCEntry(vect3ub(3,2,5), vect3ub(5,4,9), vect3ub(9,11,3), vect3ub(3,5,9)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(3,1,9), vect3ub(9,11,3)));
	mctable.push_back(MCEntry(vect3ub(3,0,4), vect3ub(4,9,11), vect3ub(11,3,4)));
	mctable.push_back(MCEntry(vect3ub(3,1,9), vect3ub(9,11,3)));
	mctable.push_back(MCEntry(vect3ub(1,2,5), vect3ub(5,4,1), vect3ub(9,11,6)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(9,11,6)));
	mctable.push_back(MCEntry(vect3ub(1,0,4), vect3ub(9,11,6)));
	mctable.push_back(MCEntry(vect3ub(9,11,6)));
	mctable.push_back(MCEntry(vect3ub(10,9,6), vect3ub(6,7,10)));
	mctable.push_back(MCEntry(vect3ub(0,1,6), vect3ub(6,7,10), vect3ub(10,9,4), vect3ub(4,0,6), vect3ub(6,10,4)));
	mctable.push_back(MCEntry(vect3ub(2,0,5), vect3ub(5,10,9), vect3ub(9,6,7), vect3ub(7,2,5), vect3ub(5,9,7)));
	mctable.push_back(MCEntry(vect3ub(2,1,6), vect3ub(6,7,2), vect3ub(10,9,4), vect3ub(4,5,10)));
	mctable.push_back(MCEntry(vect3ub(1,3,7), vect3ub(7,10,9), vect3ub(9,1,7)));
	mctable.push_back(MCEntry(vect3ub(0,3,7), vect3ub(7,10,9), vect3ub(9,4,0), vect3ub(0,7,9)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(1,0,5), vect3ub(5,10,9), vect3ub(9,1,5)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(10,9,4), vect3ub(4,5,10)));
	mctable.push_back(MCEntry(vect3ub(3,2,10), vect3ub(10,9,6), vect3ub(6,3,10)));
	mctable.push_back(MCEntry(vect3ub(0,2,10), vect3ub(10,9,4), vect3ub(4,0,10), vect3ub(3,1,6)));
	mctable.push_back(MCEntry(vect3ub(3,0,5), vect3ub(5,10,9), vect3ub(9,6,3), vect3ub(3,5,9)));
	mctable.push_back(MCEntry(vect3ub(3,1,6), vect3ub(10,9,4), vect3ub(4,5,10)));
	mctable.push_back(MCEntry(vect3ub(1,2,10), vect3ub(10,9,1)));
	mctable.push_back(MCEntry(vect3ub(0,2,10), vect3ub(10,9,4), vect3ub(4,0,10)));
	mctable.push_back(MCEntry(vect3ub(1,0,5), vect3ub(5,10,9), vect3ub(9,1,5)));
	mctable.push_back(MCEntry(vect3ub(10,9,4), vect3ub(4,5,10)));
	mctable.push_back(MCEntry(vect3ub(10,8,4), vect3ub(4,6,7), vect3ub(7,10,4)));
	mctable.push_back(MCEntry(vect3ub(0,1,6), vect3ub(6,7,10), vect3ub(10,8,0), vect3ub(0,6,10)));
	mctable.push_back(MCEntry(vect3ub(2,0,4), vect3ub(4,6,7), vect3ub(7,2,4), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(2,1,6), vect3ub(6,7,2), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(1,3,7), vect3ub(7,10,8), vect3ub(8,4,1), vect3ub(1,7,8)));
	mctable.push_back(MCEntry(vect3ub(0,3,7), vect3ub(7,10,8), vect3ub(8,0,7)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(1,0,4), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(3,2,10), vect3ub(10,8,4), vect3ub(4,6,3), vect3ub(3,10,4)));
	mctable.push_back(MCEntry(vect3ub(0,2,10), vect3ub(10,8,0), vect3ub(3,1,6)));
	mctable.push_back(MCEntry(vect3ub(3,0,4), vect3ub(4,6,3), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(3,1,6), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(1,2,10), vect3ub(10,8,4), vect3ub(4,1,10)));
	mctable.push_back(MCEntry(vect3ub(0,2,10), vect3ub(10,8,0)));
	mctable.push_back(MCEntry(vect3ub(1,0,4), vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(10,8,5)));
	mctable.push_back(MCEntry(vect3ub(8,9,6), vect3ub(6,7,5), vect3ub(5,8,6)));
	mctable.push_back(MCEntry(vect3ub(0,1,6), vect3ub(6,7,5), vect3ub(5,0,6), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(2,0,8), vect3ub(8,9,6), vect3ub(6,7,2), vect3ub(2,8,6)));
	mctable.push_back(MCEntry(vect3ub(2,1,6), vect3ub(6,7,2), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(1,3,7), vect3ub(7,5,8), vect3ub(8,9,1), vect3ub(1,7,8)));
	mctable.push_back(MCEntry(vect3ub(0,3,7), vect3ub(7,5,0), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(1,0,8), vect3ub(8,9,1)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(3,2,5), vect3ub(5,8,9), vect3ub(9,6,3), vect3ub(3,5,9)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(3,1,6), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(3,0,8), vect3ub(8,9,6), vect3ub(6,3,8)));
	mctable.push_back(MCEntry(vect3ub(3,1,6), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(1,2,5), vect3ub(5,8,9), vect3ub(9,1,5)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(1,0,8), vect3ub(8,9,1)));
	mctable.push_back(MCEntry(vect3ub(8,9,4)));
	mctable.push_back(MCEntry(vect3ub(5,4,6), vect3ub(6,7,5)));
	mctable.push_back(MCEntry(vect3ub(0,1,6), vect3ub(6,7,5), vect3ub(5,0,6)));
	mctable.push_back(MCEntry(vect3ub(2,0,4), vect3ub(4,6,7), vect3ub(7,2,4)));
	mctable.push_back(MCEntry(vect3ub(2,1,6), vect3ub(6,7,2)));
	mctable.push_back(MCEntry(vect3ub(1,3,7), vect3ub(7,5,4), vect3ub(4,1,7)));
	mctable.push_back(MCEntry(vect3ub(0,3,7), vect3ub(7,5,0)));
	mctable.push_back(MCEntry(vect3ub(2,3,7), vect3ub(1,0,4)));
	mctable.push_back(MCEntry(vect3ub(2,3,7)));
	mctable.push_back(MCEntry(vect3ub(3,2,5), vect3ub(5,4,6), vect3ub(6,3,5)));
	mctable.push_back(MCEntry(vect3ub(0,2,5), vect3ub(3,1,6)));
	mctable.push_back(MCEntry(vect3ub(3,0,4), vect3ub(4,6,3)));
	mctable.push_back(MCEntry(vect3ub(3,1,6)));
	mctable.push_back(MCEntry(vect3ub(1,2,5), vect3ub(5,4,1)));
	mctable.push_back(MCEntry(vect3ub(0,2,5)));
	mctable.push_back(MCEntry(vect3ub(1,0,4)));
	mctable.push_back(MCEntry());
}



inline vect3f findZero(vect3f &p1, vect3f &p2, float v1, float v2)
{
	return (p1*v2 - p2*v1) / (v2 - v1);
}


bool MCVisitor::onNode (TraversalData &td)
{
	// boundary check
	if (!StreamingVisitor::onNode(td))
		return false;

	// skip if not crossing zero
	if (g.do_above_below && td.node.data && td.depth - 1 > g.maxDepthMem && td.node.above_below_zero() != 3)
		return false;

	return true;
}

bool MCVisitor::onVertex(TraversalData td[2][2][2])
{
	// boundary check
	if (!StreamingVisitor::onVertex(td))
		return false;

	// skip if not all leaves
	for (int i = 0; i < 8; i++)
	{
		const int x = i&1;
		const int y = (i>>1)&1;
		const int z = (i>>2)&1;
		if (td[x][y][z].node.data)
			return true;
	}

		// skip if no points
	if (g.do_surf_only_at_points)
	{
		bool one_has = false;
		for (int i = 0; i < 8; i++)
		{
			const int x = i&1;
			const int y = (i>>1)&1;
			const int z = i>>2;
			if (td[x][y][z].pt_num)
			{
				one_has = true;
				break;
			}
		}
		if (!one_has)
			return true;
	}

	// find the values at the corners and the index
	float vals[8];
	int positive[8];
	vect3f corners[8];
	int index = 0;

	for (int n = 0; n < 8; n++)
	{
		const int k = n&1;
		const int j = (n>>1)&1;
		const int i = (n>>2)&1;

		TraversalData &ch = td[i][j][k];

		if (g.do_blur)
			vals[n] = (*ch.blur_val - average_val) * ch.cellsize;
		else
			vals[n] = (ch.value - average_val) * ch.cellsize;

		float half = ch.cellsize*.5f;
		corners[n]((ch.x+half)*g.cellscale-g.celloffset, (ch.y+half)*g.cellscale-g.celloffset, (ch.z+half)*g.cellscale-g.celloffset);

		positive[n] = vals[n] >= 0 ? 1 : 0;
		index |= (positive[n] << n);
	}

	if (index == 0 || index == 255)
		return true; // skip if all in or all out

	// find the vertices on the edges
	vect3f edges[12];
	int edgeInd [ 12 ];
	const int edgeList[12][2] = {{0,1},{0,2},{1,3},{2,3},{0,4},{1,5},{2,6},{3,7},{4,5},{4,6},{5,7},{6,7}};
	for ( int i = 0; i < 12; i++ )
	{
		if ( positive [ edgeList [ i ] [ 0 ] ] != positive [ edgeList [ i ] [ 1 ] ] )
		{
			edges [ i ] = findZero ( corners [ edgeList [ i ] [ 0 ] ], corners [ edgeList [ i ] [ 1 ] ], vals [ edgeList [ i ] [ 0 ] ], vals [ edgeList [ i ] [ 1 ] ] );
			if (g.output_file)
			{
#if 0
				char type = 0;
				fwrite ( &type, sizeof ( char ), 1, fptr );
				fwrite ( &(edges [ i ].v [ 0 ]), sizeof ( float ), 3, fptr );
#else			
				vect3f pos = edges [ i ]; // put into original coordinate system
				pos -= vect3f(.5,.5,.5);
				pos /= scale_factor;
				pos += center;

				char type = 0;
				fwrite ( &type, sizeof ( char ), 1, fptr );
				fwrite ( &(pos.v [ 0 ]), sizeof ( float ), 3, fptr );
#endif
				edgeInd [ i ] = g.vertCounter++;
			}
		}
	}

	// triangulate
	MCEntry &entry = mctable[index];
	for (int i = 0; i < entry.tris.size(); i++)
	{
		if (g.output_screen)
		{
			Triangle tri;
			vec3Copy(tri.verts[0], edges[entry.tris[i][0]]);
			vec3Copy(tri.verts[1], edges[entry.tris[i][1]]);
			vec3Copy(tri.verts[2], edges[entry.tris[i][2]]);
			g.reconstructed.triangles.push_back(tri);
		}

		if (g.output_file)
		{
			char type = 1;
			fwrite ( &type, sizeof ( char ), 1, fptr );
			fwrite ( &(edgeInd[entry.tris[i][2]]), sizeof ( int ), 1, fptr );
			fwrite ( &(edgeInd[entry.tris[i][1]]), sizeof ( int ), 1, fptr );
			fwrite ( &(edgeInd[entry.tris[i][0]]), sizeof ( int ), 1, fptr );
		}
	}
	return true;
}


FILE *fptr = 0;
void create_surface(int from, int to)
{
	Traversal trav;
	if (!g.do_eval_pass && !g.do_blur)
		trav.do_evaluation = true;

	static int fileNum = 1;

	// do the slab
	printf("surf slab %d/%d\n", from, streaminfo.cells_total);

	if (g.output_file && fptr == 0)
		fptr = fopen ( "output00.bobj", "wb" );
	else if (g.output_file && ftell(fptr) > 300*(1<<20))
	{
		char filename [ 256 ];
		sprintf ( filename, "output%02d.bobj", fileNum );
		fclose ( fptr );
		fptr = fopen ( filename, "wb" );
		fileNum++;
		g.vertCounter = 1;
	}

	MCVisitor visitorSlice(from, to, fptr);
	trav.traverse(g.tree, &visitorSlice);

	// do the border
	MCVisitor visitorBorder(to, fptr);
	trav.traverse(g.tree, &visitorBorder);

	// delete old cells
	if (g.do_streaming)
		g.tree.removeUpTo(to);

	// close the file
	if (fptr && to == streaminfo.cells_total)
		fclose(fptr);
}
