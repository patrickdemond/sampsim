/*=========================================================================

  Program:  sampsim
  Module:   base_object.cxx
  Language: C++

=========================================================================*/

#include "base_object.h"

#include "utilities.h"

#include <json/value.h>

namespace sampsim
{
  void base_object::debug( std::string message, ... ) const
  {
    if( this->debug_mode )
    {
      va_list args;
      va_start( args, message );

      message = "DEBUG[" + this->get_name() + "]: " + message;
      utilities::output( message, args );
    }
  }
}
