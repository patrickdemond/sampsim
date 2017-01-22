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
#include "summary.h"
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
    this->number_of_samples = 1;
    this->number_of_towns = 1;
    this->write_sample_number = 0; // 0 => all samples
    this->current_size = 0;
    this->current_town_size = 0;
    this->one_per_household = false;
    this->resample_towns = false;
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
    this->number_of_towns = object->number_of_towns;
    this->write_sample_number = object->write_sample_number;
    this->current_size = object->current_size;
    this->current_town_size = object->current_town_size;
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

    // get the population's total number of individuals
    this->population->set_sample_mode( false );
    unsigned int total_individuals = this->population->get_number_of_individuals();

    // create look-up table of towns based on their first and last individual number (sequentially)
    unsigned int town_index = 0;
    unsigned int cumulative_individuals = 0;
    std::vector< std::pair< unsigned int, sampsim::town* > > town_lookup;
    town_lookup.resize( this->population->get_number_of_towns() );
    for( auto town_it = this->population->get_town_list_cbegin();
         town_it != this->population->get_town_list_cend();
         ++town_it )
    {
      sampsim::town *town = *town_it;
      cumulative_individuals += town->get_number_of_individuals();
      town_lookup[town_index] = std::pair< unsigned int, sampsim::town* >( cumulative_individuals, town );
      town_index++;
    }

    // now sample towns
    unsigned int individual_chunk = floor( total_individuals / this->number_of_towns );
    std::vector< town_list_type > sampled_town_list;
    for( unsigned int iteration = 0; iteration < this->number_of_samples; iteration++ )
    {
      town_list_type town_list;
      if( 0 == iteration || this->resample_towns )
      {
        unsigned int select_individual = utilities::random( 1, individual_chunk );
        for( unsigned int town_index = 0; town_index < this->number_of_towns; town_index++ )
        {
          // find the town who has the select-individual
          sampsim::town *last_town;
          auto lookup_it = town_lookup.cbegin();
          for( ; lookup_it != town_lookup.cend(); ++lookup_it )
          {
            if( select_individual < lookup_it->first ) break;
          }
          town_list.push_back( lookup_it->second );
          select_individual += individual_chunk;
        }
      }
      else
      {
        // make a copy of the first town list
        for( auto it = sampled_town_list.cbegin()->cbegin(); it != sampled_town_list.cbegin()->cend(); ++it )
        {
          town_list.push_back( *it );
        }
      }

      sampled_town_list.push_back( town_list );
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
      if( 0 < iteration ) this->reset_for_next_sample();

      bool first = true;
      int household_count = 0;

      // sample each town in the sampled town list
      for( auto it = sampled_town_list[iteration].cbegin(); it != sampled_town_list[iteration].cend(); ++it )
      {
        sampsim::town *town = *it;
        if( first ) first = false;
        else this->reset_for_next_sample( false );

        // create a building-tree from a list of all buildings in the town
        building_list_type building_list;
        for( auto tile_it = town->get_tile_list_cbegin();
             tile_it != town->get_tile_list_cend();
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

        if( utilities::verbose )
          utilities::output( "selecting from a list of %d buildings", building_list.size() );

        // keep selecting buildings until the ending condition has been met
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

          // select households within the building (this is step 4 of the algorithm)
          for( auto household_it = b->get_household_list_begin();
               household_it != b->get_household_list_end();
               ++household_it )
          {
            int count = 0;
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
                i->select( this->get_sample_weight( i ) );
                count++;
                if( this->get_one_per_household() ) break;
              }
            }

            if( count )
            {
              this->current_size += count;
              this->current_town_size += count;
              household_count++;

              // only select another household if we haven't reached our ending condition
              if( this->is_sample_complete() ) break;
            }
          }
          tree.remove( b );
        }
      }

      sampsim::population* sampled_population = new sampsim::population;
      sampled_population->copy( this->population ); // will only copy selected individuals
      this->sampled_population_list.push_back( sampled_population );

      if( 1 < this->number_of_towns )
      {
        utilities::output(
          "finished generating %s sample #%d, %d households selected across %d towns",
          this->get_type().c_str(),
          iteration + 1,
          household_count,
          this->number_of_towns);
      }
      else
      {
        utilities::output(
          "finished generating %s sample #%d, %d households selected",
          this->get_type().c_str(),
          iteration + 1,
          household_count );
      }
    }
    this->population->set_sample_mode( false );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::reset_for_next_sample( const bool full )
  {
    // full means we reset the population as well, this happens after all towns have been sampled
    if( full )
    {
      this->current_size = 0;
      this->first_building = NULL;
      if( this->population ) this->population->unselect();
    }
    this->current_town_size = 0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double sample::get_sample_weight( const sampsim::individual* individual ) const
  {
    unsigned int population_total = individual->get_population()->get_number_of_individuals();
    // return the ratio of the number of individuals in the individual's population to town
    double population_to_town_ratio =
      static_cast< double >( population_total ) /
      static_cast< double >( individual->get_town()->get_number_of_individuals() );

    // when choosing one individual per household include ratio of household size to (one) individual
    double household_to_individual_ratio = this->one_per_household ?
      static_cast< double >( individual->get_household()->get_number_of_individuals() ) :
      1.0;

    return 0 == population_total ? 0 : population_to_town_ratio * household_to_individual_ratio;
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

    sampsim::summary total_summary;
    std::vector< double > mean_vector, stdev_vector, squared_sum_vector;
    std::vector< sampsim::summary* > summary_vector;

    mean_vector.resize( sampsim::summary::category_size, 0 );
    stdev_vector.resize( sampsim::summary::category_size, 0 );
    squared_sum_vector.resize( sampsim::summary::category_size, 0 );
    summary_vector.reserve( this->sampled_population_list.size() );

    // get summaries of all populations and add them up as we go
    for( unsigned int samp_index = 0; samp_index < this->sampled_population_list.size(); samp_index++ )
    {
      sampsim::summary *sum = this->sampled_population_list[samp_index]->get_summary();
      summary_vector.push_back( sum );
      total_summary.add( this->sampled_population_list[samp_index] );
    }

    // determine standard deviations for all prevalences
    for( int cat_index = 0; cat_index < sampsim::summary::category_size; cat_index++ )
    {
      mean_vector[cat_index] = total_summary.get_count_fraction( cat_index );

      for( unsigned int samp_index = 0; samp_index < this->sampled_population_list.size(); samp_index++ )
      {
        double diff = 
          summary_vector[samp_index]->get_count_fraction( cat_index ) - mean_vector[cat_index];
        squared_sum_vector[cat_index] += diff*diff;
      }
      stdev_vector[cat_index] = sqrt(
        squared_sum_vector[cat_index] /
        static_cast<double>( this->sampled_population_list.size() - 1 )
      );
    }

    stream << "sampled total count: "
           << total_summary.get_count( sampsim::summary::all, sampsim::summary::diseased )
           << " diseased of "
           << total_summary.get_count( sampsim::summary::all, sampsim::summary::all )
           << " total "
           << "(prevalence " << mean_vector[0] << " (" << stdev_vector[0] << "))" << std::endl;

    stream << "sampled adult count: "
           << total_summary.get_count( sampsim::summary::adult, sampsim::summary::diseased )
           << " diseased of "
           << total_summary.get_count( sampsim::summary::adult, sampsim::summary::all )
           << " total "
           << "(prevalence " << mean_vector[1] << " (" << stdev_vector[1] << "))" << std::endl;

    stream << "sampled child count: "
           << total_summary.get_count( sampsim::summary::child, sampsim::summary::diseased )
           << " diseased of "
           << total_summary.get_count( sampsim::summary::child, sampsim::summary::all )
           << " total "
           << "(prevalence " << mean_vector[2] << " (" << stdev_vector[2] << "))" << std::endl;

    stream << "sampled male count: "
           << total_summary.get_count( sampsim::summary::male, sampsim::summary::diseased )
           << " diseased of "
           << total_summary.get_count( sampsim::summary::male, sampsim::summary::all )
           << " total "
           << "(prevalence " << mean_vector[3] << " (" << stdev_vector[3] << "))" << std::endl;

    stream << "sampled female count: "
           << total_summary.get_count( sampsim::summary::female, sampsim::summary::diseased )
           << " diseased of "
           << total_summary.get_count( sampsim::summary::female, sampsim::summary::all )
           << " total "
           << "(prevalence " << mean_vector[4] << " (" << stdev_vector[4] << "))" << std::endl;

    stream << "sampled male adult count: "
           << total_summary.get_count( sampsim::summary::adult_male, sampsim::summary::diseased )
           << " diseased of "
           << total_summary.get_count( sampsim::summary::adult_male, sampsim::summary::all )
           << " total "
           << "(prevalence " << mean_vector[5] << " (" << stdev_vector[5] << "))" << std::endl;

    stream << "sampled female adult count: "
           << total_summary.get_count( sampsim::summary::adult_female, sampsim::summary::diseased )
           << " diseased of "
           << total_summary.get_count( sampsim::summary::adult_female, sampsim::summary::all )
           << " total "
           << "(prevalence " << mean_vector[6] << " (" << stdev_vector[6] << "))" << std::endl;

    stream << "sampled male child count: "
           << total_summary.get_count( sampsim::summary::child_male, sampsim::summary::diseased )
           << " diseased of "
           << total_summary.get_count( sampsim::summary::child_male, sampsim::summary::all )
           << " total "
           << "(prevalence " << mean_vector[7] << " (" << stdev_vector[7] << "))" << std::endl;

    stream << "sampled female child count: "
           << total_summary.get_count( sampsim::summary::child_female, sampsim::summary::diseased )
           << " diseased of "
           << total_summary.get_count( sampsim::summary::child_female, sampsim::summary::all )
           << " total "
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
  void sample::set_number_of_towns( const unsigned int towns )
  {
    if( utilities::verbose ) utilities::output( "setting number_of_towns to %d", towns );
    this->number_of_towns = towns;
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
  void sample::set_resample_towns( const bool resample_towns )
  {
    if( utilities::verbose )
      utilities::output( "setting resample_towns to %s", resample_towns ? "true" : "false" );
    this->resample_towns = resample_towns;
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
    this->number_of_towns = json["number_of_towns"].asUInt();
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
    json["number_of_towns"] = this->number_of_towns;
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
    stream << "# number_of_towns: " << this->number_of_towns << std::endl;
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
