/*=========================================================================

  Program:  sampsim
  Module:   test_sample_square_gps.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the sample::square_gps class
//

#include "UnitTest++.h"

#include "square_gps_sample.h"
#include "common.h"
#include "population.h"
#include "utilities.h"

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_sample_square_gps )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population );

  stringstream temp_population_filename;
  temp_population_filename << "/tmp/sampsim" << sampsim::utilities::random( 1000000, 9999999 );
  population->write( temp_population_filename.str(), false );
   
  cout << "Testing reading population from memory..." << endl;
  sampsim::sample::square_gps *sample1 = new sampsim::sample::square_gps;
  CHECK( sample1->set_population( population ) );

  cout << "Testing reading population from disk..." << endl;
  temp_population_filename << ".json";
  sampsim::sample::square_gps *sample2 = new sampsim::sample::square_gps;
  CHECK( sample2->set_population( temp_population_filename.str() ) );

  int sample_size = 100;
  sample1->set_number_of_samples( 1 );
  sample1->set_size( sample_size );
  sample1->set_number_of_squares( 64 );
  sample1->set_one_per_household( true );
  sample1->generate();

  stringstream temp_sample_filename;
  temp_sample_filename << "/tmp/sampsim" << sampsim::utilities::random( 1000000, 9999999 );

  cout << "Testing writing sample to disk in csv format..." << endl;
  try { sample1->write( temp_sample_filename.str(), true ); }
  catch(...) { CHECK( false ); }
  stringstream command;
  vector<string> parts;
  string individual_filename = temp_sample_filename.str() + ".individual.csv";
  command << "wc -l " << individual_filename;
  parts = sampsim::utilities::explode( sampsim::utilities::exec( command.str() ), " " );
  CHECK( 100 < atoi( parts[0].c_str() ) ); // at least 100 lines
  command.str( "" );
  command.clear();
  string household_filename = temp_sample_filename.str() + ".household.csv";
  command << "wc -l " << household_filename;
  parts = sampsim::utilities::explode( sampsim::utilities::exec( command.str() ), " " );
  CHECK( 100 < atoi( parts[0].c_str() ) ); // at least 100 lines

  cout << "Testing writing sample to disk in json format..." << endl;
  try { sample1->write( temp_sample_filename.str(), false ); }
  catch(...) { CHECK( false ); }
  temp_sample_filename << ".json";

  // clean up
  remove( temp_population_filename.str().c_str() );
  remove( temp_sample_filename.str().c_str() );
  remove( individual_filename.c_str() );
  remove( household_filename.c_str() );
  sampsim::utilities::safe_delete( population );
  sampsim::utilities::safe_delete( sample1 );
  sampsim::utilities::safe_delete( sample2 );
}
