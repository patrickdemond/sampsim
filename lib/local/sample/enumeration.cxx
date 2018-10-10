/*=========================================================================

  Program:  sampsim
  Module:   enumeration.cxx
  Language: C++

=========================================================================*/

#include "enumeration.h"

#include "building_tree.h"

#include <json/value.h>
#include <json/writer.h>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void enumeration::copy( const enumeration* object )
  {
    this->target_size = object->target_size;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* enumeration::select_next_building( sampsim::building_tree& tree )
  {
    // TODO: implement
    building_list_type list = tree.get_building_list();
    auto building_it = list.begin();
    std::advance( building_it, utilities::random( 0, list.size() - 1 ) );
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
    this->target_size = json["target_size"].asUInt();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void enumeration::to_json( Json::Value &json ) const
  {
    sized_sample::to_json( json );
    json["target_size"] = this->target_size;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string enumeration::get_csv_header() const
  {
    std::stringstream stream;
    stream << sized_sample::get_csv_header();
    stream << "# target_size: " << this->target_size << std::endl;
    return stream.str();
  }
}
}
