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
  CHECK( sum->get_count( ANY_AGE, ANY_SEX, DISEASED ) <= sum->get_count( ANY_AGE, ANY_SEX ) );
  CHECK( 0 <= sum->get_count( ANY_AGE, ANY_SEX, DISEASED ) );
  CHECK( town_size_max * number_of_towns >= sum->get_count( ANY_AGE, ANY_SEX, DISEASED ) );
  CHECK( sum->get_count( ANY_AGE, MALE, DISEASED ) <= sum->get_count( ANY_AGE, MALE ) );
  CHECK( 0 <= sum->get_count( ANY_AGE, MALE, DISEASED ) );
  CHECK( town_size_max * number_of_towns >= sum->get_count( ANY_AGE, MALE, DISEASED ) );
  CHECK( sum->get_count( ANY_AGE, FEMALE, DISEASED ) <= sum->get_count( ANY_AGE, FEMALE ) );
  CHECK( 0 <= sum->get_count( ANY_AGE, FEMALE, DISEASED ) );
  CHECK( town_size_max * number_of_towns >= sum->get_count( ANY_AGE, FEMALE, DISEASED ) );
  CHECK( sum->get_count( ADULT, ANY_SEX, DISEASED ) <= sum->get_count( ADULT, ANY_SEX ) );
  CHECK( 0 <= sum->get_count( ADULT, ANY_SEX, DISEASED ) );
  CHECK( town_size_max * number_of_towns >= sum->get_count( ADULT, ANY_SEX, DISEASED ) );
  CHECK( sum->get_count( ADULT, MALE, DISEASED ) <= sum->get_count( ADULT, MALE ) );
  CHECK( 0 <= sum->get_count( ADULT, MALE, DISEASED ) );
  CHECK( town_size_max * number_of_towns >= sum->get_count( ADULT, MALE, DISEASED ) );
  CHECK( sum->get_count( ADULT, FEMALE, DISEASED ) <= sum->get_count( ADULT, FEMALE ) );
  CHECK( 0 <= sum->get_count( ADULT, FEMALE, DISEASED ) );
  CHECK( town_size_max * number_of_towns >= sum->get_count( ADULT, FEMALE, DISEASED ) );
  CHECK( sum->get_count( CHILD, ANY_SEX, DISEASED ) <= sum->get_count( CHILD, ANY_SEX ) );
  CHECK( 0 <= sum->get_count( CHILD, ANY_SEX, DISEASED ) );
  CHECK( town_size_max * number_of_towns >= sum->get_count( CHILD, ANY_SEX, DISEASED ) );
  CHECK( sum->get_count( CHILD, MALE, DISEASED ) <= sum->get_count( CHILD, MALE ) );
  CHECK( 0 <= sum->get_count( CHILD, MALE, DISEASED ) );
  CHECK( town_size_max * number_of_towns >= sum->get_count( CHILD, MALE, DISEASED ) );
  CHECK( sum->get_count( CHILD, FEMALE, DISEASED ) <= sum->get_count( CHILD, FEMALE ) );
  CHECK( 0 <= sum->get_count( CHILD, FEMALE, DISEASED ) );
  CHECK( town_size_max * number_of_towns >= sum->get_count( CHILD, FEMALE, DISEASED ) );

  cout << "Turning on sample mode" << endl;
  population->set_sample_mode( true );

  sum = population->get_summary();

  cout << "Testing that population now has a count of zero..." << endl;
  CHECK_EQUAL( 0, sum->get_count( ANY_AGE, ANY_SEX ) );
  CHECK_EQUAL( 0, sum->get_count( ANY_AGE, MALE ) );
  CHECK_EQUAL( 0, sum->get_count( ANY_AGE, FEMALE ) );
  CHECK_EQUAL( 0, sum->get_count( ADULT, ANY_SEX ) );
  CHECK_EQUAL( 0, sum->get_count( ADULT, MALE ) );
  CHECK_EQUAL( 0, sum->get_count( ADULT, FEMALE ) );
  CHECK_EQUAL( 0, sum->get_count( CHILD, ANY_SEX ) );
  CHECK_EQUAL( 0, sum->get_count( CHILD, MALE ) );
  CHECK_EQUAL( 0, sum->get_count( CHILD, FEMALE ) );

  cout << "Testing that population prevalence now has a count of zero..." << endl;
  CHECK_EQUAL( 0, sum->get_count( ANY_AGE, ANY_SEX, sampsim::DISEASED ) );
  CHECK_EQUAL( 0, sum->get_count( ANY_AGE, MALE, sampsim::DISEASED ) );
  CHECK_EQUAL( 0, sum->get_count( ANY_AGE, FEMALE, sampsim::DISEASED ) );
  CHECK_EQUAL( 0, sum->get_count( ADULT, ANY_SEX, sampsim::DISEASED ) );
  CHECK_EQUAL( 0, sum->get_count( ADULT, MALE, sampsim::DISEASED ) );
  CHECK_EQUAL( 0, sum->get_count( ADULT, FEMALE, sampsim::DISEASED ) );
  CHECK_EQUAL( 0, sum->get_count( CHILD, ANY_SEX, sampsim::DISEASED ) );
  CHECK_EQUAL( 0, sum->get_count( CHILD, MALE, sampsim::DISEASED ) );
  CHECK_EQUAL( 0, sum->get_count( CHILD, FEMALE, sampsim::DISEASED ) );

  cout << "Testing that population with selected individual has non-zero count..." << endl;
  individual->select();
  sum = population->get_summary();
  CHECK( 0 != sum->get_count() );

  cout << "Testing that population with unselected individual has a count of zero..." << endl;
  individual->unselect();
  sum = population->get_summary();
  CHECK_EQUAL( 0, sum->get_count( ANY_AGE, ANY_SEX ) );
  CHECK_EQUAL( 0, sum->get_count( ANY_AGE, MALE ) );
  CHECK_EQUAL( 0, sum->get_count( ANY_AGE, FEMALE ) );
  CHECK_EQUAL( 0, sum->get_count( ADULT, ANY_SEX ) );
  CHECK_EQUAL( 0, sum->get_count( ADULT, MALE ) );
  CHECK_EQUAL( 0, sum->get_count( ADULT, FEMALE ) );
  CHECK_EQUAL( 0, sum->get_count( CHILD, ANY_SEX ) );
  CHECK_EQUAL( 0, sum->get_count( CHILD, MALE ) );
  CHECK_EQUAL( 0, sum->get_count( CHILD, FEMALE ) );

  cout << "Turning off sample mode" << endl;
  population->set_sample_mode( false );

  sum = population->get_summary();

  cout << "Testing population size..." << endl;
  CHECK( 0 != sum->get_count( ANY_AGE, ANY_SEX ) );
  CHECK( 0 != sum->get_count( ANY_AGE, MALE ) );
  CHECK( 0 != sum->get_count( ANY_AGE, FEMALE ) );
  CHECK( 0 != sum->get_count( ADULT, ANY_SEX ) );
  CHECK( 0 != sum->get_count( ADULT, MALE ) );
  CHECK( 0 != sum->get_count( ADULT, FEMALE ) );
  CHECK( 0 != sum->get_count( CHILD, ANY_SEX ) );
  CHECK( 0 != sum->get_count( CHILD, MALE ) );
  CHECK( 0 != sum->get_count( CHILD, FEMALE ) );

  cout << "Testing population prevalence..." << endl;
  CHECK( 0 != sum->get_count( ANY_AGE, ANY_SEX, sampsim::DISEASED ) );
  CHECK( 0 != sum->get_count( ANY_AGE, MALE, sampsim::DISEASED ) );
  CHECK( 0 != sum->get_count( ANY_AGE, FEMALE, sampsim::DISEASED ) );
  CHECK( 0 != sum->get_count( ADULT, ANY_SEX, sampsim::DISEASED ) );
  CHECK( 0 != sum->get_count( ADULT, MALE, sampsim::DISEASED ) );
  CHECK( 0 != sum->get_count( ADULT, FEMALE, sampsim::DISEASED ) );
  CHECK( 0 != sum->get_count( CHILD, ANY_SEX, sampsim::DISEASED ) );
  CHECK( 0 != sum->get_count( CHILD, MALE, sampsim::DISEASED ) );
  CHECK( 0 != sum->get_count( CHILD, FEMALE, sampsim::DISEASED ) );

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
  CHECK_EQUAL( sum->get_count( ANY_AGE, ANY_SEX, DISEASED ), read_sum->get_count( ANY_AGE, ANY_SEX, DISEASED ) );
  CHECK_EQUAL( sum->get_count( ANY_AGE, ANY_SEX, HEALTHY ), read_sum->get_count( ANY_AGE, ANY_SEX, HEALTHY ) );
  CHECK_EQUAL( sum->get_count( ANY_AGE, MALE, DISEASED ), read_sum->get_count( ANY_AGE, MALE, DISEASED ) );
  CHECK_EQUAL( sum->get_count( ANY_AGE, MALE, HEALTHY ), read_sum->get_count( ANY_AGE, MALE, HEALTHY ) );
  CHECK_EQUAL( sum->get_count( ANY_AGE, FEMALE, DISEASED ), read_sum->get_count( ANY_AGE, FEMALE, DISEASED ) );
  CHECK_EQUAL( sum->get_count( ANY_AGE, FEMALE, HEALTHY ), read_sum->get_count( ANY_AGE, FEMALE, HEALTHY ) );
  CHECK_EQUAL( sum->get_count( ADULT, ANY_SEX, DISEASED ), read_sum->get_count( ADULT, ANY_SEX, DISEASED ) );
  CHECK_EQUAL( sum->get_count( ADULT, ANY_SEX, HEALTHY ), read_sum->get_count( ADULT, ANY_SEX, HEALTHY ) );
  CHECK_EQUAL( sum->get_count( ADULT, MALE, DISEASED ), read_sum->get_count( ADULT, MALE, DISEASED ) );
  CHECK_EQUAL( sum->get_count( ADULT, MALE, HEALTHY ), read_sum->get_count( ADULT, MALE, HEALTHY ) );
  CHECK_EQUAL( sum->get_count( ADULT, FEMALE, DISEASED ), read_sum->get_count( ADULT, FEMALE, DISEASED ) );
  CHECK_EQUAL( sum->get_count( ADULT, FEMALE, HEALTHY ), read_sum->get_count( ADULT, FEMALE, HEALTHY ) );
  CHECK_EQUAL( sum->get_count( CHILD, ANY_SEX, DISEASED ), read_sum->get_count( CHILD, ANY_SEX, DISEASED ) );
  CHECK_EQUAL( sum->get_count( CHILD, ANY_SEX, HEALTHY ), read_sum->get_count( CHILD, ANY_SEX, HEALTHY ) );
  CHECK_EQUAL( sum->get_count( CHILD, MALE, DISEASED ), read_sum->get_count( CHILD, MALE, DISEASED ) );
  CHECK_EQUAL( sum->get_count( CHILD, MALE, HEALTHY ), read_sum->get_count( CHILD, MALE, HEALTHY ) );
  CHECK_EQUAL( sum->get_count( CHILD, FEMALE, DISEASED ), read_sum->get_count( CHILD, FEMALE, DISEASED ) );
  CHECK_EQUAL( sum->get_count( CHILD, FEMALE, HEALTHY ), read_sum->get_count( CHILD, FEMALE, HEALTHY ) );

  // clean up
  remove( temp_filename.str().c_str() );
  remove( individual_filename.c_str() );
  remove( household_filename.c_str() );
  sampsim::utilities::safe_delete( population );
  sampsim::utilities::safe_delete( population_read );
}
