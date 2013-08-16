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

#include "coordinate.h"
#include "distribution.h"

#include <map>
#include <string>

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

    /**
     * Generate the random
     */
    virtual void generate();

  protected:

  private:
  };
}
}

/** @} end of doxygen group */

#endif