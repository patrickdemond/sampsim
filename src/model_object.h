/*=========================================================================

  Program:  sampsim
  Module:   model_object.h
  Language: C++

=========================================================================*/

/**
 * @class model_object
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief Abstract base class for all model classes
 *        These are classes which describe the population.
 */

#ifndef __sampsim_model_object_h
#define __sampsim_model_object_h

#include <json/value.h>

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class model_object
  {
  public:
    model_object() : sample_mode( false ) {}
    ~model_object() {}

    bool get_sample_mode() const { return this->sample_mode; }
    void set_sample_mode( const bool sample_mode ) { this->sample_mode = sample_mode; }

  protected:

  private:
    bool sample_mode;
  };
}

/** @} end of doxygen group */

#endif
