/*=========================================================================

  Program:  sampsim
  Module:   gps.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_gps_h
#define __sampsim_sample_gps_h

#include "sample/sized_sample.h"

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
   * @class gps
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief Base class for GPS sampling methods
   * @details
   * A base class for all sampling methods based on GPS sampling.  These samplers select random
   * points within a town and use that point to determine how to select buildings.
   */
  class gps : public sized_sample
  {
  public:
    /**
     * Constructor
     */
    gps() : number_of_buildings( 0 ) {}

  protected:
    /**
     * Determines an individual's sample weight
     */
    virtual double get_immediate_sample_weight( const sampsim::individual* ) const;

  public:
    unsigned int number_of_buildings;
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
