/*=========================================================================

  Program:  sampsim
  Module:   sized_sample.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_sized_sample_h
#define __sampsim_sample_sized_sample_h

#include "sample/sample.h"

#include "utilities.h"

#include <string>

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

/**
 * @namespace sampsim::sample
 * @brief A namespace containing all sampling classes
 */
namespace sample
{
  /**
   * @class sized_sample
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief An abstract base class for all sampling methods
   * @details
   * All extending sampling methods work by selecting buildings until the sample size has been met.
   */
  class sized_sample : public sample
  {
  public:
    /**
     * Constructor
     */
    sized_sample();

    // defining pure abstract methods
    void copy( const base_object* o ) { this->copy( static_cast<const sized_sample*>( o ) ); }
    void copy( const sized_sample* );
    virtual void from_json( const Json::Value& );
    virtual void to_json( Json::Value& ) const;

    /**
     * Sets the number of individuals to select per sample
     */
    void set_size( const unsigned int size );

    /**
     * Returns the number of individuals to select per sample
     */
    unsigned int get_size() const { return this->size; }

    /**
     * Returns the header for generated CSV files
     * 
     * The header includes all parameters used by the sampling method
     */
    virtual std::string get_csv_header() const;

  protected:
    /**
     * Returns whether or not the sampling is complete
     */
    virtual bool is_sample_complete();

  private:
    /**
     * How many individuals to select in each sample
     */
    unsigned int size;
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
