/*=========================================================================

  Program:  sampsim
  Module:   building_catalogue.cxx
  Language: C++

=========================================================================*/

#include "building_catalogue.h"

#include "building.h"
#include "enumeration.h"
#include "town.h"

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_catalogue::building_catalogue( building_list_type building_list, unsigned int threshold )
  {
    this->threshold = threshold;

    sampsim::town *t = building_list.front()->get_town();
    enumeration *e = new enumeration(
      true,
      extent_type( coordinate(), coordinate( t->get_x_width(), t->get_y_width() ) ),
      building_list
    );
    this->build( e );
    utilities::safe_delete( e );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_catalogue::building_catalogue( const building_catalogue& building_catalogue )
  {
    this->enumeration_list = building_catalogue.enumeration_list;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_catalogue::~building_catalogue()
  {
    // delete all enumerations
    std::for_each( this->enumeration_list.begin(), this->enumeration_list.end(), utilities::safe_delete_type() );
    this->enumeration_list.empty();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building_catalogue::build( enumeration* e )
  {
    // if the enumeration is below the threshold then add it to the enumeration list
    if( e->get_number_of_buildings() < this->threshold )
    {
      this->enumeration_list.push_back( new enumeration( e ) );
    }
    else
    {
      // split the enumeration in two and build each one
      auto e_pair = e->split();
      this->build( e_pair.first );
      this->build( e_pair.second );
      utilities::safe_delete( e_pair.first );
      utilities::safe_delete( e_pair.second );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string building_catalogue::to_string( enumeration* )
  {
    return "";
  }
}
