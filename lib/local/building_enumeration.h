/*=========================================================================

  Program:  sampsim
  Module:   building_enumeration.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_building_enumeration_h
#define __sampsim_building_enumeration_h

#include "utilities.h"

#include <utility>

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class building;

  /**
   * @class building_enumeration
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
  class building_enumeration
  {
  private:
    /**
     * @struct enumeration
     * @brief An internal struct for handling individual enumerations
     */
    struct enumeration
    {
      /**
       * Constructor
       */
      enumeration()
      {
        this->horizontal = true;
      }

      void copy( enumeration* e )
      {
        this->building_list = e->building_list;
        this->lower_left_coordinate = e->lower_left_coordinate;
        this->upper_right_coordinate = e->upper_right_coordinate;
        this->horizontal = e->horizontal;
      }

      std::pair< enumeration, enumeration > split();

      /**
       * The list of buildings in the enumeration
       */
      building_list_type building_list;

      /**
       * The enumeration's lower-left coordinate
       */
      coordinate lower_left_coordinate;

      /**
       * The enumeration's upper-right coordinate
       */
      coordinate upper_right_coordinate;

      /**
       * Whether the enumeration was created by a horizontal split
       */
      bool horizontal;
    };

  public:
    /**
     * Constructor
     */
    building_enumeration( building_list_type, unsigned int );

    /**
     * Constructor
     * 
     * Copies an existing building_enumeration
     */
    building_enumeration( const building_enumeration& );

  private:
    /**
     * A recursive function used to create a list of enumerations
     */
    void build( enumeration& );

    /**
     * A recursive function used to create a string representation of a enumeration
     */
    static std::string to_string( enumeration* );

    /**
     * The list of all enumerations
     */
    std::vector< enumeration > enumeration_list;

    /**
     * The number of buildings an single enumeration must stay below
     */
    unsigned int threshold;
  };
}

/** @} end of doxygen group */

#endif
