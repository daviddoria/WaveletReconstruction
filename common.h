// Author: Josiah Manson

#pragma once

#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

#include "vect.h"

using namespace std;

// forward declaration of classes
struct OctNode;
struct OctTree;

// miscellaneous classes
struct Triangle
{
	vect3f verts[3];
};

struct Mesh
{
	vector<Triangle> triangles;

	void clear()
	{
		triangles.clear();
	}
};

struct OrientedPoint
{
	vect3f norm, pos;
	float ds;
};

struct Index
{
	Index()
	{
		val = 0;
	}
	Index(const int v)
	{
		val = v;
	}
	Index(const unsigned char x_, const unsigned char y_, const unsigned char z_)
	{
		val = x_ | (y_ << 1) | (z_ << 2);
	}

	union
	{
		struct
		{
			unsigned int x : 1;
			unsigned int y : 1;
			unsigned int z : 1;
		};
		unsigned char val;
	};
};

// tree structures
struct OctNode
{
	// everything of the node is stored in this array (OctNode is a fancy pointer)
	unsigned char *data;

	// offsets of fields into the data array
	static int data_size;
	static int node_count;

	static int o_children;
	static int o_coeffs;
	static int o_pt_num; // point number in leaves
	static int o_pts_node; // point number in nodes
	static int o_func_vals;
	static int o_blur_vals;
	static int o_neighbor_cells;
	static int o_neighbors;
	static int o_above_below_zero;

	static void calc_offsets();

	// constructors
	OctNode(){data = 0;}
	~OctNode(){}

	void allocate();
	void clear();

	// generic accessors
	template <class T> T* gp(int offset){assert(data!=0); return (T*)(data+offset);} // get a pointer at offset from above table
	template <class T> T& ga(int offset, Index i){assert(data!=0); return ((T*)(data+offset))[i.val];} // get an index into an array at offset
	template <class T> T& gv(int offset){assert(data!=0); return *(T*)(data+offset);} // get a value at offset
	
	// specialized accessors for the data fields
	OctNode& neighbors(int d, int v){return ((OctNode*)(data+o_neighbors))[(d<<1)+v];}
	unsigned char& above_below_zero(){return gv<unsigned char>(o_above_below_zero);}
	int& pts_node(){return gv<int>(o_pts_node);}
	float& blur_vals(int x, int y, int z){return ga<float>(o_blur_vals, Index(x,y,z));}
	float& blur_vals_linear(int i){return gp<float>(o_blur_vals)[i];}
	int& pt_num(int x, int y, int z){return ga<int>(o_pt_num, Index(x,y,z));}
	int& pt_num_linear(int i){return gp<int>(o_pt_num)[i];}
	float& func_vals(int x, int y, int z){return ga<float>(o_func_vals, Index(x,y,z));}
	float& func_vals_linear(int i){return gp<float>(o_func_vals)[i];}
	unsigned char& neighbor_cells(int x, int y, int z){return ga<unsigned char>(o_neighbor_cells, Index(x,y,z));}
	unsigned char& neighbor_cells_linear(int i){return gp<unsigned char>(o_neighbor_cells)[i];}
	float& coeffs(int x, int y, int z){return ga<float>(o_coeffs, Index(x,y,z));}
	float& coeffs_linear(int i){return gp<float>(o_coeffs)[i];}
	OctNode& children(int x, int y, int z){return ga<OctNode>(o_children, Index(x,y,z));}
	OctNode& children_linear(int i){return gp<OctNode>(o_children)[i];}

	// helper functions
	OctNode getNeighbor(int x, int y, int z); // 0,0,0 = self, -1,-1,-1 = bottom left, 1,1,1 = top right
	OctNode getNeighborUnsafe(int x, int y, int z); // 0,0,0 = self, -1,-1,-1 = bottom left, 1,1,1 = top right

	void removeChildren();
	void detachFromNeighbors();
	
	float getValueAtPoint(vect3f &pos, vect3f mine, vect3f ext); // used to create plane

	// calculation functions
	void countPoints(vect3f &pt, int maxDepth, int depth, vect3f &mine, vect3f &ext, int num);

	void coeffs_haar(OrientedPoint &pt, int maxDepth, int depth, vect3f mine, vect3f &ext, int minDepth);
	void coeffs_daub4(OrientedPoint &pt, int maxDepth, int depth, vect3f &mine, vect3f &ext, int minDepth);

	bool prune(int from, int to, int minz, int extz);
};


struct OctTree
{
	void countPoints(vect3f &pt, int num = 1);

	void coeffs_haar(OrientedPoint &pt, int minDepth = -1, int maxDepthRecur = 100);
	void coeffs_daub4(OrientedPoint &pt, int minDepth = 1, int maxDepthRecur = 100);

	void removeHighRes(int zvalue);
	void removeHighRes(OctNode node, int size, int z, int zvalue, int depth);

	void removeUpTo(int zvalue);
	void removeUpTo(OctNode node, const int size, const int z, const int zvalue);

	// data
	OctNode root;
	vect3f mine, maxe; // min and max extent
};

// traversal classes
struct TraversalData
{
	OctNode node;
	float value;
	int pt_num;

	unsigned char *neighborCells; // pointer, because it is written to sometimes
	float *blur_val; // pointer, because it is written to sometimes

	short x,y,z; // x,y needed ony for marching cubes
	short cellsize; // this could be calculated from the depth, but is it worth it?

	unsigned char depth;
	bool isCopy;
};

struct Visitor;

struct Traversal
{
	Traversal()
	{
		do_evaluation = false;
	}
	bool do_evaluation;

	void traverse(OctTree &tree, Visitor *v);
	
protected:
	void genNodeChild(TraversalData &ctd, TraversalData &td, int x, int y, int z);

	void traverseNode(Visitor *v, TraversalData &td);
	void traverseFace(Visitor *v, TraversalData td[2], int orientation);
	void traverseEdge(Visitor *v, TraversalData td[2][2], int orientation);
	void traverseVertex(Visitor *v, TraversalData td[2][2][2]);
	
	void recFace0(Visitor *v, TraversalData ch[2][2][2]);
	void recFace1(Visitor *v, TraversalData ch[2][2][2]);
	void recFace2(Visitor *v, TraversalData ch[2][2][2]);

	void recEdge0(Visitor *v, TraversalData ch[2][2][2]);
	void recEdge1(Visitor *v, TraversalData ch[2][2][2]);
	void recEdge2(Visitor *v, TraversalData ch[2][2][2]);
};

// visitors
struct Visitor
{
	virtual bool onNode(TraversalData &n) {return true;}
	virtual bool onFace(TraversalData td[2], int orientation) {return true;}
	virtual bool onEdge(TraversalData td[2][2], int orientation) {return true;}
	virtual bool onVertex(TraversalData td[2][2][2]) {return true;}
};

struct StreamingVisitor : public Visitor
{
	int z_low, z_high;

	StreamingVisitor(int low, int high);
	StreamingVisitor(int z_border);
	TraversalData *minSize(TraversalData *a, TraversalData *b);
	bool insideOrCrossing(int zl, int zm, int zh);
	virtual bool onNode(TraversalData &td);
	virtual bool onFace(TraversalData td[2], int orientation);
	virtual bool onEdge(TraversalData td[2][2], int orientation);
	virtual bool onVertex(TraversalData td[2][2][2]);
};

// streaming info
struct Process
{
	Process(void (*f)(int from, int to))
	{
		completed = 0;
		finished = false;
		func = f;
	}
	Process()
	{
		completed = 0;
		finished = false;
		func = 0;
	}
	int completed; // the number of cells that have been completed
	bool finished; // if the pipe has finished running
	void (*func)(int from, int to); // function pointer
};

struct StreamInfo
{
	int cells_total; // total number of cells across
	int cells_mem; // the number of cells across stored in memory
	int stride; // the number of fine cells per cell in memory
};

// functions
void reconstruct();

void initMCTable();
void readBBox(vect3f &mine, vect3f &maxe, ifstream &file);
bool readPointUpTo(OrientedPoint &pt_read, ifstream &file, const vect3f& center, const float scale_factor, OctTree &tree, int &pts_index, int pts_num, int which_stream, int upto);
void readPointsForDisplay(string filename);

// passes to stick in pipelines
void connect_neighbors_first(int from, int to);
void connect_neighbors_second(int from, int to);
void blur_second(int from, int to);
void blur_in_memory();
void eval_daub4(int start, int end);
void eval_haar(int start, int end);
double calc_average();
void create_surface(int from, int to);
void prune(int from, int to);
void colorPlane();
void average_nonstreaming(int from, int to);
void removeHighResAll(OctNode node, int depth);