/*=========================================================================

  Program:  sampsim
  Module:   base_object.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_base_object_h
#define __sampsim_base_object_h

#include <ostream>

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  /**
   * @class base_object
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief Abstract base class for most classes.
   */
  class base_object
  {
  protected:
    /**
     * Deserialize object
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void from_json( const Json::Value& ) = 0;

    /**
     * Serialize object
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void to_json( Json::Value& ) const = 0;

    /**
     * Output object to two CSV files (households and individuals)
     * 
     * All objects must provide an implementation for printing to a CSV (comma separated value) format.
     * Two streams are expected, the first is for household data and the second for individual data.
     */
    virtual void to_csv( std::ostream&, std::ostream& ) const = 0;
  };
}

/** @} end of doxygen group */

#endif
