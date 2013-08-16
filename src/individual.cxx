/*=========================================================================

  Program:  sampsim
  Module:   individual.cxx
  Language: C++

=========================================================================*/

#include "individual.h"

#include "household.h"

#include <ostream>
#include <json/value.h>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  individual::individual( household *parent )
  {
    this->parent = parent;
    this->male = true;
    this->adult = true;
    this->disease = false;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  individual::~individual()
  {
    // we're holding a light reference to the parent, don't delete it
    this->parent = 0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* individual::get_building() const { return this->parent->get_building(); }
  tile* individual::get_tile() const { return this->parent->get_tile(); }
  population* individual::get_population() const { return this->parent->get_population(); }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::from_json( const Json::Value &json )
  {
    // TODO: implement
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["sex"] = Json::Value( this->male ? "m" : "f" );
    json["age"] = Json::Value( this->adult ? "a" : "c" );
    json["disease"] = this->disease ? 1 : 0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::to_csv( std::ostream &stream ) const
  {
    stream << ( this->male ? "m" : "f" ) << ","
           << ( this->adult ? "a" : "c" ) << ","
           << ( this->disease ? 1 : 0 );
  }
}
