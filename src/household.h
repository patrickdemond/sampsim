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

  // individual list type
  typedef std::vector< individual* > individual_list_type;

  class household : public base_object
  {
  public:
    household( building *parent );
    ~household();

    /**
     * Iterator access
     */
    individual_list_type::iterator get_individual_list_begin()
    { return this->individual_list.begin(); }
    individual_list_type::iterator get_individual_list_end()
    { return this->individual_list.end(); }
    individual_list_type::const_iterator get_individual_list_cbegin() const
    { return this->individual_list.cbegin(); }
    individual_list_type::const_iterator get_individual_list_cend() const
    { return this->individual_list.cend(); }

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
    virtual void to_json( Json::Value& ) const;
    virtual void to_csv( std::ofstream&, std::ofstream& ) const;

    /**
     * Counts the total population of the household
     */
    int count_population() const;

  protected:

  private:
    building *parent;
    double income;
    double disease_risk;

    individual_list_type individual_list;
  };
}

/** @} end of doxygen group */

#endif
