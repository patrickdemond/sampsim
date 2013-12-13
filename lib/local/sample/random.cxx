/*=========================================================================

  Program:  sampsim
  Module:   random.cxx
  Language: C++

=========================================================================*/

#include "random.h"

#include "building_tree.h"

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* random::select_next_building( sampsim::building_tree& tree )
  {
    // simple: pick a random building
    building_list_type list = tree.get_building_list();
    auto building_it = list.begin();
    std::advance( building_it, utilities::random( 0, list.size() - 1 ) );
    return *building_it;
  }
}
}
