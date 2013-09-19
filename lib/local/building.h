/*=========================================================================

  Program:  sampsim
  Module:   building.h
  Language: C++

=========================================================================*/

/**
 * @class building
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief A building which belongs to a tile and contains households
 */

#ifndef __sampsim_building_h
#define __sampsim_building_h

#include "model_object.h"

#include "coordinate.h"
#include "utilities.h"

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class household;
  class population;
  class tile;

  class building : public model_object
  {
  public:
    building( tile *parent );
    ~building();

    /**
     * Iterator access
     */
    household_list_type::iterator get_household_list_begin()
    { return this->household_list.begin(); }
    household_list_type::iterator get_household_list_end()
    { return this->household_list.end(); }
    household_list_type::const_iterator get_household_list_cbegin() const
    { return this->household_list.cbegin(); }
    household_list_type::const_iterator get_household_list_cend() const
    { return this->household_list.cend(); }

    /**
     * Lineage methods
     */
    tile* get_tile() const { return this->parent; }
    population* get_population() const;

    /**
     * Generate the building by generating all households in the building
     */
    void generate();
    virtual void from_json( const Json::Value& );
    virtual void to_json( Json::Value& ) const;
    virtual void to_csv( std::ostream&, std::ostream& ) const;

    /**
     * Counts the total population of the building
     */
    int count_population() const;

    coordinate get_position() const { return this->position; }
    bool is_selected() const { return this->selected; }
    void select() { this->selected = true; }
    void unselect();

  protected:

  private:
    tile *parent;
    coordinate position;
    bool selected;

    household_list_type household_list;
  };
}

/** @} end of doxygen group */

#endif
