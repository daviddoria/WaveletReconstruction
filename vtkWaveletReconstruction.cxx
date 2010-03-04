#include "vtkWaveletReconstruction.h"

#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include "vtkSmartPointer.h"
#include "vtkTriangle.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"

#include "global.h"
#include "reconstruct.h"
#include "common.h"

vtkCxxRevisionMacro(vtkWaveletReconstruction, "$Revision: 1.70 $");
vtkStandardNewMacro(vtkWaveletReconstruction);

vtkWaveletReconstruction::vtkWaveletReconstruction()
{

}

int vtkWaveletReconstruction::RequestData(vtkInformation *vtkNotUsed(request),
                                             vtkInformationVector **inputVector,
                                             vtkInformationVector *outputVector)
{
  
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
    
  // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(
      inInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  vtkPolyData *output = vtkPolyData::SafeDownCast(
      outInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  
      // offset info
  if (!g.do_eval_pass && !g.do_blur)
    g.do_above_below = false; // the function value isn't known in time to do above_below_zero
  if (!g.do_streaming && g.do_blur)
    g.do_above_below = false; // the function value isn't known in time to do above_below_zero

  if (g.wavelet != haar)
    g.do_eval_pass = true; // only haar wavelets have a simple enough evaluation to do on the fly
  if (g.do_streaming || !g.output_screen || (!g.do_eval_pass && !g.do_blur))
    g.do_load_plane = false; // getting function values requires the tree to be in memory

  g.showPlane = g.do_load_plane;
  g.showPoints = g.do_load_points;

  OctNode::calc_offsets();

  // create tree
  OctTree &tree = g.tree;
  tree.root.allocate();

  if (g.wavelet == daub4)
  {
    g.maxDepth = g.depth + 1;
    if (g.do_streaming)
      g.maxDepthMem = (g.maxDepth-2)*.6 + 2.5;
    else
      g.maxDepthMem = 1;
    g.res = 1 << (g.maxDepth + 1); // 2 cells per 'leaf' node
    g.cellscale = 4.0 / (g.res);
    g.celloffset = 1 + .5*g.cellscale;
  }
  else if (g.wavelet == haar)
  {
    g.maxDepth = g.depth - 1;
    if (g.do_streaming)
      g.maxDepthMem = g.maxDepth*.6 + 0.5;
    else
      g.maxDepthMem = -1;
    g.res = 1 << (g.maxDepth + 1); // 2 cells per 'leaf' node
    g.cellscale = 1.0 / (g.res);
    g.celloffset = 0;
  }


  streaminfo.cells_total = g.res; // 2 cells per 'leaf' node
  streaminfo.cells_mem = 1 << max(0, g.maxDepthMem);
  streaminfo.stride = streaminfo.cells_total / streaminfo.cells_mem;

  // start timing
  clock_t t_start = clock();

  if (g.do_streaming)
  {
    // FIRST PASS
    printf("starting first pass\n");
    printf("depth of tree in first pass: %d\n", g.maxDepthMem);
    
    vector<Process> first_pass;
    if (g.wavelet == haar && !g.do_prune)
      first_pass.push_back(Process(count_and_coeffs_haar_first));
    else
    {
      first_pass.push_back(Process(count_points_first));
      
      if (g.do_prune)
        first_pass.push_back(Process(prune));

      if (g.wavelet == daub4)
        first_pass.push_back(Process(calc_coeffs_daub4_first));
      else if (g.wavelet == haar)
        first_pass.push_back(Process(calc_coeffs_haar_first));
    }
    execPipeline(first_pass);

    removeHighResAll(g.tree.root, 0);
    
    //printf("NODE_COUNT = %d\n", OctNode::node_count);

    if (g.do_fill_in_mem)
    {
      if (g.wavelet == daub4)
        fill_in_mem(tree.root.children(0,0,0).children(1,1,1), 2);
      else if (g.wavelet == haar)
        fill_in_mem(tree.root);
    }

    if (g.do_eval_pass)
    {
      if (g.wavelet == daub4)
        eval_daub4(0, streaminfo.cells_total);
      else if (g.wavelet == haar)
        eval_haar(0, streaminfo.cells_total);
    }

    if (g.do_blur)
      blur_in_memory();

    average_val = calc_average();
  }
  clock_t t_middle = clock();

  // SECOND PASS
  printf("starting second pass\n");
  printf("depth of tree in second pass: %d\n", g.maxDepth);
  
  vector<Process> second_pass;
  if (g.wavelet == haar && !g.do_prune)
    second_pass.push_back(Process(count_and_coeffs_haar_second));
  else
  {
    second_pass.push_back(Process(count_points_second));

    if (g.do_prune)
      second_pass.push_back(Process(prune));

    if (g.wavelet == daub4)
      second_pass.push_back(Process(calc_coeffs_daub4_second));
    else if (g.wavelet == haar)
      second_pass.push_back(Process(calc_coeffs_haar_second));
  }
  
  if (g.do_eval_pass)
  {
    if (g.wavelet == daub4)
      second_pass.push_back(Process(eval_daub4));
    else if (g.wavelet == haar)
      second_pass.push_back(Process(eval_haar));
  }

  if (g.do_blur)
    second_pass.push_back(Process(blur_second));
  else if (!g.do_streaming)
    second_pass.push_back(Process(average_nonstreaming)); // average is calculated in blur

  second_pass.push_back(Process(create_surface));

  execPipeline(second_pass);

  clock_t t_end = clock();

  // PRINT INFORMATION
//  printf("nodes pruned: %d\n", pruned);
  printf("average function value: %f\n", average_val);
  printf("first pass time: %f\n\n", float(t_middle - t_start) / CLOCKS_PER_SEC);
  printf("second pass time: %f\n\n", float(t_end - t_middle) / CLOCKS_PER_SEC);
  printf("total time: %f\n\n", float(t_end - t_start) / CLOCKS_PER_SEC);
      
  vtkSmartPointer<vtkPoints> points = 
      vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> triangles = 
      vtkSmartPointer<vtkCellArray>::New();
  
  for(unsigned int i = 0; i < g.reconstructed.triangles.size(); i++)
    {
      Triangle t = g.reconstructed.triangles[i];
      
      vtkSmartPointer<vtkTriangle> tri = 
          vtkSmartPointer<vtkTriangle>::New();
      /*
      triangle->GetPointIds()->SetId(0, 0);
      triangle->GetPointIds()->SetId(1, 0);
      triangle->GetPointIds()->SetId(2, 0);
      */
      
      tri->GetPoints()->SetPoint(0, t.verts[0].v[0], t.verts[0].v[1], t.verts[0].v[2]);
      tri->GetPoints()->SetPoint(1, t.verts[1].v[0], t.verts[1].v[1], t.verts[1].v[2]);
      tri->GetPoints()->SetPoint(2, t.verts[2].v[0], t.verts[2].v[1], t.verts[2].v[2]);
      
      triangles->InsertNextCell(tri);
    }
    
  output->SetPoints(points);
  output->SetPolys(triangles);
  
  return 1;
}


//----------------------------------------------------------------------------
void vtkWaveletReconstruction::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

