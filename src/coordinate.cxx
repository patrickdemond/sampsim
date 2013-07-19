/*=========================================================================

  Program:  sampsim
  Module:   coordinate.cxx
  Language: C++

=========================================================================*/

#include "coordinate.h"

#include <json/value.h>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void coordinate::to_json( Json::Value &json )
  {
    json = Json::Value( Json::objectValue );
    json["x"] = this->x;
    json["y"] = this->y;
  }
}
