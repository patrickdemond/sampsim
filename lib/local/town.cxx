/*=========================================================================

  Program:  sampsim
  Module:   town.cxx
  Language: C++

=========================================================================*/

#include "town.h"

#include "building.h"
#include "household.h"
#include "individual.h"
#include "population.h"
#include "summary.h"
#include "tile.h"
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
  town::town( population *parent, const unsigned int index )
  {
    this->parent = parent;
    this->index = index;
    this->number_of_tiles_x = 0;
    this->number_of_tiles_y = 0;
    this->has_river = false;
    this->river_banks[0] = line();
    this->river_banks[1] = line();
    this->mean_income = new trend;
    this->sd_income = new trend;
    this->mean_disease = new trend;
    this->sd_disease = new trend;
    this->mean_exposure = new trend;
    this->sd_exposure = new trend;
    this->population_density = new trend;
    this->number_of_individuals = 0;
    this->number_of_selected_individuals = 0;
    for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ )
      this->number_of_selected_diseased_individuals.push_back( 0 );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  town::~town()
  {
    // delete all tiles
    for( auto it = this->tile_list.begin(); it != this->tile_list.end(); ++it )
      utilities::safe_delete( it->second );
    this->tile_list.clear();

    // delete all trends
    delete this->mean_income;
    delete this->sd_income;
    delete this->mean_disease;
    delete this->sd_disease;
    delete this->mean_exposure;
    delete this->sd_exposure;
    delete this->population_density;

    // we're holding a light reference to the parent, don't delete it
    this->parent = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::create()
  {
    std::pair< unsigned int, unsigned int > index;

    std::stringstream stream;
    stream << "creating town #" << ( this->index + 1 );
    utilities::output( stream.str() );

    population *pop = this->get_population();

    // define the river's parameters based on whether it has a river or not
    if( this->has_river )
    {
      coordinate intercept = coordinate( utilities::random() * this->get_x_width(),
                                         utilities::random() * this->get_y_width() );
      intercept.set_centroid( this->get_centroid() );
      double angle = safe_subtract( utilities::random(), 0.5 ) * M_PI; // from -pi/2 to pi/2
      double sin_angle = sin( angle );
      double cos_angle = cos( angle );
      double width = this->get_population()->get_river_width();

      this->river_banks[0].angle = angle;
      this->river_banks[0].intercept = coordinate( intercept.x + sin_angle * width / 2,
                                                   safe_subtract( intercept.y, cos_angle * width / 2 ) );
      this->river_banks[1].angle = angle;
      this->river_banks[1].intercept = coordinate( safe_subtract( intercept.x, sin_angle * width / 2 ),
                                                   intercept.y + cos_angle * width / 2 );
    }
    else
    {
      this->river_banks[0] = line();
      this->river_banks[1] = line();
    }

    // delete all tiles
    for( auto it = this->tile_list.begin(); it != this->tile_list.end(); ++it )
      utilities::safe_delete( it->second );
    this->tile_list.clear();

    // create the needed distributions
    this->population_distribution.set_poisson( this->mean_household_population - 1 );

    // create tiles
    for( unsigned int y = 0; y < this->number_of_tiles_y; y++ )
    {
      for( unsigned int x = 0; x < this->number_of_tiles_x; x++ )
      {
        index = std::pair< unsigned int, unsigned int >( x, y );
        tile *t = new tile( this, index );
        t->set_population_density( this->population_density->get_value( t->get_centroid() ) );

        t->create();
        this->tile_list[index] = t;
        this->number_of_individuals += t->get_number_of_individuals();
      }
    }

    stream.str( "" );
    stream << "finished creating town #" << ( this->index + 1 );
    utilities::output( stream.str() );

    this->get_population()->expire_summary();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::define()
  {
    std::pair< unsigned int, unsigned int > index;

    std::stringstream stream;
    stream << "defining town #" << ( this->index + 1 );
    utilities::output( stream.str() );

    population *pop = this->get_population();

    // define all tiles
    for( auto it = this->tile_list.begin(); it != this->tile_list.end(); ++it )
    {
      // set the income and disease risk then define the tile
      tile *t = it->second;
      coordinate centroid = t->get_centroid();
      t->set_mean_income( this->mean_income->get_value( centroid ) );
      t->set_sd_income( this->sd_income->get_value( centroid ) );
      t->set_mean_disease( this->mean_disease->get_value( centroid ) );
      t->set_sd_disease( this->sd_disease->get_value( centroid ) );
      t->set_mean_exposure( this->mean_exposure->get_value( centroid ) );
      t->set_sd_exposure( this->sd_exposure->get_value( centroid ) );
      t->define();
    }

    // now that the town has been created and all tiles defined we can determine disease status
    // we are going to do this in a standard generalized-linear-model way, by constructing a linear
    // function of the various contributing factors

    // create a matrix of all individuals (rows) and their various disease predictor factors
    const unsigned int number_of_disease_weights = pop->get_number_of_disease_weights();
    double value[number_of_disease_weights], total[number_of_disease_weights];
    for( unsigned int c = 0; c < number_of_disease_weights; c++ ) total[c] = 0;

    std::vector< double > matrix[number_of_disease_weights];
    for( unsigned int c = 0; c < number_of_disease_weights; c++ )
      matrix[c].resize( this->number_of_individuals );
    individual_list_type individual_list;
    individual_list.resize( this->number_of_individuals );

    unsigned int individual_index = 0;

    for( auto tile_it = this->tile_list.begin();
         tile_it != this->tile_list.end();
         ++tile_it )
    {
      for( auto building_it = tile_it->second->get_building_list_cbegin();
           building_it != tile_it->second->get_building_list_cend();
           ++building_it )
      {
        value[5] = ( *building_it )->get_pocket_factor();
        for( auto household_it = ( *building_it )->get_household_list_cbegin();
             household_it != ( *building_it )->get_household_list_cend();
             ++household_it )
        {
          double exposure_risk = ( *household_it )->get_exposure_risk();
          value[0] = ( *household_it )->get_number_of_individuals();
          value[1] = ( *household_it )->get_income();
          value[2] = ( *household_it )->get_disease_risk();

          for( auto individual_it = ( *household_it )->get_individual_list_cbegin();
               individual_it != ( *household_it )->get_individual_list_cend();
               ++individual_it )
          {
            ( *individual_it )->set_exposure( utilities::random() < exposure_risk );
            value[3] = ADULT == ( *individual_it )->get_age() ? 1 : 0;
            value[4] = MALE == ( *individual_it )->get_sex() ? 1 : 0;

            for( unsigned int c = 0; c < number_of_disease_weights; c++ )
            {
              total[c] += value[c];
              matrix[c][individual_index] = value[c];
            }

            // keep a reference to the individual for writing to later
            individual_list[individual_index] = ( *individual_it );

            individual_index++;
          }
        }
      }
    }

    // subtract the mean of a column from each of its values and divide by the column's sd
    double mean[number_of_disease_weights], sd[number_of_disease_weights];

    for( unsigned int c = 0; c < number_of_disease_weights; c++ )
    {
      mean[c] = total[c] / this->number_of_individuals;
      sd[c] = 0;
      for( unsigned int i = 0; i < this->number_of_individuals; i++ )
      {
        double diff = safe_subtract( matrix[c][i], mean[c] );
        sd[c] += diff*diff;
      }
      sd[c] = sqrt( sd[c] / ( this->number_of_individuals - 1 ) );

      for( unsigned int i = 0; i < this->number_of_individuals; i++ )
      {
        matrix[c][i] = 0 == sd[c]
                     ? 0.0 // avoid division by 0
                     : ( 1 == c ? -1 : 1 ) * // income should have an inverse relationship to disease
                       safe_subtract( matrix[c][i], mean[c] ) / sd[c]; // normalize values
      }
    }

    // Determine the target_prevalence factor
    // This is an ad-hoc transformation described in the disease status documentation
    double base_probability, probability;
    double adjusted_prevalence_factor = ( sin( (M_PI/2)*(2*pop->get_target_prevalence() - 1) ) + 1 )/2;
    double target_prevalence_factor = log( 1/adjusted_prevalence_factor - 1 );

    // factor in weights, compute disease probability then set disease status for all individuals
    for( unsigned int i = 0; i < this->number_of_individuals; i++ )
    {
      double eta = 0;
      for( unsigned int c = 0; c < number_of_disease_weights; c++ )
        eta += matrix[c][i] * pop->get_disease_weight_by_index( c );

      eta -= target_prevalence_factor;
      base_probability = 1 / ( 1 + exp( -eta ) );
      for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ )
      {
        // probability is equal to the base probability times the relative risk (max of 0.9)
        probability = base_probability * ( individual_list[i]->is_exposed() ? utilities::rr[rr] : 1.0 );
        if( 0.9 < probability ) probability = 0.9;
        individual_list[i]->set_disease( rr, utilities::random() < probability );
      }
    }

    stream.str( "" );
    stream << "finished defining town #" << ( this->index + 1 ) << ", "
           << this->number_of_individuals << " individuals generated";
    utilities::output( stream.str() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::from_json( const Json::Value &json )
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
    this->index = json["index"].asUInt();
    this->number_of_tiles_x = json["number_of_tiles_x"].asUInt();
    this->number_of_tiles_y = json["number_of_tiles_y"].asUInt();
    this->has_river = json["has_river"].asBool();
    this->river_banks[0].from_json( json["river_bank_0"] );
    this->river_banks[1].from_json( json["river_bank_1"] );

    this->mean_household_population = json["mean_household_population"].asDouble();
    this->mean_income->from_json( json["mean_income"] );
    this->sd_income->from_json( json["sd_income"] );
    this->mean_disease->from_json( json["mean_disease"] );
    this->sd_disease->from_json( json["sd_disease"] );
    this->mean_exposure->from_json( json["mean_exposure"] );
    this->sd_exposure->from_json( json["sd_exposure"] );
    this->population_density->from_json( json["population_density"] );

    std::pair< unsigned int, unsigned int > index;
    for( unsigned int c = 0; c < json["tile_list"].size(); c++ )
    {
      Json::Value tile_json = json["tile_list"][c];
      index = std::pair< unsigned int, unsigned int >(
        tile_json["x_index"].asUInt(), tile_json["y_index"].asUInt() );
      tile *t = new tile( this, index );
      t->from_json( tile_json );
      this->tile_list[index] = t;
      this->number_of_individuals += t->get_number_of_individuals();
    }

    std::stringstream stream( "" );
    stream << "finished reading town #" << ( this->index + 1 ) << ", "
           << this->number_of_individuals << " individuals loading";
    utilities::output( stream.str() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["version"] = utilities::get_version();
    json["index"] = this->index;
    json["number_of_tiles_x"] = this->number_of_tiles_x;
    json["number_of_tiles_y"] = this->number_of_tiles_y;
    json["has_river"] = this->has_river;
    this->river_banks[0].to_json( json["river_bank_0"] );
    this->river_banks[1].to_json( json["river_bank_1"] );
    json["tile_list"] = Json::Value( Json::arrayValue );
    json["tile_list"].resize( this->tile_list.size() );
    this->mean_income->to_json( json["mean_income"] );
    this->sd_income->to_json( json["sd_income"] );
    this->mean_disease->to_json( json["mean_disease"] );
    this->sd_disease->to_json( json["sd_disease"] );
    this->mean_exposure->to_json( json["mean_exposure"] );
    this->sd_exposure->to_json( json["sd_exposure"] );
    this->population_density->to_json( json["population_density"] );

    unsigned int index = 0;
    for( auto it = this->tile_list.cbegin(); it != this->tile_list.cend(); ++it, ++index )
      it->second->to_json( json["tile_list"][index] );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::to_csv(
    std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    // put in the parameters
    std::stringstream stream;
    stream << "#" << std::endl
           << "# town parameters (index " << this->index << ")" << std::endl
           << "# --------------------------" << std::endl
           << "# number_of_tiles_x: " << this->number_of_tiles_x << std::endl
           << "# number_of_tiles_y: " << this->number_of_tiles_y << std::endl
           << "# has_river: " << this->has_river << std::endl
           << "# river_bank_0 x_intercept: " << this->river_banks[0].intercept.x << std::endl
           << "# river_bank_0 y_intercept: " << this->river_banks[0].intercept.y << std::endl
           << "# river_bank_0 angle: " << this->river_banks[0].angle << std::endl
           << "# river_bank_1 x_intercept: " << this->river_banks[1].intercept.x << std::endl
           << "# river_bank_1 y_intercept: " << this->river_banks[1].intercept.y << std::endl
           << "# river_bank_1 angle: " << this->river_banks[1].angle << std::endl

           << "# mean_income trend: " << this->mean_income->to_string() << std::endl
           << "# sd_income trend: " << this->sd_income->to_string() << std::endl
           << "# mean_disease trend: " << this->mean_disease->to_string() << std::endl
           << "# sd_disease trend: " << this->sd_disease->to_string() << std::endl
           << "# mean_exposure trend: " << this->mean_exposure->to_string() << std::endl
           << "# sd_exposure trend: " << this->sd_exposure->to_string() << std::endl
           << "# population density trend: " << this->population_density->to_string() << std::endl
           << "#" << std::endl;

    household_stream << stream.str();
    individual_stream << stream.str();

    for( auto it = this->tile_list.cbegin(); it != this->tile_list.cend(); ++it )
      it->second->to_csv( household_stream, individual_stream );

    household_stream << std::endl;
    individual_stream << std::endl;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::set_number_of_tiles_x( const unsigned int number_of_tiles_x )
  {
    if( utilities::verbose ) utilities::output( "setting number_of_tiles_x to %d", number_of_tiles_x );
    this->number_of_tiles_x = number_of_tiles_x;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::set_number_of_tiles_y( const unsigned int number_of_tiles_y )
  {
    if( utilities::verbose ) utilities::output( "setting number_of_tiles_y to %d", number_of_tiles_y );
    this->number_of_tiles_y = number_of_tiles_y;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::set_has_river( const bool has_river )
  {
    if( utilities::verbose ) utilities::output( "setting has_river to %s", has_river ? "true" : "false" );
    this->has_river = has_river;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::set_mean_household_population( const double mean_household_population )
  {
    if( utilities::verbose )
      utilities::output( "setting mean_household_population to %f", mean_household_population );
    this->mean_household_population = mean_household_population;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::set_income( trend *mean, trend *sd )
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
  void town::set_disease( trend *mean, trend *sd )
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
  void town::set_exposure( trend *mean, trend *sd )
  {
    if( utilities::verbose )
    {
      utilities::output( "setting exposure trend mean to %s", mean->to_string().c_str() );
      utilities::output( "setting exposure trend sd to %s", sd->to_string().c_str() );
      utilities::output( "setting exposure trend mean to %s", mean->to_string().c_str() );
      utilities::output( "setting exposure trend sd to %s", sd->to_string().c_str() );
    }
    this->mean_exposure->copy( mean );
    this->sd_exposure->copy( sd );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::set_number_of_disease_pockets( const unsigned int count )
  {
    if( utilities::verbose ) utilities::output( "setting number of disease pockets to %d", count );
    this->disease_pocket_list.clear();
    coordinate c = this->get_centroid();
    for( unsigned int i = 0; i < count; i++ )
      this->disease_pocket_list.push_back(
        coordinate( 2 * c.x * utilities::random(), 2 * c.y * utilities::random() ) );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::assert_summary()
  {
    this->get_population()->assert_summary();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::rebuild_summary()
  {
    this->sum.reset();
    for( auto it = this->tile_list.begin(); it != this->tile_list.end(); ++it )
    {
      it->second->rebuild_summary();
      this->sum.add( it->second );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  coordinate town::get_centroid() const
  {
    coordinate centroid( this->number_of_tiles_x, this->number_of_tiles_y );
    centroid *= this->get_population()->get_tile_width() / 2;
    return centroid;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double town::get_x_width() const
  {
    return this->number_of_tiles_x * this->get_population()->get_tile_width();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double town::get_y_width() const
  {
    return this->number_of_tiles_y * this->get_population()->get_tile_width();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double town::get_area() const
  {
    return this->get_x_width() * this->get_y_width();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::select()
  {
    this->selected = true;
    this->parent->select();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::unselect()
  {
    this->get_population()->expire_summary();
    this->selected = false;

    // unselect all buildings
    for( auto tile_it = this->tile_list.begin(); tile_it != this->tile_list.end(); ++tile_it )
      tile_it->second->unselect();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::copy( const town* object )
  {
    this->selected = object->selected;
    this->index = object->index;
    this->number_of_tiles_x = object->number_of_tiles_x;
    this->number_of_tiles_y = object->number_of_tiles_y;
    this->mean_household_population = object->mean_household_population;
    this->has_river = object->has_river;
    this->river_banks[0].copy( &( object->river_banks[0] ) );
    this->river_banks[1].copy( &( object->river_banks[1] ) );
    this->mean_income->copy( object->mean_income );
    this->sd_income->copy( object->sd_income );
    this->mean_disease->copy( object->mean_disease );
    this->sd_disease->copy( object->sd_disease );
    this->mean_exposure->copy( object->mean_exposure );
    this->sd_exposure->copy( object->sd_exposure );
    this->population_density->copy( object->population_density );
    this->number_of_selected_individuals = object->number_of_selected_individuals;
    for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ )
      this->number_of_selected_diseased_individuals[rr] = object->number_of_selected_diseased_individuals[rr];

    // delete all tiles
    for( auto it = this->tile_list.begin(); it != this->tile_list.end(); ++it )
      utilities::safe_delete( it->second );
    this->tile_list.clear();

    // create and copy tiles
    for( auto it = object->tile_list.cbegin(); it != object->tile_list.cend(); ++it )
    {
      tile *t = new tile( this, it->first );
      t->copy( it->second );
      this->tile_list[it->first] = t;
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::select_all()
  {
    for( auto it = this->tile_list.cbegin(); it != this->tile_list.cend(); ++it ) it->second->select_all();
  }
}
