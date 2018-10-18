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
  void random::copy( const random* object ) {} // nothing to do

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* random::select_next_building( building_list_type& building_list )
  {
    // simple: pick a random building
    auto building_it = building_list.begin();
    std::advance( building_it, utilities::random( 0, building_list.size() - 1 ) );
    return *building_it;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double random::get_immediate_sample_weight( const sampsim::individual* individual ) const
  {
    // Since towns are chosen using PPS, and there is a fixed number of people sampled per town, the
    // samples are "self-weighted" (ie: the probability of any person being chosen is the same for
    // everyone in the sample)
    return 1.0;
  }
}
}
