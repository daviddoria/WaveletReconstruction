FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ../)

ADD_PARAVIEW_PLUGIN(WaveletReconstructionPlugin "1.0"
  SERVER_MANAGER_XML ../vtkWaveletReconstruction.xml
SERVER_MANAGER_SOURCES ../vtkWaveletReconstruction.cxx
  ../global.cpp 
  ../blur.cpp 
  ../average.cpp 
  ../coeffs_daub4.cpp 
  ../coeffs_haar.cpp 
  ../connectneighbors.cpp 
  ../eval_daub4.cpp 
  ../eval_haar.cpp  
  ../marchingcubes.cpp 
  ../matrix.cpp 
  ../octnode.cpp 
  ../octtree.cpp 
  ../parse.cpp 
  ../prune.cpp 
  ../readpts.cpp 
  ../reconstruct.cpp 
  ../streamingvisitor.cpp 
  ../traverse.cpp
)

