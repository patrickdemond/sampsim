/*=========================================================================

  Program:  sampsim
  Module:   test_enumeration.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the enumeration class
//

#include "UnitTest++.h"

#include "building.h"
#include "common.h"
#include "enumeration.h"
#include "population.h"
#include "tile.h"
#include "town.h"
#include "utilities.h"

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_enumeration )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population );

  unsigned int count = 1;
  for( auto town_it = population->get_town_list_begin();
       town_it != population->get_town_list_end();
       ++town_it )
  {
    cout << "Testing town #" << count++ << endl;

    // create a list of all buildings in the town
    building_list_type building_list;
    sampsim::town *town = *town_it;
    for( auto tile_it = town->get_tile_list_begin();
         tile_it != town->get_tile_list_end();
         ++tile_it )
    {
      sampsim::tile *tile = tile_it->second;
      building_list.insert(
        building_list.begin(),
        tile->get_building_list_begin(),
        tile->get_building_list_end() );

    }

    // create an enumeration using the building list
    sampsim::enumeration *e = new enumeration(
      true,
      extent_type( coordinate(), coordinate( town->get_x_width(), town->get_y_width() ) ),
      building_list
    );

    // split the enumeration
    enumeration_pair_type e_pair1 = e->split();

    cout << "Testing that split enumerations have the opposite directionality to their parent" << endl;
    CHECK_EQUAL( e->get_horizontal(), !e_pair1.first->get_horizontal() );
    CHECK_EQUAL( e->get_horizontal(), !e_pair1.second->get_horizontal() );

    cout << "Testing that split enumeration boundaries match their parent" << endl;
    CHECK_EQUAL( e_pair1.first->get_extent().first, e->get_extent().first );
    if( e->get_horizontal() ) CHECK_EQUAL( e_pair1.first->get_extent().second.x, e_pair1.second->get_extent().first.x );
    else CHECK_EQUAL( e_pair1.first->get_extent().second.y, e_pair1.second->get_extent().first.y );
    CHECK_EQUAL( e_pair1.second->get_extent().second, e->get_extent().second );

    cout << "Testing that buildings in each split enumeration falls inside their parent enumeration's extent" << endl;
    extent_type e1 = e_pair1.first->get_extent();
    for( auto it = e_pair1.first->get_building_list_cbegin(); it != e_pair1.first->get_building_list_cend(); ++it )
    {
      coordinate c = (*it)->get_position();
      CHECK( e->get_extent().first.x <= c.x && c.x < e->get_extent().second.x ); 
      CHECK( e->get_extent().first.y <= c.y && c.y < e->get_extent().second.y ); 
    }

    extent_type e2 = e_pair1.second->get_extent();
    for( auto it = e_pair1.second->get_building_list_cbegin(); it != e_pair1.second->get_building_list_cend(); ++it )
    {
      coordinate c = (*it)->get_position();
      CHECK( e->get_extent().first.x <= c.x && c.x < e->get_extent().second.x ); 
      CHECK( e->get_extent().first.y <= c.y && c.y < e->get_extent().second.y ); 
    }

    // now split the sub-enumeration
    enumeration_pair_type e_pair2 = e_pair1.second->split();

    cout << "Testing that doubly split enumerations have the opposite directionality to their parent" << endl;
    CHECK_EQUAL( e_pair1.second->get_horizontal(), !e_pair2.first->get_horizontal() );
    CHECK_EQUAL( e_pair1.second->get_horizontal(), !e_pair2.second->get_horizontal() );

    cout << "Testing that doubly split enumeration boundaries match their parent" << endl;
    CHECK_EQUAL( e_pair2.first->get_extent().first, e_pair1.second->get_extent().first );
    if( e_pair1.second->get_horizontal() ) CHECK_EQUAL( e_pair2.first->get_extent().second.x, e_pair2.second->get_extent().first.x );
    else CHECK_EQUAL( e_pair2.first->get_extent().second.y, e_pair2.second->get_extent().first.y );
    CHECK_EQUAL( e_pair2.second->get_extent().second, e_pair1.second->get_extent().second );

    cout << "Testing that buildings in each doubly split enumeration falls inside their parent enumeration's extent" << endl;
    e1 = e_pair2.first->get_extent();
    for( auto it = e_pair2.first->get_building_list_cbegin(); it != e_pair2.first->get_building_list_cend(); ++it )
    {
      coordinate c = (*it)->get_position();
      CHECK( e_pair1.second->get_extent().first.x <= c.x && c.x < e_pair1.second->get_extent().second.x ); 
      CHECK( e_pair1.second->get_extent().first.y <= c.y && c.y < e_pair1.second->get_extent().second.y ); 
    }

    e2 = e_pair2.second->get_extent();
    for( auto it = e_pair2.second->get_building_list_cbegin(); it != e_pair2.second->get_building_list_cend(); ++it )
    {
      coordinate c = (*it)->get_position();
      CHECK( e_pair1.second->get_extent().first.x <= c.x && c.x < e_pair1.second->get_extent().second.x ); 
      CHECK( e_pair1.second->get_extent().first.y <= c.y && c.y < e_pair1.second->get_extent().second.y ); 
    }

    sampsim::utilities::safe_delete( e );
    sampsim::utilities::safe_delete( e_pair1.first );
    sampsim::utilities::safe_delete( e_pair1.second );
    sampsim::utilities::safe_delete( e_pair2.first );
    sampsim::utilities::safe_delete( e_pair2.second );
  }

  // clean up
  sampsim::utilities::safe_delete( population );
}
