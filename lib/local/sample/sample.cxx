/*=========================================================================

  Program:  sampsim
  Module:   sample.cxx
  Language: C++

=========================================================================*/

#include "sample.h"

#include "building.h"
#include "building_tree.h"
#include "household.h"
#include "individual.h"
#include "population.h"
#include "tile.h"
#include "town.h"

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
    this->number_of_samples = 0;
    this->write_sample_number = 0; // 0 => all samples
    this->current_size = 0;
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
    std::for_each(
      this->sampled_population_list.begin(),
      this->sampled_population_list.end(),
      utilities::safe_delete_type() );
    this->sampled_population_list.clear();
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

    // create multinomial function using town populations (and unselect all towns in the process)
    this->population->set_sample_mode( false );
    int total_individuals = this->population->count_individuals().second;
    std::map< sampsim::town*, double > coefficients;
    for( auto town_it = this->population->get_town_list_cbegin();
         town_it != this->population->get_town_list_cend();
         ++town_it )
    {
      sampsim::town *town = *town_it;
      coefficients[town] = static_cast< double >( town->count_individuals().second ) /
                           static_cast< double >( total_individuals );
    }
    this->population->set_sample_mode( true );

    // delete all sampled populations
    std::for_each(
      this->sampled_population_list.begin(),
      this->sampled_population_list.end(),
      utilities::safe_delete_type() );
    this->sampled_population_list.clear();
    this->sampled_population_list.reserve( this->number_of_samples );

    // run selection number_of_samples times
    for( unsigned int iteration = 0; iteration < this->number_of_samples; iteration++ )
    {
      this->reset_for_next_sample();

      // select a town to sample
      sampsim::town *use_town = NULL;
      double lower = 0.0;
      double target = utilities::random();
      for( auto it = coefficients.cbegin(); it != coefficients.cend(); ++it )
      {
        double upper = lower + it->second;
        if( lower <= target && target < upper ) use_town = it->first;
        lower = upper;
      }

      // if we get here and we haven't selected a town then the total number of individuals in all towns
      // in the population doesn't appear to be the same as the population's count of individuals
      if( !use_town )
        throw std::runtime_error( "Cannot generate sample, error in multinomial distribution" );

      // create a list of all buildings in the selected town
      building_list_type building_list;

      for( auto tile_it = use_town->get_tile_list_cbegin();
           tile_it != use_town->get_tile_list_cend();
           ++tile_it )
      {
        for( auto building_it = tile_it->second->get_building_list_cbegin();
             building_it != tile_it->second->get_building_list_cend();
             ++building_it )
        {
          building_list.push_back( *building_it );
        }
      }

      // store the building list in a tree
      sampsim::building_tree tree( building_list );

      utilities::output( "selecting from a list of %d buildings", building_list.size() );

      // keep selecting buildings until the ending condition has been met
      int household_count = 0;
      while( !this->is_sample_complete() )
      {
        if( tree.is_empty() )
        {
          std::cout << "WARNING: unable to fulfill the sample's ending condition" << std::endl;
          break;
        }

        building* b = this->select_next_building( tree );

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
            this->current_size += count;
            household_count++;

            // only select another household if we haven't reached our ending condition
            if( this->is_sample_complete() ) break;
          }
        }
        tree.remove( b );
      }

      sampsim::population* sampled_population = new sampsim::population;
      sampled_population->copy( this->population ); // will only copy selected individuals
      this->sampled_population_list.push_back( sampled_population );

      utilities::output(
        "finished generating %s sample #%d, %d households selected",
        this->get_type().c_str(),
        iteration,
        household_count );
    }
    this->population->set_sample_mode( false );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::reset_for_next_sample()
  {
    this->current_size = 0;
    this->first_building = NULL;
    if( this->population )
      for( auto town_it = this->population->get_town_list_cbegin();
           town_it != this->population->get_town_list_cend();
           ++town_it )
        (*town_it)->unselect();
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
  void sample::write_summary( const std::string filename ) const
  {
    this->population->write_summary( filename );

    std::ofstream stream( filename + ".txt", std::ofstream::app );

    unsigned int individual_count = 0, diseased_individual_count = 0;
    for( auto it = this->get_sampled_population_list_cbegin();
              it != this->get_sampled_population_list_cend();
              ++it )
    {
      std::pair<unsigned int, unsigned int> count = (*it)->count_individuals();
      diseased_individual_count += count.first;
      individual_count += count.second;
    }

    double prevalence = static_cast<double>( diseased_individual_count ) / static_cast<double>( individual_count ),
           squared_sum = 0.0;
    for( auto it = this->get_sampled_population_list_cbegin();
              it != this->get_sampled_population_list_cend();
              ++it )
    {
      std::pair<unsigned int, unsigned int> count = (*it)->count_individuals();
      double diff = ( static_cast<double>( count.first ) / static_cast<double>( count.second ) ) - prevalence;
      squared_sum += diff*diff;
    }
    double stdev = sqrt( squared_sum / this->sampled_population_list.size() );

    stream << "sampled invididual count: " << individual_count << std::endl;
    stream << "sampled diseased individual count: " << diseased_individual_count << std::endl;
    stream << "sampled prevalence: " << prevalence << " (" << stdev << ")" << std::endl;
    
    stream.close();
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
  void sample::set_number_of_samples( const unsigned int samples )
  {
    if( utilities::verbose ) utilities::output( "setting number_of_samples to %d", samples );
    this->number_of_samples = samples;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_write_sample_number( const unsigned int number )
  {
    if( utilities::verbose ) utilities::output( "setting write_sample_number to %d", number );
    this->write_sample_number = number;
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
    this->number_of_samples = json["number_of_samples"].asUInt();
    this->one_per_household = json["one_per_household"].asBool();
    this->age = sampsim::get_age_type( json["age"].asString() );
    this->sex = sampsim::get_sex_type( json["sex"].asString() );
    this->population->from_json( json["population"] );

    this->sampled_population_list.reserve( json["sampled_population_list"].size() );
    for( unsigned int c = 0; c < json["sampled_population_list"].size(); c++ )
    {
      sampsim::population *p = new sampsim::population();
      p->from_json( json["sampled_population_list"][c] );
      this->sampled_population_list.push_back( p );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["type"] = this->get_type().c_str();
    json["seed"] = this->seed;
    json["number_of_samples"] = this->number_of_samples;
    json["one_per_household"] = this->one_per_household;
    json["age"] = sampsim::get_age_type_name( this->age );
    json["sex"] = sampsim::get_sex_type_name( this->sex );

    unsigned int diseased = 0, total = 0;
    for( auto it = this->get_sampled_population_list_cbegin();
              it != this->get_sampled_population_list_cend();
              ++it )
    {
      std::pair<unsigned int, unsigned int> count = (*it)->count_individuals();
      diseased += count.first;
      total += count.second;
    }

    double prevalence = static_cast<double>( diseased ) / static_cast<double>( total ), squared_sum = 0.0;
    for( auto it = this->get_sampled_population_list_cbegin();
              it != this->get_sampled_population_list_cend();
              ++it )
    {
      std::pair<unsigned int, unsigned int> count = (*it)->count_individuals();
      double diff = ( static_cast<double>( count.first ) / static_cast<double>( count.second ) ) - prevalence;
      squared_sum += diff*diff;
    }

    json["sampled_diseased_individual_sum"] = diseased;
    json["sampled_individual_sum"] = total;
    json["sampled_disease_prevalence"] = prevalence;
    json["sampled_disease_stdev"] = sqrt( squared_sum / this->sampled_population_list.size() );
    json["population"] = Json::Value( Json::objectValue );
    this->population->to_json( json["population"] );

    json["sampled_population_list"] = Json::Value( Json::arrayValue );
    for( auto it = this->sampled_population_list.cbegin(); it != this->sampled_population_list.cend(); ++it )
    {
      Json::Value child;
      (*it)->to_json( child );
      json["sampled_population_list"].append( child );
    }
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
    stream << "# number_of_samples: " << this->number_of_samples << std::endl;
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

    if( 0 < this->write_sample_number )
    {
      if( this->write_sample_number > this->sampled_population_list.size() )
      {
        std::stringstream stream;
        stream << "Told to write sample #" << this->write_sample_number << " but only "
               << this->sampled_population_list.size() << " samples exist.";
        throw std::runtime_error( stream.str() );
      }

      this->sampled_population_list[this->write_sample_number-1]->to_csv( household_stream, individual_stream );
    }
    else
    { // write all samples
      unsigned int number = 1;
      for( auto it = this->sampled_population_list.cbegin(); it != this->sampled_population_list.cend(); ++it )
      {
        household_stream << "# sample " << number << std::endl
                         << "# -----------------------------------------------------------------------"
                         << std::endl;
        individual_stream << "# sample " << number << std::endl
                          << "# -----------------------------------------------------------------------"
                          << std::endl;
        (*it)->to_csv( household_stream, individual_stream );
        number++;
      }
    }
  }
}
}
