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

    // defining pure abstract methods
    std::string get_name() const { return "arc_epi"; }
    void copy( const base_object* o ) { this->copy( static_cast<const arc_epi*>( o ) ); }
    void copy( const arc_epi* );
    void from_json( const Json::Value& );
    void to_json( Json::Value& ) const;

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
     * Returns the header for generated CSV files
     * 
     * The header includes all parameters used by the sampling method
     */
    virtual std::string get_csv_header() const;

  protected:
    /**
     * Determine the initial list of buildings to choose from
     */
    virtual void determine_initial_building_list( sampsim::building_tree&, building_list_type& );

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
