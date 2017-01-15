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
    this->age = sampsim::get_age_type( json["age"].asString() );
    this->sex = sampsim::get_sex_type( json["sex"].asString() );
    this->disease = 1 == json["disease"].asUInt();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["age"] = Json::Value( sampsim::get_age_type_name( this->age ) );
    json["sex"] = Json::Value( sampsim::get_sex_type_name( this->sex ) );
    json["disease"] = this->disease ? 1 : 0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::to_csv( std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    individual_stream << sampsim::get_age_type_name( this->age ) << ","
                      << sampsim::get_sex_type_name( this->sex ) << ","
                      << ( this->disease ? 1 : 0 );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  summary* individual::get_summary()
  {
    if( this->sum.is_expired() )
    {
      bool sample_mode = this->get_population()->get_sample_mode();
      if( !sample_mode || this->is_selected() )
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
      this->sum.set_expired( false );
    }

    return &( this->sum );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::vector< std::pair<unsigned int, unsigned int> > individual::count_individuals() const
  {
    bool sample_mode = this->get_population()->get_sample_mode();
    std::vector< std::pair<unsigned int, unsigned int> > count_vector;
    count_vector.resize( 9, std::pair<unsigned int, unsigned int>( 0, 0 ) );
    if( !sample_mode || this->is_selected() )
    {
      if( this->is_disease() )
      {
        count_vector[0].first++;
        if( ADULT == this->get_age() ) count_vector[1].first++;
        else if( CHILD == this->get_age() ) count_vector[2].first++;
        if( MALE == this->get_sex() ) count_vector[3].first++;
        else if( FEMALE == this->get_sex() ) count_vector[4].first++;
        if( ADULT == this->get_age() && MALE == this->get_sex() ) count_vector[5].first++;
        if( ADULT == this->get_age() && FEMALE == this->get_sex() ) count_vector[6].first++;
        if( CHILD == this->get_age() && MALE == this->get_sex() ) count_vector[7].first++;
        if( CHILD == this->get_age() && FEMALE == this->get_sex() ) count_vector[8].first++;
      }
      else
      {
        count_vector[0].second++;
        if( ADULT == this->get_age() ) count_vector[1].second++;
        else if( CHILD == this->get_age() ) count_vector[2].second++;
        if( MALE == this->get_sex() ) count_vector[3].second++;
        else if( FEMALE == this->get_sex() ) count_vector[4].second++;
        if( ADULT == this->get_age() && MALE == this->get_sex() ) count_vector[5].second++;
        if( ADULT == this->get_age() && FEMALE == this->get_sex() ) count_vector[6].second++;
        if( CHILD == this->get_age() && MALE == this->get_sex() ) count_vector[7].second++;
        if( CHILD == this->get_age() && FEMALE == this->get_sex() ) count_vector[8].second++;
      }
    }

    return count_vector;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::select( const double sample_weight )
  {
    this->sample_weight = sample_weight;
    this->selected = true;
    this->parent->select();
  }
}
