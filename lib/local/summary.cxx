/*=========================================================================

  Program:  sampsim
  Module:   summary.cxx
  Language: C++

=========================================================================*/

#include "summary.h"

#include "model_object.h"

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void summary::add( model_object* model )
  {
    for( unsigned int rr = 0; rr < utilities::rr_size; rr++ )
    {
      for( unsigned int i = 0; i < 8; i++ )
      {
        this->count[rr][i] += model->sum.count[rr][i];
        this->weighted_count[rr][i] += model->sum.weighted_count[rr][i];
      }
    }
  }
}
