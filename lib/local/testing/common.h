/*=========================================================================

  Program:  sampsim
  Module:   common.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .SECTION Description
// Common functions used by unit tests.
//

#ifndef __sampsim_common_h
#define __sampsim_common_h

#include "population.h"
#include "trend.h"
#include "utilities.h"

#include <sstream>

using namespace std;
using namespace sampsim;

inline void create_test_population(
  sampsim::population *population,
  int number_of_towns = 8,
  int town_size_min = 5000,
  int town_size_max = 100000,
  double popdens_mx = 0,
  double popdens_my = 0 )
{
  sampsim::utilities::verbose = false;
  stringstream stream;
  stream << time( NULL );
  population->set_seed( stream.str() );
  population->set_number_of_towns( number_of_towns );
  population->set_town_size_min( town_size_min );
  population->set_town_size_max( town_size_max );
  population->set_town_size_shape( 1.0 );
  population->set_mean_household_population( 4 );
  population->set_tile_width( 1 );
  population->set_number_of_tiles_x( 10 );
  population->set_number_of_tiles_y( 10 );
  population->set_number_of_disease_pockets( 3 );
  population->set_population_density_slope( popdens_mx, popdens_my );
  population->generate();
}

#endif // __common_h
