#include "global.h"
#include <cstdio>

void average_node(OctNode n, float val, int &pts_sum, float &val_sum)
{
	bool have_child = false;
	for (int child = 0; child < 8; child++)
	{
		if (n.children_linear(child).data)
		{
			have_child = true;
			average_node(n.children_linear(child), g.do_blur ? n.blur_vals_linear(child) : n.func_vals_linear(child), pts_sum, val_sum);
		}
	}

	if (!have_child)
	{
		int pts = n.pts_node();
		pts_sum += pts;
		val_sum += val * pts;
	}
}


void average_child(OctNode n, int &pts_sum, float &val_sum)
{
	for (int child = 0; child < 8; child++)
	{
		if (n.children_linear(child).data)
			average_child(n.children_linear(child), pts_sum, val_sum);
		else
		{
			int pts = n.pt_num_linear(child);
			pts_sum += pts;
			val_sum += (g.do_blur ? n.blur_vals_linear(child) : n.func_vals_linear(child)) * pts;
		}
	}
}

void average_haar_node(OctNode n, float val, int &pts_sum, float &val_sum)
{
	bool have_child = false;
	for (int child = 0; child < 8; child++)
	{
		float func_val = val;

		for (int coeff = 0; coeff < 8; coeff++)
		{
			int _and = coeff & child;
			func_val += ((_and >> 2) ^ (_and >> 1) ^ _and) & 1 ? - n.coeffs_linear(coeff) : n.coeffs_linear(coeff);
		}

		if (n.children_linear(child).data)
		{
			have_child = true;
			average_haar_node(n.children_linear(child),func_val, pts_sum, val_sum);
		}
	}

	if (!have_child)
	{
		int pts = n.pts_node();
		pts_sum += pts;
		val_sum += val * pts;
	}
}


void average_haar_child(OctNode n, float val, int &pts_sum, float &val_sum)
{
	for (int child = 0; child < 8; child++)
	{
		float func_val = val;

		for (int coeff = 0; coeff < 8; coeff++)
		{
			int _and = coeff & child;
			func_val += ((_and >> 2) ^ (_and >> 1) ^ _and) & 1 ? - n.coeffs_linear(coeff) : n.coeffs_linear(coeff);
		}

		if (n.children_linear(child).data)
			average_haar_child(n.children_linear(child),func_val, pts_sum, val_sum);
		else
		{
			int pts = n.pt_num_linear(child);
			pts_sum += pts;
			val_sum += func_val * pts;
		}
	}
}

double calc_average()
{
	printf("calc average func val at pts\n");

	int pts = 0;
	float val = 0;

	if (!g.do_eval_pass && !g.do_blur && g.wavelet == haar)
	{
		if (g.do_pts_node)
			average_haar_node(g.tree.root, 0, pts, val);
		else
			average_haar_child(g.tree.root, 0, pts, val);
	}
	else if (g.do_pts_node)
	{
		average_node(g.tree.root, 0, pts, val);
	}
	else
	{
		average_child(g.tree.root, pts, val);
	}

	return val / pts;
}
