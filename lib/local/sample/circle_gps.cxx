/*=========================================================================

  Program:  sampsim
  Module:   circle_gps.cxx
  Language: C++

=========================================================================*/

#include "circle_gps.h"

#include "building.h"
#include "individual.h"
#include "population.h"
#include "town.h"

#include <json/value.h>
#include <json/writer.h>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void circle_gps::copy( const circle_gps* object )
  {
    this->radius = object->radius;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void circle_gps::reset_for_next_sample( const bool full )
  {
    sized_sample::reset_for_next_sample( full );
    this->number_of_circles = 0;
  };

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* circle_gps::select_next_building( building_list_type& building_list )
  {
    // make sure the sampling radius is set and greater than 0
    if( 0 >= this->radius )
      throw std::runtime_error( "Tried to sample without first setting the sampling radius" );

    building_list_type circle_building_list;
    town *t = ( *building_list.cbegin() )->get_town();
    coordinate centroid = t->get_centroid();

    // keep selecting a random point until there is at least one building found in the resulting circle
    while( 0 == circle_building_list.size() && this->number_of_circles < 100000 )
    {
      // select a random point within the population's bounds
      coordinate p( 2 * centroid.x * utilities::random(), 2 * centroid.y * utilities::random() );
      if( utilities::verbose )
        utilities::output(
          "town %d, iteration #%d: searching circle at %0.3f, %0.3f with radius %0.3f",
          t->get_index(),
          this->number_of_circles + 1,
          p.x, p.y,
          this->radius );

      // make a list of all buildings inside the GPS circle
      for( auto it = building_list.cbegin(); it != building_list.cend(); ++it )
      {
        building* b = *it;
        if( this->radius >= b->get_position().distance( p ) ) circle_building_list.push_back( b );
      }

      if( 0 == circle_building_list.size() && utilities::verbose )
        utilities::output( "no buildings found in GPS circle" );

      this->number_of_circles++;
    }

    this->number_of_buildings = circle_building_list.size();

    if( 0 == this->number_of_buildings )
      throw std::runtime_error(
        "Unable to find building in GPS circle after 100000 attempts.  You must either lower the sample size or increase the radius." );

    // select a random building from the list of those in the circle
    int index = utilities::random( 0, this->number_of_buildings - 1 );
    auto selected_it = circle_building_list.begin();
    std::advance( selected_it, index );
    if( utilities::verbose )
      utilities::output(
        "selecting building %d of %d in GPS circle",
        index + 1,
        this->number_of_buildings );

    return *selected_it;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double circle_gps::get_post_sample_weight_factor() const
  {
    // multiply by area_of_town / ( area_of_circle * number_of_circles )
    town *t = *( this->population->get_town_list_cbegin() );
    double area_of_town = t->get_area(); // assuming that all towns have the same physical size
    double area_of_circles = M_PI * this->radius * this->radius * this->number_of_circles;
    return gps::get_post_sample_weight_factor() * area_of_town / area_of_circles;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void circle_gps::from_json( const Json::Value &json )
  {
    sized_sample::from_json( json );
    this->radius = json["radius"].asDouble();
    this->number_of_circles = json["number_of_circles"].asDouble();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void circle_gps::to_json( Json::Value &json ) const
  {
    sized_sample::to_json( json );
    json["radius"] = this->radius;
    json["number_of_circles"] = this->number_of_circles;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string circle_gps::get_csv_header() const
  {
    std::stringstream stream;
    stream << sized_sample::get_csv_header();
    stream << "# radius: " << this->radius << std::endl;
    stream << "# number_of_circles: " << this->number_of_circles << std::endl;
    return stream.str();
  }
}
}
