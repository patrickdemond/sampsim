/*=========================================================================

  Program:  sampsim
  Module:   sample.cxx
  Language: C++

=========================================================================*/

#include "sample.h"

#include "archive.h"
#include "archive_entry.h"
#include "building.h"
#include "household.h"
#include "individual.h"
#include "population.h"
#include "summary.h"
#include "tile.h"
#include "town.h"

#include <fstream>
#include <json/reader.h>
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
    this->seed = "";
    this->use_sample_weights = false;
    this->sample_part = 1;
    this->number_of_sample_parts = 1;
    this->number_of_samples = 1;
    this->first_sample_index = 0;
    this->last_sample_index = 0;
    this->number_of_towns = 1;
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
    this->use_sample_weights = object->use_sample_weights;
    this->sample_part = object->sample_part;
    this->number_of_sample_parts = object->number_of_sample_parts;
    this->number_of_samples = object->number_of_samples;
    this->number_of_towns = object->number_of_towns;
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

    // modify the seed if we are running the sample in parts
    if( 1 < this->number_of_sample_parts && 1 < this->sample_part )
      this->set_seed( std::to_string( atoi( this->get_seed().c_str() ) + this->sample_part - 1 ) );

    // get the population's total number of individuals
    this->population->set_sample_mode( false );
    unsigned int total_individuals = this->population->get_number_of_individuals();

    // create look-up table of towns based on their first and last individual number (sequentially)
    unsigned int cumulative_individuals = 0;
    std::vector< std::pair< unsigned int, sampsim::town* > > town_lookup;
    std::vector< unsigned int > town_count_list;
    town_lookup.resize( this->population->get_number_of_towns() );
    for( auto town_it = this->population->get_town_list_cbegin();
         town_it != this->population->get_town_list_cend();
         ++town_it )
    {
      sampsim::town *town = *town_it;
      unsigned int count = town->get_number_of_individuals();
      town_count_list.push_back( count );
      cumulative_individuals += count;
      town_lookup.push_back( std::pair< unsigned int, sampsim::town* >( cumulative_individuals, town ) );
    }

    // now sample towns
    unsigned int individual_chunk = floor( total_individuals / this->number_of_towns );
    std::vector< std::vector< unsigned int > > sampled_town_index_list;
    for( unsigned int iteration = this->first_sample_index; iteration <= this->last_sample_index; iteration++ )
    {
      std::vector< unsigned int > town_index_list;
      if( this->first_sample_index == iteration || this->resample_towns )
      {
        unsigned int select_individual = utilities::random( 1, individual_chunk );
        for( unsigned int town_index = 0; town_index < this->number_of_towns; town_index++ )
        {
          // find the town who has the select-individual
          for( unsigned int index = 0; index < town_lookup.size(); ++index )
          {
            if( select_individual < town_lookup[index].first )
            {
              town_index_list.push_back( index );
              break;
            }
          }
          select_individual += individual_chunk;
        }
      }
      else
      {
        // make a copy of the first town index list
        for( auto it = sampled_town_index_list.cbegin()->cbegin();
             it != sampled_town_index_list.cbegin()->cend();
             ++it ) town_index_list.push_back( *it );
      }

      sampled_town_index_list.push_back( town_index_list );
    }

    this->population->set_sample_mode( true );

    // delete all sampled populations
    std::for_each(
      this->sampled_population_list.begin(),
      this->sampled_population_list.end(),
      utilities::safe_delete_type() );
    this->sampled_population_list.clear();
    this->sampled_population_list.reserve( this->last_sample_index - this->first_sample_index + 1 );

    // run selection from the first to the last sample index
    for( unsigned int iteration = this->first_sample_index; iteration <= this->last_sample_index; iteration++ )
    {
      try
      {
        if( this->first_sample_index < iteration ) this->reset_for_next_sample();

        bool first = true;
        int household_count = 0;

        // sample each town in the sampled town list
        int s_index = iteration - this->first_sample_index;
        for( auto it = sampled_town_index_list[s_index].cbegin();
             it != sampled_town_index_list[s_index].cend();
             ++it )
        {
          sampsim::town *town = town_lookup[*it].second;
          this->current_town_individual_fraction = 0 == total_individuals ? 0.0 :
            town_count_list[town->get_index()] / static_cast< double >( total_individuals );
          if( first ) first = false;
          else this->reset_for_next_sample( false );

          building_list_type building_list;
          this->create_building_list( town, building_list );

          if( utilities::verbose )
            utilities::output( "selecting from a list of %d buildings", building_list.size() );

          // create a list of all sampled individuals so that we can weight them after selection is done
          individual_list_type selected_individual_list;

          // keep selecting buildings until the ending condition has been met
          building* last_building = NULL;
          while( !this->is_sample_complete() )
          {
            if( building_list.empty() )
            {
              std::cout << "WARNING: unable to fulfill the sample's ending condition" << std::endl;
              break;
            }

            building* b = this->select_next_building( building_list );
            if( b == last_building )
            {
              utilities::output( "there are %d buildings left in the list", building_list.size() );
              std::cout << "WARNING: unable to fulfill the sample's ending condition ("
                        << building_list.size()
                        << " buildings left)" << std::endl;
              break;
            }
            last_building = b;

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
                  if( this->use_sample_weights ) selected_individual_list.push_back( i );
                  i->select();
                  if( this->use_sample_weights ) i->set_sample_weight( this->get_immediate_sample_weight( i ) );

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

            auto it = std::find( building_list.begin(), building_list.end(), b );
            if( it == building_list.end() ) std::cout << "ERROR: Can't find " << b << " in sample's building list" << std::endl;
            else building_list.erase( it );
          }

          // apply post-sample weighting factor to all selected individuals
          if( this->use_sample_weights )
          {
            double factor = this->get_post_sample_weight_factor();

            // now determine the sample weight for all selected individuals
            for( auto individual_it = selected_individual_list.begin();
                 individual_it != selected_individual_list.end();
                 ++individual_it )
            {
              individual *i = *individual_it;
              i->set_sample_weight( i->get_sample_weight() * factor );
            }
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
      catch( std::runtime_error &e )
      {
        std::cout << "ERROR: " << e.what() << std::endl;
      }
    }
    this->population->set_sample_mode( false );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::create_building_list( sampsim::town *town, building_list_type &building_list )
  {
    // create a linked list all buildings in the town
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
  double sample::get_immediate_sample_weight( const sampsim::individual* individual ) const
  {
    // when choosing one individual per household include ratio of household size to (one) individual
    return ( this->one_per_household ?
      static_cast< double >( individual->get_household()->get_summary()->get_count( 0, this->age, this->sex ) ) :
      1.0 );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double sample::get_post_sample_weight_factor() const
  {
    return 1.0 / this->current_town_individual_fraction;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool sample::read( const std::string filename )
  {
    utilities::output( "reading %s sample from %s", this->get_type().c_str(), filename.c_str() );

    bool success = true;
    try
    {
      Json::Value sampler_root, population_root;
      Json::Reader reader;
      file_list_type files = utilities::read_gzip( filename );
      bool sampler_loaded = false, population_loaded = false;

      for( auto it = files.cbegin(); it != files.cend() && success; ++it )
      {
        std::vector< std::string > parts = utilities::explode( it->first, "." );
        int size = parts.size();
        if( "sampler" == parts.at( size-2 ) )
        {
          success = reader.parse( it->second, sampler_root, false );
          if( success ) this->from_json( sampler_root );
          sampler_loaded = true;
        }
        else if( "population" == parts.at( size-2 ) )
        {
          success = reader.parse( it->second, population_root, false );
          if( success )
          {
            this->population = new sampsim::population;
            this->population->from_json( population_root );
            this->owns_population = true;
          }
          population_loaded = true;
        }
      }

      if( !sampler_loaded )
      {
        std::cout << "ERROR: sample file \"" << filename << "\" is missing its .sampler file" << std::endl;
      }
      else if( !population_loaded )
      {
        std::cout << "ERROR: sample file \"" << filename << "\" is missing its .population file" << std::endl;
      }
      else if( success )
      {
        this->population->set_use_sample_weights( this->use_sample_weights );
        this->sampled_population_list.resize( this->number_of_samples, NULL );

        for( auto it = files.cbegin(); it != files.cend() && success; ++it )
        {
          // now get all sampled populations
          std::vector< std::string > parts = utilities::explode( it->first, "." );
          int size = parts.size();
          if( "sampler" != parts.at( size-2 ) && "population" != parts.at( size-2 ) )
          {
            unsigned int index = atoi( parts.at( size-2 ).substr(1).c_str() ) - 1;

            Json::Value sampled_population_root;
            success = reader.parse( it->second, sampled_population_root, false );
            if( success )
            {
              sampsim::population* sampled_population = new sampsim::population;
              sampled_population->from_json( sampled_population_root );
              sampled_population->select_all();
              this->sampled_population_list[index] = sampled_population;
            }
          }
        }
      }

      if( !success )
      {
        std::cout << "ERROR: failed to parse file \"" << filename << "\"" << std::endl
                  << reader.getFormattedErrorMessages();
      }
    }
    catch( std::runtime_error &e )
    {
      std::cout << "ERROR: " << e.what() << std::endl;
      success = false;
    }

    return success;
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
      int sample_width = floor( log10( this->number_of_samples ) ) + 1;
      std::stringstream stream, household_stream, individual_stream;
      file_list_type files;

      for( unsigned int s = this->first_sample_index + 1; s < this->last_sample_index + 2; s++ )
      {
        stream.str( "" );
        household_stream.str( "" );
        individual_stream.str( "" );
        stream << filename;
        if( 1 < this->number_of_samples ) stream << ".s" << std::setw( sample_width ) << std::setfill( '0' ) << s;
        utilities::write_sample_number = s;
        this->to_csv( household_stream, individual_stream );

        files[stream.str() + ".household.csv"] = household_stream.str();
        files[stream.str() + ".individual.csv"] = individual_stream.str();
      }

      utilities::write_gzip( filename + ".flat", files, true );
    }
    else
    {
      int sample_width = floor( log10( this->number_of_samples ) ) + 1;
      std::stringstream stream;
      file_list_type files;
      Json::StyledWriter writer;
      Json::Value sampler_root, population_root;

      // write the sampler's data
      this->to_json( sampler_root );
      files[filename + ".sampler.json"] = writer.write( sampler_root );

      // write the population's data
      this->population->to_json( population_root );
      files[filename + ".population.json"] = writer.write( population_root );

      // write the sampled populations' data
      unsigned int s = this->first_sample_index + 1;
      for( auto it = this->sampled_population_list.cbegin(); it != this->sampled_population_list.cend(); ++it )
      {
        stream.str( "" );
        stream << filename;
        if( 1 < this->number_of_samples ) stream << ".s" << std::setw( sample_width ) << std::setfill( '0' ) << s;
        Json::Value sampled_population_root;
        (*it)->to_json( sampled_population_root );
        files[stream.str() + ".json"] = writer.write( sampled_population_root );
        s++;
      }

      utilities::write_gzip( filename + ".json", files, true );
    }

    utilities::output( "finished writing %s sample", this->get_type().c_str() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::write_summary( const std::string filename ) const
  {
    this->population->write_summary( filename );

    std::ofstream stream( filename + ".csv", std::ofstream::app );
    stream << std::endl;

    // get summaries of all populations and add them up as we go
    std::vector< sampsim::summary* > summary_list;
    for( auto it = this->sampled_population_list.cbegin(); it != this->sampled_population_list.cend(); ++it )
      summary_list.push_back( (*it)->get_summary() );
    sampsim::summary::write( summary_list, this->use_sample_weights, stream );

    stream.close();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::write_variance( const std::string filename ) const
  {
    std::stringstream name_stream;
    std::pair< double, double > variance;
    for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ )
    {
      name_stream.str( "" );
      name_stream << filename << "." << utilities::rr[rr] << ".variance.csv";
      std::ofstream stream( name_stream.str(), std::ofstream::app );

      // calculate the proportion and variance for all populations
      for( auto it = this->sampled_population_list.cbegin(); it != this->sampled_population_list.cend(); ++it )
      {
        variance = (*it)->get_variance( rr );
        stream << variance.first << "," << variance.second << std::endl;
      }

      stream.close();
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool sample::set_population( const std::string filename )
  {
    this->delete_population();
    this->population = new sampsim::population;
    bool result = this->population->read( filename );
    this->population->set_sample_mode( true );
    this->population->set_use_sample_weights( this->use_sample_weights );
    this->owns_population = true;
    return result;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool sample::set_population( sampsim::population *population )
  {
    this->delete_population();
    this->population = population; // copy pointer but do not delete when finished
    this->population->set_sample_mode( true );
    this->population->set_use_sample_weights( this->use_sample_weights );
    this->owns_population = false;
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
  void sample::set_use_sample_weights( const bool use_sample_weights )
  {
    if( utilities::verbose )
      utilities::output( "setting use_sample_weights to %s", use_sample_weights ? "true" : "false" );
    this->use_sample_weights = use_sample_weights;
    if( this->population ) this->population->set_use_sample_weights( this->use_sample_weights );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::recalculate_sample_indeces()
  {
    double fraction = static_cast< double >( this->number_of_samples ) /
                      static_cast< double >( this->number_of_sample_parts );
    this->first_sample_index = floor( fraction * ( this->sample_part - 1 ) );
    this->last_sample_index = floor( fraction * this->sample_part ) - 1;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_sample_part( const unsigned int samples )
  {
    if( utilities::verbose ) utilities::output( "setting sample_part to %d", samples );
    this->sample_part = samples;
    this->recalculate_sample_indeces();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_number_of_sample_parts( const unsigned int samples )
  {
    if( utilities::verbose ) utilities::output( "setting number_of_sample_parts to %d", samples );
    this->number_of_sample_parts = samples;
    this->recalculate_sample_indeces();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_number_of_samples( const unsigned int samples )
  {
    if( utilities::verbose ) utilities::output( "setting number_of_samples to %d", samples );
    this->number_of_samples = samples;
    this->recalculate_sample_indeces();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::set_number_of_towns( const unsigned int towns )
  {
    if( utilities::verbose ) utilities::output( "setting number_of_towns to %d", towns );
    this->number_of_towns = towns;
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
      stream << "Tried to unserialize \"" << json["type"].asString() << "\" sample as a \""
             << this->get_type() << "\" sample";
      throw std::runtime_error( stream.str() );
    }

    this->seed = json["seed"].asString();
    this->use_sample_weights = json["use_sample_weights"].asBool();
    this->number_of_samples = json["number_of_samples"].asUInt();
    this->number_of_towns = json["number_of_towns"].asUInt();
    this->one_per_household = json["one_per_household"].asBool();
    this->age = sampsim::get_age_type( json["age"].asString() );
    this->sex = sampsim::get_sex_type( json["sex"].asString() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["type"] = this->get_type().c_str();
    json["seed"] = this->seed;
    json["use_sample_weights"] = this->use_sample_weights;
    json["number_of_samples"] = this->number_of_samples;
    json["number_of_towns"] = this->number_of_towns;
    json["one_per_household"] = this->one_per_household;
    json["age"] = sampsim::get_age_type_name( this->age );
    json["sex"] = sampsim::get_sex_type_name( this->sex );
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
    stream << "# use_sample_weights: " << ( this->use_sample_weights ? "true" : "false" ) << std::endl;
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
    int index = utilities::write_sample_number - this->first_sample_index - 1;
    this->sampled_population_list[index]->to_csv( household_stream, individual_stream );
  }
}
}
