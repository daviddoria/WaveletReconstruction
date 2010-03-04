#include "global.h"
#include <cstdio>

void eval_haar(int start, int end, OctNode n, int size, int z, float val)
{
	// compute values
	for (int child = 0; child < 8; child++)
	{
		n.func_vals_linear(child) = val;

		for (int coeff = 0; coeff < 8; coeff++)
		{
			int _and = coeff & child;
			n.func_vals_linear(child) += ((_and >> 2) ^ (_and >> 1) ^ _and) & 1 ? - n.coeffs_linear(coeff) : n.coeffs_linear(coeff);
		}
	}

	// recur
	int size2 = size >> 1;

	bool low = z < end && start < z + size2;
	bool high = z + size2 < end && start < z + size;

	if (low)
	{
		for (int child = 0; child < 4; child++)
		{
			if (n.children(child&1,child>>1,0).data)
				eval_haar(start, end, n.children(child&1,child>>1,0), size2, z, n.func_vals(child&1,child>>1,0));
		}
	}
	if (high)
	{
		for (int child = 0; child < 4; child++)
		{
			if (n.children(child&1,child>>1,1).data)
				eval_haar(start, end, n.children(child&1,child>>1,1), size2, z+size2, n.func_vals(child&1,child>>1,1));
		}
	}

	// crossing zero
	if (g.do_above_below && !g.do_blur)
	{
		// having calculated function values, determine if above or below iso-cutoff
		// if no child, then use the function value, otherwise, use the booleans
		unsigned char &above_below_zero = n.above_below_zero();
		above_below_zero = 0;

		for (int i = 0; i < 8; i++)
		{
			const int x = i&1;
			const int y = (i>>1)&1;
			const int z = (i>>2)&1;

			if (n.children(x,y,z).data)
				above_below_zero |= n.children(x,y,z).above_below_zero();
			else if (n.func_vals(x,y,z) >= average_val)
				above_below_zero |= 1;
			else
				above_below_zero |= 2;
		}
	}
}

void eval_haar(int start, int end)
{
	printf("eval slab %d/%d\n", start, streaminfo.cells_total);

	int size = 1 << (g.maxDepth + 1);
	int z = 0;
	OctNode n = g.tree.root;

	eval_haar(start, end, n, size, z, 0);
}
