/*=========================================================================

  Program:  sampsim
  Module:   household.cxx
  Language: C++

=========================================================================*/

#include "household.h"

#include "building.h"
#include "household.h"
#include "individual.h"
#include "population.h"
#include "tile.h"
#include "utilities.h"

#include <ostream>
#include <json/value.h>
#include <stdexcept>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  household::household( building *parent )
  {
    this->parent = parent;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  household::~household()
  {
    // delete all individuals
    std::for_each( this->individual_list.begin(), this->individual_list.end(), utilities::safe_delete_type() );

    // we're holding a light reference to the parent, don't delete it
    this->parent = 0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  tile* household::get_tile() const
  {
    return NULL == this->parent ? NULL : this->parent->get_tile();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  population* household::get_population() const
  {
    return NULL == this->parent ? NULL : this->parent->get_population();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::generate()
  {
    // make sure the household has a parent
    if( NULL == this->parent ) throw std::runtime_error( "Tried to generate an orphaned household" );

    std::normal_distribution<double> normal;
    std::poisson_distribution<int> poisson;

    tile *tile = this->get_tile();
    population *population = this->get_population();

    // income and disease are Normal deviates from the tile average
    this->income = tile->get_income_distribution()->generate_value();
    this->disease_risk = tile->get_disease_risk_distribution()->generate_value();

    // We'll use 1 + distribution so that there are no empty households
    int size = population->get_population_distribution()->generate_value() + 1;
    
    // make the first individual an adult of random sex
    bool male = 0 == utilities::random( 0, 1 );
    individual *i = new individual( this );
    i->set_male( male );
    i->set_adult( true );
    this->individual_list.push_back( i );

    // now make the rest of the members of this household
    for( int c = 1; c < size; c++ )
    {
      individual *i = new individual( this );

      if( 1 == c )
      { // the first must be an adult of opposite sex of the first
        i->set_male( !male );
        i->set_adult( true );
      }
      else
      { // the rest are children of random sex
        i->set_male( 0 == utilities::random( 0, 1 ) );
        i->set_adult( false );
      }

      this->individual_list.push_back( i );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::from_json( const Json::Value &json )
  {
    this->income = json["income"].asDouble();
    this->disease_risk = json["disease_risk"].asDouble();

    this->individual_list.reserve( json["individual_list"].size() );
    for( unsigned int c = 0; c < json["individual_list"].size(); c++ )
    {
      individual *i = new individual( this );
      i->from_json( json["individual_list"][c] );
      this->individual_list.push_back( i );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::to_json( Json::Value &json, bool single_selection, age_type age, sex_type sex ) const
  {
    json = Json::Value( Json::objectValue );
    json["income"] = this->income;
    json["disease_risk"] = this->disease_risk;
    json["individual_list"] = Json::Value( Json::arrayValue );
    json["individual_list"].resize( this->individual_list.size() );

    int index = 0;
    for( auto it = this->individual_list.cbegin(); it != this->individual_list.cend(); ++it, ++index )
      ( *it )->to_json( json["individual_list"][index] );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::to_csv( std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    population *population = this->get_population();

    // write the household index and position to the household stream
    household_stream << utilities::household_index << ",";
    this->get_building()->get_position().to_csv( household_stream, population->get_centroid() );
    household_stream << "," << this->income << "," << this->disease_risk << std::endl;

    // write all individuals in this household to the individual stream
    for( auto it = this->individual_list.begin(); it != this->individual_list.end(); ++it )
    {
      individual_stream << utilities::household_index << ",";
      ( *it )->to_csv( individual_stream );
      individual_stream << std::endl;
    }

    utilities::household_index++;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  int household::count_population() const
  {
    return this->individual_list.size();
  }
}
