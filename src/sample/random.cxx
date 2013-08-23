/*=========================================================================

  Program:  sampsim
  Module:   random.cxx
  Language: C++

=========================================================================*/

#include "random.h"

#include "building.h"
#include "household.h"
#include "individual.h"
#include "population.h"
#include "tile.h"
#include "utilities.h"

#include <list>
#include <stdexcept>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void random::generate()
  {
    utilities::output( "generating random sample" );

    // check to make sure the age and sex restrictions are valid
    if( UNKNOWN_AGE_TYPE == this->get_age() )
      throw std::runtime_error( "Cannot generate random, age type is unknown" );
    if( UNKNOWN_SEX_TYPE == this->get_sex() )
      throw std::runtime_error( "Cannot generate random, sex type is unknown" );

    utilities::random_engine.seed( atoi( this->get_seed().c_str() ) );

    // create a list of all households
    std::list< household* > remaining_household_list;

    for( auto tile_it = this->population->get_tile_list_cbegin();
         tile_it != this->population->get_tile_list_cend();
         ++tile_it )
    {
      for( auto building_it = tile_it->second->get_building_list_cbegin();
           building_it != tile_it->second->get_building_list_cend();
           ++building_it )
      {
        for( auto household_it = (*building_it)->get_household_list_cbegin();
             household_it != (*building_it)->get_household_list_cend();
             ++household_it )
        remaining_household_list.push_back( *household_it );
      }
    }

    utilities::output( "selecting from a list of %d households", remaining_household_list.size() );

    // keep randomly selecting households until we've filled our sample size
    int running_count = 0;
    while( this->get_size() > running_count )
    {
      if( 0 == remaining_household_list.size() )
      {
        std::cout << "WARNING: unable to fulfill sample size" << std::endl;
        break;
      }

      auto household_it = remaining_household_list.begin();
      std::advance( household_it, utilities::random( 0, remaining_household_list.size() - 1 ) );
      household *h = *household_it;

      int count = 0;
      // select individuals within the household
      for( auto individual_it = h->get_individual_list_begin();
           individual_it != h->get_individual_list_end();
           ++individual_it )
      {
        individual *i = *individual_it;
        if( ( ANY_AGE == this->get_age() || this->get_age() == i->get_age() ) &&
            ( ANY_SEX == this->get_sex() || this->get_sex() == i->get_sex() ) )
        {
          i->set_selected( true );
          count++;
          if( this->get_one_per_household() ) break;
        }
      }

      if( count )
      {
        this->household_list.push_back( h );
        running_count += count;
      }

      remaining_household_list.erase( household_it );
    }

    utilities::output(
      "finished generating random sample, %d households selected", this->household_list.size() );
  }
}
}
