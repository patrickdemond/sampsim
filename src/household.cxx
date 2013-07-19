/*=========================================================================

  Program:  sampsim
  Module:   household.cxx
  Language: C++

=========================================================================*/

#include "household.h"

#include "building.h"
#include "household.h"
#include "individual.h"
#include "simulation.h"
#include "tile.h"
#include "utilities.h"

#include <fstream>
#include <json/value.h>
#include <random>

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
    // we're holding a light reference to the parent, don't delete it
    this->parent = 0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::generate()
  {
    // Randomly determine the number of individuals in the household using a poisson distribution
    // We'll use 1 + Poisson( mean - 1 ) so that there are no empty households
    double mean_population = this->parent->get_parent()->get_parent()->get_mean_household_population();
    std::poisson_distribution<int> distribution( mean_population - 1 );
    int population = distribution( utilities::random_engine ) + 1;
    
    // make the first individual an adult of random sex
    bool male = 0 == utilities::random( 0, 1 );
    individual *i = new individual( this );
    i->set_male( male );
    i->set_adult( true );
    this->individual_list.push_back( i );

    // now make the rest of the members of this household
    for( int c = 1; c < population; c++ )
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
  void household::to_json( Json::Value &json )
  {
    json = Json::Value( Json::objectValue );
    json["individual_list"] = Json::Value( Json::arrayValue );
    json["individual_list"].resize( this->individual_list.size() );

    int index = 0;
    std::vector< individual* >::const_iterator it; 
    for( it = this->individual_list.begin(); it != this->individual_list.end(); ++it, ++index )
      ( *it )->to_json( json["individual_list"][index] );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void household::to_csv( std::ofstream &household_stream, std::ofstream &individual_stream )
  {
    // write the household index and position to the household stream
    household_stream << utilities::household_index << ",";
    this->parent->get_position().to_csv( household_stream );
    household_stream << std::endl;

    // write all individuals in this household to the individual stream
    std::vector< individual* >::const_iterator it;
    for( it = this->individual_list.begin(); it != this->individual_list.end(); ++it )
    {
      individual_stream << utilities::household_index << ",";
      ( *it )->to_csv( individual_stream );
      individual_stream << std::endl;
    }

    utilities::household_index++;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  int household::get_population() const
  {
    return this->individual_list.size();
  }
}
