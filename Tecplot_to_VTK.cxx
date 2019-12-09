#include <iostream>
#include <vtkSmartPointer.h>e
#include <vtkTecplotReader.h>

int main ( int argc, char *argv[] ) {
  if (argc != 1) {
    std::cerr << "Input Filename needed" << std::endl;
  }
  vtkSmartPointer<vtkTecplotReader> tp_reader = vtkSmartPointer<vtkTecplotReader>::New();
  std::string inputfile = argv[1]
  tp_reader->SetFileName("test_tecplot.dat");
  tp_reader->Update();

  return 0;
}