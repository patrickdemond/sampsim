/*=========================================================================

  Program:  sampsim
  Module:   test_sample_sample.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the sample::sample class
//

#include "UnitTest++.h"

#include "building.h"
#include "common.h"
#include "household.h"
#include "individual.h"
#include "population.h"
#include "tile.h"
#include "town.h"
#include "random.h"

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_sample_sample )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population, 10, 10000, 25000 );

  population->write( "a", true );

  sampsim::sample::random *sample = new sampsim::sample::random;
  sample->set_population( population );
  sample->set_number_of_samples( 1 );
  sample->set_number_of_towns( 1 );
  sample->set_size( 100 );
  sample->generate();
  sample->write( "b", true );

  auto pop_it = sample->get_sampled_population_list_cbegin();
  sampsim::population *sampled_population = *pop_it;
  for( auto sampled_town_it = sampled_population->get_town_list_cbegin();
       sampled_town_it != sampled_population->get_town_list_cend();
       ++sampled_town_it )
  {
    sampsim::town *sampled_town = *sampled_town_it;
    for( auto sampled_tile_it = sampled_town->get_tile_list_begin();
         sampled_tile_it != sampled_town->get_tile_list_end();
         ++sampled_tile_it )
    {
      sampsim::tile *sampled_tile = sampled_tile_it->second;
      std::pair< sampsim::coordinate, sampsim::coordinate > extent = sampled_tile->get_extent();
      for( auto sampled_building_it = sampled_tile->get_building_list_begin();
           sampled_building_it != sampled_tile->get_building_list_end();
           ++sampled_building_it )
      {
        sampsim::building *sampled_building = *sampled_building_it;
        for( auto sampled_household_it = sampled_building->get_household_list_begin();
             sampled_household_it != sampled_building->get_household_list_end();
             ++sampled_household_it )
        {
          sampsim::household *sampled_household = *sampled_household_it;
          for( auto sampled_individual_it = sampled_household->get_individual_list_begin();
               sampled_individual_it != sampled_household->get_individual_list_end();
               ++sampled_individual_it )
          {
            sampsim::individual *sampled_individual = *sampled_individual_it;

            // find this individual in the unsampled population by index and make sure the details match
            bool done = false;
            for( auto town_it = population->get_town_list_cbegin();
                 town_it != population->get_town_list_cend();
                 ++town_it )
            {
              sampsim::town *town = *town_it;
              for( auto tile_it = town->get_tile_list_begin();
                   tile_it != town->get_tile_list_end();
                   ++tile_it )
              {
                sampsim::tile *tile = tile_it->second;
                std::pair< sampsim::coordinate, sampsim::coordinate > extent = tile->get_extent();
                for( auto building_it = tile->get_building_list_begin();
                     building_it != tile->get_building_list_end();
                     ++building_it )
                {
                  sampsim::building *building = *building_it;
                  for( auto household_it = building->get_household_list_begin();
                       household_it != building->get_household_list_end();
                       ++household_it )
                  {
                    sampsim::household *household = *household_it;
                    for( auto individual_it = household->get_individual_list_begin();
                         individual_it != household->get_individual_list_end();
                         ++individual_it )
                    {
                      sampsim::individual *individual = *individual_it;
                      if( sampled_individual->get_index() == individual->get_index() )
                      {
                        CHECK_EQUAL(
                          sampled_individual->get_household()->get_index(),
                          individual->get_household()->get_index()
                        );
                        CHECK_EQUAL( sampled_individual->get_sex(), individual->get_sex() );
                        CHECK_EQUAL( sampled_individual->get_age(), individual->get_age() );
                        for( unsigned int rr = 0; rr < sampsim::utilities::rr_size; rr++ )
                          CHECK_EQUAL( sampled_individual->is_disease( rr ), individual->is_disease( rr ) );
                        done = true;
                      }

                      if( done ) break;
                    }
                    if( done ) break;
                  }
                  if( done ) break;
                }
                if( done ) break;
              }
              if( done ) break;
            }
          }
        }
      }
    }
  }
}
