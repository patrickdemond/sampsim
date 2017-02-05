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
    this->state = UNKNOWN_STATE_TYPE;
    this->sample_weight = 1.0;
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
    this->state = i->state;
    this->sample_weight = i->sample_weight;
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
    population *pop = this->get_population();
    this->index = json["index"].asUInt();
    this->age = sampsim::get_age_type( json["age"].asString() );
    this->sex = sampsim::get_sex_type( json["sex"].asString() );
    this->state = 1 == json["disease"].asUInt() ? DISEASED : HEALTHY;
    this->sample_weight = pop->get_use_sample_weights() ? json["sample_weight"].asDouble() : 1.0;
    pop->assert_individual_index( this->index );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["index"] = this->index;
    json["age"] = Json::Value( sampsim::get_age_type_name( this->age ) );
    json["sex"] = Json::Value( sampsim::get_sex_type_name( this->sex ) );
    json["disease"] = DISEASED == this->state ? 1 : 0;
    if( this->get_population()->get_use_sample_weights() ) json["sample_weight"] = this->sample_weight;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void individual::to_csv( std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    individual_stream << this->index << ","
                      << sampsim::get_age_type_name( this->age ) << ","
                      << sampsim::get_sex_type_name( this->sex ) << ","
                      << ( DISEASED == this->state ? 1 : 0 );
    if( this->get_population()->get_use_sample_weights() ) individual_stream << "," << this->sample_weight;
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
      int index = -1;
      index = ADULT == this->get_age()
            ? ( MALE == this->get_sex()
              ? ( HEALTHY == this->get_state() ? summary::adult_male_healthy : summary::adult_male_diseased )
              : ( HEALTHY == this->get_state() ? summary::adult_female_healthy : summary::adult_female_diseased )
            ) : (
                MALE == this->get_sex()
              ? ( HEALTHY == this->get_state() ? summary::child_male_healthy : summary::child_male_diseased )
              : ( HEALTHY == this->get_state() ? summary::child_female_healthy : summary::child_female_diseased )
            );

      this->sum.count[index]++;
      if( this->get_population()->get_use_sample_weights() )
        this->sum.weighted_count[index] += this->sample_weight;
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
