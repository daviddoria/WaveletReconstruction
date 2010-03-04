// Author: Josiah Manson

#pragma once

#include "common.h"
#include "array2d.h"
#include <string>

using namespace std;

enum WaveletType
{
	haar,
	daub4
};

struct Global
{
	Global();
	
	string pointsfile; // the input file
	int depth; // reconstruction depth -- 2^depth useful cells with function values (actual tree depth is calculated off this)
	WaveletType wavelet; // what wavelet basis to use
	bool do_prune; // pruning adds a pass, splits the count and coefficient passes for haar, and costs 8 bytes
	bool do_blur; // blurring adds a pass, and costs 32 bytes
	bool do_above_below; // needs blur or eval (values in nodes), speeds up surface reconstruction, and costs 1 byte
	bool do_pts_node; // trades accuracy of d_sigma for memory savings, and saves 28 bytes
	bool do_eval_pass; // (only haar can avoid this pass) stores function values in node, adds a pass, and costs 32 bytes
	bool do_fill_in_mem; // (setting this to false can save memory, but will occasionally create artifacts) create uniform depth tree in memory
	bool do_streaming; // stream or do it all in memory
	bool do_surf_only_at_points; // extract the surface only where there are points (useful for non-closed surfaces)
	bool output_screen; // write triangles to buffer that will be displayed, and creates a window (totally eats memory, but is convenient)
	bool output_file;  // write triangles to a file
	bool stay_open;
	bool do_load_points;
	bool do_load_plane;

	int width, height; // screen resolution
	vect3f bg_color; // background color

	bool mouse_left, mouse_right, mouse_middle;
	float mouse_home_x, mouse_home_y;
	float mouse_prev_x, mouse_prev_y;

	Mesh reconstructed; // reconstructed surface
	vector<OrientedPoint> points; // points only used for display
	bool showLines;
	bool showSurf;
	bool showPoints;
	bool showPlane;

	float rotation[16];
	vect3f focus;
	float zoom;
	int viewFile;
	
	OctTree tree;
	int maxDepth, maxDepthMem; // acutal tree depth that takes into account wavelet
	int res; // resolution in cells across
	float cellscale;
	float celloffset;

	int vertCounter; // used to write output triangle file

	void writeView();
	void readView();

	Array2D<vect3f> plane_img; // plane stuff
	int plane_dir;
	int plane_offset;
	unsigned int plane_tex;
};

extern Global g;
extern StreamInfo streaminfo;
extern double average_val;
extern float scale_factor;
extern vect3f center;