/*=========================================================================

  Program:  sampsim
  Module:   test_population.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the population class
//

#include "UnitTest++.h"

#include "building.h"
#include "common.h"
#include "household.h"
#include "individual.h"
#include "population.h"
#include "tile.h"

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_population )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population );
  sampsim::tile *tile = population->get_tile_list_begin()->second;
  sampsim::building *building = *tile->get_building_list_begin();
  sampsim::household *household = *building->get_household_list_begin();
  sampsim::individual *individual = *household->get_individual_list_begin();

  cout << "Testing population size..." << endl;
  CHECK( 0 != population->count_population() );

  cout << "Turning on sample mode" << endl;
  population->set_sample_mode( true );
  
  cout << "Testing that population now has a count of zero..." << endl;
  CHECK_EQUAL( 0, population->count_population() );

  cout << "Testing that population with selected individual has non-zero count..." << endl;
  individual->select();
  CHECK( 0 != population->count_population() );

  cout << "Testing that population with unselected individual has a count of zero..." << endl;
  individual->unselect();
  CHECK_EQUAL( 0, population->count_population() );

  cout << "Turning off sample mode" << endl;
  population->set_sample_mode( false );

  cout << "Testing population size..." << endl;
  CHECK( 0 != population->count_population() );

  cout << "Testing population centroid..." << endl;
  sampsim::coordinate centroid = population->get_centroid();
  CHECK_EQUAL( 5, centroid.x );
  CHECK_EQUAL( 5, centroid.y );

  cout << "Testing population area..." << endl;
  CHECK_EQUAL( 100, population->get_area() );

  delete population;
}
