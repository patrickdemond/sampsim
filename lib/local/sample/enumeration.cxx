/*=========================================================================

  Program:  sampsim
  Module:   enumeration.cxx
  Language: C++

=========================================================================*/

#include "enumeration.h"

#include "building_catalogue.h"
#include "../enumeration.h"

#include <json/value.h>
#include <json/writer.h>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  enumeration::enumeration()
  {
    this->threshold = 100;
    this->catalogue = NULL;
    this->active_enumeration = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  enumeration::~enumeration()
  {
    utilities::safe_delete( this->catalogue );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void enumeration::copy( const enumeration* object )
  {
    this->threshold = object->threshold;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void enumeration::reset_for_next_sample( const bool full )
  {
    sized_sample::reset_for_next_sample( full );

    this->active_enumeration = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void enumeration::create_building_list( sampsim::town *town, building_list_type &building_list )
  {
    sized_sample::create_building_list( town, building_list );

    // create the catalogue from the newly created building list
    this->catalogue = new sampsim::building_catalogue( building_list, this->threshold );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* enumeration::select_next_building( building_list_type &building_list )
  {
    // randomly choose one of the enumerations in the building catalogue (if one hasn't already been choosen)
    int iteration = 0;
    while( NULL == this->active_enumeration && iteration < 1000 )
    {
      auto enumeration_it = this->catalogue->get_enumeration_list_begin();
      std::advance( enumeration_it, utilities::random( 0, this->catalogue->get_number_of_enumerations() - 1 ) );
      if( 0 < (*enumeration_it)->get_number_of_buildings() ) this->active_enumeration = *enumeration_it;
      iteration++;
    }

    if( NULL == this->active_enumeration )
      throw std::runtime_error( "No enumerations can be found which contain buildings." );
    else if( 0 == this->active_enumeration->get_number_of_buildings() )
      throw std::runtime_error(
        "No buildings left in the enumeration, please either select a larger enumeration threshold size or a smaller sample size." );

    // select a random building within the active enumeration
    auto building_it = this->active_enumeration->get_building_list_begin();
    std::advance( building_it, utilities::random( 0, this->active_enumeration->get_number_of_buildings() - 1 ) );

    // remove that building from the enumeration (careful, this will change the building_it for some reason)
    building* b = *building_it;
    this->active_enumeration->remove_building( building_it );

    return b;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double enumeration::get_immediate_sample_weight( const sampsim::individual* individual ) const
  {
    return 1.0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void enumeration::from_json( const Json::Value &json )
  {
    sized_sample::from_json( json );
    this->threshold = json["threshold"].asUInt();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void enumeration::to_json( Json::Value &json ) const
  {
    sized_sample::to_json( json );
    json["threshold"] = this->threshold;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string enumeration::get_csv_header() const
  {
    std::stringstream stream;
    stream << sized_sample::get_csv_header();
    stream << "# threshold: " << this->threshold << std::endl;
    return stream.str();
  }
}
}
