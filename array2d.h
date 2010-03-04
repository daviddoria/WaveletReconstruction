#pragma once

#include <vector>

using namespace std;

template <class T>
struct Array2D
{
	Array2D()
	{
		width = height = 0;
	}

	vector<T> data;
	int width, height;

	void resize(int w, int h)
	{
		data.resize(w*h);
		width = w;
		height = h;
	}

	T &operator()(int x, int y)
	{
		return data[y*width + x];
	}

	void clear()
	{
		width = height = 0;
		data.clear();
	}
};
