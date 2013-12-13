/*=========================================================================

  Program:  sampsim
  Module:   sample.cxx
  Language: C++

=========================================================================*/

#include "sample.h"

#include "building.h"
#include "household.h"
#include "individual.h"
#include "population.h"
#include "tile.h"

#include <fstream>
#include <json/value.h>
#include <json/writer.h>
#include <stdexcept>

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
    this->first_building = NULL;
    this->population = NULL;
    this->owns_population = false;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  sample::~sample()
  {
    this->delete_population();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::create_population()
  {
    if( this->population ) this->delete_population();
    this->population = new sampsim::population;
    this->population->set_sample_mode( true );
    this->owns_population = true;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::delete_population()
  {
    if( this->population )
    {
      if( owns_population ) utilities::safe_delete( this->population );
      this->population = NULL;
      this->owns_population = false;
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::generate()
  {
    utilities::output( "generating %s sample", this->get_type().c_str() );

    // check to make sure the age and sex restrictions are valid
    if( UNKNOWN_AGE_TYPE == this->get_age() )
      throw std::runtime_error( "Cannot generate sample, age type is unknown" );
    if( UNKNOWN_SEX_TYPE == this->get_sex() )
      throw std::runtime_error( "Cannot generate sample, sex type is unknown" );

    // create a list of all buildings
    std::list< building* > remaining_building_list;

    for( auto tile_it = this->population->get_tile_list_cbegin();
         tile_it != this->population->get_tile_list_cend();
         ++tile_it )
    {
      for( auto building_it = tile_it->second->get_building_list_cbegin();
           building_it != tile_it->second->get_building_list_cend();
           ++building_it )
      {
        (*building_it)->unselect(); // unselect the building
        remaining_building_list.push_back( *building_it );
      }
    }

    utilities::output( "selecting from a list of %d buildings", remaining_building_list.size() );

    // keep selecting buildings until we've filled our sample size
    int individual_count = 0;
    int household_count = 0;
    this->first_building = NULL;
    while( this->get_size() > individual_count )
    {
      if( 0 == remaining_building_list.size() )
      {
        std::cout << "WARNING: unable to fulfill sample size" << std::endl;
        break;
      }

      auto building_it = this->select_next_building( remaining_building_list );
      building *b = *building_it;

      // set the first building
      if( NULL == this->first_building ) this->first_building = b;

      int count = 0;
      // select households within the building (this is step 4 of the algorithm)
      for( auto household_it = b->get_household_list_begin();
           household_it != b->get_household_list_end();
           ++household_it )
      {
        household *h = *household_it;

        // select individuals within the household
        for( auto individual_it = h->get_individual_list_begin();
             individual_it != h->get_individual_list_end();
             ++individual_it )
        {
          individual *i = *individual_it;
          if( ( ANY_AGE == this->get_age() || this->get_age() == i->get_age() ) &&
              ( ANY_SEX == this->get_sex() || this->get_sex() == i->get_sex() ) )
          {
            i->select();
            count++;
            if( this->get_one_per_household() ) break;
          }
        }

        if( count )
        {
          individual_count += count;
          household_count++;

          // only select another household if we haven't reached our count
          if( this->get_size() <= individual_count ) break;
        }
      }
      remaining_building_list.erase( building_it );
    }

    utilities::output(
      "finished generating %s sample, %d households selected",
      this->get_type().c_str(),
      household_count );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::write( const std::string filename, const bool flat_file ) const
  {
    utilities::output(
      "writing %s sample to %s.%s",
      this->get_type().c_str(),
      filename.c_str(),
      flat_file ? "*.csv" : "json" );

    if( flat_file )
    {
      std::ofstream household_stream( filename + ".household.csv", std::ofstream::out );
      if( !household_stream.is_open() )
      {
        std::stringstream stream;
        stream << "Unable to open \"" << filename << ".household.csv\" for writing";
        throw std::runtime_error( stream.str() );
      }

      std::ofstream individual_stream( filename + ".individual.csv", std::ofstream::out );
      if( !individual_stream.is_open() )
      {
        std::stringstream stream;
        stream << "Unable to open \"" << filename << ".individual.csv\" for writing";
        throw std::runtime_error( stream.str() );
      }

      this->to_csv( household_stream, individual_stream );
      household_stream.close();
      individual_stream.close();
    }
    else
    {
      std::ofstream json_stream( filename + ".json", std::ofstream::out );
      if( !json_stream.is_open() )
      {
        std::stringstream stream;
        stream << "Unable to open \"" << filename << ".json\" for writing";
        throw std::runtime_error( stream.str() );
      }

      Json::Value root;
      this->to_json( root );
      Json::StyledWriter writer;
      json_stream << writer.write( root );
      json_stream.close();
    }

    utilities::output( "finished writing %s sample", this->get_type().c_str() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool sample::set_population( const std::string filename )
  {
    this->create_population();
    return this->population->read( filename );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool sample::set_population( sampsim::population *population )
  {
    this->delete_population();
    this->population = population; // copy pointer but do not delete when finished
    this->population->set_sample_mode( true );
    return true;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_seed( const std::string seed )
  {
    if( utilities::verbose ) utilities::output( "setting seed to %s", seed.c_str() );
    this->seed = seed;
    utilities::random_engine.seed( atoi( this->get_seed().c_str() ) );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_size( const unsigned int size )
  {
    if( utilities::verbose ) utilities::output( "setting size to %d", size );
    this->size = size;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_one_per_household( const bool one_per_household )
  {
    if( utilities::verbose )
      utilities::output( "setting one_per_household to %s", one_per_household ? "true" : "false" );
    this->one_per_household = one_per_household;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_age( const age_type age )
  {
    if( utilities::verbose )
      utilities::output( "setting age to %s", sampsim::get_age_type_name( age ).c_str() );
    this->age = age;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_sex( const sex_type sex )
  {
    if( utilities::verbose )
      utilities::output( "setting sex to %s", sampsim::get_sex_type_name( sex ).c_str() );
    this->sex = sex;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::from_json( const Json::Value &json )
  {
    // make sure the type is correct
    if( this->get_type() != json["type"].asString() )
    {
      std::stringstream stream;
      stream << "Tried to unserialize " << json["type"].asString() << " sample as a "
             << this->get_type() << " sample";
      throw std::runtime_error( stream.str() );
    }

    this->seed = json["seed"].asString();
    this->size = json["size"].asInt();
    this->one_per_household = json["one_per_household"].asBool();
    this->age = sampsim::get_age_type( json["age"].asString() );
    this->sex = sampsim::get_sex_type( json["sex"].asString() );
    this->population->from_json( json["population"] );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["type"] = this->get_type().c_str();
    json["seed"] = this->seed;
    json["size"] = this->size;
    json["one_per_household"] = this->one_per_household;
    json["age"] = sampsim::get_age_type_name( this->age );
    json["sex"] = sampsim::get_sex_type_name( this->sex );
    json["population"] = Json::Value( Json::objectValue );
    this->population->to_json( json["population"] );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string sample::get_csv_header() const
  {
    std::stringstream stream;
    stream << "# sampling parameters" << std::endl;
    stream << "# -----------------------------------------------------------------------" << std::endl;
    stream << "# version: " << SAMPSIM_VERSION_MAJOR << "." << SAMPSIM_VERSION_MINOR
           << "." << SAMPSIM_VERSION_PATCH << std::endl;
    stream << "# type: " << this->get_type() << std::endl;
    stream << "# seed: " << this->seed << std::endl;
    stream << "# size: " << this->size << std::endl;
    stream << "# one_per_household: " << ( this->one_per_household ? "true" : "false" ) << std::endl;
    stream << "# age: " << sampsim::get_age_type_name( this->age ) << std::endl;
    stream << "# sex: " << sampsim::get_sex_type_name( this->sex ) << std::endl;
    return stream.str();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::to_csv( std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    household_stream << this->get_csv_header() << std::endl;
    individual_stream << this->get_csv_header() << std::endl;
    this->population->to_csv( household_stream, individual_stream );
  }
}
}
