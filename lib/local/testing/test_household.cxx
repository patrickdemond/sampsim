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
#include "town.h"
#include "utilities.h"

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_household )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population );
  std::vector< std::pair<unsigned int, unsigned int> > count_vector;

  for( auto town_it = population->get_town_list_begin();
       town_it != population->get_town_list_end();
       ++town_it )
  {
    sampsim::town *town = *town_it;
    for( auto tile_it = town->get_tile_list_begin();
         tile_it != town->get_tile_list_end();
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
          count_vector = household->count_individuals();
          CHECK( 0 != ( count_vector[0].first + count_vector[0].second ) );

          cout << "Turning on sample mode" << endl;
          population->set_sample_mode( true );

          cout << "Testing that household now has no population..." << endl;
          count_vector = household->count_individuals();
          for( auto it = count_vector.begin(); it != count_vector.end(); it++ )
            CHECK_EQUAL( 0, (*it).first + (*it).second );

          cout << "Testing that household with selected individual has population..." << endl;
          individual->select();
          count_vector = household->count_individuals();
          CHECK( 0 != ( count_vector[0].first + count_vector[0].second ) );

          cout << "Testing that household with unselected individual has no population..." << endl;
          individual->unselect();
          count_vector = household->count_individuals();
          for( auto it = count_vector.begin(); it != count_vector.end(); it++ )
            CHECK_EQUAL( 0, (*it).first + (*it).second );

          cout << "Testing that unselected household has no population..." << endl;
          individual->select();
          household->unselect();
          count_vector = household->count_individuals();
          for( auto it = count_vector.begin(); it != count_vector.end(); it++ )
            CHECK_EQUAL( 0, (*it).first + (*it).second );

          cout << "Turning off sample mode" << endl;
          population->set_sample_mode( false );

          cout << "Testing household population..." << endl;
          count_vector = household->count_individuals();
          CHECK( 0 != ( count_vector[0].first + count_vector[0].second ) );
        }
      }
    }
  }

  // clean up
  sampsim::utilities::safe_delete( population );
}
