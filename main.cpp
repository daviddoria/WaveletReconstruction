#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

#include "vtkWaveletReconstruction.h"

int main(int argc, char **argv)
{
  vtkSmartPointer<vtkXMLPolyDataReader> reader = 
      vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader->SetFileName(argv[1]);
  reader->Update();
  
  vtkSmartPointer<vtkWaveletReconstruction> waveletReconstructionFilter = 
      vtkSmartPointer<vtkWaveletReconstruction>::New();
  waveletReconstructionFilter->SetInputConnection(reader->GetOutputPort());
  waveletReconstructionFilter->Update();
  
  vtkSmartPointer<vtkXMLPolyDataWriter> writer = 
      vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(argv[2]);
  writer->SetInputConnection(waveletReconstructionFilter->GetOutputPort());
  writer->Write();
  
}
