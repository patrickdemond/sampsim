/*=========================================================================

  Program:  sampsim
  Module:   enumeration.cxx
  Language: C++

=========================================================================*/

#include "enumeration.h"

#include "building_catalogue.h"

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
  void enumeration::create_building_list( sampsim::town *town, building_list_type &building_list )
  {
    sized_sample::create_building_list( town, building_list );

    // create the catalogue from the newly created building list
    this->catalogue = new sampsim::building_catalogue( building_list, this->threshold );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* enumeration::select_next_building( building_list_type &building_list )
  {
    // TODO: implement
    auto building_it = building_list.begin();
    std::advance( building_it, utilities::random( 0, building_list.size() - 1 ) );
    return *building_it;
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
