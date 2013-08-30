/*=========================================================================

  Program:  sampsim
  Module:   sample.h
  Language: C++

=========================================================================*/

/**
 * @class sample
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief An abstract base class for all sampling methods
 */

#ifndef __sampsim_sample_sample_h
#define __sampsim_sample_sample_h

#include "base_object.h"

#include "utilities.h"

#include <list>
#include <string>

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
class population;
namespace sample
{
  class sample : public sampsim::base_object
  {
  public:
    sample();
    ~sample();

    /**
     * Generates the sample by calling select_next_household() until sample size has been met
     */
    virtual void generate();
    void write( const std::string filename, const bool flat_file = false ) const;

    /** 
     * Iterator access
     */
    household_list_type::iterator get_household_list_begin()
    { return this->household_list.begin(); }
    household_list_type::iterator get_household_list_end()
    { return this->household_list.end(); }
    household_list_type::const_iterator get_household_list_cbegin() const
    { return this->household_list.cbegin(); }
    household_list_type::const_iterator get_household_list_cend() const
    { return this->household_list.cend(); }
    
    virtual std::string get_type() const = 0;
    bool set_population( const std::string filename );
    void set_seed( const std::string seed );
    std::string get_seed() const { return this->seed; }
    void set_size( const unsigned int size );
    unsigned int get_size() const { return this->size; }
    void set_one_per_household( const bool one_per_household );
    bool get_one_per_household() const { return this->one_per_household; }
    void set_age( const age_type age );
    age_type get_age() const { return this->age; }
    void set_sex( const sex_type sex );
    sex_type get_sex() const { return this->sex; }

    virtual void from_json( const Json::Value& );
    virtual void to_json( Json::Value& ) const;
    virtual void to_csv( std::ostream&, std::ostream& ) const;

  protected:
    /**
     * Algorithm which selects households based on the sampling method
     */
    virtual std::list< household* >::iterator select_next_household( std::list< household* >& ) = 0;

  private:
    sampsim::population *population;
    household_list_type household_list;
    bool ready;
    std::string seed;
    unsigned int size;
    bool one_per_household;
    age_type age;
    sex_type sex;
  };
}
}

/** @} end of doxygen group */

#endif
