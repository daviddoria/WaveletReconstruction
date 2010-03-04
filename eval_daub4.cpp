#include "global.h"
#include <cstdio>


struct CoeffTable6;

struct CoeffTable4
{
	float data[4][4][4];

	float eval()
	{
#if 0
		const float A = -0.3660254037844386; // (1 - sqrt(3))/2
		const float B = 1.3660254037844386; // (1 + sqrt(3))/2
#else
		const float A = .5;
		const float B = .5;
#endif
		return data[1][1][1] * A*A*A +
			   data[1][1][2] * A*A*B +
			   data[1][2][1] * A*B*A +
			   data[1][2][2] * A*B*B +
			   data[2][1][1] * B*A*A +
			   data[2][1][2] * B*A*B +
			   data[2][2][1] * B*B*A +
			   data[2][2][2] * B*B*B;
	}

	void clear()
	{
		for (int m = 0; m < 4*4*4; m++)
			((float*)data)[m] = 0;
	}

	void tensor(float *X, float *Y, float *Z)
	{
		for (int x = 0; x < 4; x++)
			for (int y = 0; y < 4; y++)
				for (int z = 0; z < 4; z++)
					data[x][y][z] = X[x]*Y[y]*Z[z];
	}

	void add(CoeffTable4 &tab, float coeff, int x, int y, int z)
	{
		// source ranges
		int s_x0 = max(0, 2*x);
		int s_y0 = max(0, 2*y);
		int s_z0 = max(0, 2*z);
		int s_x1 = min(4, 4+2*x);
		int s_y1 = min(4, 4+2*y);
		int s_z1 = min(4, 4+2*z);

		// destination offsets
		int d_x = -2*x;
		int d_y = -2*y;
		int d_z = -2*z;

		for (int i = s_x0; i < s_x1; i++)
			for (int j = s_y0; j < s_y1; j++)
				for (int k = s_z0; k < s_z1; k++)
					data[d_x+i][d_y+j][d_z+k] += tab.data[i][j][k] * coeff;
	}

	void subdivide(CoeffTable6 &sub);
};

struct CoeffTable6
{
	float data[6][6][6];

	void clear()
	{
		for (int m = 0; m < 6*6*6; m++)
			((float*)data)[m] = 0;
	}
};

struct SubdivData
{
	SubdivData()
	{
		static float A = (1.0 + sqrt(3.0)) / 4.0;
		static float B = (3.0 + sqrt(3.0)) / 4.0;
		static float C = (3.0 - sqrt(3.0)) / 4.0;
		static float D = (1.0 - sqrt(3.0)) / 4.0;

		static float phi[4] = {A, B, C, D};
		static float psi[4] = {-D, C, -B, A};

		for (int x = 0; x < 2; x++)
			for (int y = 0; y < 2; y++)
				for (int z = 0; z < 2; z++)
					ctables[x][y][z].tensor(x ? psi : phi, y ? psi : phi, z ? psi : phi);
	}

	CoeffTable4 ctables[2][2][2];
};

static SubdivData sdat;

void CoeffTable4::subdivide(CoeffTable6 &sub)
{
	static float A = (1.0 + sqrt(3.0)) / 4.0;
	static float B = (3.0 + sqrt(3.0)) / 4.0;
	static float C = (3.0 - sqrt(3.0)) / 4.0;
	static float D = (1.0 - sqrt(3.0)) / 4.0;

	// subdivide in 3 passes; one in each direction
	float line[4];

	// x direction
	for (int y = 0; y < 4; y++)
	{
		for (int z = 0; z < 4; z++)
		{
			// copy into line
			for (int i = 0; i < 4; i++)
				line[i] = data[i][y][z];

			// subdivide
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 2; j++)
					sub.data[i*2+j][y][z] = line[i]*(j?D:C) + line[i+1]*(j?B:A);
		}
	}

	// y direction
	for (int x = 0; x < 6; x++)
	{
		for (int z = 0; z < 4; z++)
		{
			// copy into line
			for (int i = 0; i < 4; i++)
				line[i] = sub.data[x][i][z];

			// subdivide
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 2; j++)
					sub.data[x][i*2+j][z] = line[i]*(j?D:C) + line[i+1]*(j?B:A);
		}
	}

	// z direction
	for (int x = 0; x < 6; x++)
	{
		for (int y = 0; y < 6; y++)
		{
			// copy into line
			for (int i = 0; i < 4; i++)
				line[i] = sub.data[x][y][i];

			// subdivide
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 2; j++)
					sub.data[x][y][i*2+j] = line[i]*(j?D:C) + line[i+1]*(j?B:A);
		}
	}
}

void eval_daub4(int start, int end, OctNode n, int size, int zpos, CoeffTable4 &table4)
{
	// collect neighbor coefficients
	for (int u = -1; u <= 1; u++)
	{
		for (int v = -1; v <= 1; v++)
		{
			for (int w = -1; w <= 1; w++)
			{
				OctNode node = n.getNeighbor(u, v, w);
				if (node.data == 0)
					continue;

				// for all coefficients
				for (int m = 0; m < 8; m++)
				{
					const int i = m&1;
					const int j = (m>>1)&1;
					const int k = (m>>2)&1;

					const float coeff = node.coeffs(i,j,k);
					if (coeff != 0)
					{
						// add table
						table4.add(sdat.ctables[i][j][k], coeff, -u, -v, -w);
					}
				}

			}
		}
	}

	// subdivide 4x4x4 table into 6x6x6 table
	CoeffTable6 table6;
	table4.subdivide(table6);

	// recur and eval on each child
	int size2 = size >> 1;

	bool do_recur[2] = {zpos < end && start < zpos + size2,
						zpos + size2 < end && start < zpos + size};

	for (int child = 0; child < 8; child++)
	{
		int x = child&1;
		int y = (child>>1)&1;
		int z = child>>2;

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				for (int k = 0; k < 4; k++)
					table4.data[i][j][k] = table6.data[i+x*2][j+y*2][k+z*2];

		n.func_vals(x,y,z) = table4.eval();

		if (do_recur[z] && n.children(x,y,z).data)
			eval_daub4(start, end, n.children(x,y,z), size2, zpos+size2*z, table4);
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

void eval_daub4(int start, int end)
{
	printf("eval slab %d/%d\n", start, streaminfo.cells_total);

	CoeffTable4 ct4;
	ct4.clear();
	eval_daub4(start, end, g.tree.root, streaminfo.cells_total, 0, ct4);
}
