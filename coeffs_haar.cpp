#include "global.h"

struct Haar
{
	static float psi(float t) {return t <= .5 ? 1 : -1;}
	static float int_psi(float t) {return t <= .5 ? t : 1-t;}
};

void OctNode::coeffs_haar(OrientedPoint &pt, int maxDepth, int depth, vect3f mine, vect3f &ext, int minDepth)
{
	// split the node and push down point
	vect3f ext2 = ext*.5;
	vect3f center = mine + ext2;

	if ( depth > minDepth )
	{
		int ind [ 3 ] = {0, 0, 0};
		float hData [ 3 ], psiData [ 3 ];
		vect3f lp; // local position

		for (int i = 0; i < 3; i++)
		{
			lp[i] = (pt.pos[i] - mine[i]) / ext[i];
			if ( lp [ i ] > 0.5 )
			{
				ind [ i ] = 1;
				psiData [ i ] = -1;
				hData [ i ] = 1 - lp [ i ];
			}
			else
			{
				psiData [ i ] = 1;
				hData [ i ] = lp [ i ];
			}
		}

		// push down
		if (children(ind[0],ind[1],ind[2]).data == 0)
		{
			// it's a leaf
			if (g.do_pts_node)
				pt.norm *= ( 1.0f / ((float)pts_node() ));
			else
				pt.norm *= ( 1.0f / ((float)pt_num(ind[0],ind[1],ind[2]) ));
		}
		else
		{
			// not a leaf
			vect3f off(ext2[0]*ind[0], ext2[1]*ind[1], ext2[2]*ind[2]);
			children(ind[0],ind[1],ind[2]).coeffs_haar(pt, maxDepth, depth+1, mine+off, ext2, minDepth);
		}
		pt.norm *= 0.25f;

		hData [ 0 ] *= pt.norm [ 0 ];
		hData [ 1 ] *= pt.norm [ 1 ];
		hData [ 2 ] *= pt.norm [ 2 ];

		/*if (depth == 0) // constant offset doesn't matter if getting average value
			coeffs(0,0,0) += ( lp[0] + lp[1] + lp[2] ) * .333333333333;*/

		coeffs(0,0,1) += ( hData[2] );
		coeffs(0,1,0) += ( hData[1] );
		coeffs(1,0,0) += ( hData[0] );
		coeffs(0,1,1) += ( psiData[2]*hData[1] + psiData[1]*hData[2] ) * 0.5;
		coeffs(1,1,0) += ( psiData[0]*hData[1] + psiData[1]*hData[0] ) * 0.5;
		coeffs(1,0,1) += ( psiData[0]*hData[2] + psiData[2]*hData[0] ) * 0.5;
		coeffs(1,1,1) += ( psiData[0]*psiData[1]*hData[2] + psiData[0]*psiData[2]*hData[1] + psiData[1]*psiData[2]*hData[0] ) * .333333333333;
	}
	else
	{
		int ix = 0, iy = 0, iz = 0;
		if (pt.pos[0] > center[0])
			ix = 1;
		if (pt.pos[1] > center[1])
			iy = 1;
		if (pt.pos[2] > center[2])
			iz = 1;

		// push down
		if (children(ix,iy,iz).data == 0)
		{
			// it's a leaf
			if (g.do_pts_node)
				pt.norm *= (1.0f / ((float)pts_node()));
			else
				pt.norm *= (1.0f / ((float)pt_num(ix,iy,iz)));
		}
		else
		{
			// not a leaf
			vect3f off(ext2[0]*ix, ext2[1]*iy, ext2[2]*iz);
			children(ix,iy,iz).coeffs_haar(pt, maxDepth, depth+1, mine+off, ext2, minDepth);
		}

		pt.norm *= 0.25f;
	}	
}
