/*=========================================================================

  Program:  sampsim
  Module:   epi.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_epi_h
#define __sampsim_sample_epi_h

#include "sample/sized_sample.h"

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
  class epi : public sized_sample
  {
  public:
    /**
     * Constructor
     */
    epi();

    // defining pure abstract methods
    void copy( const base_object* o ) { this->copy( static_cast<const epi*>( o ) ); }
    void copy( const epi* );
    virtual void from_json( const Json::Value& );
    virtual void to_json( Json::Value& ) const;

    /**
     * Sets the number of sectors to divide the town into.
     * 
     * When sampling a town it is possible to divide the town into N evenly sized sectors such that
     * one sample will be taken in each sector containing 1/N the total sample size.
     */
    void set_number_of_sectors( const unsigned int number_of_sectors );

    /**
     * Returns the number of sectors to divide the town into.
     */
    unsigned int get_number_of_sectors() const { return this->number_of_sectors; }

    /**
     * Returns the current sector being processed
     */
    int get_current_sector() const { return this->current_sector_index + 1; }

    /**
     * Returns the lower and upper bounds of the current sector
     */
    std::pair< double, double > get_next_sector_range();

    /**
     * Sets the number of households to skip when selecting the next nearest household.
     */
    void set_skip( const unsigned int skip );

    /**
     * Returns the number of households to skip when selecting the next household.
     */
    unsigned int get_skip() const { return this->skip; }

    /**
     * Returns the angle used in the selection of the initial building
     */
    double get_start_angle() { return this->start_angle; }

    /**
     * Sets the angle used in the selection of the initial building
     * 
     * Note, if using multiple sectors then the angle selected for each sector will be the initial
     * set angle rotated by the angle of a sector.  This will result in a uniform distribution of
     * angles situated at the same sub-angle in each sector.
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
     * Determines an individual's sample weight
     */
    virtual double get_sample_weight( const sampsim::individual* ) const;

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
    virtual building* select_next_building( sampsim::building_tree& );

    /**
     * Extends parent method
     */
    virtual void reset_for_next_sample( const bool full = true );

    /**
     * Is called when needing to select the next angle belonging to the next sector
     */
    virtual void determine_next_start_angle();

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

  private:
    /**
     * The total number of sectors to divide the town into for sampling.
     * 
     * When sampling a town it is possible to divide the town into N evenly sized sectors such that
     * one sample will be taken in each sector containing 1/N the total sample size.
     */
    unsigned int number_of_sectors;

    /**
     * The current sector being sampled
     */
    int current_sector_index;

    /**
     * The number of households to skip when selecting the next household
     */
    unsigned int skip;
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
