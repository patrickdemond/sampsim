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
  std::list< household* >::iterator random::select_next_household( std::list< household* > &list )
  {
    // simple: pick a random household
    auto household_it = list.begin();
    std::advance( household_it, utilities::random( 0, list.size() - 1 ) );
    return household_it;
  }
}
}
