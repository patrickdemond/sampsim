/*=========================================================================

  Program:  sampsim
  Module:   circle_gps.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_circle_gps_h
#define __sampsim_sample_circle_gps_h

#include "sample/sample.h"

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
   * @class circle_gps
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief Random sampler
   * @details
   * A concrete implementation of the sample class.  This sampler selects random "GPS"
   * positions in the town and then randomly selects one of the buildings within a given
   * distance from that central GPS point.  This proceedure is repeated until the sample
   * size is met.
   */
  class circle_gps : public sample
  {
  public:
    /**
     * Construction
     */
     circle_gps() : radius( 0 ) {}

    /**
     * Returns the name of the object's class
     */
    std::string get_name() const { return "circle_gps"; }

    /**
     * Returns the name of the sampling method
     */
    virtual std::string get_type() const { return "GPS"; }

    /** 
     * Sets the width of the strip (in meters) used to determine the initial building list
     */
    void set_radius( double radius ) { this->radius = radius; }

    /** 
     * Returns the width of the strip (in meters) used to determine the initial building list
     */
    double get_radius() { return this->radius; }

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
