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
  class population;
  class tile;

  class household : public base_object
  {
  public:
    household( building *parent );
    ~household();

    /**
     * Lineage methods
     */
    building* get_building() const { return this->parent; }
    tile* get_tile() const;
    population* get_population() const;

    /**
     * Generates the household by generating all individuals in the household
     */
    void generate();
    virtual void to_json( Json::Value& );
    virtual void to_csv( std::ofstream&, std::ofstream& );

    /**
     * Counts the total population of the household
     */
    int count_population() const;

  protected:

  private:
    building *parent;
    double income;
    double disease_risk;

    std::vector< individual* > individual_list;
  };
}

/** @} end of doxygen group */

#endif
