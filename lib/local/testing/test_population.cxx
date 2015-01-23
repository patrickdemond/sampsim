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
#include "town.h"

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_population )
{
  // create a population
  int number_of_towns = 8;
  int town_size_min = 5000;
  int town_size_max = 100000;
  sampsim::population *population = new sampsim::population;
  create_test_population( population, number_of_towns, town_size_min, town_size_max );
  
  sampsim::town *town = *population->get_town_list_begin();
  sampsim::tile *tile = town->get_tile_list_begin()->second;
  sampsim::building *building = *tile->get_building_list_begin();
  sampsim::household *household = *building->get_household_list_begin();
  sampsim::individual *individual = *household->get_individual_list_begin();

  std::pair<unsigned int, unsigned int> total = population->count_individuals();

  cout << "Testing population size..." << endl;
  CHECK( town_size_min * number_of_towns <= total.second );
  CHECK( town_size_max * number_of_towns >= total.second );

  cout << "Testing population prevalence..." << endl;
  CHECK( total.first <= total.second );
  CHECK( town_size_min * number_of_towns <= total.first );
  CHECK( town_size_max * number_of_towns >= total.first );

  cout << "Turning on sample mode" << endl;
  population->set_sample_mode( true );

  total = population->count_individuals();

  cout << "Testing that population now has a count of zero..." << endl;
  CHECK_EQUAL( 0, total.second );

  cout << "Testing that population prevalence now has a count of zero..." << endl;
  CHECK_EQUAL( 0, total.first );

  cout << "Testing that population with selected individual has non-zero count..." << endl;
  individual->select();
  CHECK( 0 != population->count_individuals().second );

  cout << "Testing that population with unselected individual has a count of zero..." << endl;
  individual->unselect();
  CHECK_EQUAL( 0, population->count_individuals().second );

  cout << "Turning off sample mode" << endl;
  population->set_sample_mode( false );

  total = population->count_individuals();

  cout << "Testing population size..." << endl;
  CHECK( 0 != total.second );

  cout << "Testing population prevalence..." << endl;
  CHECK( 0 != total.first );

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
  CHECK( 5000 < atoi( parts[0].c_str() ) ); // at least 10000 lines
  command.str( "" );
  command.clear();
  string household_filename = temp_filename.str() + ".household.csv";
  command << "wc -l " << household_filename;
  parts = sampsim::utilities::explode( sampsim::utilities::exec( command.str() ), " " );
  CHECK( 1000 < atoi( parts[0].c_str() ) ); // at least 2500 lines

  cout << "Testing writing population to disk in json format..." << endl;
  try { population->write( temp_filename.str(), false ); }
  catch(...) { CHECK( false ); }

  cout << "Testing reading population from disk..." << endl;
  temp_filename << ".json";
  sampsim::population *population_read = new sampsim::population;
  CHECK( population_read->read( temp_filename.str() ) );
  total = population->count_individuals();
  std::pair<unsigned int, unsigned int> total_read = population_read->count_individuals();
  CHECK_EQUAL( total.second, total_read.second );
  CHECK_EQUAL( total.first, total_read.first );

  // clean up
  remove( temp_filename.str().c_str() );
  remove( individual_filename.c_str() );
  remove( household_filename.c_str() );
  sampsim::utilities::safe_delete( population );
  sampsim::utilities::safe_delete( population_read );
}
