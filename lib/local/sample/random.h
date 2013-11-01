/*=========================================================================

  Program:  sampsim
  Module:   random.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_random_h
#define __sampsim_sample_random_h

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
   * @class random
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief Random sampler
   * @details
   * A concrete implementation of the sample class.  This is a pure random sampler.  It random
   * selects households until the sample size has been met.
   */
  class random : public sample
  {
  public:
    /**
     * Returns the name of the sampling method
     */
    virtual std::string get_type() const { return "random"; }

  protected:
    /**
     * Algorithm which selects households based on the sampling method
     */
    virtual std::list< household* >::iterator select_next_household( std::list< household* >& );
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
