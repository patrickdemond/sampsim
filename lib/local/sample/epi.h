/*=========================================================================

  Program:  sampsim
  Module:   epi.h
  Language: C++

=========================================================================*/

/**
 * @class epi
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief Base class for EPI sampling methods
 */

#ifndef __sampsim_sample_epi_h
#define __sampsim_sample_epi_h

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
  class epi : public sample
  {
  public:
    epi() : angle( 0 ), first_house_index( 0 ), current_household( NULL ) {}

    virtual void generate();

    void to_json( Json::Value& ) const;
    virtual std::string get_csv_header() const;

  protected:
    /**
     * Algorithm which selects households based on the sampling method
     */
    virtual std::list< household* >::iterator select_next_household( std::list< household* >& );

    double angle;
    int first_house_index;
    household *current_household;
  };
}
}

/** @} end of doxygen group */

#endif
