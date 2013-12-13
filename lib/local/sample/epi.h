/*=========================================================================

  Program:  sampsim
  Module:   epi.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_epi_h
#define __sampsim_sample_epi_h

#include "sample/sample.h"

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
   * @class epi
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief Base class for EPI sampling methods
   * @details
   * A base class for all sampling methods based on the World Health Organization's Expanded Program
   * on Immunization (EPI).  These samplers start at the centre of the population, choose a random
   * direction, randomly selects one of the buildings in that direction and proceeds to sample
   * the nearest buildings from the previously selected building until the sample size is met.
   */
  class epi : public sample
  {
  public:
    /**
     * Constructor
     */
    epi();

    /**
     * Generates the sample by calling select_next_building() until sample size has been met
     */
    virtual void generate();

    /**
     * Serialize the sample
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    void to_json( Json::Value& ) const;

    /**
     * Returns the header for generated CSV files
     * 
     * The header includes all parameters used by the sampling method
     */
    virtual std::string get_csv_header() const;

    /**
     * Returns the angle used in the selection of the initial building
     */
    double get_start_angle() { return this->start_angle; }

    /**
     * Sets the angle used in the selection of the initial building
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

  protected:
    /**
     * Algorithm which selects buildings based on the sampling method
     */
    virtual building* select_next_building( sampsim::building_tree& );

    /**
     * Whether the start angle has been defined
     */
    bool start_angle_defined;

    /**
     * The angle used in the selection of the initial building
     */
    double start_angle;

    /**
     * The index of the first building selected from all of those found in the direction of the
     * starting angle
     */
    int first_building_index;

    /**
     * A reference to the currently selected building
     */
    building *current_building;
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
