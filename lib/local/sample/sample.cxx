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
  void sample::copy( const sample* object )
  {
    this->number_of_samples = object->number_of_samples;
    this->write_sample_number = object->write_sample_number;
    this->current_size = object->current_size;
    this->one_per_household = object->one_per_household;
    this->age = object->age;
    this->sex = object->sex;
    std::cout << "WARNING: copying samples is unable to preserve the first_building selected by the sampler"
              << std::endl;
    this->first_building = NULL;

    this->delete_population();
    if( object->owns_population ) this->population->copy( object->population );
    else this->population = object->population;
    this->owns_population = object->owns_population;

    std::for_each(
      this->sampled_population_list.begin(),
      this->sampled_population_list.end(),
      utilities::safe_delete_type() );
    this->sampled_population_list.clear();
    this->sampled_population_list.reserve( this->number_of_samples );

    auto oit = object->sampled_population_list.cbegin();
    auto it = this->sampled_population_list.begin();
    for( ; oit != object->sampled_population_list.cend() && it != this->sampled_population_list.end();
         ++oit, ++it )
    {
      ( *it )->copy( *oit );
    }
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
    this->delete_population();
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
    std::vector< std::pair<unsigned int, unsigned int> > count_vector = this->population->count_individuals();
    unsigned int total_individuals = count_vector[0].first + count_vector[0].second;
    std::map< sampsim::town*, double > coefficients;
    for( auto town_it = this->population->get_town_list_cbegin();
         town_it != this->population->get_town_list_cend();
         ++town_it )
    {
      sampsim::town *town = *town_it;
      count_vector = town->count_individuals();
      unsigned int town_individuals = count_vector[0].first + count_vector[0].second;
      coefficients[town] = static_cast< double >( town_individuals ) /
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
        if( NULL == this->first_building )
        {
          this->first_building = b;
          std::cout << "######## setting first building to " << b << " at " << b->get_position().get_a() << std::endl;
        }

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

    std::vector< std::pair<unsigned int, unsigned int> > total_vector;
    std::vector< double > mean_vector, stdev_vector, squared_sum_vector;
    std::vector< std::vector< std::pair<unsigned int, unsigned int> > > sample_count_vector;

    mean_vector.resize( 9, 0 );
    stdev_vector.resize( 9, 0 );
    squared_sum_vector.resize( 9, 0 );
    sample_count_vector.reserve( this->sampled_population_list.size() );

    for( unsigned int s = 0; s < this->sampled_population_list.size(); s++ )
    {
      sample_count_vector[s] = this->sampled_population_list[s]->count_individuals();
      for( std::vector< std::pair<unsigned int, unsigned int> >::size_type i = 0; i < 9; i++ )
      {
        total_vector[i].first += sample_count_vector[s][i].first;
        total_vector[i].second += sample_count_vector[s][i].second;
      }
    }

    // determine standard deviations for all prevalences
    for( std::vector< std::pair<unsigned int, unsigned int> >::size_type i = 0; i < 9; i++ )
    {
      for( unsigned int s = 0; s < this->sampled_population_list.size(); s++ )
      {
        double diff = 
          ( static_cast<double>( sample_count_vector[s][i].first ) /
            static_cast<double>( sample_count_vector[s][i].first + sample_count_vector[s][i].second ) ) -
          ( static_cast<double>( total_vector[i].first ) /
            static_cast<double>( total_vector[i].first + total_vector[i].second ) );
        squared_sum_vector[i] += diff*diff;
      }
      mean_vector[i] = squared_sum_vector[i] /
                       static_cast<double>( total_vector[i].first + total_vector[i].second );
    }

    stream << "individual count: " << total_vector[0].first << " diseased of "
           << ( total_vector[0].first + total_vector[0].second ) << " total "
           << "(prevalence " << mean_vector[0] << " (" << stdev_vector[0] << "))" << std::endl;

    stream << "adult count: " << total_vector[1].first << " diseased of "
           << ( total_vector[1].first + total_vector[1].second ) << " total "
           << "(prevalence " << mean_vector[1] << " (" << stdev_vector[1] << "))" << std::endl;

    stream << "child count: " << total_vector[2].first << " diseased of "
           << ( total_vector[2].first + total_vector[2].second ) << " total "
           << "(prevalence " << mean_vector[2] << " (" << stdev_vector[2] << "))" << std::endl;

    stream << "male count: " << total_vector[3].first << " diseased of "
           << ( total_vector[3].first + total_vector[3].second ) << " total "
           << "(prevalence " << mean_vector[3] << " (" << stdev_vector[3] << "))" << std::endl;

    stream << "female count: " << total_vector[4].first << " diseased of "
           << ( total_vector[4].first + total_vector[4].second ) << " total "
           << "(prevalence " << mean_vector[4] << " (" << stdev_vector[4] << "))" << std::endl;

    stream << "male adult count: " << total_vector[5].first << " diseased of "
           << ( total_vector[5].first + total_vector[5].second ) << " total "
           << "(prevalence " << mean_vector[5] << " (" << stdev_vector[5] << "))" << std::endl;

    stream << "female adult count: " << total_vector[6].first << " diseased of "
           << ( total_vector[6].first + total_vector[6].second ) << " total "
           << "(prevalence " << mean_vector[6] << " (" << stdev_vector[6] << "))" << std::endl;

    stream << "male child count: " << total_vector[7].first << " diseased of "
           << ( total_vector[7].first + total_vector[7].second ) << " total "
           << "(prevalence " << mean_vector[7] << " (" << stdev_vector[7] << "))" << std::endl;

    stream << "female child count: " << total_vector[8].first << " diseased of "
           << ( total_vector[8].first + total_vector[8].second ) << " total "
           << "(prevalence " << mean_vector[8] << " (" << stdev_vector[8] << "))" << std::endl;

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
