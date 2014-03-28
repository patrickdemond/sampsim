/*=========================================================================

  Program:  sampsim
  Module:   test_household.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the household class
//

#include "UnitTest++.h"

#include "building.h"
#include "common.h"
#include "household.h"
#include "individual.h"
#include "population.h"
#include "tile.h"
#include "utilities.h"

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_household )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population );

  for( auto tile_it = population->get_tile_list_begin();
       tile_it != population->get_tile_list_end();
       ++tile_it )
  {
    sampsim::tile *tile = tile_it->second;
    for( auto building_it = tile->get_building_list_cbegin();
         building_it != tile->get_building_list_cend();
         ++building_it )
    {
      sampsim::building *building = *building_it;
      for( auto household_it = building->get_household_list_cbegin();
           household_it != building->get_household_list_cend();
           ++household_it )
      {
        sampsim::household *household = *household_it;
        sampsim::individual *individual = *household->get_individual_list_begin();

        cout << "Testing household population..." << endl;
        CHECK( 0 != household->count_individuals() );

        cout << "Turning on sample mode" << endl;
        population->set_sample_mode( true );
        
        cout << "Testing that household now has no population..." << endl;
        CHECK_EQUAL( 0, household->count_individuals() );

        cout << "Testing that household with selected individual has population..." << endl;
        individual->select();
        CHECK( 0 != household->count_individuals() );

        cout << "Testing that household with unselected individual has no population..." << endl;
        individual->unselect();
        CHECK_EQUAL( 0, household->count_individuals() );

        cout << "Testing that unselected household has no population..." << endl;
        individual->select();
        household->unselect();
        CHECK_EQUAL( 0, household->count_individuals() );

        cout << "Turning off sample mode" << endl;
        population->set_sample_mode( false );

        cout << "Testing household population..." << endl;
        CHECK( 0 != household->count_individuals() );
      }
    }
  }

  // clean up
  sampsim::utilities::safe_delete( population );
}
