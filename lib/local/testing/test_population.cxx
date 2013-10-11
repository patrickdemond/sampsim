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

  stringstream temp_filename;
  temp_filename << "/tmp/sampsim" << sampsim::utilities::random( 1000000, 9999999 );

  cout << "Testing writing population to disk in csv format..." << endl;
  try { population->write( temp_filename.str(), true ); }
  catch(...) { CHECK( false ); }
  stringstream command;
  vector<string> parts;
  string individual_filename = temp_filename.str() + ".individual.csv";
  command << "wc -l " << individual_filename;
  parts = sampsim::utilities::explode( sampsim::utilities::exec( command.str() ), " " );
  CHECK( 10000 < atoi( parts[0].c_str() ) ); // at least 10000 lines
  command.str( "" );
  command.clear();
  string household_filename = temp_filename.str() + ".household.csv";
  command << "wc -l " << household_filename;
  parts = sampsim::utilities::explode( sampsim::utilities::exec( command.str() ), " " );
  CHECK( 2500 < atoi( parts[0].c_str() ) ); // at least 2500 lines

  cout << "Testing writing population to disk in json format..." << endl;
  try { population->write( temp_filename.str(), false ); }
  catch(...) { CHECK( false ); }

  cout << "Testing reading population from disk..." << endl;
  temp_filename << ".json";
  sampsim::population *population_read = new sampsim::population;
  CHECK( population_read->read( temp_filename.str() ) );
  CHECK_EQUAL( population->count_population(), population_read->count_population() );

  // clean up
  remove( temp_filename.str().c_str() );
  remove( individual_filename.c_str() );
  remove( household_filename.c_str() );
  sampsim::utilities::safe_delete( population );
  sampsim::utilities::safe_delete( population_read );
}
