/*=========================================================================

  Program:  sampsim
  Module:   sized_sample.cxx
  Language: C++

=========================================================================*/

#include "sized_sample.h"

#include <fstream>
#include <json/value.h>
#include <json/writer.h>
#include <stdexcept>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  sized_sample::sized_sample()
  {
    this->size = 0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sized_sample::copy( const sized_sample* object )
  {
    this->size = object->size;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sized_sample::set_size( const unsigned int size )
  {
    if( utilities::verbose ) utilities::output( "setting size to %d", size );
    this->size = size;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool sized_sample::is_sample_complete()
  {
    // quit once the town being sampled has reached or is above the sample size
    return this->get_current_town_size() >= this->size;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sized_sample::from_json( const Json::Value &json )
  {
    sample::from_json( json );
    this->size = json["size"].asUInt();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sized_sample::to_json( Json::Value &json ) const
  {
    sample::to_json( json );
    json["size"] = this->size;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string sized_sample::get_csv_header() const
  {
    std::stringstream stream;
    stream << sample::get_csv_header();
    stream << "# size: " << this->size << std::endl;
    return stream.str();
  }
}
}
