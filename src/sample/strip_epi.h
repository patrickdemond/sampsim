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
 * @brief EPI method which picks the initial household using a strip
 */

#ifndef __sampsim_sample_strip_epi_h
#define __sampsim_sample_strip_epi_h

#include "sample/epi.h"

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
  class strip_epi : public epi
  {
  public:
    strip_epi() : strip_width( 0 ) {}

    virtual std::string get_type() const { return "strip EPI"; }

    void set_strip_width( double angle ) { this->strip_width = angle; }
    double get_strip_width() { return this->strip_width; }

  protected:
    /**
     * Algorithm which selects households based on the sampling method
     */
    virtual std::list< household* >::iterator select_next_household( std::list< household* >& );

  private:
    double strip_width;
  };
}
}

/** @} end of doxygen group */

#endif
