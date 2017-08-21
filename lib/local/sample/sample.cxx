/*=========================================================================

  Program:  sampsim
  Module:   sample.cxx
  Language: C++

=========================================================================*/

#include "sample.h"

#include "archive.h"
#include "archive_entry.h"
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
    for( unsigned int iteration = this->first_sample_index; iteration <= this->last_sample_index; iteration++ )
    {
      town_list_type town_list;
      if( this->first_sample_index == iteration || this->resample_towns )
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
    this->sampled_population_list.reserve( this->last_sample_index - this->first_sample_index + 1 );

    // run selection from the first to the last sample index
    for( unsigned int iteration = this->first_sample_index; iteration <= this->last_sample_index; iteration++ )
    {
      if( this->first_sample_index < iteration ) this->reset_for_next_sample();

      bool first = true;
      int household_count = 0;

      // sample each town in the sampled town list
      int sampled_town_list_index = iteration - this->first_sample_index;
      for( auto it = sampled_town_list[sampled_town_list_index].cbegin();
           it != sampled_town_list[sampled_town_list_index].cend();
           ++it )
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
        building* last_building = NULL;
        while( !this->is_sample_complete() )
        {
          if( tree.is_empty() )
          {
            std::cout << "WARNING: unable to fulfill the sample's ending condition" << std::endl;
            break;
          }

          building* b = this->select_next_building( tree );
          if( b == last_building )
          {
            utilities::output( "there are %d buildings left in the tree", tree.get_building_list().size() );
            std::cout << "WARNING: unable to fulfill the sample's ending condition ("
                      << tree.get_building_list().size()
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
                if( this->use_sample_weights ) i->select( this->get_sample_weight( i ) );
                else i->select();
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
    unsigned int pop_count = individual->get_population()->get_summary()->get_count( this->age, this->sex );

    if( 0 == pop_count )
    {
      return 0;
    }
    else
    {
      unsigned int town_count = individual->get_town()->get_summary()->get_count( this->age, this->sex );

      return
        // return the ratio of the number of individuals in the individual's population to town
        static_cast< double >( pop_count ) / static_cast< double >( town_count )
        *
        // when choosing one individual per household include ratio of household size to (one) individual
        this->one_per_household ?
          static_cast< double >( individual->get_household()->get_summary()->get_count( this->age, this->sex ) ) :
          1.0;
    }
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
      Json::Value root;
      this->to_json( root );
      Json::StyledWriter writer;
      utilities::write_gzip( filename + ".json", writer.write( root ) );
    }

    utilities::output( "finished writing %s sample", this->get_type().c_str() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void sample::write_summary( const std::string filename ) const
  {
    this->population->write_summary( filename );

    std::ofstream stream( filename + ".txt", std::ofstream::app );

    sampsim::summary total_summary;
    double mean_array[9], stdev_array[9], squared_sum_array[9];
    double wmean_array[9], wstdev_array[9], wsquared_sum_array[9];
    std::vector< sampsim::summary* > summary_vector;

    summary_vector.reserve( this->sampled_population_list.size() );

    // get summaries of all populations and add them up as we go
    for( unsigned int samp_index = 0; samp_index < this->sampled_population_list.size(); samp_index++ )
    {
      sampsim::summary *sum = this->sampled_population_list[samp_index]->get_summary();
      summary_vector.push_back( sum );
      total_summary.add( this->sampled_population_list[samp_index] );
    }

    // determine standard deviations for all prevalences
    age_type a;
    sex_type s;
    for( unsigned int index = 0; index < 9; index++ )
    {
      if( 0 <= index && index < 3 ) a = ANY_AGE;
      else if( 3 <= index && index < 6 ) a = ADULT;
      else if( 6 <= index && index < 9 ) a = CHILD;

      if( 0 == index % 3 ) s = ANY_SEX;
      else if( 1 == index % 3 ) s = MALE;
      else if( 2 == index % 3 ) s = FEMALE;

      double diff;
      mean_array[index] = total_summary.get_count_fraction( a, s );
      if( this->use_sample_weights ) wmean_array[index] = total_summary.get_weighted_count_fraction( a, s );

      for( unsigned int samp_index = 0; samp_index < this->sampled_population_list.size(); samp_index++ )
      {
        diff = summary_vector[samp_index]->get_count_fraction( a, s ) - mean_array[index];
        squared_sum_array[index] += diff*diff;
        if( this->use_sample_weights )
        {
          diff = summary_vector[samp_index]->get_weighted_count_fraction( a, s ) - wmean_array[index];
          wsquared_sum_array[index] += diff*diff;
        }
      }
      stdev_array[index] = sqrt(
        squared_sum_array[index] / static_cast<double>( this->sampled_population_list.size() - 1 )
      );
      if( this->use_sample_weights )
      {
        wstdev_array[index] = sqrt(
          wsquared_sum_array[index] / static_cast<double>( this->sampled_population_list.size() - 1 )
        );
      }
    }

    stream << "sampled total count: " << total_summary.get_count( ANY_AGE, ANY_SEX, DISEASED )
           << " diseased of " << total_summary.get_count( ANY_AGE, ANY_SEX )
           << " total (prevalence " << mean_array[0] << " (" << stdev_array[0] << "))";
    if( this->use_sample_weights )
      stream << " (weighted prevalence " << wmean_array[0] << " (" << wstdev_array[0] << "))";
    stream << std::endl;

    stream << "sampled adult count: " << total_summary.get_count( ADULT, ANY_SEX, DISEASED )
           << " diseased of " << total_summary.get_count( ADULT, ANY_SEX )
           << " total (prevalence " << mean_array[3] << " (" << stdev_array[3] << "))";
    if( this->use_sample_weights )
      stream << " (weighted prevalence " << wmean_array[3] << " (" << wstdev_array[3] << "))";
    stream << std::endl;

    stream << "sampled child count: " << total_summary.get_count( CHILD, ANY_SEX, DISEASED )
           << " diseased of " << total_summary.get_count( CHILD, ANY_SEX )
           << " total (prevalence " << mean_array[6] << " (" << stdev_array[6] << "))";
    if( this->use_sample_weights )
      stream << " (weighted prevalence " << wmean_array[6] << " (" << wstdev_array[6] << "))";
    stream << std::endl;

    stream << "sampled male count: " << total_summary.get_count( ANY_AGE, MALE, DISEASED )
           << " diseased of " << total_summary.get_count( ANY_AGE, MALE )
           << " total (prevalence " << mean_array[1] << " (" << stdev_array[1] << "))";
    if( this->use_sample_weights )
      stream << " (weighted prevalence " << wmean_array[1] << " (" << wstdev_array[1] << "))";
    stream << std::endl;

    stream << "sampled female count: " << total_summary.get_count( ANY_AGE, FEMALE, DISEASED )
           << " diseased of " << total_summary.get_count( ANY_AGE, FEMALE )
           << " total (prevalence " << mean_array[2] << " (" << stdev_array[2] << "))";
    if( this->use_sample_weights )
      stream << " (weighted prevalence " << wmean_array[2] << " (" << wstdev_array[2] << "))";
    stream << std::endl;

    stream << "sampled male adult count: " << total_summary.get_count( ADULT, MALE, DISEASED )
           << " diseased of " << total_summary.get_count( ADULT, MALE )
           << " total (prevalence " << mean_array[4] << " (" << stdev_array[4] << "))";
    if( this->use_sample_weights )
      stream << " (weighted prevalence " << wmean_array[4] << " (" << wstdev_array[4] << "))";
    stream << std::endl;

    stream << "sampled female adult count: " << total_summary.get_count( ADULT, FEMALE, DISEASED )
           << " diseased of " << total_summary.get_count( ADULT, FEMALE )
           << " total (prevalence " << mean_array[5] << " (" << stdev_array[5] << "))";
    if( this->use_sample_weights )
      stream << " (weighted prevalence " << wmean_array[5] << " (" << wstdev_array[5] << "))";
    stream << std::endl;

    stream << "sampled male child count: " << total_summary.get_count( CHILD, MALE, DISEASED )
           << " diseased of " << total_summary.get_count( CHILD, MALE )
           << " total (prevalence " << mean_array[7] << " (" << stdev_array[7] << "))";
    if( this->use_sample_weights )
      stream << " (weighted prevalence " << wmean_array[7] << " (" << wstdev_array[7] << "))";
    stream << std::endl;

    stream << "sampled female child count: " << total_summary.get_count( CHILD, FEMALE, DISEASED )
           << " diseased of " << total_summary.get_count( CHILD, FEMALE )
           << " total (prevalence " << mean_array[8] << " (" << stdev_array[8] << "))";
    if( this->use_sample_weights )
      stream << " (weighted prevalence " << wmean_array[8] << " (" << wstdev_array[8] << "))";
    stream << std::endl;

    stream.close();
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
      stream << "Tried to unserialize " << json["type"].asString() << " sample as a "
             << this->get_type() << " sample";
      throw std::runtime_error( stream.str() );
    }

    this->seed = json["seed"].asString();
    this->use_sample_weights = json["use_sample_weights"].asBool();
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
    json["use_sample_weights"] = this->use_sample_weights;
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
