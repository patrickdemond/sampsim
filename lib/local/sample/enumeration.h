/*=========================================================================

  Program:  sampsim
  Module:   enumeration.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_enumeration_h
#define __sampsim_sample_enumeration_h

#include "sample/sized_sample.h"

#include <list>

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class building_catalogue;
  class enumeration;

/**
 * @addtogroup sample
 * @{
 */

namespace sample
{
  /**
   * @class enumeration
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief Enum sampler
   * @details
   * A concrete implementation of the sample class.
   * TODO: describe
   */
  class enumeration : public sized_sample
  {
  public:
    /**
     * Constructor
     */
    enumeration();

    /**
     * Destructor
     */
    ~enumeration();

    // defining pure abstract methods
    std::string get_name() const { return "enumeration"; }
    void copy( const base_object* o ) { this->copy( static_cast<const enumeration*>( o ) ); }
    void copy( const enumeration* );
    void from_json( const Json::Value& );
    void to_json( Json::Value& ) const;

    /**
     * Returns the name of the sampling method
     */
    virtual std::string get_type() const { return "enumeration"; }

    /**
     * Determines an individual's sample weight
     */
    virtual double get_immediate_sample_weight( const sampsim::individual* ) const;

    /**
     * Sets the number of buildings an single enumeration must stay below
     */
    void set_threshold( unsigned int threshold ) { this->threshold = threshold; }

    /**
     * Returns the number of buildings an single enumeration must stay below
     */
    unsigned int get_threshold() { return this->threshold; }

    /**
     * Returns the header for generated CSV files
     * 
     * The header includes all parameters used by the sampling method
     */
    virtual std::string get_csv_header() const;

  protected:
    virtual void reset_for_next_sample( const bool full = true );

    /**
     * TODO: document
     */
    virtual void create_building_list( sampsim::town*, building_list_type &building_list );

    /**
     * Algorithm which selects buildings based on the sampling method
     */
    virtual building* select_next_building( building_list_type& );

  private:
    /**
     * The building catalogue
     */
    sampsim::building_catalogue *catalogue;

    /**
     * The enumeration which has been selected to sample buildings from
     */
    sampsim::enumeration *active_enumeration;

    /**
     * The number of buildings an single enumeration must stay below
     */
    unsigned int threshold;
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
