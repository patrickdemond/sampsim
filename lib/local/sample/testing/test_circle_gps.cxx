/*=========================================================================

  Program:  sampsim
  Module:   test_sample_circle_gps.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the sample::circle_gps class
//

#include "UnitTest++.h"

#include "circle_gps_sample.h"
#include "common.h"
#include "population.h"
#include "utilities.h"

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_sample_circle_gps )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population );

  stringstream temp_population_filename;
  unsigned int random1 = sampsim::utilities::random( 1000000, 9999999 );
  temp_population_filename << "/tmp/sampsim" << random1;
  population->write( temp_population_filename.str(), false );
   
  cout << "Testing reading population from memory..." << endl;
  sampsim::sample::circle_gps *sample1 = new sampsim::sample::circle_gps;
  CHECK( sample1->set_population( population ) );

  cout << "Testing reading population from disk..." << endl;
  temp_population_filename << ".json.tar.gz";
  sampsim::sample::circle_gps *sample2 = new sampsim::sample::circle_gps;
  CHECK( sample2->set_population( temp_population_filename.str() ) );

  int sample_size = 100;
  sample1->set_number_of_samples( 1 );
  sample1->set_number_of_towns( 1 );
  sample1->set_size( sample_size );
  sample1->set_radius( 0.5 );
  sample1->set_one_per_household( true );
  sample1->generate();

  stringstream temp_sample_filename;
  unsigned int random2 = sampsim::utilities::random( 1000000, 9999999 );
  temp_sample_filename << "/tmp/sampsim" << random2;

  cout << "Testing writing sample to disk in csv format..." << endl;
  try { sample1->write( temp_sample_filename.str(), true ); }
  catch(...) { CHECK( false ); }
  stringstream command;
  command << "tar -zxvf " << temp_sample_filename.str() + ".flat.tar.gz -C /";
  sampsim::utilities::exec( command.str() );
  vector<string> parts;
  string individual_filename = temp_sample_filename.str() + ".individual.csv";
  command.str( "" );
  command.clear();
  command << "wc -l " << individual_filename;
  parts = sampsim::utilities::explode( sampsim::utilities::exec( command.str() ), " " );
  CHECK( 100 < atoi( parts[0].c_str() ) ); // at least 100 lines
  command.str( "" );
  command.clear();
  string household_filename = temp_sample_filename.str() + ".household.csv";
  command.str( "" );
  command.clear();
  command << "wc -l " << household_filename;
  parts = sampsim::utilities::explode( sampsim::utilities::exec( command.str() ), " " );
  CHECK( 100 < atoi( parts[0].c_str() ) ); // at least 100 lines

  cout << "Testing writing sample to disk in json format..." << endl;
  try { sample1->write( temp_sample_filename.str(), false ); }
  catch(...) { CHECK( false ); }

  // clean up
  command.str( "" );
  command.clear();
  command << "rm " << "/tmp/sampsim" << random1 << "* " << "/tmp/sampsim" << random2 << "*";
  sampsim::utilities::exec( command.str() );
  sampsim::utilities::safe_delete( population );
  sampsim::utilities::safe_delete( sample1 );
  sampsim::utilities::safe_delete( sample2 );
}
