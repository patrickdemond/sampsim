/*=========================================================================

  Program:  sampsim
  Module:   test_sample_arc_epi.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the sample::arc_epi class
//

#include "UnitTest++.h"

#include "arc_epi_sample.h"
#include "building.h"
#include "common.h"
#include "household.h"
#include "individual.h"
#include "population.h"
#include "tile.h"
#include "town.h"
#include "utilities.h"

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_sample_arc_epi )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population );

  stringstream temp_population_filename;
  unsigned int random1 = sampsim::utilities::random( 1000000, 9999999 );
  temp_population_filename << "/tmp/sampsim" << random1;
  population->write( temp_population_filename.str(), false );
   
  cout << "Testing reading population from memory..." << endl;
  sampsim::sample::arc_epi *sample1 = new sampsim::sample::arc_epi;
  CHECK( sample1->set_population( population ) );

  cout << "Testing reading population from disk..." << endl;
  temp_population_filename << ".json.tar.gz";
  sampsim::sample::arc_epi *sample2 = new sampsim::sample::arc_epi;
  CHECK( sample2->set_population( temp_population_filename.str() ) );

  int sample_size = 100;
  sample1->set_number_of_samples( 1 );
  sample1->set_number_of_towns( 1 );
  sample1->set_use_quadrants( false );
  sample1->set_size( sample_size );
  sample1->set_arc_angle( M_PI / 30 );
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

  for( double start_angle = -M_PI; start_angle <= M_PI; start_angle += M_PI / 9 )
  {
    sampsim::sample::arc_epi *sample = new sampsim::sample::arc_epi;
    CHECK( sample->set_population( population ) );
    sample->set_number_of_samples( 1 );
    sample->set_number_of_towns( 1 );
    sample->set_use_quadrants( false );
    sample->set_size( sample_size );
    sample->set_arc_angle( M_PI / 30 );
    sample->set_one_per_household( true );

    sample->set_start_angle( start_angle );
    double angle1 = sample->get_start_angle() - sample->get_arc_angle() / 2.0;
    double angle2 = sample->get_start_angle() + sample->get_arc_angle() / 2.0;
    sample->generate();

    // make sure a building was chosen
    sampsim::building *first_building = sample->get_first_building();
    CHECK( first_building );

    if( first_building )
    {
      double a = first_building->get_position().get_a();
      if( -M_PI > angle1 || M_PI < angle2 )
      {
        if( 0 < a )
        {
          CHECK( angle1 <= a );
          CHECK( M_PI >= a );
        }
        else
        {
          CHECK( -M_PI <= a );
          CHECK( angle2 > a );
        }
      }
      else
      {
        CHECK( a >= angle1 );
        CHECK( a < angle2 );
      }
    }
    sampsim::utilities::safe_delete( sample );
  }

  cout << "Testing adult-female only sampling..." << endl;
  sampsim::age_type age = sampsim::get_age_type( "adult" );
  sampsim::sex_type sex = sampsim::get_sex_type( "female" );
  population->unselect();
  sampsim::sample::arc_epi *sample3 = new sampsim::sample::arc_epi;
  CHECK( sample3->set_population( population ) );
  sample3->set_number_of_samples( 1 );
  sample3->set_number_of_towns( 1 );
  sample3->set_use_quadrants( false );
  sample3->set_size( sample_size );
  sample3->set_arc_angle( M_PI / 30 );
  sample3->set_age( age );
  sample3->set_sex( sex );
  sample3->set_one_per_household( false );
  sample3->generate();

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
  population->unselect();
  sampsim::sample::arc_epi *sample4 = new sampsim::sample::arc_epi;
  CHECK( sample4->set_population( population ) );
  sample4->set_number_of_samples( 1 );
  sample4->set_number_of_towns( 1 );
  sample4->set_use_quadrants( false );
  sample4->set_size( sample_size );
  sample4->set_arc_angle( M_PI / 30 );
  sample4->set_age( age );
  sample4->set_sex( sex );
  sample4->set_one_per_household( false );
  sample4->generate();

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
  population->unselect();
  sampsim::sample::arc_epi *sample5 = new sampsim::sample::arc_epi;
  CHECK( sample5->set_population( population ) );
  sample5->set_number_of_samples( 1 );
  sample5->set_number_of_towns( 1 );
  sample5->set_use_quadrants( false );
  sample5->set_size( sample_size );
  sample5->set_arc_angle( M_PI / 30 );
  sample5->set_age( sampsim::get_age_type( "either" ) );
  sample5->set_sex( sampsim::get_sex_type( "either" ) );
  sample5->set_one_per_household( true );
  sample5->generate();

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

  // clean up
  command.str( "" );
  command.clear();
  command << "rm " << "/tmp/sampsim" << random1 << "* " << "/tmp/sampsim" << random2 << "*";
  sampsim::utilities::exec( command.str() );
  sampsim::utilities::safe_delete( population );
  sampsim::utilities::safe_delete( sample1 );
  sampsim::utilities::safe_delete( sample2 );
  sampsim::utilities::safe_delete( sample3 );
  sampsim::utilities::safe_delete( sample4 );
  sampsim::utilities::safe_delete( sample5 );
}
