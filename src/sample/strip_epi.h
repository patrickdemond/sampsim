/*=========================================================================

  Program:  sampsim
  Module:   strip_epi.h
  Language: C++

=========================================================================*/

/**
 * @class strip_epi
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief Random sampler
 */

#ifndef __sampsim_sample_strip_epi_h
#define __sampsim_sample_strip_epi_h

#include "sample/sample.h"

#include "coordinate.h"
#include "distribution.h"

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
  class strip_epi : public sample
  {
  public:
    strip_epi() {}
    ~strip_epi() {}

    virtual std::string get_type() const { return "strip EPI"; }

  protected:
    /**
     * Algorithm which selects households based on the sampling method
     */
    virtual std::list< household* >::iterator select_next_household( std::list< household* >& ) const;

  private:
  };
}
}

/** @} end of doxygen group */

#endif
