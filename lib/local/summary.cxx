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
    for( int cat_index = 0; cat_index < category_size; cat_index++ )
    {
      for( int state_index = 0; state_index < state_size; state_index++ )
      {
        this->count[cat_index][state_index] = 0;
        this->weight[cat_index][state_index] = 0.0;
      }
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void summary::add( summary* sum )
  {
    for( int cat_index = 0; cat_index < category_size; cat_index++ )
    {
      for( int state_index = 0; state_index < state_size; state_index++ )
      {
        this->count[cat_index][state_index] += sum->count[cat_index][state_index];
        this->weight[cat_index][state_index] += sum->weight[cat_index][state_index];
      }
    }
  }
}
