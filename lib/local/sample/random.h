/*=========================================================================

  Program:  sampsim
  Module:   random.h
  Language: C++

=========================================================================*/

/**
 * @class random
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief Random sampler
 */

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
namespace sample
{
  class random : public sample
  {
  public:
    random() {}
    ~random() {}

    virtual std::string get_type() const { return "random"; }

  protected:
    /**
     * Algorithm which selects households based on the sampling method
     */
    virtual std::list< household* >::iterator select_next_household( std::list< household* >& );

  private:
  };
}
}

/** @} end of doxygen group */

#endif
