/*=========================================================================

  Program:  sampsim
  Module:   epi.cxx
  Language: C++

=========================================================================*/

#include "epi.h"

#include "building.h"
#include "building_tree.h"
#include "town.h"

#include <cmath>
#include <json/value.h>
#include <json/writer.h>
#include <limits>
#include <stdexcept>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  epi::epi()
  {
    this->skip = 1;
    this->first_building_index = 0;
    this->current_building = NULL;
    this->initial_building_list.clear();
    this->tree = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  epi::~epi()
  {
    utilities::safe_delete( this->tree );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::copy( const epi* object )
  {
    this->skip = object->skip;
    this->first_building_index = object->first_building_index;
    this->current_building = object->current_building;
    this->tree = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::create_building_list( sampsim::town *town, building_list_type &building_list )
  {
    sized_sample::create_building_list( town, building_list );

    // create the tree from the newly created building list
    this->tree = new sampsim::building_tree( building_list );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* epi::select_next_building( building_list_type &building_list )
  {
    building* b = NULL;

    if( NULL != this->current_building )
    {
      // 5. find the nearest building (distance measured in a straight path (as the crow flies))

      // we do this step "skip" times, then select the last in the set
      coordinate position = this->current_building->get_position();

      // first make sure we have enough buildings left
      if( building_list.size() <= this->skip )
        throw std::runtime_error(
          "Ran out of buildings to sample.  You must either lower the sample size or increase the lowest town population." );

      for( int i = 0; i < this->skip; i++ )
      {
        // find the nearest building, make note of its position and remove it if it isn't the
        // last building in the loop
        b = this->tree->find_nearest( position );
        if( NULL == b )
          throw std::runtime_error(
            "Ran out of buildings to sample.  You must either lower the sample size or increase the lowest town population." );

        this->tree->remove( b );
        position = b->get_position();
        if( utilities::verbose )
        {
          if( i == this->skip-1 ) utilities::output( "closest building %d: selected", i+1 );
          else utilities::output( "closest building %d: skipping", i+1 );
        }
      }
    }

    this->current_building = b;
    return b;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::reset_for_next_sample( const bool full )
  {
    sized_sample::reset_for_next_sample( full );

    this->first_building_index = 0;
    this->current_building = NULL;
    this->initial_building_list.clear();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::set_skip( const unsigned int skip )
  {
    if( utilities::verbose ) utilities::output( "setting skip to %d", skip );
    this->skip = skip;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double epi::get_immediate_sample_weight( const sampsim::individual* individual ) const
  {
    // Since towns are chosen using PPS, and there is a fixed number of people sampled per town, the
    // samples are "self-weighted" (ie: the probability of any person being chosen is the same for
    // everyone in the sample)
    return 1.0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::from_json( const Json::Value &json )
  {
    sized_sample::from_json( json );
    this->skip = json["skip"].asUInt();
    this->first_building_index = json["first_building_index"].asUInt();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::to_json( Json::Value &json ) const
  {
    sized_sample::to_json( json );
    json["skip"] = this->skip;
    json["first_building_index"] = this->first_building_index;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string epi::get_csv_header() const
  {
    std::stringstream stream;
    stream << sized_sample::get_csv_header();
    stream << "# skip: " << this->skip << std::endl;
    return stream.str();
  }
}
}
