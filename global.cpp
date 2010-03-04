// Author: Josiah Manson

#include "global.h"
#include "parse.h"

using namespace std;

Global g;
StreamInfo streaminfo;
double average_val = 1.2;

Global::Global()
{
	output_screen = true;
	output_file = false;

	depth = 7;
	wavelet = haar;
	do_prune = false;
	do_blur = false;
	do_above_below = true;
	do_pts_node = false;
	do_eval_pass = false;
	do_streaming = true;
	do_surf_only_at_points = false;
	do_load_points = false;
	do_load_plane = false;

	pointsfile = "";

	//*** things you probably won't change
	do_fill_in_mem = true;
	stay_open = false;

	width = 1000;
	height = 1000;

	bg_color[0] = bg_color[1] = bg_color[2] = 1;

	showLines = false;
	showSurf = true;
	showPoints = true;
	showPlane = true;

	focus(0.5,0.5,0.5);
	zoom = 1.5;

	//*** things you will never want to change
	// set rotation to identity
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			rotation[i*4 + j] = (i == j ? 1 : 0);
	}

	mouse_left = false;
	mouse_right = false;
	mouse_middle = false;
	
	viewFile = 1;

	vertCounter = 1;
}

void Global::writeView()
{
	string str = "savedView_" + toStr(viewFile) + ".mat";
	ofstream f(str.c_str());
	
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			f << rotation[i*4 + j] << " ";
		f << endl;
	}

	for (int j = 0; j < 3; j++)
		f << focus[j] << " ";
	f << endl;

	f << zoom << endl;

	f.close();
}

void Global::readView()
{
	string str = "savedView_" + toStr(viewFile) + ".mat";
	ifstream f(str.c_str());
	assert(!f.fail());
	
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			f >> rotation[i*4 + j];
	}

	for (int j = 0; j < 3; j++)
		f >> focus[j];

	f >> zoom;

	f.close();
}

