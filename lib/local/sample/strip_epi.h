/*=========================================================================

  Program:  sampsim
  Module:   strip_epi.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_strip_epi_h
#define __sampsim_sample_strip_epi_h

#include "sample/epi.h"

#include <list>

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{

/**
 * @addtogroup sample
 * @{
 */

namespace sample
{
  /**
   * @class strip_epi
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief EPI method which picks the initial building using a strip
   * @details
   * A contrete implementation of the sample class.  This EPI method uses a strip to determine
   * which buildings are considered to be inside the list of buildings defined by the initial
   * starting angle.
   */
  class strip_epi : public epi
  {
  public:
    /**
     * Constructor
     */
    strip_epi() : strip_width( 0 ) {}

    /**
     * Returns the name of the object's class
     */
    std::string get_name() const { return "strip_epi"; }

    /**
     * Returns the name of the sampling method
     */
    virtual std::string get_type() const { return "strip EPI"; }

    /**
     * Sets the width of the strip (in meters) used to determine the initial building list
     */
    void set_strip_width( double width ) { this->strip_width = width; }

    /**
     * Returns the width of the strip (in meters) used to determine the initial building list
     */
    double get_strip_width() { return this->strip_width; }

    /**
     * Deserialize the sample
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    void from_json( const Json::Value& );

    /**
     * Serialize the sample
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    void to_json( Json::Value& ) const;

    /**
     * Returns the header for generated CSV files
     * 
     * The header includes all parameters used by the sampling method
     */
    virtual std::string get_csv_header() const;

  protected:
    /**
     * Algorithm which selects buildings based on the sampling method
     */
    virtual building* select_next_building( sampsim::building_tree& );

  private:
    /**
     * The width of the strip (in meters) used to determine the initial building list
     */
    double strip_width;
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
