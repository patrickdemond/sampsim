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
#include "town.h"
#include "tile.h"
#include "utilities.h"

#include <json/value.h>
#include <stdexcept>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  household::household( building *parent )
  {
    this->parent = parent;
    this->selected = false;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  household::~household()
  {
    // delete all individuals
    std::for_each( this->individual_list.begin(), this->individual_list.end(), utilities::safe_delete_type() );
    this->individual_list.empty();

    // we're holding a light reference to the parent, don't delete it
    this->parent = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  tile* household::get_tile() const
  {
    return NULL == this->parent ? NULL : this->parent->get_tile();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  town* household::get_town() const
  {
    return NULL == this->parent ? NULL : this->parent->get_town();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  population* household::get_population() const
  {
    return NULL == this->parent ? NULL : this->parent->get_population();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::create()
  {
    // make sure the household has a parent
    if( NULL == this->parent ) throw std::runtime_error( "Tried to create an orphaned household" );

    // We'll use 1 + distribution so that there are no empty households
    int size = this->get_town()->get_population_distribution()->generate_value() + 1;

    // create the first individual an adult of random sex
    bool male = 0 == utilities::random( 0, 1 );
    individual *i = new individual( this );
    i->set_age( ADULT );
    i->set_sex( male ? MALE : FEMALE );
    this->individual_list.push_back( i );

    // now make the rest of the members of this household
    for( int c = 1; c < size; c++ )
    {
      individual *i = new individual( this );

      if( 1 == c )
      { // the first must be an adult of opposite sex of the first
        i->set_sex( !male ? MALE : FEMALE );
        i->set_age( ADULT );
      }
      else
      { // the rest are children of random sex
        i->set_sex( 0 == utilities::random( 0, 1 ) ? MALE : FEMALE );
        i->set_age( CHILD );
      }

      this->individual_list.push_back( i );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::define()
  {
    // income and disease are Normal deviates from the tile average
    tile *tile = this->get_tile();
    this->income = tile->get_income_distribution()->generate_value();
    this->debug( "setting income to %f", this->income );
    this->disease_risk = tile->get_disease_risk_distribution()->generate_value();
    this->debug( "setting disease risk to %f", this->disease_risk );
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
  void household::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );

    json["income"] = this->income;
    json["disease_risk"] = this->disease_risk;
    json["individual_list"] = Json::Value( Json::arrayValue );

    bool sample_mode = this->get_population()->get_sample_mode();
    for( auto it = this->individual_list.cbegin(); it != this->individual_list.cend(); ++it )
    {
      individual *i = *it;
      if( !sample_mode || i->is_selected() )
      {
        Json::Value child;
        i->to_json( child );
        json["individual_list"].append( child );
      }
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::to_csv( std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    population *population = this->get_population();
    unsigned int town_index = this->get_town()->get_index();

    // write the household index and position to the household stream
    household_stream << town_index << "," << utilities::household_index << ",";
    this->get_building()->get_position().to_csv( household_stream, individual_stream );
    household_stream << "," << this->count_individuals().second
                     << "," << this->income << ","
                     << this->disease_risk;

    // write all individuals in this household to the individual stream
    bool disease = false;
    bool sample_mode = this->get_population()->get_sample_mode();
    for( auto it = this->individual_list.begin(); it != this->individual_list.end(); ++it )
    {
      individual *i = *it;
      if( !sample_mode || i->is_selected() )
      {
        individual_stream << town_index << "," << utilities::household_index << ",";
        if( !disease ) disease = i->is_disease();
        i->to_csv( household_stream, individual_stream );
        individual_stream << std::endl;
      }
    }

    // finish writing the household stream
    household_stream << "," << ( disease ? "1" : "0" ) << std::endl;

    utilities::household_index++;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::pair<unsigned int, unsigned int> household::count_individuals() const
  {
    bool sample_mode = this->get_population()->get_sample_mode();
    std::pair<unsigned int, unsigned int> count( 0, 0 );
    for( auto it = this->individual_list.cbegin(); it != this->individual_list.cend(); ++it )
      if( !sample_mode || (*it)->is_selected() )
      {
        if( (*it)->is_disease() ) count.first++; // count of all individuals with disease
        count.second++; // count of all individuals
      }

    return count;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::select()
  {
    this->selected = true;
    this->get_building()->select();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::unselect()
  {
    this->selected = false;
    for( auto it = this->individual_list.begin(); it != this->individual_list.end(); ++it )
      (*it)->unselect();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::copy( const household* object )
  {
    this->income = object->income;
    this->disease_risk = object->disease_risk;
    this->selected = object->selected;

    // delete all individuals
    std::for_each( this->individual_list.begin(), this->individual_list.end(), utilities::safe_delete_type() );
    this->individual_list.empty();

    bool sample_mode = this->get_population()->get_sample_mode();
    for( auto it = object->individual_list.cbegin(); it != object->individual_list.cend(); ++it )
    {
      if( !sample_mode || (*it)->is_selected() )
      {
        individual *i = new individual( this );
        i->copy( *it );
        this->individual_list.push_back( i );
      }
    }
  }
}
