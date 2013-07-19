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

#ifndef __building_h
#define __building_h

#include "base_object.h"

#include "coordinate.h"

#include <vector>

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class household;
  class tile;

  class building : public base_object
  {
  public:
    building( tile *parent );
    ~building();
    tile* get_parent() { return this->parent; }

    /**
     * Generate the building by generating all households in the building
     */
    void generate();
    virtual void to_json( Json::Value& );
    virtual void to_csv( std::ofstream&, std::ofstream& );

    /**
     * Counts the total population of the building
     */
    int get_population() const;

    coordinate get_position() const { return this->position; }

  protected:

  private:
    tile *parent;
    coordinate position;

    std::vector< household* > household_list;
  };
}

/** @} end of doxygen group */

#endif
