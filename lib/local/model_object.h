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

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class model_object
  {
  public:
    model_object() {}
    ~model_object() {}

  protected:

  private:
  };
}

/** @} end of doxygen group */

#endif
