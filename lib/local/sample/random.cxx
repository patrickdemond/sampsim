/*=========================================================================

  Program:  sampsim
  Module:   random.cxx
  Language: C++

=========================================================================*/

#include "random.h"

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::list< building* >::iterator random::select_next_building( std::list< building* > &list )
  {
    // simple: pick a random building
    auto building_it = list.begin();
    std::advance( building_it, utilities::random( 0, list.size() - 1 ) );
    return building_it;
  }
}
}
