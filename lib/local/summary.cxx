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
    for( int i = 0; i < 8; i++ )
    {
      this->count[i] += model->sum.count[i];
      this->weighted_count[i] += model->sum.weighted_count[i];
    }
  }
}
