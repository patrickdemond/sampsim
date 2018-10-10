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
     * Sets the target enumeration size (number of individuals)
     */
    void set_target_size( double angle ) { this->target_size = angle; }

    /**
     * Returns the target enumeration size (number of individuals)
     */
    double get_target_size() { return this->target_size; }

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
    virtual building* select_next_building( building_tree& );

  private:
    /**
     * The target number of individuals in each enumeration
     */
    unsigned int target_size;
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
