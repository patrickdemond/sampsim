/*=========================================================================

  Program:  sampsim
  Module:   epi.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_epi_h
#define __sampsim_sample_epi_h

#include "sample/sized_sample.h"

#include "building_tree.h"

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
   * on Immunization (EPI).  These samplers start at some building in the town and proceeds to sample
   * nearby buildings from the previously selected building.
   */
  class epi : public sized_sample
  {
  public:
    /**
     * Constructor
     */
    epi();

    /**
     * Destructor
     */
    ~epi();

    // defining pure abstract methods
    void copy( const base_object* o ) { this->copy( static_cast<const epi*>( o ) ); }
    void copy( const epi* );
    virtual void from_json( const Json::Value& );
    virtual void to_json( Json::Value& ) const;

    /**
     * Sets the number of households to skip when selecting the next nearest household.
     */
    void set_skip( const unsigned int skip );

    /**
     * Returns the number of households to skip when selecting the next household.
     */
    unsigned int get_skip() const { return this->skip; }

    /**
     * Determines an individual's sample weight
     */
    virtual double get_immediate_sample_weight( const sampsim::individual* ) const;

    /**
     * Returns the header for generated CSV files
     * 
     * The header includes all parameters used by the sampling method
     */
    virtual std::string get_csv_header() const;

  protected:
    /**
     * TODO: document
     */
    virtual void create_building_list( sampsim::town*, building_list_type &building_list );

    /**
     * Algorithm which selects buildings based on the sampling method
     */
    virtual building* select_next_building( building_list_type& );

    /**
     * Gets a list of all buildings to choose the initial building from
     */
    virtual void determine_initial_building_list( building_list_type& ) = 0;

    /**
     * Extends parent method
     */
    virtual void reset_for_next_sample( const bool full = true );

    /**
     * The index of the first building selected from the initial list of buildings
     */
    int first_building_index;

    /**
     * A reference to the currently selected building
     */
    building *current_building;

    /**
     * The list of all buildings the first building was selected from
     */
    building_list_type initial_building_list;

    /**
     * TODO: document
     */
    sampsim::building_tree *tree;

  private:
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
