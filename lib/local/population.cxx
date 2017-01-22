/*=========================================================================

  Program:  sampsim
  Module:   population.cxx
  Language: C++

=========================================================================*/

#include "population.h"

#include "building.h"
#include "household.h"
#include "individual.h"
#include "summary.h"
#include "town.h"
#include "trend.h"
#include "utilities.h"

#include <ctime>
#include <fstream>
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>
#include <stdexcept>
#include <utility>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  population::population()
  {
    this->sample_mode = false;
    this->current_household_index = 0;
    this->current_individual_index = 0;
    this->seed = "";
    this->number_of_towns = 1;
    this->number_of_tiles_x = 0;
    this->number_of_tiles_y = 0;
    this->tile_width = 0;
    this->target_prevalence = 0.5;
    this->population_density_slope[0] = 0.0;
    this->population_density_slope[1] = 0.0;
    this->river_probability = 0.0;
    this->river_width = 0.0;
    this->number_of_disease_pockets = 0;
    for( unsigned int c = 0; c < population::NUMBER_OF_DISEASE_WEIGHTS; c++ )
      this->disease_weights[c] = 1.0;
    this->mean_household_population = 0;
    this->mean_income = new trend;
    this->sd_income = new trend;
    this->mean_disease = new trend;
    this->sd_disease = new trend;
    this->pocket_kernel_type = "exponential";
    this->pocket_scaling = 1.0;
    this->town_size_min = 0.0;
    this->town_size_max = 0.0;
    this->town_size_shape = 0.0;
    this->number_of_individuals = 0;
    this->expired = true;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  population::~population()
  {
    // delete all towns
    std::for_each( this->town_list.begin(), this->town_list.end(), utilities::safe_delete_type() );
    this->town_list.clear();

    // delete all trends
    utilities::safe_delete( this->mean_income );
    utilities::safe_delete( this->sd_income );
    utilities::safe_delete( this->mean_disease );
    utilities::safe_delete( this->sd_disease );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_sample_mode( const bool sample_mode )
  {
    this->sample_mode = sample_mode;
    this->expire_summary();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::create()
  {
    utilities::output( "creating population" );

    // delete all towns and turn off sample mode (in case it is on)
    this->current_household_index = 0;
    this->current_individual_index = 0;
    std::for_each( this->town_list.begin(), this->town_list.end(), utilities::safe_delete_type() );
    this->town_list.clear();
    this->set_sample_mode( false );

    // create a distribution to determine town size
    this->town_size_distribution.set_pareto(
      this->town_size_min, this->town_size_shape, this->town_size_max );

    // create towns
    for( unsigned int i = 0; i < this->number_of_towns; i++ )
    {
      town *t = new town( this, i );
      t->set_number_of_tiles_x( this->number_of_tiles_x );
      t->set_number_of_tiles_y( this->number_of_tiles_y );
      t->set_mean_household_population( this->mean_household_population );
      
      // determine whether to add a river to this town
      bool has_river = 0 < this->river_width && utilities::random() < this->river_probability;
      t->set_has_river( has_river );
      t->set_number_of_disease_pockets( this->number_of_disease_pockets );

      int individuals = this->town_size_distribution.generate_value();
      if( utilities::verbose )
        utilities::output( "creating town with target size of %d individuals", individuals );

      // determine the base population density for this town
      double town_x_width = this->tile_width * this->number_of_tiles_x;
      double town_y_width = this->tile_width * this->number_of_tiles_y;

      double b00 = individuals / town_x_width / town_y_width;
      double b01 = b00 * this->population_density_slope[0] / town_x_width;
      double b10 = b00 * this->population_density_slope[1] / town_y_width;

      // now make sure this is the value at the centre of the town
      b00 -= b01 * town_x_width / 2 + b10 * town_y_width / 2;

      trend* population_density = t->get_population_density();
      population_density->set_b00( b00 );
      population_density->set_b01( b01 );
      population_density->set_b10( b10 );
      population_density->set_b02( 0.0 );
      population_density->set_b20( 0.0 );
      population_density->set_b11( 0.0 );

      t->create();
      this->town_list.push_back( t );
      this->number_of_individuals += t->get_number_of_individuals();
    }

    utilities::output( "finished creating population" );

    this->expire_summary();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::define()
  {
    utilities::output( "defining population" );

    // calculate the mean( log(individuals) ) from all towns
    double sum_log_individual_count = 0;
    std::vector< double > town_log_individual_count;
    for( auto it = this->town_list.cbegin(); it != this->town_list.cend(); ++it )
    {
      double log_individuals = log10( this->number_of_individuals );
      town_log_individual_count.push_back( log_individuals );
      sum_log_individual_count += log_individuals;
    }
    double mean_log_individual_count = sum_log_individual_count / this->number_of_towns;

    // now set the regression factor for all trends in each town
    auto log_it = town_log_individual_count.cbegin();
    for( auto town_it = this->town_list.cbegin(); town_it != this->town_list.cend(); ++town_it )
    {
      town *t = *town_it;
      double factor = safe_subtract( *log_it, mean_log_individual_count );

      // here we set the regression factor for all trends
      // Note: the regression factor shouldn't be confused with the coefficient's regression coefficient.
      // This parameter is set when defining the trend, not here.
      t->get_mean_income()->copy( this->mean_income );
      t->get_mean_income()->set_regression_factor( factor );
      t->get_sd_income()->copy( this->sd_income );
      t->get_sd_income()->set_regression_factor( factor );
      t->get_mean_disease()->copy( this->mean_disease );
      t->get_mean_disease()->set_regression_factor( factor );
      t->get_sd_disease()->copy( this->sd_disease );
      t->get_sd_disease()->set_regression_factor( factor );

      t->set_number_of_disease_pockets( this->number_of_disease_pockets );
      t->define();

      ++log_it;
    }

    utilities::output( "finished defining population, %d individuals generated", this->number_of_individuals );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool population::read( const std::string filename )
  {
    utilities::output( "reading population from %s", filename.c_str() );

    std::ifstream stream( filename, std::ifstream::in );

    Json::Value root;
    Json::Reader reader;
    bool success = reader.parse( stream, root, false );

    if( !success )
    {
      std::cout << "ERROR: failed to parse population file \"" << filename << "\"" << std::endl
                << reader.getFormattedErrorMessages();
    }
    else try
    {
      this->from_json( root );
    }
    catch( std::runtime_error &e )
    {
      std::cout << e.what() << std::endl;
      success = false;
    }

    stream.close();

    return success;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::write( const std::string filename, const bool flat_file ) const
  {
    utilities::output( "writing population to %s.%s", filename.c_str(), flat_file ? "*.csv" : "json" );

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

    utilities::output( "finished writing population" );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::write_summary( const std::string filename )
  {
    std::ofstream stream( filename + ".txt", std::ofstream::out );

    summary* sum = this->get_summary();

    stream << "total count: " << sum->get_count( summary::all, summary::diseased )
           << " diseased of " << sum->get_count( summary::all, summary::all ) << " total "
           << "(prevalence " << sum->get_count_fraction( summary::all ) << ")" << std::endl;

    stream << "adult count: " << sum->get_count( summary::adult, summary::diseased )
           << " diseased of " << sum->get_count( summary::adult, summary::all ) << " total "
           << "(prevalence " << sum->get_count_fraction( summary::adult ) << ")" << std::endl;

    stream << "child count: " << sum->get_count( summary::child, summary::diseased )
           << " diseased of " << sum->get_count( summary::child, summary::all ) << " total "
           << "(prevalence " << sum->get_count_fraction( summary::child ) << ")" << std::endl;

    stream << "male count: " << sum->get_count( summary::male, summary::diseased )
           << " diseased of " << sum->get_count( summary::male, summary::all ) << " total "
           << "(prevalence " << sum->get_count_fraction( summary::male ) << ")" << std::endl;

    stream << "female count: " << sum->get_count( summary::female, summary::diseased )
           << " diseased of " << sum->get_count( summary::female, summary::all ) << " total "
           << "(prevalence " << sum->get_count_fraction( summary::female ) << ")" << std::endl;

    stream << "adult male count: " << sum->get_count( summary::adult_male, summary::diseased )
           << " diseased of " << sum->get_count( summary::adult_male, summary::all ) << " total "
           << "(prevalence " << sum->get_count_fraction( summary::adult_male ) << ")" << std::endl;

    stream << "adult female count: " << sum->get_count( summary::adult_female, summary::diseased )
           << " diseased of " << sum->get_count( summary::adult_female, summary::all ) << " total "
           << "(prevalence " << sum->get_count_fraction( summary::adult_female ) << ")" << std::endl;

    stream << "child male count: " << sum->get_count( summary::child_male, summary::diseased )
           << " diseased of " << sum->get_count( summary::child_male, summary::all ) << " total "
           << "(prevalence " << sum->get_count_fraction( summary::child_male ) << ")" << std::endl;

    stream << "child female count: " << sum->get_count( summary::child_female, summary::diseased )
           << " diseased of " << sum->get_count( summary::child_female, summary::all ) << " total "
           << "(prevalence " << sum->get_count_fraction( summary::child_female ) << ")" << std::endl;

    stream.close();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::from_json( const Json::Value &json )
  {
    // check to make sure the version is compatible
    if( !json.isMember( "version" ) )
      throw std::runtime_error( "ERROR: Cannot determine population generator version" );
    std::string version = json["version"].asString();
    if( version != utilities::get_version() )
    {
      std::stringstream stream;
      stream << "ERROR: Cannot read population, incompatible version ("
             << version << " != " << utilities::get_version() << ")";
      throw std::runtime_error( stream.str() );
    }

    this->number_of_individuals = 0;
    this->seed = json["seed"].asString();
    this->number_of_towns = json["number_of_towns"].asUInt();
    this->town_size_min = json["town_size_min"].asDouble();
    this->town_size_max = json["town_size_max"].asDouble();
    this->town_size_shape = json["town_size_shape"].asDouble();
    this->number_of_tiles_x = json["number_of_tiles_x"].asUInt();
    this->number_of_tiles_y = json["number_of_tiles_y"].asUInt();
    this->tile_width = json["tile_width"].asDouble();
    this->target_prevalence = json["target_prevalence"].asDouble();
    this->population_density_slope[0] = json["population_density_slope"][0].asDouble();
    this->population_density_slope[1] = json["population_density_slope"][1].asDouble();
    for( unsigned int c = 0; c < population::NUMBER_OF_DISEASE_WEIGHTS; c++ )
      this->disease_weights[c] = json["disease_weights"][c].asDouble();

    this->mean_household_population = json["mean_household_population"].asDouble();
    this->mean_income->from_json( json["mean_income"] );
    this->sd_income->from_json( json["sd_income"] );
    this->mean_disease->from_json( json["mean_disease"] );
    this->sd_disease->from_json( json["sd_disease"] );

    if( utilities::verbose ) utilities::output( "reading %d towns", json["town_list"].size() );
    this->town_list.reserve( json["town_list"].size() );
    for( unsigned int c = 0; c < json["town_list"].size(); c++ )
    {
      town *t = new town( this, c );
      t->from_json( json["town_list"][c] );
      this->town_list.push_back( t );
      this->number_of_individuals += t->get_number_of_individuals();
    }

    this->expire_summary();

    utilities::output( "finished reading population, %d individuals loaded", this->number_of_individuals );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["version"] = utilities::get_version();
    json["seed"] = this->seed;
    json["number_of_towns"] = this->number_of_towns;
    json["town_size_min"] = this->town_size_min;
    json["town_size_max"] = this->town_size_max;
    json["town_size_shape"] = this->town_size_shape;
    json["number_of_tiles_x"] = this->number_of_tiles_x;
    json["number_of_tiles_y"] = this->number_of_tiles_y;
    json["tile_width"] = this->tile_width;
    json["target_prevalence"] = this->target_prevalence;
    json["population_density_slope"] = Json::Value( Json::arrayValue );
    json["population_density_slope"].resize( 2 );
    json["population_density_slope"][0] = this->population_density_slope[0];
    json["population_density_slope"][1] = this->population_density_slope[1];
    json["disease_weights"] = Json::Value( Json::arrayValue );
    json["disease_weights"].resize( population::NUMBER_OF_DISEASE_WEIGHTS );
    for( unsigned int c = 0; c < population::NUMBER_OF_DISEASE_WEIGHTS; c++ )
      json["disease_weights"][c] = this->disease_weights[c];
    json["mean_household_population"] = this->mean_household_population;
    this->mean_income->to_json( json["mean_income"] );
    this->sd_income->to_json( json["sd_income"] );
    this->mean_disease->to_json( json["mean_disease"] );
    this->sd_disease->to_json( json["sd_disease"] );
    json["town_list"] = Json::Value( Json::arrayValue );

    for( auto it = this->town_list.cbegin(); it != this->town_list.cend(); ++it )
    {
      town *t = *it;
      if( !this->sample_mode || t->is_selected() )
      {
        Json::Value child;
        t->to_json( child );
        json["town_list"].append( child );
      }
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::to_csv(
    std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    // put in the parameters
    std::stringstream stream;
    stream << "#" << std::endl
           << "# population parameters" << std::endl
           << "# ---------------------" << std::endl
           << "# version: " << utilities::get_version() << std::endl
           << "# seed: " << this->seed << std::endl
           << "# towns: " << this->number_of_towns << std::endl
           << "# town_size_min: " << this->town_size_min << std::endl
           << "# town_size_max: " << this->town_size_max << std::endl
           << "# town_size_shape: " << this->town_size_shape << std::endl
           << "# tile_width: " << this->tile_width << std::endl
           << "# target_prevalence: " << this->target_prevalence << std::endl
           << "# population_density_slope: " << this->population_density_slope[0] << ", "
                                             << this->population_density_slope[1] << std::endl
           << "#" << std::endl
           << "# dweight_population: " << this->disease_weights[0] << std::endl
           << "# dweight_income: " << this->disease_weights[1] << std::endl
           << "# dweight_risk: " << this->disease_weights[2] << std::endl
           << "# dweight_age: " << this->disease_weights[3] << std::endl
           << "# dweight_sex: " << this->disease_weights[4] << std::endl
           << "# dweight_pocket: " << this->disease_weights[5] << std::endl
           << "#" << std::endl
           << "# mean_household_pop: " << this->mean_household_population << std::endl
           << "# mean_income trend: " << this->mean_income->to_string() << std::endl
           << "# sd_income trend: " << this->sd_income->to_string() << std::endl
           << "# mean_disease trend: " << this->mean_disease->to_string() << std::endl
           << "# sd_disease trend: " << this->sd_disease->to_string() << std::endl
           << "#" << std::endl << std::endl;

    household_stream << stream.str();
    individual_stream << stream.str();

    // put in the csv headers
    household_stream << "town_index,household_index,x,y,r,a,individuals,income,disease_risk,disease"
                     << std::endl;
    individual_stream << "town_index,household_index,individual_index,sex,age,disease" << std::endl;

    for( auto it = this->town_list.cbegin(); it != this->town_list.cend(); ++it )
      ( *it )->to_csv( household_stream, individual_stream );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_seed( const std::string seed )
  {
    if( utilities::verbose ) utilities::output( "setting seed to %s", seed.c_str() );
    this->seed = seed;
    utilities::random_engine.seed( atoi( this->seed.c_str() ) );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_number_of_towns( const unsigned int number_of_towns )
  {
    if( utilities::verbose ) utilities::output( "setting number_of_towns to %d", number_of_towns );
    this->number_of_towns = number_of_towns;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_town_size_min( const double town_size_min )
  {
    if( utilities::verbose ) utilities::output( "setting town_size_min to %f", town_size_min );
    this->town_size_min = town_size_min;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_town_size_max( const double town_size_max )
  {
    if( utilities::verbose ) utilities::output( "setting town_size_max to %f", town_size_max );
    this->town_size_max = town_size_max;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_town_size_shape( const double town_size_shape )
  {
    if( utilities::verbose ) utilities::output( "setting town_size_shape to %f", town_size_shape );
    this->town_size_shape = town_size_shape;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_number_of_tiles_x( const unsigned int number_of_tiles_x )
  {
    if( utilities::verbose ) utilities::output( "setting number_of_tiles_x to %d", number_of_tiles_x );
    this->number_of_tiles_x = number_of_tiles_x;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_number_of_tiles_y( const unsigned int number_of_tiles_y )
  {
    if( utilities::verbose ) utilities::output( "setting number_of_tiles_y to %d", number_of_tiles_y );
    this->number_of_tiles_y = number_of_tiles_y;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_population_density_slope( double mx, double my )
  {
    if( -1 > mx )
    {
      utilities::output( "warning: tried to set popdens_mx to %f, forcing minimum value of -1", mx );
      mx = -1;
    }
    else if( 1 < mx )
    {
      utilities::output( "warning: tried to set popdens_mx to %f, forcing maximum value of 1", mx );
      mx = 1;
    }

    if( -1 > my )
    {
      utilities::output( "warning: tried to set popdens_my to %f, forcing minimum value of -1", my );
      my = -1;
    }
    else if( 1 < my )
    {
      utilities::output( "warning: tried to set popdens_my to %f, forcing maximum value of 1", my );
      my = 1;
    }

    if( utilities::verbose ) utilities::output( "setting population density slope to %f, %f", mx, my );
    this->population_density_slope[0] = mx;
    this->population_density_slope[1] = my;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_tile_width( const double tile_width )
  {
    if( utilities::verbose ) utilities::output( "setting tile_width to %f meters", tile_width );
    this->tile_width = tile_width;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_target_prevalence( const double target_prevalence )
  {
    if( utilities::verbose ) utilities::output( "setting target_prevalence to %f meters", target_prevalence );
    this->target_prevalence = target_prevalence;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_river_probability( const double probability )
  {
    if( utilities::verbose ) utilities::output( "setting probability of a river to %f", probability );
    this->river_probability = probability;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_river_width( const double width )
  {
    if( utilities::verbose ) utilities::output( "setting width of a river to %f", width );
    this->river_width = width;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_number_of_disease_pockets( const unsigned int count )
  {
    if( utilities::verbose ) utilities::output( "setting number of disease pockets to %d", count );
    this->number_of_disease_pockets = count;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_pocket_kernel_type( const std::string type )
  {
    if( utilities::verbose ) utilities::output( "setting pocket_kernel_type to \"%s\"", type.c_str() );
    if( "exponential" != type &&
        "inverse_square" != type &&
        "gaussian" != type )
    {
      utilities::output( "invalid kernel type \"%s\", using \"exponential\" instead", type.c_str() );
      this->pocket_kernel_type = "exponential";
    }
    else this->pocket_kernel_type = type;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_pocket_scaling( const double scale )
  {
    if( utilities::verbose ) utilities::output( "setting pocket_scaling to %f", scale );
    this->pocket_scaling = scale;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double population::get_disease_weight_by_index( unsigned int index )
  {
    if( NUMBER_OF_DISEASE_WEIGHTS <= index )
    {
      std::stringstream stream;
      stream << "Tried to reference disease weight index " << index << " which is out of bounds";
      throw std::runtime_error( stream.str() );
    }

    return this->disease_weights[index];
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_mean_household_population( const double mean_household_population )
  {
    if( utilities::verbose )
      utilities::output( "setting mean_household_population to %f", mean_household_population );
    this->mean_household_population = mean_household_population;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_income( trend *mean, trend *sd )
  {
    if( utilities::verbose )
    {
      utilities::output( "setting income trend mean to %s", mean->to_string().c_str() );
      utilities::output( "setting income trend sd to %s", sd->to_string().c_str() );
      utilities::output( "setting income trend mean to %s", mean->to_string().c_str() );
      utilities::output( "setting income trend sd to %s", sd->to_string().c_str() );
    }
    this->mean_income->copy( mean );
    this->sd_income->copy( sd );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_disease( trend *mean, trend *sd )
  {
    if( utilities::verbose )
    {
      utilities::output( "setting disease trend mean to %s", mean->to_string().c_str() );
      utilities::output( "setting disease trend sd to %s", sd->to_string().c_str() );
      utilities::output( "setting disease trend mean to %s", mean->to_string().c_str() );
      utilities::output( "setting disease trend sd to %s", sd->to_string().c_str() );
    }
    this->mean_disease->copy( mean );
    this->sd_disease->copy( sd );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::assert_summary()
  {
    if( this->expired ) this->rebuild_summary();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::rebuild_summary()
  {
    this->sum.reset();
    for( auto it = this->town_list.begin(); it != this->town_list.end(); ++it )
    {
      (*it)->rebuild_summary();
      this->sum.add( *it );
    }
    this->expired = false;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::copy( const population* object )
  {
    // copy population parameters
    this->sample_mode = object->sample_mode;
    this->seed = object->seed;
    this->number_of_tiles_x = object->number_of_tiles_x;
    this->number_of_tiles_y = object->number_of_tiles_y;
    this->tile_width = object->tile_width;
    this->target_prevalence = object->target_prevalence;
    this->population_density_slope[0] = object->population_density_slope[0];
    this->population_density_slope[1] = object->population_density_slope[1];
    this->river_probability = object->river_probability;
    this->river_width = object->river_width;
    this->number_of_disease_pockets = object->number_of_disease_pockets;
    for( unsigned int c = 0; c < population::NUMBER_OF_DISEASE_WEIGHTS; c++ )
      this->disease_weights[c] = object->disease_weights[c];
    this->mean_household_population = object->mean_household_population;
    this->mean_income->copy( object->mean_income );
    this->sd_income->copy( object->sd_income );
    this->mean_disease->copy( object->mean_disease );
    this->sd_disease->copy( object->sd_disease );
    this->pocket_kernel_type = object->pocket_kernel_type;
    this->pocket_scaling = object->pocket_scaling;
    this->town_size_min =  object->town_size_min;
    this->town_size_max =  object->town_size_max;
    this->town_size_shape =  object->town_size_shape;

    // delete all towns
    std::for_each( this->town_list.begin(), this->town_list.end(), utilities::safe_delete_type() );
    this->town_list.clear();

    unsigned int index = 0;
    for( auto it = object->town_list.cbegin(); it != object->town_list.cend(); ++it )
    {
      if( !this->sample_mode || (*it)->is_selected() )
      {
        town *t = new town( this, index );
        t->copy( *it );
        this->town_list.push_back( t );
      }
    }
    this->number_of_towns = index + 1;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::select()
  {
    this->expire_summary();
    this->selected = true;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::unselect()
  {
    this->expire_summary();
    this->selected = false;
    for( auto town_it = this->get_town_list_cbegin(); town_it != this->get_town_list_cend(); ++town_it )
      (*town_it)->unselect();
  }
}
