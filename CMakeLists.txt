cmake_minimum_required(VERSION 2.6)

PROJECT(WaveletReconstruction)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

ADD_EXECUTABLE(WaveletReconstruction main.cpp vtkWaveletReconstruction.cxx global.cpp blur.cpp average.cpp coeffs_daub4.cpp coeffs_haar.cpp connectneighbors.cpp eval_daub4.cpp eval_haar.cpp  marchingcubes.cpp matrix.cpp octnode.cpp octtree.cpp parse.cpp plane.cpp prune.cpp readpts.cpp reconstruct.cpp streamingvisitor.cpp traverse.cpp)
TARGET_LINK_LIBRARIES(WaveletReconstruction vtkHybrid glut GLU GL Xmu X11 boost_thread-mt) #originally boost_thread
