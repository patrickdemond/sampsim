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
 * @brief EPI method which picks the initial household using a sector
 */

#ifndef __sampsim_sample_arc_epi_h
#define __sampsim_sample_arc_epi_h

#include "sample/epi.h"

#include <list>

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
class household;
namespace sample
{
  class arc_epi : public epi
  {
  public:
    arc_epi() : arc_angle( 0 ) {}

    virtual std::string get_type() const { return "arc EPI"; }

    void set_arc_angle( double angle ) { this->arc_angle = angle; }
    double get_arc_angle() { return this->arc_angle; }

    void to_json( Json::Value& ) const;
    virtual std::string get_csv_header() const;

  protected:
    /**
     * Algorithm which selects households based on the sampling method
     */
    virtual std::list< household* >::iterator select_next_household( std::list< household* >& );

  private:
    double arc_angle;
  };
}
}

/** @} end of doxygen group */

#endif
