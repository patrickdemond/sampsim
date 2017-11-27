/*=========================================================================

  Program:  sampsim
  Module:   test_household.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the household class
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
#include "utilities.h"

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_household )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population, 8, 100, 250 );
  sampsim::summary *sum;

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
        for( auto household_it = building->get_household_list_cbegin();
             household_it != building->get_household_list_cend();
             ++household_it )
        {
          sampsim::household *household = *household_it;
          sampsim::individual *individual = *household->get_individual_list_begin();

          cout << "Testing household population..." << endl;
          sum = household->get_summary();
          for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ ) CHECK( 0 != sum->get_count( rr ) );

          cout << "Turning on sample mode" << endl;
          population->set_sample_mode( true );

          cout << "Testing that household now has no population..." << endl;
          sum = household->get_summary();
          for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ )
          {
            CHECK_EQUAL( 0, sum->get_count( rr, ANY_AGE, ANY_SEX ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ANY_AGE, MALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ANY_AGE, FEMALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ADULT, ANY_SEX ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ADULT, MALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ADULT, FEMALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, CHILD, ANY_SEX ) );
            CHECK_EQUAL( 0, sum->get_count( rr, CHILD, MALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, CHILD, FEMALE ) );
          }

          cout << "Testing that household with selected individual has population..." << endl;
          individual->select();
          sum = household->get_summary();
          for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ ) CHECK( 0 != sum->get_count( rr ) );

          cout << "Testing that household with unselected individual has no population..." << endl;
          individual->unselect();
          sum = household->get_summary();
          for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ )
          {
            CHECK_EQUAL( 0, sum->get_count( rr, ANY_AGE, ANY_SEX ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ANY_AGE, MALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ANY_AGE, FEMALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ADULT, ANY_SEX ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ADULT, MALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ADULT, FEMALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, CHILD, ANY_SEX ) );
            CHECK_EQUAL( 0, sum->get_count( rr, CHILD, MALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, CHILD, FEMALE ) );
          }

          cout << "Testing that unselected household has no population..." << endl;
          individual->select();
          household->unselect();
          sum = household->get_summary();
          for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ )
          {
            CHECK_EQUAL( 0, sum->get_count( rr, ANY_AGE, ANY_SEX ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ANY_AGE, MALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ANY_AGE, FEMALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ADULT, ANY_SEX ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ADULT, MALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, ADULT, FEMALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, CHILD, ANY_SEX ) );
            CHECK_EQUAL( 0, sum->get_count( rr, CHILD, MALE ) );
            CHECK_EQUAL( 0, sum->get_count( rr, CHILD, FEMALE ) );
          }

          cout << "Turning off sample mode" << endl;
          population->set_sample_mode( false );

          cout << "Testing household population..." << endl;
          sum = household->get_summary();
          for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ ) CHECK( 0 != sum->get_count( rr ) );
        }
      }
    }
  }

  // clean up
  sampsim::utilities::safe_delete( population );
}
