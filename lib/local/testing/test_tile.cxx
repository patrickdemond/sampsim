/*=========================================================================

  Program:  sampsim
  Module:   test_tile.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the tile class
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

TEST( test_tile )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population, 2, 10000, 25000 );
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
      sampsim::building *building = *tile->get_building_list_begin();
      sampsim::household *household = *building->get_household_list_begin();
      sampsim::individual *individual = *household->get_individual_list_begin();
      unsigned int x = tile_it->first.first;
      unsigned int y = tile_it->first.second;
      double width = population->get_tile_width();
      sampsim::coordinate centroid = tile->get_centroid();
      std::pair< sampsim::coordinate, sampsim::coordinate > extent = tile->get_extent();

      cout << "Testing the tile's centroid at index (" << x << ", " << y << ")..." << endl;
      CHECK_EQUAL( ( x + 0.5 ) * width, centroid.x );
      CHECK_EQUAL( ( y + 0.5 ) * width, centroid.y );

      cout << "Testing the tile's extent..." << endl;
      CHECK_EQUAL( x * width, extent.first.x );
      CHECK_EQUAL( y * width, extent.first.y );
      CHECK_EQUAL( ( x + 1 ) * width, extent.second.x );
      CHECK_EQUAL( ( y + 1 ) * width, extent.second.y );

      cout << "Testing tile population..." << endl;
      sum = tile->get_summary();
      for( int cat_index = 0; cat_index < sampsim::summary::category_size; cat_index++ )
        CHECK( 0 != sum->get_count( cat_index ) );

      cout << "Turning on sample mode" << endl;
      population->set_sample_mode( true );

      cout << "Testing that tile now has no population..." << endl;
      sum = tile->get_summary();
      for( int cat_index = 0; cat_index < sampsim::summary::category_size; cat_index++ )
        CHECK_EQUAL( 0, sum->get_count( cat_index ) );

      cout << "Testing that tile with selected individual has population..." << endl;
      individual->select();
      sum = tile->get_summary();
      CHECK( 0 != sum->get_count() );

      cout << "Testing that tile with unselected individual has no population..." << endl;
      individual->unselect();
      sum = tile->get_summary();
      for( int cat_index = 0; cat_index < sampsim::summary::category_size; cat_index++ )
        CHECK_EQUAL( 0, sum->get_count( cat_index ) );

      cout << "Turning off sample mode" << endl;
      population->set_sample_mode( false );

      cout << "Testing tile population..." << endl;
      sum = tile->get_summary();
      for( int cat_index = 0; cat_index < sampsim::summary::category_size; cat_index++ )
        CHECK( 0 != sum->get_count( cat_index ) );
    }
  }

  // clean up
  sampsim::utilities::safe_delete( population );
}
