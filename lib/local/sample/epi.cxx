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
    this->number_of_sectors = 1;
    this->current_sector_index = -1;
    this->skip = 1;
    this->periphery = false;
    this->start_angle_defined = false;
    this->start_angle = 0;
    this->first_building_index = 0;
    this->current_building = NULL;
    this->periphery_building_selected = false;
    this->buildings_in_current_sector = 0;
    this->initial_building_list.clear();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::copy( const epi* object )
  {
    this->number_of_sectors = object->number_of_sectors;
    this->current_sector_index = object->current_sector_index;
    this->skip = object->skip;
    this->periphery = object->periphery;
    this->start_angle_defined = object->start_angle_defined;
    this->start_angle = object->start_angle;
    this->first_building_index = object->first_building_index;
    this->current_building = object->current_building;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* epi::select_next_building( sampsim::building_tree &tree )
  {
    building* b = NULL;

    // check to see if we have to move to the next sector and reset the current building if we do
    double size_fraction = static_cast< double >( this->get_size() ) /
                           static_cast< double >( this->number_of_sectors );
    if( this->get_current_town_size() >= size_fraction * this->get_current_sector() )
    {
      this->current_building = NULL;
      this->buildings_in_current_sector = 0;
    }

    if( this->periphery &&
        !this->periphery_building_selected &&
        // whether we've selected half of the sample required for this sector
        2 * this->number_of_sectors * this->buildings_in_current_sector >= this->get_size() )
    {
      // get the furthest building from the center of the town
      b = *std::max_element(
        this->initial_building_list.cbegin(),
        this->initial_building_list.cend(),
        []( building* b1, building* b2 ) -> bool {
          return b1->get_town()->get_centroid().distance( b1->get_position() ) <
                 b2->get_town()->get_centroid().distance( b2->get_position() );
        }
      );
      this->periphery_building_selected = true;
      if( utilities::verbose )
        utilities::output(
          "selected furthest building from center of town out of %d buildings in arc",
          this->initial_building_list.size() );
    }
    else
    {
      if( NULL == this->current_building )
      {
        // 1. determine the start angle
        this->determine_next_start_angle();

        // 2. get list of all buildings in the area determined by the sub-algorithm in the chosen direction
        this->determine_initial_building_list( tree );
        if( 0 == this->initial_building_list.size() )
          throw std::runtime_error(
            "Unable to find initial building after 1000 attempts.  You must either lower the sample size or increase the initial selection area." );

        // 3. select a building from the list produced by step 2
        if( this->periphery )
        {
          // get the closest building to the center of the town
          b = *std::min_element(
            this->initial_building_list.cbegin(),
            this->initial_building_list.cend(),
            []( building* b1, building* b2 ) -> bool {
              return b1->get_town()->get_centroid().distance( b1->get_position() ) <
                     b2->get_town()->get_centroid().distance( b2->get_position() );
            }
          );
          this->periphery_building_selected = false;
          if( utilities::verbose )
            utilities::output(
              "selected closest building to center of town out of %d buildings in arc",
              this->initial_building_list.size() );
        }
        else
        {
          this->first_building_index = utilities::random( 0, this->initial_building_list.size() - 1 );
          auto initial_it = this->initial_building_list.begin();
          std::advance( initial_it, this->first_building_index );
          b = *initial_it;
          if( utilities::verbose )
            utilities::output(
              "selected building %d of %d in arc",
              this->first_building_index + 1,  
              this->initial_building_list.size() );
        }
      }   
      else
      {   
        // 5. find the nearest building (distance measured in a straight path (as the crow flies))

        // we do this step "skip" times, then select the last in the set
        coordinate position = this->current_building->get_position();

        // first make sure we have enough buildings left
        if( tree.get_building_list().size() <= this->skip )
          throw std::runtime_error(
            "Ran out of buildings to sample.  You must either lower the sample size or increase the lowest town population." );

        for( int i = 0; i < this->skip; i++ )
        {
          // find the nearest building, make note of its position and remove it if it isn't the
          // last building in the loop
          b = tree.find_nearest( position );
          position = b->get_position();
          if( i < ( this->skip - 1 ) ) tree.remove( b );
          if( utilities::verbose )
          {
            if( i == this->skip-1 ) utilities::output( "closest building %d: selected", i+1 );
            else utilities::output( "closest building %d: skipping", i+1 );
          }
        }
      }
    }

    this->buildings_in_current_sector++;
    this->current_building = b;
    return b;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::reset_for_next_sample( const bool full )
  {
    sized_sample::reset_for_next_sample( full );

    this->current_sector_index = -1;
    this->start_angle_defined = false;
    this->start_angle = 0;
    this->first_building_index = 0;
    this->periphery_building_selected = false;
    this->buildings_in_current_sector = 0;
    this->current_building = NULL;
    this->initial_building_list.clear();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::determine_next_start_angle()
  {
    // increment to the next sector
    this->current_sector_index++;

    if( this->start_angle_defined )
    {
      // if this isn't the first sector then rotate start_angle by the width of a sector
      if( 0 < this->current_sector_index )
      {
        this->start_angle += ( 2*M_PI / static_cast< double >( this->number_of_sectors ) );
        if( 2*M_PI <= this->start_angle ) this->start_angle -= 2*M_PI;
      }
    }
    else
    {
      // get the next sector and select a random angle within its range
      std::pair< double, double > angles = this->get_next_sector_range();
      this->start_angle = utilities::random() * safe_subtract( angles.second, angles.first ) + angles.first;
    }

//    if( utilities::verbose )
      utilities::output(
        "Beginning sector %d of %d with a starting angle of %0.3f radians",
        this->get_current_sector(),
        this->number_of_sectors,
        this->start_angle );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::set_number_of_sectors( const unsigned int number_of_sectors )
  {
    if( utilities::verbose ) utilities::output( "setting number of sectors to %d", number_of_sectors );
    this->number_of_sectors = number_of_sectors;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::pair< double, double > epi::get_next_sector_range()
  {
    // make sure that we aren't out of bounds
    if( this->current_sector_index == this->number_of_sectors )
      throw std::runtime_error(
        "Tried to get sector range but index is out of bounds" );

    std::pair< double, double > angles;
    angles.first = -M_PI + 2*M_PI *
                   static_cast< double >( this->current_sector_index ) /
                   static_cast< double >( this->number_of_sectors );
    angles.second = angles.first + 2*M_PI *
                    static_cast< double >( this->current_sector_index + 1 ) /
                    static_cast< double >( this->number_of_sectors );

    return angles;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::set_skip( const unsigned int skip )
  {
    if( utilities::verbose ) utilities::output( "setting skip to %d", skip );
    this->skip = skip;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::set_periphery( const bool periphery )
  {
    if( utilities::verbose ) utilities::output( "setting periphery to %s", periphery ? "true" : "false" );
    this->periphery = periphery;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double epi::get_sample_weight( const sampsim::individual* individual ) const
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
    this->number_of_sectors = json["number_of_sectors"].asUInt();
    this->skip = json["skip"].asUInt();
    this->periphery = json["periphery"].asBool();
    this->start_angle = json["start_angle"].asDouble();
    this->first_building_index = json["first_building_index"].asUInt();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::to_json( Json::Value &json ) const
  {
    sized_sample::to_json( json );
    json["number_of_sectors"] = this->number_of_sectors;
    json["skip"] = this->skip;
    json["periphery"] = this->periphery;
    json["start_angle"] = this->start_angle;
    json["first_building_index"] = this->first_building_index;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string epi::get_csv_header() const
  {
    std::stringstream stream;
    stream << sized_sample::get_csv_header();
    stream << "# number_of_sectors: " << this->number_of_sectors << std::endl;
    stream << "# skip: " << this->skip << std::endl;
    stream << "# periphery: " << ( this->periphery ? "true" : "false" ) << std::endl;
    return stream.str();
  }
}
}
