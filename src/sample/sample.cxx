/*=========================================================================

  Program:  sampsim
  Module:   sample.cxx
  Language: C++

=========================================================================*/

#include "sample.h"

#include "utilities.h"

#include <json/value.h>
#include <json/writer.h>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  sample::sample()
  {
    this->size = 0;
    this->one_per_household = false;
    this->age = ANY_AGE;
    this->sex = ANY_SEX;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::write( const std::string filename, const bool flat_file ) const
  {
    sampsim::utilities::output( "writting sample to %s.%s", filename.c_str(), flat_file ? "*.csv" : "json" );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_seed( const std::string seed )
  {
    if( sampsim::utilities::verbose ) sampsim::utilities::output( "setting seed to %s", seed.c_str() );
    this->seed = seed;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_size( const unsigned int size )
  {
    if( sampsim::utilities::verbose ) sampsim::utilities::output( "setting size to %d", size );
    this->size = size;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_one_per_household( const bool one_per_household )
  {
    if( sampsim::utilities::verbose )
      sampsim::utilities::output( "setting one_per_household to %s",
                                  one_per_household ? "true" : "false" );
    this->one_per_household = one_per_household;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_age_type( const age_type age )
  {
    if( sampsim::utilities::verbose )
      sampsim::utilities::output( "setting age_type to %s",
                                  sampsim::sample::get_age_type_name( age ).c_str() );
    this->age = age;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_sex_type( const sex_type sex )
  {
    if( sampsim::utilities::verbose )
      sampsim::utilities::output( "setting sex_type to %s",
                                  sampsim::sample::get_sex_type_name( sex ).c_str() );
    this->sex = sex;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["seed"] = this->seed;
    json["size"] = this->size;
    json["one_per_household"] = this->one_per_household;
    json["age"] = sampsim::sample::get_age_type_name( this->age );
    json["sex"] = sampsim::sample::get_sex_type_name( this->sex );

    /*
    for( household_list_type::const_iterator it = this->household_list.cbegin();
         it != this->household_list.cend();
         ++it )
      it->second->to_json( json["household_list"][index] );
    */
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::to_csv( std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    // put in the parameters
    std::stringstream stream;
    stream << "# version: " << SAMPSIM_VERSION_MAJOR << "." << SAMPSIM_VERSION_MINOR
           << "." << SAMPSIM_VERSION_PATCH << std::endl;
    stream << "# seed: " << this->seed << std::endl;
    stream << "# size: " << this->size << std::endl;
    stream << "# one_per_household: " << ( this->one_per_household ? "true" : "false" ) << std::endl;
    stream << "# age: " << sampsim::sample::get_age_type_name( this->age ) << std::endl;
    stream << "# sex: " << sampsim::sample::get_sex_type_name( this->sex ) << std::endl;
    stream << std::endl;

    household_stream << stream.str();
    individual_stream << stream.str();

    // put in the csv headers
    household_stream << "index,x,y,r,a,income,disease_risk" << std::endl;
    individual_stream << "household_index,sex,age,disease" << std::endl;

    /*
    for( household_list_type::const_iterator it = this->household_list.cbegin();
         it != this->household_list.cend();
         ++it )
      it->second->to_csv( household_stream, individual_stream );
    */
  }
}
}