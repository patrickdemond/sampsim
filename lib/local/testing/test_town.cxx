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
#include "utilities.h"

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_population )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population );
  sampsim::summary *sum;

  for( auto town_it = population->get_town_list_begin();
       town_it != population->get_town_list_end();
       ++town_it )
  {
    sampsim::town *town = *town_it;
    sampsim::tile *tile = town->get_tile_list_begin()->second;
    sampsim::building *building = *tile->get_building_list_begin();
    sampsim::household *household = *building->get_household_list_begin();
    sampsim::individual *individual = *household->get_individual_list_begin();

    cout << "Testing town size..." << endl;
    sum = town->get_summary();
    CHECK( 0 != sum->get_count( sampsim::ANY_AGE, sampsim::ANY_SEX ) );
    CHECK( 0 != sum->get_count( sampsim::ANY_AGE, sampsim::MALE ) );
    CHECK( 0 != sum->get_count( sampsim::ANY_AGE, sampsim::FEMALE ) );
    CHECK( 0 != sum->get_count( sampsim::ADULT, sampsim::ANY_SEX ) );
    CHECK( 0 != sum->get_count( sampsim::ADULT, sampsim::MALE ) );
    CHECK( 0 != sum->get_count( sampsim::ADULT, sampsim::FEMALE ) );
    CHECK( 0 != sum->get_count( sampsim::CHILD, sampsim::ANY_SEX ) );
    CHECK( 0 != sum->get_count( sampsim::CHILD, sampsim::MALE ) );
    CHECK( 0 != sum->get_count( sampsim::CHILD, sampsim::FEMALE ) );

    cout << "Testing town prevalence..." << endl;
    for( age_type a = sampsim::ADULT; a <= sampsim::CHILD; a++ )
      for( sex_type s = sampsim::MALE; s <= sampsim::FEMALE; s++ )
        CHECK( 0 != sum->get_count( a, s, DISEASED ) );

    cout << "Turning on sample mode" << endl;
    population->set_sample_mode( true );

    cout << "Testing that town now has a count of zero..." << endl;
    sum = town->get_summary();
    CHECK_EQUAL( 0, sum->get_count( sampsim::ANY_AGE, sampsim::ANY_SEX ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::ANY_AGE, sampsim::MALE ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::ANY_AGE, sampsim::FEMALE ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::ADULT, sampsim::ANY_SEX ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::ADULT, sampsim::MALE ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::ADULT, sampsim::FEMALE ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::CHILD, sampsim::ANY_SEX ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::CHILD, sampsim::MALE ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::CHILD, sampsim::FEMALE ) );

    cout << "Testing that town prevalence now has a count of zero..." << endl;
    for( age_type a = sampsim::ADULT; a <= sampsim::CHILD; a++ )
      for( sex_type s = sampsim::MALE; s <= sampsim::FEMALE; s++ )
        CHECK_EQUAL( 0, sum->get_count( a, s, DISEASED ) );

    cout << "Testing that town with selected individual has non-zero count..." << endl;
    individual->select();
    sum = town->get_summary();
    CHECK( 0 != sum->get_count() );

    cout << "Testing that town with unselected individual has a count of zero..." << endl;
    individual->unselect();
    sum = town->get_summary();
    CHECK_EQUAL( 0, sum->get_count( sampsim::ANY_AGE, sampsim::ANY_SEX ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::ANY_AGE, sampsim::MALE ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::ANY_AGE, sampsim::FEMALE ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::ADULT, sampsim::ANY_SEX ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::ADULT, sampsim::MALE ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::ADULT, sampsim::FEMALE ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::CHILD, sampsim::ANY_SEX ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::CHILD, sampsim::MALE ) );
    CHECK_EQUAL( 0, sum->get_count( sampsim::CHILD, sampsim::FEMALE ) );

    cout << "Turning off sample mode" << endl;
    population->set_sample_mode( false );

    cout << "Testing town centroids..." << endl;
    sampsim::coordinate centroid = town->get_centroid();
    CHECK_EQUAL( 5, centroid.x );
    CHECK_EQUAL( 5, centroid.y );

    cout << "Testing town size..." << endl;
    sum = town->get_summary();
    CHECK( 0 != sum->get_count( sampsim::ANY_AGE, sampsim::ANY_SEX ) );
    CHECK( 0 != sum->get_count( sampsim::ANY_AGE, sampsim::MALE ) );
    CHECK( 0 != sum->get_count( sampsim::ANY_AGE, sampsim::FEMALE ) );
    CHECK( 0 != sum->get_count( sampsim::ADULT, sampsim::ANY_SEX ) );
    CHECK( 0 != sum->get_count( sampsim::ADULT, sampsim::MALE ) );
    CHECK( 0 != sum->get_count( sampsim::ADULT, sampsim::FEMALE ) );
    CHECK( 0 != sum->get_count( sampsim::CHILD, sampsim::ANY_SEX ) );
    CHECK( 0 != sum->get_count( sampsim::CHILD, sampsim::MALE ) );
    CHECK( 0 != sum->get_count( sampsim::CHILD, sampsim::FEMALE ) );

    cout << "Testing town prevalence..." << endl;
    for( age_type a = sampsim::ADULT; a <= sampsim::CHILD; a++ )
      for( sex_type s = sampsim::MALE; s <= sampsim::FEMALE; s++ )
        CHECK_EQUAL( 0 != sum->get_count( a, s, DISEASED ) );

    cout << "Testing town area..." << endl;
    CHECK_EQUAL( 100, town->get_area() );
  }

  // clean up
  sampsim::utilities::safe_delete( population );
}
