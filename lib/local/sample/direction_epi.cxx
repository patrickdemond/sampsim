/*=========================================================================

  Program:  sampsim
  Module:   direction_epi.cxx
  Language: C++

=========================================================================*/

#include "direction_epi.h"

#include "building.h"
#include "building_tree.h"
#include "coordinate.h"
#include "population.h"
#include "town.h"

#include <cmath>
#include <json/value.h>
#include <json/writer.h>
#include <limits>
#include <stdexcept>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  direction_epi::direction_epi()
  {
    this->use_quadrants = false;
    this->current_quadrant_index = -1;
    this->completed_quadrants = 0;
    this->periphery = false;
    this->start_angle = 0;
    this->periphery_building_selected = false;
    this->buildings_in_current_quadrant = 0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void direction_epi::copy( const direction_epi* object )
  {
    this->use_quadrants = object->use_quadrants;
    this->current_quadrant_index = object->current_quadrant_index;
    this->completed_quadrants = object->completed_quadrants;
    this->periphery = object->periphery;
    this->start_angle = object->start_angle;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* direction_epi::select_next_building( building_list_type &building_list )
  {
    building* b = NULL;

    if( this->use_quadrants )
    {
      // check to see if we have to move to the next quadrant and reset the current building if we do
      double size_fraction = static_cast< double >( this->get_size() ) / 4.0;
      if( this->get_current_town_size() >= size_fraction * ( this->completed_quadrants + 1 ) )
      {
        this->current_building = NULL;
        this->buildings_in_current_quadrant = 0;
      }
    }

    if( !this->use_quadrants && this->periphery && !this->periphery_building_selected )
    {
      // get the furthest building from the center of the town
      b = *std::max_element(
        this->initial_building_list.cbegin(),
        this->initial_building_list.cend(),
        []( building* b1, building* b2 ) -> bool {
          return b1->get_town()->get_centroid().distance( b1->get_position() ) <
                 b2->get_town()->get_centroid().distance( b2->get_position() );
        }
      );
      this->tree->remove( b );
      this->periphery_building_selected = true;
      if( utilities::verbose )
        utilities::output(
          "selected furthest building from center of town out of %d buildings in arc",
          this->initial_building_list.size() );
    }
    else
    {
      if( NULL == this->current_building )
      {
        // 1. determine the start angle (and quadrant)
        this->determine_next_start_angle();

        // 2. get list of all buildings in the area determined by the sub-algorithm in the chosen direction
        this->determine_initial_building_list( building_list );
        if( 0 == this->initial_building_list.size() )
          throw std::runtime_error(
            "Unable to find initial building after 1000 attempts.  You must either lower the sample size or increase the initial selection area." );

        // 3. select a building from the list produced by step 2
        if( this->periphery )
        {
          // get the closest building to the center of the town
          b = *std::min_element(
            this->initial_building_list.cbegin(),
            this->initial_building_list.cend(),
            []( building* b1, building* b2 ) -> bool {
              return b1->get_town()->get_centroid().distance( b1->get_position() ) <
                     b2->get_town()->get_centroid().distance( b2->get_position() );
            }
          );
          this->periphery_building_selected = false;
          if( utilities::verbose )
            utilities::output(
              "selected closest building to center of town out of %d buildings in arc",
              this->initial_building_list.size() );
        }
        else
        {
          this->first_building_index = utilities::random( 0, this->initial_building_list.size() - 1 );
          auto initial_it = this->initial_building_list.begin();
          std::advance( initial_it, this->first_building_index );
          b = *initial_it;
          if( utilities::verbose )
            utilities::output(
              "selected building %d of %d in arc",
              this->first_building_index + 1,
              this->initial_building_list.size() );
        }
        this->tree->remove( b );
      }
      else
      {
        // let the parent class select the next building (it is automatically removed from the tree)
        b = epi::select_next_building( building_list );
      }
    }

    this->buildings_in_current_quadrant++;
    this->current_building = b;

    return b;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void direction_epi::reset_for_next_sample( const bool full )
  {
    epi::reset_for_next_sample( full );

    this->current_quadrant_index = -1;
    this->completed_quadrants = 0;
    this->start_angle = 0;
    this->periphery_building_selected = false;
    this->buildings_in_current_quadrant = 0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void direction_epi::determine_next_start_angle()
  {
    if( this->use_quadrants )
    {
      if( -1 == this->current_quadrant_index )
      { // if we haven't selected a quadrant yet then pick a random one
        this->current_quadrant_index = utilities::random( 0, 3 );
      }
      else 
      { // otherwise go to the next quadrant
        this->completed_quadrants++;
        this->current_quadrant_index = 3 == this->current_quadrant_index ? 0 : this->current_quadrant_index + 1;
      }
    }

    if( !this->start_angle_defined ) this->start_angle = 2 * M_PI * utilities::random();

    if( utilities::verbose )
    {
      if( this->use_quadrants )
      {
        utilities::output(
          "Beginning quadrant %s with a starting angle of %0.3f radians",
          this->get_current_quadrant(),
          this->start_angle
        );
      }
      else
      {
        utilities::output( "Starting angle of %0.3f radians", this->start_angle );
      }
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  sampsim::coordinate direction_epi::get_angle_centroid()
  {
    sampsim::town *town = *( this->population->get_town_list_cbegin() );
    sampsim::coordinate c = town->get_centroid();

    if( this->use_quadrants )
    {
      // adjust centroid to center of quadrant
      if( 0 == this->current_quadrant_index ) c *= 1.5;
      else if( 1 == this->current_quadrant_index ) { c.x /= 2; c.y *= 1.5; }
      else if( 2 == this->current_quadrant_index ) { c /= 2; }
      else if( 3 == this->current_quadrant_index ) { c.x *= 1.5; c.y /= 2; }
    }

    return c;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool direction_epi::in_current_quadrant( const sampsim::building *b ) const
  {
    if( !this->use_quadrants ) return true;

    sampsim::coordinate c = b->get_town()->get_centroid();
    sampsim::coordinate p = b->get_position();
    if( 0 == this->current_quadrant_index ) return p.x >= c.x && p.y >= c.y;
    else if( 1 == this->current_quadrant_index ) return p.x < c.x && p.y >= c.y;
    else if( 2 == this->current_quadrant_index ) return p.x < c.x && p.y < c.y;
    else if( 3 == this->current_quadrant_index ) return p.x >= c.x && p.y < c.y;
    return false;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void direction_epi::set_use_quadrants( const bool use_quadrants )
  {
    if( utilities::verbose ) utilities::output( "setting use-quadrants to %s", use_quadrants ? "true" : "false" );
    this->use_quadrants = use_quadrants;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void direction_epi::set_periphery( const bool periphery )
  {
    if( utilities::verbose ) utilities::output( "setting periphery to %s", periphery ? "true" : "false" );
    this->periphery = periphery;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void direction_epi::from_json( const Json::Value &json )
  {
    epi::from_json( json );
    this->use_quadrants = json["use_quadrants"].asBool();
    this->periphery = json["periphery"].asBool();
    this->start_angle = json["start_angle"].asDouble();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void direction_epi::to_json( Json::Value &json ) const
  {
    epi::to_json( json );
    json["use_quadrants"] = this->use_quadrants;
    json["periphery"] = this->periphery;
    json["start_angle"] = this->start_angle;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string direction_epi::get_csv_header() const
  {
    std::stringstream stream;
    stream << epi::get_csv_header();
    stream << "# use_quadrants: " << this->use_quadrants << std::endl;
    stream << "# periphery: " << ( this->periphery ? "true" : "false" ) << std::endl;
    return stream.str();
  }
}
}
