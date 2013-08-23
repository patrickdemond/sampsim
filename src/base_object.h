/*=========================================================================

  Program:  sampsim
  Module:   base_object.h
  Language: C++

=========================================================================*/

/**
 * @class base_object
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief Abstract base class for all classes
 */

#ifndef __sampsim_base_object_h
#define __sampsim_base_object_h

#include <json/value.h>

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class base_object
  {
  public:
    base_object() {}
    ~base_object() {}

    /**
     * Reads and writes the object's details to/from the provided json object.
     */
    virtual void from_json( const Json::Value& ) = 0;
    virtual void to_json( Json::Value& ) const = 0;

  protected:

  private:
  };
}

/** @} end of doxygen group */

#endif
