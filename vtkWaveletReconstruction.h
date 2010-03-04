#ifndef __vtkWaveletReconstruction_h
#define __vtkWaveletReconstruction_h

#include "vtkPolyDataAlgorithm.h"


class vtkWaveletReconstruction : public vtkPolyDataAlgorithm 
{
public:
  vtkTypeRevisionMacro(vtkWaveletReconstruction,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkWaveletReconstruction *New();
	    
protected:
  vtkWaveletReconstruction();
  
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
  vtkWaveletReconstruction(const vtkWaveletReconstruction&);  // Not implemented.
  void operator=(const vtkWaveletReconstruction&);  // Not implemented.


};

#endif
