/*=========================================================================

  Program:  sampsim
  Module:   arc_epi.h
  Language: C++

=========================================================================*/

/**
 * @class arc_epi
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief Random sampler
 */

#ifndef __sampsim_sample_arc_epi_h
#define __sampsim_sample_arc_epi_h

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
  class arc_epi : public sample
  {
  public:
    arc_epi() {}
    ~arc_epi() {}

    virtual std::string get_type() const { return "arc EPI"; }

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
