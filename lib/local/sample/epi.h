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

#include <float.h>
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
    epi();

    virtual void generate();

    void to_json( Json::Value& ) const;
    virtual std::string get_csv_header() const;

    double get_start_angle() { return this->start_angle; }
    void set_start_angle( double start_angle )
    {
      this->start_angle_defined = true;
      this->start_angle = start_angle;
    }
    void unset_start_angle()
    {
      this->start_angle_defined = false;
    }

  protected:
    /**
     * Algorithm which selects households based on the sampling method
     */
    virtual std::list< household* >::iterator select_next_household( std::list< household* >& );

    bool start_angle_defined;
    double start_angle;
    int first_house_index;
    household *current_household;
  };
}
}

/** @} end of doxygen group */

#endif
