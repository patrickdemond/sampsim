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

#include "coordinate.h"
#include "distribution.h"

#include <map>
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
    enum age_type
    {
      UNKNOWN_AGE_TYPE = 0,
      ANY_AGE,
      ADULT,
      CHILD
    };

    inline static sample::age_type get_age_type( const std::string name )
    {
      if( "either" == name ) return ANY_AGE;
      else if( "adult" == name ) return ADULT;
      else if( "child" == name ) return CHILD;
      return UNKNOWN_AGE_TYPE;
    }

    inline static std::string get_age_type_name( const sample::age_type type )
    {
      if( ANY_AGE == type ) return "either";
      else if( ADULT == type ) return "adult";
      else if( CHILD == type ) return "child";
      return "unknown";
    }

    enum sex_type
    {
      UNKNOWN_SEX_TYPE = 0,
      ANY_SEX,
      FEMALE,
      MALE
    };

    inline static sample::sex_type get_sex_type( const std::string name )
    {
      if( "either" == name ) return ANY_SEX;
      else if( "female" == name ) return FEMALE;
      else if( "male" == name ) return MALE;
      return UNKNOWN_SEX_TYPE;
    }

    inline static std::string get_sex_type_name( const sample::sex_type type )
    {
      if( ANY_SEX == type ) return "either";
      else if( FEMALE == type ) return "female";
      else if( MALE == type ) return "male";
      return "unknown";
    }

  class sample : public sampsim::base_object
  {
  public:
    sample();
    ~sample();

    /**
     * Generate the sample
     */
    virtual void generate() = 0;
    void write( const std::string filename, const bool flat_file = false ) const;

    bool set_population( const std::string filename );
    void set_seed( const std::string seed );
    std::string get_seed() const { return this->seed; }
    void set_size( const unsigned int size );
    unsigned int get_size() const { return this->size; }
    void set_one_per_household( const bool one_per_household );
    bool get_one_per_household() const { return this->one_per_household; }
    void set_age_type( const age_type age );
    age_type get_age_type() const { return this->age; }
    std::string get_age_type_name() const { return sampsim::sample::get_age_type_name( this->age ); }
    void set_sex_type( const sex_type sex );
    sex_type get_sex_type() const { return this->sex; }
    std::string get_sex_type_name() const { return sampsim::sample::get_sex_type_name( this->sex ); }

    virtual void from_json( const Json::Value& );
    virtual void to_json( Json::Value& ) const;
    virtual void to_csv( std::ostream&, std::ostream& ) const;

  protected:

  private:
    bool ready;
    sampsim::population *population;
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
