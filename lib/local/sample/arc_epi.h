/*=========================================================================

  Program:  sampsim
  Module:   arc_epi.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_arc_epi_h
#define __sampsim_sample_arc_epi_h

#include "sample/epi.h"

#include <list>

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
class building;

/**
 * @addtogroup sample
 * @{
 */

namespace sample
{
  /**
   * @class arc_epi
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief EPI method which picks the initial building using a sector
   * @details
   * A contrete implementation of the sample class.  This EPI method uses an arc to determine
   * which buildings are considered to be inside the list of buildings defined by the initial
   * starting angle.
   */
  class arc_epi : public epi
  {
  public:
    /**
     * Constructor
     */
    arc_epi() : arc_angle( 0 ) {}

    /**
     * Returns the name of the sampling method
     */
    virtual std::string get_type() const { return "arc EPI"; }

    /**
     * Sets the size of the arc (in radians) used to determine the initial building list
     */
    void set_arc_angle( double angle ) { this->arc_angle = angle; }

    /**
     * Returns the size of the arc (in radians) used to determine the initial building list
     */
    double get_arc_angle() { return this->arc_angle; }

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
    virtual std::list< building* >::iterator select_next_building( std::list< building* >& );

  private:
    /**
     * The size of the arc (in radians) used to determine the initial building list
     */
    double arc_angle;
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
