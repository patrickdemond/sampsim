/*=========================================================================

  Program:  sampsim
  Module:   test_sample_vanilla.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the sample::random class
//

#include "UnitTest++.h"

#include "arc_epi_sample.h"
#include "circle_gps_sample.h"
#include "random_sample.h"
#include "square_gps_sample.h"
#include "strip_epi_sample.h"
#include "population.h"
#include "trend.h"
#include "utilities.h"

#include <cmath>

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_sample_vanilla )
{
  // create the vanilla population
  sampsim::population *population = new sampsim::population;

  sampsim::utilities::verbose = false;
  stringstream stream;
  stream << time( NULL );
  population->set_seed( stream.str() );
  population->set_number_of_towns( 1 );
  population->set_town_size_min( 10000 );
  population->set_town_size_max( 10000 );
  population->set_town_size_shape( 1.0 );
  population->set_tile_width( 1 );
  population->set_number_of_tiles_x( 10 );
  population->set_number_of_tiles_y( 10 );
  population->set_population_density_slope( 0, 0 );
  population->set_river_probability( 0 );
  population->set_number_of_disease_pockets( 0 );
  population->set_disease_weights( 0.0, 0.0, 1.0, 0.0, 0.0, 0.0 );
  population->set_mean_household_population( 3 );

  sampsim::trend *mean_income = population->get_mean_income();
  mean_income->set_b00( 1.0 );
  mean_income->set_b01( 0.0 );
  mean_income->set_b10( 0.0 );
  mean_income->set_b02( 0.0 );
  mean_income->set_b20( 0.0 );
  mean_income->set_b11( 0.0 );

  sampsim::trend *sd_income = population->get_sd_income();
  sd_income->set_b00( 0.0 );
  sd_income->set_b01( 0.0 );
  sd_income->set_b10( 0.0 );
  sd_income->set_b02( 0.0 );
  sd_income->set_b20( 0.0 );
  sd_income->set_b11( 0.0 );

  sampsim::trend *mean_disease = population->get_mean_disease();
  mean_disease->set_b00( 1.0 );
  mean_disease->set_b01( 0.0 );
  mean_disease->set_b10( 0.0 );
  mean_disease->set_b02( 0.0 );
  mean_disease->set_b20( 0.0 );
  mean_disease->set_b11( 0.0 );

  sampsim::trend *sd_disease = population->get_sd_disease();
  sd_disease->set_b00( 0.0 );
  sd_disease->set_b01( 0.0 );
  sd_disease->set_b10( 0.0 );
  sd_disease->set_b02( 0.0 );
  sd_disease->set_b20( 0.0 );
  sd_disease->set_b11( 0.0 );

  population->generate();

  pair<unsigned int, unsigned int> count;

  count = population->count_individuals();
  double diseased_fraction = static_cast<double>( count.first ) / static_cast<double>( count.second );
  cout << "Making sure that the disease prevalence, " << diseased_fraction << ", is near 0.5..." << endl;
  CHECK_EQUAL( 0.5, round( 10 * diseased_fraction ) / 10 );

  unsigned int sample_size_list[3] = { 10, 100, 1000 };

  // ARC EPI SAMPLING
  for( int i = 0; i < 3; i++ )
  {
    sampsim::sample::arc_epi *arc_epi_sample = new sampsim::sample::arc_epi;
    arc_epi_sample->set_population( population );
    arc_epi_sample->set_number_of_samples( 100 );
    arc_epi_sample->set_size( sample_size_list[i] );
    arc_epi_sample->set_arc_angle( M_PI / 30 );
    arc_epi_sample->set_age( sampsim::get_age_type( "child" ) );
    arc_epi_sample->generate();

    unsigned int sampled_diseased = 0;
    unsigned int sampled_total = 0;
    for( auto it = arc_epi_sample->get_sampled_population_list_cbegin();
         it != arc_epi_sample->get_sampled_population_list_cend();
         ++it )
    {
      count = (*it)->count_individuals();
      sampled_diseased += count.first;
      sampled_total += count.second;
    }

    diseased_fraction = static_cast<double>( sampled_diseased ) / static_cast<double>( sampled_total );
    cout << "Making sure that the disease prevalence, " << diseased_fraction << ", is near 0.5..." << endl;
    CHECK_EQUAL( 0.5, round( 10 * diseased_fraction ) / 10 );

    // determine the standard deviation
    double squared_sum = 0.0;
    for( auto it = arc_epi_sample->get_sampled_population_list_cbegin();
         it != arc_epi_sample->get_sampled_population_list_cend();
         ++it )
    {
      count = (*it)->count_individuals();
      double diff = ( static_cast<double>( count.first ) / static_cast<double>( count.second ) ) -
                    diseased_fraction;
      squared_sum += diff*diff;
    }

    double std = sqrt( squared_sum / 100 );
    cout << "Arc EPI (100 samples if size " << sample_size_list[i]
         << ") prevalence: " << diseased_fraction << " (s = " << std << ")" << endl;

    sampsim::utilities::safe_delete( arc_epi_sample );
  }

  // STRIP EPI SAMPLING
  for( int i = 0; i < 3; i++ )
  {
    sampsim::sample::strip_epi *strip_epi_sample = new sampsim::sample::strip_epi;
    strip_epi_sample->set_population( population );
    strip_epi_sample->set_number_of_samples( 100 );
    strip_epi_sample->set_size( sample_size_list[i] );
    strip_epi_sample->set_strip_width( 100 );
    strip_epi_sample->set_age( sampsim::get_age_type( "child" ) );
    strip_epi_sample->generate();

    unsigned int sampled_diseased = 0;
    unsigned int sampled_total = 0;
    for( auto it = strip_epi_sample->get_sampled_population_list_cbegin();
         it != strip_epi_sample->get_sampled_population_list_cend();
         ++it )
    {
      count = (*it)->count_individuals();
      sampled_diseased += count.first;
      sampled_total += count.second;
    }

    diseased_fraction = static_cast<double>( sampled_diseased ) / static_cast<double>( sampled_total );
    cout << "Making sure that the disease prevalence, " << diseased_fraction << ", is near 0.5..." << endl;
    CHECK_EQUAL( 0.5, round( 10 * diseased_fraction ) / 10 );

    // determine the standard deviation
    double squared_sum = 0.0;
    for( auto it = strip_epi_sample->get_sampled_population_list_cbegin();
         it != strip_epi_sample->get_sampled_population_list_cend();
         ++it )
    {
      count = (*it)->count_individuals();
      double diff = ( static_cast<double>( count.first ) / static_cast<double>( count.second ) ) -
                    diseased_fraction;
      squared_sum += diff*diff;
    }

    double std = sqrt( squared_sum / 100 );
    cout << "Strip EPI (100 samples if size " << sample_size_list[i]
         << ") prevalence: " << diseased_fraction << " (s = " << std << ")" << endl;

    sampsim::utilities::safe_delete( strip_epi_sample );
  }

  // SQUARE GPS SAMPLING
  for( int i = 0; i < 3; i++ )
  {
    sampsim::sample::square_gps *square_gps_sample = new sampsim::sample::square_gps;
    square_gps_sample->set_population( population );
    square_gps_sample->set_number_of_samples( 100 );
    square_gps_sample->set_size( sample_size_list[i] );
    square_gps_sample->set_number_of_squares( 32 );
    square_gps_sample->set_age( sampsim::get_age_type( "child" ) );
    square_gps_sample->generate();

    unsigned int sampled_diseased = 0;
    unsigned int sampled_total = 0;
    for( auto it = square_gps_sample->get_sampled_population_list_cbegin();
         it != square_gps_sample->get_sampled_population_list_cend();
         ++it )
    {
      count = (*it)->count_individuals();
      sampled_diseased += count.first;
      sampled_total += count.second;
    }

    diseased_fraction = static_cast<double>( sampled_diseased ) / static_cast<double>( sampled_total );
    cout << "Making sure that the disease prevalence, " << diseased_fraction << ", is near 0.5..." << endl;
    CHECK_EQUAL( 0.5, round( 10 * diseased_fraction ) / 10 );

    // determine the standard deviation
    double squared_sum = 0.0;
    for( auto it = square_gps_sample->get_sampled_population_list_cbegin();
         it != square_gps_sample->get_sampled_population_list_cend();
         ++it )
    {
      count = (*it)->count_individuals();
      double diff = ( static_cast<double>( count.first ) / static_cast<double>( count.second ) ) -
                    diseased_fraction;
      squared_sum += diff*diff;
    }

    double std = sqrt( squared_sum / 100 );
    cout << "Square GPS (100 samples if size " << sample_size_list[i]
         << ") prevalence: " << diseased_fraction << " (s = " << std << ")" << endl;

    sampsim::utilities::safe_delete( square_gps_sample );
  }

  // CIRCLE GPS SAMPLING
  for( int i = 0; i < 3; i++ )
  {
    sampsim::sample::circle_gps *circle_gps_sample = new sampsim::sample::circle_gps;
    circle_gps_sample->set_population( population );
    circle_gps_sample->set_number_of_samples( 100 );
    circle_gps_sample->set_size( sample_size_list[i] );
    circle_gps_sample->set_radius( 0.1 );
    circle_gps_sample->set_age( sampsim::get_age_type( "child" ) );
    circle_gps_sample->generate();

    unsigned int sampled_diseased = 0;
    unsigned int sampled_total = 0;
    for( auto it = circle_gps_sample->get_sampled_population_list_cbegin();
         it != circle_gps_sample->get_sampled_population_list_cend();
         ++it )
    {
      count = (*it)->count_individuals();
      sampled_diseased += count.first;
      sampled_total += count.second;
    }

    diseased_fraction = static_cast<double>( sampled_diseased ) / static_cast<double>( sampled_total );
    cout << "Making sure that the disease prevalence, " << diseased_fraction << ", is near 0.5..." << endl;
    CHECK_EQUAL( 0.5, round( 10 * diseased_fraction ) / 10 );

    // determine the standard deviation
    double squared_sum = 0.0;
    for( auto it = circle_gps_sample->get_sampled_population_list_cbegin();
         it != circle_gps_sample->get_sampled_population_list_cend();
         ++it )
    {
      count = (*it)->count_individuals();
      double diff = ( static_cast<double>( count.first ) / static_cast<double>( count.second ) ) -
                    diseased_fraction;
      squared_sum += diff*diff;
    }

    double std = sqrt( squared_sum / 100 );
    cout << "Circle GPS (100 samples if size " << sample_size_list[i]
         << ") prevalence: " << diseased_fraction << " (s = " << std << ")" << endl;

    sampsim::utilities::safe_delete( circle_gps_sample );
  }

  // RANDOM SAMPLING
  for( int i = 0; i < 3; i++ )
  {
    sampsim::sample::random *random_sample = new sampsim::sample::random;
    random_sample->set_population( population );
    random_sample->set_number_of_samples( 100 );
    random_sample->set_size( sample_size_list[i] );
    random_sample->set_age( sampsim::get_age_type( "child" ) );
    random_sample->generate();

    unsigned int sampled_diseased = 0;
    unsigned int sampled_total = 0;
    for( auto it = random_sample->get_sampled_population_list_cbegin();
         it != random_sample->get_sampled_population_list_cend();
         ++it )
    {
      count = (*it)->count_individuals();
      sampled_diseased += count.first;
      sampled_total += count.second;
    }

    diseased_fraction = static_cast<double>( sampled_diseased ) / static_cast<double>( sampled_total );
    cout << "Making sure that the disease prevalence, " << diseased_fraction << ", is near 0.5..." << endl;
    CHECK_EQUAL( 0.5, round( 10 * diseased_fraction ) / 10 );

    // determine the standard deviation
    double squared_sum = 0.0;
    for( auto it = random_sample->get_sampled_population_list_cbegin();
         it != random_sample->get_sampled_population_list_cend();
         ++it )
    {
      count = (*it)->count_individuals();
      double diff = ( static_cast<double>( count.first ) / static_cast<double>( count.second ) ) -
                    diseased_fraction;
      squared_sum += diff*diff;
    }

    double std = sqrt( squared_sum / 100 );
    cout << "Random (100 samples if size " << sample_size_list[i]
         << ") prevalence: " << diseased_fraction << " (s = " << std << ")" << endl;

    sampsim::utilities::safe_delete( random_sample );
  }

  sampsim::utilities::safe_delete( population );
}
