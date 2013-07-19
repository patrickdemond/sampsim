/*=========================================================================

  Program:  sampsim
  Module:   household.h
  Language: C++

=========================================================================*/

/**
 * @class household
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief A household which belongs to a building and contains individuals
 */

#ifndef __household_h
#define __household_h

#include "base_object.h"

#include <vector>

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class building;
  class individual;

  class household : public base_object
  {
  public:
    household( building *parent );
    ~household();
    building* get_parent() { return this->parent; }

    /**
     * Generates the household by generating all individuals in the household
     */
    void generate();
    virtual void to_json( Json::Value& );
    virtual void to_csv( std::ofstream&, std::ofstream& );

    /**
     * Counts the total population of the household
     */
    int get_population() const;

  protected:

  private:
    building *parent;
    std::vector< individual* > individual_list;
  };
}

/** @} end of doxygen group */

#endif
