/*=========================================================================

  Program:  sampsim
  Module:   summary.cxx
  Language: C++

=========================================================================*/

#include "summary.h"

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void summary::reset()
  {
    for( int i = 0; i < category_size; i++ )
    {
      for( int j = 0; j < state_size; j++ )
      {
        this->count[i][j] = 0;
        this->weight[i][j] = 0.0;
      }
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void summary::add( summary* sum )
  {
    for( int i = 0; i < category_size; i++ )
    {
      for( int j = 0; j < state_size; j++ )
      {
        this->count[i][j] += sum->count[i][j];
        this->weight[i][j] += sum->weight[i][j];
      }
    }
  }
}
