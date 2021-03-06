/*=========================================================================

  Program:  sampsim
  Module:   test_building.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the building class
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

TEST( test_building )
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
      std::pair< sampsim::coordinate, sampsim::coordinate > extent = tile->get_extent();
      cout << "Tile extent is " << extent.first << " to " << extent.second << endl;
      for( auto building_it = tile->get_building_list_begin();
           building_it != tile->get_building_list_end();
           ++building_it )
      {
        sampsim::building *building = *building_it;
        sampsim::household *household = *building->get_household_list_begin();
        sampsim::individual *individual = *household->get_individual_list_begin();
        sampsim::coordinate position = building->get_position();

        cout << "Testing position of building at " << position << "..." << endl;
        CHECK( position.x >= extent.first.x );
        CHECK( position.x < extent.second.x );
        CHECK( position.y >= extent.first.y );
        CHECK( position.y < extent.second.y );

        cout << "Testing building population..." << endl;
        sum = building->get_summary();
        for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ ) CHECK( 0 != sum->get_count( rr ) );

        cout << "Turning on sample mode" << endl;
        population->set_sample_mode( true );

        cout << "Testing that building now has no population..." << endl;
        sum = building->get_summary();
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

        cout << "Testing that building with selected individual has population..." << endl;
        individual->select();
        sum = building->get_summary();
        for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ ) CHECK( 0 != sum->get_count( rr ) );

        cout << "Testing that building with unselected individual has no population..." << endl;
        individual->unselect();
        sum = building->get_summary();
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

        cout << "Testing that unselected building has no population..." << endl;
        individual->select();
        building->unselect();
        sum = building->get_summary();
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

        cout << "Testing building population..." << endl;
        sum = building->get_summary();
        for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ )
        {
          CHECK( 0 != sum->get_count( rr ) );
          CHECK( 0 != sum->get_count( rr, ADULT ) );
          if( 2 < sum->get_count( rr ) ) CHECK( 0 != sum->get_count( rr, CHILD ) );
          if( 1 < sum->get_count( rr ) )
          {
            CHECK( 0 != sum->get_count( rr, ANY_AGE, MALE ) );
            CHECK( 0 != sum->get_count( rr, ANY_AGE, FEMALE ) );
          }
        }
      }
    }
  }

  // clean up
  sampsim::utilities::safe_delete( population );
}
