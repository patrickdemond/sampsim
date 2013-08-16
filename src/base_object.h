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
     * Writes the object's details to the provided json object.
     */
    virtual void to_json( Json::Value &json ) const = 0;

  protected:

  private:
  };
}

/** @} end of doxygen group */

#endif
