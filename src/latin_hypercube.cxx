/*=========================================================================

  Program:  sampsim
  Module:   latin_hypercube.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// An executable which generates random latin hypercube samples
//

#include <iostream>

#include "latin_center.hpp"

using namespace std;

int main( const int argc, const char** argv )
{
  // TODO: need to make dims, points and seed argv input parameters
  int dims = 2, points = 4;
  int seed = 1;
  double x[10000]; // TODO: make pointer with mem
  double y[100][100]; // TODO: make pointer with mem
  latin_center( dims, points, &seed, x );

  // organise points into a two-dimensional array
  for( int row = 0; row < points; row++ )
    for( int col = 0; col < dims; col++ )
      y[row][col] = x[row + points*col];

  for( int row = 0; row < points; row++ )
  {
    for( int col = 0; col < dims; col++ )
    {
      if( 0 < col ) cout << ",";
      cout << y[row][col];
    }
    cout << endl;
  }

  return EXIT_SUCCESS;
}
