/*=========================================================================

  Program:  sampsim
  Module:   direction_epi.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_direction_epi_h
#define __sampsim_sample_direction_epi_h

#include "sample/epi.h"

#include <float.h>
#include <list>

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{

/**
 * @addtogroup sample
 * @{
 */

namespace sample
{
  /**
   * @class direction_epi
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief Base class for angle-based EPI sampling methods
   * @details
   * A base class for all EPI sampling methods which choose their initial building by selecting
   * a random direction in the center of town and selecting one of the buildings in that direction.
   */
  class direction_epi : public epi
  {
  public:
    /**
     * Constructor
     */
    direction_epi();

    // defining pure abstract methods
    void copy( const base_object* o ) { this->copy( static_cast<const direction_epi*>( o ) ); }
    void copy( const direction_epi* );
    virtual void from_json( const Json::Value& );
    virtual void to_json( Json::Value& ) const;

    /**
     * Sets whether to use quadrants.
     * 
     * When sampling a town it is possible to divide the town into quadrants such that
     * one sample will be taken in each quadrant containing 1/4 the total sample size.
     */
    void set_use_quadrants( const bool use_quadrants );

    /**
     * Returns whether the town is split into quadrants.
     */
    bool get_use_quadrants() const { return this->use_quadrants; }

    /**
     * Returns the current quadrant being processed
     */
    std::string get_current_quadrant() const
    {
      if( 0 == this->current_quadrant_index ) return "I";
      else if( 1 == this->current_quadrant_index ) return "II";
      else if( 2 == this->current_quadrant_index ) return "III";
      else if( 3 == this->current_quadrant_index ) return "IV";
      else return "?";
    }

    /**
     * Sets whether to select households at the center and periphery (instead of randomly)
     */
    void set_periphery( const bool periphery );

    /**
     * Returns whether to select households at the center and periphery (instead of randomly)
     */
    bool get_periphery() const { return this->periphery; }

    /**
     * Returns the angle used in the selection of the initial building
     */
    double get_start_angle() { return this->start_angle; }

    /**
     * Sets the angle used in the selection of the initial building
     * 
     * Note, each quadrant will get a distinct start angle.
     */
    void set_start_angle( double start_angle )
    {
      this->start_angle_defined = true;
      this->start_angle = start_angle;
    }

    /**
     * Unsets the start angle, allowing it to be randomly determined upon execution
     */
    void unset_start_angle() { this->start_angle_defined = false; }

    /**
     * Returns the header for generated CSV files
     * 
     * The header includes all parameters used by the sampling method
     */
    virtual std::string get_csv_header() const;

  protected:
    /**
     * Algorithm which selects buildings based on the sampling method
     */
    virtual building* select_next_building( building_list_type& );

    /**
     * Extends parent method
     */
    virtual void reset_for_next_sample( const bool full = true );

    /**
     * Is called when needing to select the next angle belonging to the next quadrant
     */
    virtual void determine_next_start_angle();

    /**
     * Get the centroid of the selection angle (centre of town, or centre of current quadrant)
     */
    coordinate get_angle_centroid();

    /**
     * Determine whether a building is in the current quadrant
     * 
     * Note that if quadrants are not being used this will always return true
     */
    bool in_current_quadrant( const sampsim::building *b ) const;

    /**
     * Whether the start angle has been defined
     */
    bool start_angle_defined;

    /**
     * The angle used in the selection of the initial building
     */
    double start_angle;

    /**
     * Whether or not the periphery (furthest building from the center) has been selected
     * Note: this is only used when periphery mode is activated
     */
    bool periphery_building_selected;

  private:
    /**
     * Whether to divide towns up into quadrants.
     * 
     * When sampling a town it is possible to divide the town into quadrants such that
     * one sample will be taken in each quadrant containing 1/4 the total sample size.
     */
    bool use_quadrants;

    /**
     * The current quadrant being sampled
     */
    int current_quadrant_index;

    /**
     * Keeps track of how many quadrants we've sampled
     */
    unsigned int completed_quadrants;

    /**
     * Whether to select households at the center and periphery (instead of randomly)
     * 
     * Note that this is ignored when the town is split into quadrants.
     */
    bool periphery;

    /**
     * Keeps track of how many buildings have been selected in the current quadrant
     * 
     * If quadrants aren't being used then this will containt the total number of selected
     * buildings in the town.
     */
    unsigned int buildings_in_current_quadrant;
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
