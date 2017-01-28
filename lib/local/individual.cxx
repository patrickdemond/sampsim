/*=========================================================================

  Program:  sampsim
  Module:   individual.cxx
  Language: C++

=========================================================================*/

#include "individual.h"

#include "household.h"
#include "population.h"
#include "summary.h"

#include <json/value.h>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  individual::individual( household *parent )
  {
    this->parent = parent;
    this->index = this->get_population()->get_next_individual_index();
    this->age = UNKNOWN_AGE_TYPE;
    this->sex = UNKNOWN_SEX_TYPE;
    this->disease = false;
    this->sample_weight = 0.0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  individual::~individual()
  {
    // we're holding a light reference to the parent, don't delete it
    this->parent = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::copy( const individual* i )
  {
    this->index = i->index;
    this->selected = i->selected;
    this->age = i->age;
    this->sex = i->sex;
    this->disease = i->disease;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::create()
  {
    // make sure the individual has a parent
    if( NULL == this->parent ) throw std::runtime_error( "Tried to create an orphaned individual" );

    this->get_population()->expire_summary();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::define()
  {
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* individual::get_building() const
  {
    return NULL == this->parent ? NULL : this->parent->get_building();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  tile* individual::get_tile() const
  {
    return NULL == this->parent ? NULL : this->parent->get_tile();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  town* individual::get_town() const
  {
    return NULL == this->parent ? NULL : this->parent->get_town();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  population* individual::get_population() const
  {
    return NULL == this->parent ? NULL : this->parent->get_population();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::from_json( const Json::Value &json )
  {
    this->index = json["index"].asUInt();
    this->age = sampsim::get_age_type( json["age"].asString() );
    this->sex = sampsim::get_sex_type( json["sex"].asString() );
    this->disease = 1 == json["disease"].asUInt();
    this->get_population()->assert_individual_index( this->index );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["index"] = this->index;
    json["age"] = Json::Value( sampsim::get_age_type_name( this->age ) );
    json["sex"] = Json::Value( sampsim::get_sex_type_name( this->sex ) );
    json["disease"] = this->disease ? 1 : 0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::to_csv( std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    individual_stream << this->index << ","
                      << sampsim::get_age_type_name( this->age ) << ","
                      << sampsim::get_sex_type_name( this->sex ) << ","
                      << ( this->disease ? 1 : 0 );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::assert_summary()
  {
    this->get_population()->assert_summary();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::rebuild_summary()
  {
    this->sum.reset();

    if( !this->get_population()->get_sample_mode() || this->is_selected() )
    {
      if( this->is_disease() )
      {
        if( ADULT == this->get_age() ) this->sum.count[summary::adult][summary::diseased]++;
        else if( CHILD == this->get_age() ) this->sum.count[summary::child][summary::diseased]++;
        if( MALE == this->get_sex() ) this->sum.count[summary::male][summary::diseased]++;
        else if( FEMALE == this->get_sex() ) this->sum.count[summary::female][summary::diseased]++;
        if( ADULT == this->get_age() && MALE == this->get_sex() )
          this->sum.count[summary::adult_male][summary::diseased]++;
        if( ADULT == this->get_age() && FEMALE == this->get_sex() )
          this->sum.count[summary::adult_female][summary::diseased]++;
        if( CHILD == this->get_age() && MALE == this->get_sex() )
          this->sum.count[summary::child_male][summary::diseased]++;
        if( CHILD == this->get_age() && FEMALE == this->get_sex() )
          this->sum.count[summary::child_female][summary::diseased]++;
      }
      else
      {
        if( ADULT == this->get_age() ) this->sum.count[summary::adult][summary::healthy]++;
        else if( CHILD == this->get_age() ) this->sum.count[summary::child][summary::healthy]++;
        if( MALE == this->get_sex() ) this->sum.count[summary::male][summary::healthy]++;
        else if( FEMALE == this->get_sex() ) this->sum.count[summary::female][summary::healthy]++;
        if( ADULT == this->get_age() && MALE == this->get_sex() )
          this->sum.count[summary::adult_male][summary::healthy]++;
        if( ADULT == this->get_age() && FEMALE == this->get_sex() )
          this->sum.count[summary::adult_female][summary::healthy]++;
        if( CHILD == this->get_age() && MALE == this->get_sex() )
          this->sum.count[summary::child_male][summary::healthy]++;
        if( CHILD == this->get_age() && FEMALE == this->get_sex() )
          this->sum.count[summary::child_female][summary::healthy]++;
      }
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::select( const double sample_weight )
  {
    this->sample_weight = sample_weight;
    this->selected = true;
    this->parent->select();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::unselect()
  {
    this->selected = false;
    this->sample_weight = 0.0;
    this->get_population()->expire_summary();
  }
}
