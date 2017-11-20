/*=========================================================================

  Program:  sampsim
  Module:   test_sample_random.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the sample::random class
//

#include "UnitTest++.h"

#include "random_sample.h"
#include "building.h"
#include "common.h"
#include "household.h"
#include "individual.h"
#include "population.h"
#include "summary.h"
#include "tile.h"
#include "town.h"
#include "utilities.h"

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_sample_random )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population, 250, 250, 2500 );

  stringstream temp_population_filename;
  unsigned int random1 = sampsim::utilities::random( 1000000, 9999999 );
  temp_population_filename << "/tmp/sampsim" << random1;
  population->write( temp_population_filename.str(), false );
   
  cout << "Testing reading population from memory..." << endl;
  sampsim::sample::random *sample1 = new sampsim::sample::random;
  CHECK( sample1->set_population( population ) );

  cout << "Testing reading population from disk..." << endl;
  temp_population_filename << ".json.tar.gz";
  sampsim::sample::random *sample2 = new sampsim::sample::random;
  CHECK( sample2->set_population( temp_population_filename.str() ) );

  sample1->set_number_of_samples( 100 );
  sample1->set_number_of_towns( 30 );
  sample1->set_size( 30 );
  sample1->set_age( sampsim::get_age_type( "child" ) );
  sample1->generate();

  // count towns, individuals in the population
  unsigned int number_of_samples = 0;
  for( auto pop_it = sample1->get_sampled_population_list_cbegin();
            pop_it != sample1->get_sampled_population_list_cend();
            ++pop_it )
  {
    sampsim::population *pop = *pop_it;
    unsigned int number_of_towns = 0;
    for( auto it = pop->get_town_list_cbegin(); it != pop->get_town_list_cend(); ++it )
    {
      sampsim::town *town = *it;
      for( unsigned int rr = 0; rr < sampsim::utilities::rr_size; rr++ )
        CHECK( 10 <= town->get_summary()->get_count( rr ) );
      number_of_towns++;
    }
    CHECK_EQUAL( number_of_towns, sample1->get_number_of_towns() );
    number_of_samples++;
  }

  CHECK_EQUAL( number_of_samples, sample1->get_number_of_samples() );

  sampsim::summary *sum = sample1->get_population()->get_summary();
  for( unsigned int rr = 0; rr < sampsim::utilities::rr_size; rr++ )
    CHECK( ( sample1->get_number_of_towns() * sample1->get_size() ) <= sum->get_count( rr ) );

  // clean up
  stringstream command;
  command << "rm " << "/tmp/sampsim" << random1 << "*";
  sampsim::utilities::exec( command.str() );
  sampsim::utilities::safe_delete( population );
  sampsim::utilities::safe_delete( sample1 );
  sampsim::utilities::safe_delete( sample2 );
}
