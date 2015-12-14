/*=========================================================================

  Program:  sampsim
  Module:   circle_gps.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_circle_gps_h
#define __sampsim_sample_circle_gps_h

#include "sample/sized_sample.h"

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
   * @class circle_gps
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief Random sampler
   * @details
   * A concrete implementation of the sample class.  This sampler selects random "GPS"
   * positions in the town and then randomly selects one of the buildings within a given
   * distance from that central GPS point.  This proceedure is repeated until the sample
   * size is met.
   */
  class circle_gps : public sized_sample
  {
  public:
    /**
     * Construction
     */
    circle_gps() : radius( 0 ) {}

    // defining pure abstract methods
    std::string get_name() const { return "circle_gps"; }
    void copy( const base_object* o ) { this->copy( static_cast<const circle_gps*>( o ) ); }
    void copy( const circle_gps* );
    void from_json( const Json::Value& );
    void to_json( Json::Value& ) const;

    /**
     * Returns the name of the sampling method
     */
    virtual std::string get_type() const { return "GPS"; }

    /** 
     * Sets the radius of the circle used for selecting households from a GPS point
     */
    void set_radius( double radius ) { this->radius = radius; }

    /** 
     * Returns the radius of the circle used for selecting households from a GPS point
     */
    double get_radius() { return this->radius; }

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
    virtual building* select_next_building( building_tree& );

  private:
    /**
     * What distance (in meters) from a GPS point to include when searching for households
     */
    double radius;
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
