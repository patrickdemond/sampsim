/*=========================================================================

  Program:  sampsim
  Module:   test_sample_strip_epi.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the sample::strip_epi class
//

#include "UnitTest++.h"

#include "strip_epi_sample.h"
#include "building.h"
#include "common.h"
#include "household.h"
#include "individual.h"
#include "population.h"
#include "tile.h"
#include "town.h"
#include "utilities.h"

#include <stdexcept>

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_sample_strip_epi )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population );

  stringstream temp_population_filename;
  temp_population_filename << "/tmp/sampsim" << sampsim::utilities::random( 1000000, 9999999 );
  population->write( temp_population_filename.str(), false );
   
  cout << "Testing reading population from memory..." << endl;
  sampsim::sample::strip_epi *sample1 = new sampsim::sample::strip_epi;
  CHECK( sample1->set_population( population ) );

  cout << "Testing reading population from disk..." << endl;
  temp_population_filename << ".json";
  sampsim::sample::strip_epi *sample2 = new sampsim::sample::strip_epi;
  CHECK( sample2->set_population( temp_population_filename.str() ) );

  int sample_size = 100;
  sample1->set_size( sample_size );
  sample1->set_strip_width( 0.25 );
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

  for( double start_angle = -M_PI; start_angle <= M_PI; start_angle += M_PI / 9 )
  {
    sample1->set_start_angle( start_angle );
    cout << "Testing strip at " << start_angle << " radians..." << endl;
    try
    {
      sample1->generate();
    }
    catch( runtime_error &e )
    {
      cout << "No households found, skipping test" << endl;
      continue;
    }

    cout << "Testing strip " << sample1->get_strip_width() << " kilometers wide at an angle of "
         << sample1->get_start_angle() << " radians..." << endl;

    // determine the slope and intercepts for the two lines defining the strip
    sampsim::coordinate centroid = sample1->get_first_building()->get_town()->get_centroid();
    sampsim::coordinate first_building_pos = sample1->get_first_building()->get_position();
    double m = tan( start_angle );
    sampsim::coordinate p0 = centroid;
    p0.x += sample1->get_strip_width() * sin( start_angle ) / 2;
    p0.y -= sample1->get_strip_width() * cos( start_angle ) / 2;
    sampsim::coordinate p1 = centroid;
    p1.x -= sample1->get_strip_width() * sin( start_angle ) / 2;
    p1.y += sample1->get_strip_width() * cos( start_angle ) / 2;
    double b0 = p0.y - m * p0.x;
    double b1 = p1.y - m * p1.x;

    // confirm that the intercept is between the strip line's intercepts
    double b = first_building_pos.y - m * first_building_pos.x;

    cout << b0 << ", " << b << ", " << b1 << endl;
    if( b0 < b1 )
    {
      CHECK( b >= b0 );
      CHECK( b < b1 );
    }
    else
    {
      CHECK( b <= b0 );
      CHECK( b > b1 );
    }

    cout << "Testing adult-female only sampling..." << endl;
    sampsim::age_type age = sampsim::get_age_type( "adult" );
    sampsim::sex_type sex = sampsim::get_sex_type( "female" );
    sample1->set_age( age );
    sample1->set_sex( sex );
    sample1->set_one_per_household( false );
    sample1->generate();

    int individual_count = 0;
    int household_count = 0;
    for( auto town_it = population->get_town_list_begin();
         town_it != population->get_town_list_end();
         ++town_it )
    {
      sampsim::town *town = *town_it;
      for( auto tile_it = town->get_tile_list_begin();
           tile_it != town->get_tile_list_end();
           ++tile_it )
      {
        sampsim::tile *tile = tile_it->second;
        for( auto building_it = tile->get_building_list_cbegin();
             building_it != tile->get_building_list_cend();
             ++building_it )
        {
          sampsim::building *building = *building_it;
          if( building->is_selected() )
          {
            for( auto household_it = building->get_household_list_cbegin();
                 household_it != building->get_household_list_cend();
                 ++household_it )
            {
              sampsim::household *household = *household_it;
              if( household->is_selected() )
              {
                household_count++;
                for( auto individual_it = household->get_individual_list_cbegin();
                     individual_it != household->get_individual_list_cend();
                     ++individual_it )
                {
                  sampsim::individual *individual = *individual_it;
                  if( individual->is_selected() )
                  {
                    CHECK_EQUAL( age, individual->get_age() );
                    CHECK_EQUAL( sex, individual->get_sex() );
                    individual_count++;
                  }
                }
              }
            }
          }
        }
      }
    }
    cout << "Result: " << individual_count << " individuals in " << household_count << " households" << endl;
    CHECK_EQUAL( sample_size, individual_count );
    CHECK_EQUAL( sample_size, household_count );

    cout << "Testing child-male only sampling..." << endl;
    age = sampsim::get_age_type( "child" );
    sex = sampsim::get_sex_type( "male" );
    sample1->set_age( age );
    sample1->set_sex( sex );
    sample1->set_one_per_household( false );
    sample1->generate();

    individual_count = 0;
    household_count = 0;
    for( auto town_it = population->get_town_list_begin();
         town_it != population->get_town_list_end();
         ++town_it )
    {
      sampsim::town *town = *town_it;
      for( auto tile_it = town->get_tile_list_begin();
           tile_it != town->get_tile_list_end();
           ++tile_it )
      {
        sampsim::tile *tile = tile_it->second;
        for( auto building_it = tile->get_building_list_cbegin();
             building_it != tile->get_building_list_cend();
             ++building_it )
        {
          sampsim::building *building = *building_it;
          if( building->is_selected() )
          {
            for( auto household_it = building->get_household_list_cbegin();
                 household_it != building->get_household_list_cend();
                 ++household_it )
            {
              sampsim::household *household = *household_it;
              if( household->is_selected() )
              {
                household_count++;
                for( auto individual_it = household->get_individual_list_cbegin();
                     individual_it != household->get_individual_list_cend();
                     ++individual_it )
                {
                  sampsim::individual *individual = *individual_it;
                  if( individual->is_selected() )
                  {
                    CHECK_EQUAL( age, individual->get_age() );
                    CHECK_EQUAL( sex, individual->get_sex() );
                    individual_count++;
                  }
                }
              }
            }
          }
        }
      }
    }
    cout << "Result: " << individual_count << " individuals in " << household_count << " households" << endl;
    CHECK( sample_size <= individual_count );
    CHECK( sample_size > household_count );
  
    cout << "Testing one per household sampling..." << endl;
    sample1->set_age( sampsim::get_age_type( "either" ) );
    sample1->set_sex( sampsim::get_sex_type( "either" ) );
    sample1->set_one_per_household( true );
    sample1->generate();

    individual_count = 0;
    household_count = 0;
    for( auto town_it = population->get_town_list_begin();
         town_it != population->get_town_list_end();
         ++town_it )
    {
      sampsim::town *town = *town_it;
      for( auto tile_it = town->get_tile_list_begin();
           tile_it != town->get_tile_list_end();
           ++tile_it )
      {
        sampsim::tile *tile = tile_it->second;
        for( auto building_it = tile->get_building_list_cbegin();
             building_it != tile->get_building_list_cend();
             ++building_it )
        {
          sampsim::building *building = *building_it;
          if( building->is_selected() )
          {
            for( auto household_it = building->get_household_list_cbegin();
                 household_it != building->get_household_list_cend();
                 ++household_it )
            {
              sampsim::household *household = *household_it;
              if( household->is_selected() )
              {
                household_count++;
                int household_individual_count = 0;
                for( auto individual_it = household->get_individual_list_cbegin();
                     individual_it != household->get_individual_list_cend();
                     ++individual_it )
                {
                  sampsim::individual *individual = *individual_it;
                  if( individual->is_selected() )
                  {
                    individual_count++;
                    household_individual_count++;
                  }
                }
                CHECK_EQUAL( 1, household_individual_count );
              }
            }
          }
        }
      }
    }
    cout << "Result: " << individual_count << " individuals in " << household_count << " households" << endl;
    CHECK_EQUAL( sample_size, individual_count );
    CHECK_EQUAL( sample_size, household_count );
  }

  // clean up
  remove( temp_population_filename.str().c_str() );
  remove( temp_sample_filename.str().c_str() );
  remove( individual_filename.c_str() );
  remove( household_filename.c_str() );
  sampsim::utilities::safe_delete( population );
  sampsim::utilities::safe_delete( sample1 );
  sampsim::utilities::safe_delete( sample2 );
}
