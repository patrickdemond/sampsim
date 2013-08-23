/*=========================================================================

  Program:  sampsim
  Module:   sample.cxx
  Language: C++

=========================================================================*/

#include "sample.h"

#include "household.h"
#include "population.h"

#include <fstream>
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
    this->population = new sampsim::population;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  sample::~sample()
  {
    sampsim::utilities::safe_delete( this->population );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::write( const std::string filename, const bool flat_file ) const
  {
    sampsim::utilities::output( "writting sample to %s.%s", filename.c_str(), flat_file ? "*.csv" : "json" );

    if( flat_file )
    {
      std::ofstream household_stream( filename + ".household.csv", std::ofstream::out );
      std::ofstream individual_stream( filename + ".individual.csv", std::ofstream::out );
      this->to_csv( household_stream, individual_stream );
      household_stream.close();
      individual_stream.close();
    }
    else
    {
      std::ofstream stream( filename + ".json", std::ofstream::out );
      Json::Value root;
      this->to_json( root );
      Json::StyledWriter writer;
      stream << writer.write( root );
      stream.close();
    }

    utilities::output( "finished writting population" );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool sample::set_population( const std::string filename )
  {
    return this->population->read( filename );
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
                                  sampsim::get_age_type_name( age ).c_str() );
    this->age = age;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_sex_type( const sex_type sex )
  {
    if( sampsim::utilities::verbose )
      sampsim::utilities::output( "setting sex_type to %s",
                                  sampsim::get_sex_type_name( sex ).c_str() );
    this->sex = sex;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::from_json( const Json::Value &json )
  {
    this->seed = json["seed"].asString();
    this->size = json["size"].asInt();
    this->one_per_household = json["one_per_household"].asBool();
    this->age = sampsim::get_age_type( json["age"].asString() );
    this->sex = sampsim::get_sex_type( json["sex"].asString() );
    
    this->household_list.reserve( json["household_list"].size() );
    for( unsigned int c = 0; c < json["household_list"].size(); c++ )
    {   
      household *h = new household( NULL );
      h->from_json( json["household_list"][c] );
      this->household_list[c] = h;
    }   
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["seed"] = this->seed;
    json["size"] = this->size;
    json["one_per_household"] = this->one_per_household;
    json["age"] = sampsim::get_age_type_name( this->age );
    json["sex"] = sampsim::get_sex_type_name( this->sex );

    // write the households, limiting the individuals appropriately
    int index = 0;
    for( auto it = this->household_list.cbegin(); it != this->household_list.cend(); ++it, ++index )
      ( *it )->to_json( json["household_list"][index], this->one_per_household, this->age, this->sex );
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
    stream << "# age: " << sampsim::get_age_type_name( this->age ) << std::endl;
    stream << "# sex: " << sampsim::get_sex_type_name( this->sex ) << std::endl;
    stream << std::endl;

    household_stream << stream.str();
    individual_stream << stream.str();

    // put in the csv headers
    household_stream << "index,x,y,r,a,income,disease_risk" << std::endl;
    individual_stream << "household_index,sex,age,disease" << std::endl;

    for( auto it = this->household_list.begin(); it != this->household_list.end(); ++it )
      ( *it )->to_csv( household_stream, individual_stream );
  }
}
}
