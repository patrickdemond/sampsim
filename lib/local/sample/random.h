/*=========================================================================

  Program:  sampsim
  Module:   random.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_random_h
#define __sampsim_sample_random_h

#include "sample/sized_sample.h"

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
   * A concrete implementation of the sample class.  This is a pure random sampler.  It randomly
   * selects buildings until the sample size has been met.
   */
  class random : public sized_sample
  {
  public:
    // defining pure abstract methods
    std::string get_name() const { return "random"; }
    void copy( const base_object* o ) { this->copy( static_cast<const random*>( o ) ); }
    void copy( const random* );

    /**
     * Returns the name of the sampling method
     */
    virtual std::string get_type() const { return "random"; }

    /**
     * Determines an individual's sample weight
     */
    virtual double get_immediate_sample_weight( const sampsim::individual* ) const;

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
