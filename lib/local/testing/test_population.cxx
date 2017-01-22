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
#include "summary.h"
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

  sampsim::summary *sum = population->get_summary();

  cout << "Testing population size..." << endl;
  CHECK( town_size_min * number_of_towns <= sum->get_count() );
  CHECK( town_size_max * number_of_towns >= sum->get_count() );

  cout << "Testing population prevalence..." << endl;
  for( int cat_index = 0; cat_index < sampsim::summary::category_size; cat_index++ )
  {
    CHECK( sum->get_count( cat_index, sampsim::summary::diseased ) <= sum->get_count( cat_index ) );
    CHECK( 0 <= sum->get_count( cat_index, sampsim::summary::diseased ) );
    CHECK( town_size_max * number_of_towns >= sum->get_count( cat_index, sampsim::summary::diseased ) );
  }

  cout << "Turning on sample mode" << endl;
  population->set_sample_mode( true );

  sum = population->get_summary();

  cout << "Testing that population now has a count of zero..." << endl;
  for( int cat_index = 0; cat_index < sampsim::summary::category_size; cat_index++ )
    CHECK_EQUAL( 0, sum->get_count( cat_index ) );

  cout << "Testing that population prevalence now has a count of zero..." << endl;
  for( int cat_index = 0; cat_index < sampsim::summary::category_size; cat_index++ )
    CHECK_EQUAL( 0, sum->get_count( cat_index, sampsim::summary::diseased ) );

  cout << "Testing that population with selected individual has non-zero count..." << endl;
  individual->select();
  sum = population->get_summary();
  CHECK( 0 != sum->get_count() );

  cout << "Testing that population with unselected individual has a count of zero..." << endl;
  individual->unselect();
  sum = population->get_summary();
  for( int cat_index = 0; cat_index < sampsim::summary::category_size; cat_index++ )
    CHECK_EQUAL( 0, sum->get_count( cat_index ) );

  cout << "Turning off sample mode" << endl;
  population->set_sample_mode( false );

  sum = population->get_summary();

  cout << "Testing population size..." << endl;
  for( int cat_index = 0; cat_index < sampsim::summary::category_size; cat_index++ )
    CHECK( 0 != sum->get_count( cat_index ) );

  cout << "Testing population prevalence..." << endl;
  for( int cat_index = 0; cat_index < sampsim::summary::category_size; cat_index++ )
    CHECK( 0 != sum->get_count( cat_index, sampsim::summary::diseased ) );

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

  CHECK_EQUAL( population->get_number_of_individuals(), population_read->get_number_of_individuals() );
  sum = population->get_summary();
  sampsim::summary *read_sum = population_read->get_summary();
  for( int cat_index = 0; cat_index < sampsim::summary::category_size; cat_index++ )
  {
    CHECK_EQUAL( sum->get_count( cat_index, sampsim::summary::diseased ),
                 read_sum->get_count( cat_index, sampsim::summary::diseased ) );
    CHECK_EQUAL( sum->get_count( cat_index, sampsim::summary::healthy ),
                 read_sum->get_count( cat_index, sampsim::summary::healthy ) );
  }

  // clean up
  remove( temp_filename.str().c_str() );
  remove( individual_filename.c_str() );
  remove( household_filename.c_str() );
  sampsim::utilities::safe_delete( population );
  sampsim::utilities::safe_delete( population_read );
}
