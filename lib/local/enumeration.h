/*=========================================================================

  Program:  sampsim
  Module:   enumeration.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_enumeration_h
#define __sampsim_enumeration_h

#include "utilities.h"

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class building;

  /**
   * @class enumeration
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief A rectangular area within a town containing a list of buildings
   * @details
   * 
   * Buildings are divided into sub-groups (enumerations) based on their location.
   * The goal of the algorithm is to divide a list of buildings into groupings where all groups have
   * approximately the same number of buildings in it, and all buildings in a group are physically
   * close together.  This is accomplished by iteratively splitting all buildings into two physical
   * halves until a division has less than the threshold number of buildings remaining.
   * Splitting is always done in a perpendicular direction from the last split (horizontal, vertical,
   * horizontal, vertical, etc)
   */
  class enumeration
  {
  public:
    /**
     * Constructor
     */
    enumeration( bool horizontal, extent_type extent, building_list_type building_list ) :
      horizontal( horizontal ),
      extent( extent ),
      building_list( building_list ) {}

    /**
     * Constructor
     */
    enumeration( enumeration* e )
    {
      this->horizontal = e->horizontal;
      this->extent = e->extent;
      this->building_list = e->building_list;
    }

    /**
     * Access methods
     */
    bool get_horizontal() const { return this->horizontal; }
    extent_type get_extent() const { return this->extent; }

    /**
     * Splits an enumeration in half in the perpendicular direction to how it was split
     *
     * Warning: the returned pair of enumerations will have new memory allocated to them so it is up
     * to the caller of this method to manage that memory
     */
    enumeration_pair_type split();

    /**
     * Returns the number of buildings in the enumeration
     */
    unsigned int get_number_of_buildings() const
    { return this->building_list.size(); }

    /** 
     * Iterator access to child buildings
     * 
     * These methods provide iterator access to the list of buildings belonging to this tile.
     */
    building_list_type::iterator get_building_list_begin()
    { return this->building_list.begin(); }

    /** 
     * Iterator access to child buildings
     * 
     * These methods provide iterator access to the list of buildings belonging to this tile.
     */
    building_list_type::iterator get_building_list_end()
    { return this->building_list.end(); }

    /** 
     * Constant iterator access to child buildings
     * 
     * These methods provide constant iterator access to the list of buildings belonging to this tile.
     */
    building_list_type::const_iterator get_building_list_cbegin() const
    { return this->building_list.cbegin(); }

    /** 
     * Constant iterator access to child buildings
     * 
     * These methods provide constant iterator access to the list of buildings belonging to this tile.
     */
    building_list_type::const_iterator get_building_list_cend() const
    { return this->building_list.cend(); }

  private:
    /**
     * Whether the enumeration was created by a horizontal split
     */
    bool horizontal;

    /**
     * The enumeration's extent
     */
    extent_type extent;

    /**
     * The list of buildings in the enumeration
     */
    building_list_type building_list;
  };
}

/** @} end of doxygen group */

#endif
