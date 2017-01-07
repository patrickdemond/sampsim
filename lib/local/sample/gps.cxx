/*=========================================================================

  Program:  sampsim
  Module:   gps.cxx
  Language: C++

=========================================================================*/

#include "gps.h"

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double gps::get_sample_weight( const sampsim::individual* individual ) const
  {
    return sized_sample::get_sample_weight( individual ) * static_cast< double >( this->number_of_buildings );
  }
}
}
