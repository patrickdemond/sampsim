/*=========================================================================

  Program:  sampsim
  Module:   population.cxx
  Language: C++

=========================================================================*/

#include "population.h"

#include "building.h"
#include "household.h"
#include "individual.h"
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
    this->seed = "";
    this->number_of_towns = 1;
    this->number_of_tiles_x = 0;
    this->number_of_tiles_y = 0;
    this->number_of_disease_pockets = 0;
    this->tile_width = 0;
    for( unsigned int c = 0; c < population::NUMBER_OF_DISEASE_WEIGHTS; c++ )
      this->disease_weights[c] = 1.0;
    this->mean_household_population = 0;
    this->mean_income = new trend;
    this->sd_income = new trend;
    this->mean_disease = new trend;
    this->sd_disease = new trend;
    this->population_density = new trend;
    this->pocket_kernel_type = "exponential";
    this->pocket_scaling = 1.0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  population::~population()
  {
    // delete all towns
    std::for_each( this->town_list.begin(), this->town_list.end(), utilities::safe_delete_type() );
    this->town_list.clear();

    // delete all trends
    delete this->mean_income;
    delete this->sd_income;
    delete this->mean_disease;
    delete this->sd_disease;
    delete this->population_density;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::generate()
  {
    std::pair< unsigned int, unsigned int > index;

    utilities::output( "generating population" );

    // delete all towns and turn off sample mode (in case it is on)
    std::for_each( this->town_list.begin(), this->town_list.end(), utilities::safe_delete_type() );
    this->town_list.clear();
    this->set_sample_mode( false );

    // create towns
    for( unsigned int i = 0; i < this->number_of_towns; i++ )
    {
      town *t = new town( this, i );
      t->set_number_of_tiles_x( this->number_of_tiles_x );
      t->set_number_of_tiles_y( this->number_of_tiles_y );
      t->set_number_of_disease_pockets( this->number_of_disease_pockets );
      t->set_mean_household_population( this->mean_household_population );
      t->generate();

      this->town_list.push_back( t );
    }

    utilities::output( "finished generating population" );
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

    this->seed = json["seed"].asString();
    this->number_of_tiles_x = json["number_of_tiles_x"].asUInt();
    this->number_of_tiles_y = json["number_of_tiles_y"].asUInt();
    this->tile_width = json["tile_width"].asDouble();
    for( unsigned int c = 0; c < population::NUMBER_OF_DISEASE_WEIGHTS; c++ )
      this->disease_weights[c] = json["disease_weights"][c].asDouble();

    this->mean_household_population = json["mean_household_population"].asDouble();
    this->mean_income->from_json( json["mean_income"] );
    this->sd_income->from_json( json["sd_income"] );
    this->mean_disease->from_json( json["mean_disease"] );
    this->sd_disease->from_json( json["sd_disease"] );
    this->population_density->from_json( json["population_density"] );

    for( unsigned int c = 0; c < json["town_list"].size(); c++ )
    {
      Json::Value town_json = json["town_list"][c];
      town *t = new town( this, c );
      t->from_json( town_json );
      this->town_list[t->get_index()] = t;
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["version"] = utilities::get_version();
    json["seed"] = this->seed;
    json["number_of_tiles_x"] = this->number_of_tiles_x;
    json["number_of_tiles_y"] = this->number_of_tiles_y;
    json["tile_width"] = this->tile_width;
    json["disease_weights"] = Json::Value( Json::arrayValue );
    json["disease_weights"].resize( population::NUMBER_OF_DISEASE_WEIGHTS );
    for( unsigned int c = 0; c < population::NUMBER_OF_DISEASE_WEIGHTS; c++ )
      json["disease_weights"][c] = this->disease_weights[c];
    json["mean_household_population"] = this->mean_household_population;
    this->mean_income->to_json( json["mean_income"] );
    this->sd_income->to_json( json["sd_income"] );
    this->mean_disease->to_json( json["mean_disease"] );
    this->sd_disease->to_json( json["sd_disease"] );
    this->population_density->to_json( json["population_density"] );
    json["town_list"] = Json::Value( Json::arrayValue );

    unsigned int index = 0;
    for( auto it = this->town_list.cbegin(); it != this->town_list.cend(); ++it, ++index )
    {
      town *t = *it;
      if( !sample_mode || t->is_selected() )
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
    // need to reset the static household indexing variable
    utilities::household_index = 0;

    // put in the parameters
    std::stringstream stream;
    stream << "# population parameters" << std::endl;
    stream << "# -----------------------------------------------------------------------" << std::endl;
    stream << "# version: " << utilities::get_version() << std::endl;
    stream << "# seed: " << this->seed << std::endl;
    stream << "# number_of_tiles_x: " << this->number_of_tiles_x << std::endl;
    stream << "# number_of_tiles_y: " << this->number_of_tiles_y << std::endl;
    stream << "# tile_width: " << this->tile_width << std::endl;
    stream << "#" << std::endl;
    stream << "# dweight_population: " << this->disease_weights[0] << std::endl;
    stream << "# dweight_income: " << this->disease_weights[1] << std::endl;
    stream << "# dweight_risk: " << this->disease_weights[2] << std::endl;
    stream << "# dweight_age: " << this->disease_weights[3] << std::endl;
    stream << "# dweight_sex: " << this->disease_weights[4] << std::endl;
    stream << "# dweight_pocket: " << this->disease_weights[5] << std::endl;
    stream << "#" << std::endl;
    stream << "# mean_household_pop" << this->mean_household_population << std::endl;
    stream << "# mean_income trend: " << this->mean_income->to_string() << std::endl;
    stream << "# sd_income trend: " << this->sd_income->to_string() << std::endl;
    stream << "# mean_disease trend: " << this->mean_disease->to_string() << std::endl;
    stream << "# sd_disease trend: " << this->sd_disease->to_string() << std::endl;
    stream << "# popdens trend: " << this->population_density->to_string() << std::endl;
    stream << std::endl;
    
    household_stream << stream.str();
    individual_stream << stream.str();

    // put in the csv headers
    household_stream << "index,x,y,r,a,individuals,income,disease_risk" << std::endl;
    individual_stream << "household_index,sex,age,disease" << std::endl;

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
  void population::set_tile_width( const double tile_width )
  {
    if( utilities::verbose ) utilities::output( "setting tile_width to %f meters", tile_width );
    this->tile_width = tile_width;
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
  void population::set_income( const trend *mean, const trend *sd )
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
  void population::set_disease( const trend *mean, const trend *sd )
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
  void population::set_population_density( const trend *population_density )
  {
    if( utilities::verbose )
      utilities::output( "setting population density trend to %s", population_density->to_string().c_str() );
    this->population_density->copy( population_density );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  unsigned int population::count_individuals() const
  {
    unsigned int count = 0;
    for( auto it = this->town_list.cbegin(); it != this->town_list.cend(); ++it )
      count += ( *it )->count_individuals();

    return count;
  }
}
