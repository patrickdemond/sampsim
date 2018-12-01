/*=========================================================================

  Program:  sampsim
  Module:   test_sample_enum.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the sample::enum class
//

#include "UnitTest++.h"

#include "enumeration_sample.h"
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

TEST( test_sample_enumeration )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population, 250, 250, 25000 );

  stringstream temp_population_filename;
  unsigned int random1 = sampsim::utilities::random( 1000000, 9999999 );
  temp_population_filename << "/tmp/sampsim" << random1;
  population->write( temp_population_filename.str(), false );
   
  cout << "Testing reading population from memory..." << endl;
  sampsim::sample::enumeration *sample1 = new sampsim::sample::enumeration;
  CHECK( sample1->set_population( population ) );

  sample1->set_number_of_samples( 100 );
  sample1->set_number_of_towns( 30 );
  sample1->set_size( 30 );
  sample1->set_age( sampsim::get_age_type( "child" ) );
  sample1->set_threshold( 100 );
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
      for( unsigned int rr = 0; rr < sampsim::utilities::rr.size(); rr++ ) CHECK( 10 <= town->get_summary()->get_count( rr ) );
      number_of_towns++;
    }

    // some towns are selected twice, so there may be less than the total, but never more
    CHECK( number_of_towns <= sample1->get_number_of_towns() );
    number_of_samples++;
  }

  CHECK_EQUAL( number_of_samples, sample1->get_number_of_samples() );

  sampsim::summary *sum = sample1->get_population()->get_summary();
  for( unsigned int rr = 0; rr < sampsim::utilities::rr.size(); rr++ )
    CHECK( ( sample1->get_number_of_towns() * sample1->get_size() ) <= sum->get_count( rr ) );

  // clean up
  stringstream command;
  command << "rm " << "/tmp/sampsim" << random1 << "*";
  sampsim::utilities::exec( command.str() );
  sampsim::utilities::safe_delete( population );
  sampsim::utilities::safe_delete( sample1 );
}
