/*=========================================================================

  Program:  sampsim
  Module:   gps.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_gps_h
#define __sampsim_sample_gps_h

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
   * @class gps
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief Random sampler
   * @details
   * A concrete implementation of the sample class.  This is a pure gps sampler.  It gpsly
   * selects buildings until the sample size has been met.
   */
  class gps : public sample
  {
  public:
    /**
     * Returns the name of the sampling method
     */
    virtual std::string get_type() const { return "gps"; }

  protected:
    /**
     * Algorithm which selects buildings based on the sampling method
     */
    virtual building* select_next_building( building_tree& );
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
