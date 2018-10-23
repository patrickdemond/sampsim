/*=========================================================================

  Program:  sampsim
  Module:   building_catalogue.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_building_catalogue_h
#define __sampsim_building_catalogue_h

#include "utilities.h"

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class enumeration;

  /**
   * @class building_catalogue
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief Divides a list of buildings into enumerations
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
  class building_catalogue
  {
  public:
    /**
     * Constructor
     */
    building_catalogue( building_list_type, unsigned int );

    /**
     * Constructor
     * 
     * Copies an existing building_catalogue
     */
    building_catalogue( const building_catalogue& );

    /**
     * Destructor
     *
     * Deletes all enumerations belonging to this building_catalogue
     */
    ~building_catalogue();

    /** 
     * Iterator access to child enumerations
     * 
     * These methods provide iterator access to the list of enumerations belonging to the population.
     */
    enumeration_list_type::iterator get_enumeration_list_begin()
    { return this->enumeration_list.begin(); }

    /** 
     * Iterator access to child enumerations
     * 
     * These methods provide iterator access to the list of enumerations belonging to the population.
     */
    enumeration_list_type::iterator get_enumeration_list_end()
    { return this->enumeration_list.end(); }

    /** 
     * Constant iterator access to child enumerations
     * 
     * These methods provide constant iterator access to the list of enumerations belonging to the population.
     */
    enumeration_list_type::const_iterator get_enumeration_list_cbegin() const
    { return this->enumeration_list.cbegin(); }

    /** 
     * Constant iterator access to child enumerations
     * 
     * These methods provide constant iterator access to the list of enumerations belonging to the population.
     */
    enumeration_list_type::const_iterator get_enumeration_list_cend() const
    { return this->enumeration_list.cend(); }

  private:
    /**
     * A recursive function used to create a list of enumerations
     */
    void build( enumeration* );

    /**
     * A recursive function used to create a string representation of a enumeration
     */
    static std::string to_string( enumeration* );

    /**
     * The list of all enumerations
     */
    enumeration_list_type enumeration_list;

    /**
     * The number of buildings an single enumeration must stay below
     */
    unsigned int threshold;
  };
}

/** @} end of doxygen group */

#endif
