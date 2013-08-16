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

    virtual void from_json( const Json::Value& );
    virtual void to_json( Json::Value& ) const;
    virtual void to_csv( std::ostream&, std::ostream& ) const;

  protected:

  private:
  };
}
}

/** @} end of doxygen group */

#endif
