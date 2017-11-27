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
#include "summary.h"
#include "town.h"
#include "trend.h"
#include "utilities.h"

#include <json/value.h>
#include <stdexcept>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  household::household( building *parent )
  {
    this->parent = parent;
    this->index = this->get_population()->get_next_household_index();
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
    this->individual_list.reserve( size );

    // create the first individual an adult of random sex
    bool male = 0 == utilities::random( 0, 1 );
    individual *i = new individual( this );
    i->create();
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

    this->get_population()->expire_summary();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::define()
  {
    for( auto it = this->individual_list.begin(); it != this->individual_list.end(); ++it ) (*it)->define();

    town *town = this->get_town();

    distribution income_distribution;
    income_distribution.set_lognormal(
      town->get_mean_income()->get_value( this->get_building()->get_position() ),
      town->get_sd_income()->get_value( this->get_building()->get_position() ) );
    this->income = income_distribution.generate_value();
    this->debug( "setting income to %f", this->income );

    distribution disease_risk_distribution;
    disease_risk_distribution.set_normal(
      town->get_mean_disease()->get_value( this->get_building()->get_position() ),
      town->get_sd_disease()->get_value( this->get_building()->get_position() ) );
    this->disease_risk = disease_risk_distribution.generate_value();
    this->debug( "setting disease risk to %f", this->disease_risk );

    distribution exposure_risk_distribution;
    exposure_risk_distribution.set_normal(
      town->get_mean_exposure()->get_value( this->get_building()->get_position() ),
      town->get_sd_exposure()->get_value( this->get_building()->get_position() ) );
    this->exposure_risk = exposure_risk_distribution.generate_value();
    this->debug( "setting exposure risk to %f", this->exposure_risk );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::from_json( const Json::Value &json )
  {
    this->index = json["index"].asUInt();
    this->income = json["income"].asDouble();
    this->disease_risk = json["disease_risk"].asDouble();
    this->exposure_risk = json["exposure_risk"].asDouble();
    this->get_population()->assert_household_index( this->index );

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

    json["index"] = this->index;
    json["income"] = this->income;
    json["disease_risk"] = this->disease_risk;
    json["exposure_risk"] = this->exposure_risk;
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
    household_stream << town_index << "," << this->index << ",";
    this->get_building()->get_position().to_csv( household_stream, individual_stream );
    household_stream << "," << this->individual_list.size()
                     << "," << this->income << ","
                     << this->disease_risk << ","
                     << this->exposure_risk;

    // write all individuals in this household to the individual stream
    bool disease[] = { false, false, false, false };
    bool sample_mode = this->get_population()->get_sample_mode();
    for( auto it = this->individual_list.begin(); it != this->individual_list.end(); ++it )
    {
      individual *i = *it;
      if( !sample_mode || i->is_selected() )
      {
        individual_stream << town_index << "," << this->index << ",";
        for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ ) if( !disease[rr] ) disease[rr] = i->is_disease(rr);
        i->to_csv( household_stream, individual_stream );
        individual_stream << std::endl;
      }
    }

    // finish writing the household stream
    for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ ) household_stream << "," << ( disease[rr] ? "1" : "0" );
    household_stream << std::endl;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  unsigned int household::get_number_of_individuals() const
  {
    return this->individual_list.size();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::assert_summary()
  {
    this->get_population()->assert_summary();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::rebuild_summary()
  {
    this->sum.reset();
    for( auto it = this->individual_list.begin(); it != this->individual_list.end(); ++it )
    {
      (*it)->rebuild_summary();
      this->sum.add( (*it) );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::select()
  {
    this->selected = true;
    this->parent->select();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::unselect()
  {
    this->get_population()->expire_summary();
    this->selected = false;
    for( auto it = this->individual_list.begin(); it != this->individual_list.end(); ++it )
      (*it)->unselect();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::copy( const household* object )
  {
    this->index = object->index;
    this->income = object->income;
    this->disease_risk = object->disease_risk;
    this->exposure_risk = object->exposure_risk;
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
