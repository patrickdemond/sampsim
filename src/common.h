/*=========================================================================

  Program:  sampsim
  Module:   common.h
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Common functions used by project executables
//

#include "line.h"
#include "town.h"
#include "population.h"
#include "utilities.h"

// function to generate gnuplot commands
std::string gnuplot(
  sampsim::town *town,
  std::string population_name,
  unsigned int town_index,
  std::string sample_name = "" )
{
  sampsim::population *population = town->get_population();

  // the name is the sample name if one is provided, the population name if not
  std::string name = 0 == sample_name.length() ? population_name : sample_name;
  std::stringstream stream;

  stream << name << ".t"
         << std::setw( log( population->get_number_of_towns()+1 ) ) << std::setfill( '0' )
         << town_index << ".png";

  std::string image_filename = stream.str();
  stream.str( "" );
  stream << "Household plot of \\\"" << name << "\\\", town " << town_index;
  std::string title = stream.str();

  stream.str( "" );
  stream
    << "set title \"" << title << "\" font \"sans, 18\"; "
    << "set xlabel \"Position (km)\"; "
    << "set ylabel \"Position (km)\"; "
    << "set xrange [0:" << town->get_x_width() << "]; "
    << "set yrange [0:" << town->get_y_width() << "]; "
    << "set datafile separator \",\"; "
    << "set key at 35.5, 36.5; "
    << "unset colorbox; "
    << "set palette model RGB defined ( 0 0 0 1, 1 1 0 0 ); "
    << "set term png size 1200,1230; "
    << "set output \"" << image_filename << "\"; ";

  if( town->get_has_river() )
  { // add the river's bank equations
    sampsim::line* river_banks = town->get_river_banks();
    double m = tan( river_banks[0].angle );
    double b1 = river_banks[0].intercept.y - river_banks[0].intercept.x * m;
    double b2 = river_banks[1].intercept.y - river_banks[1].intercept.x * m;
    stream << "r1(x) = " << m << "*x + " << b1 << "; "
           << "r2(x) = " << m << "*x + " << b2 << "; ";
  }

  stream
    << "set termoption dashed; "
    << "plot \"" << population_name << ".household.csv\" "
    <<   "using ($1 == " << town_index << " ? $3 : 1/0):4:($8/2):10 palette pt 6 ps variable "
    <<   "title \"town " << town_index << "\"";

  if( 0 < sample_name.length() )
  {
    stream << ", "
      << "\"" << sample_name << ".household.csv\" "
      <<   "using ($1 == " << town_index << " ? $3 : 1/0):4 lc rgb \"black\" pt 3 ps 1.5 "
      <<   "title \"sampled\"";
  }

  if( town->get_has_river() ) stream << ", r1(x) lt 2 lc 0, r2(x) lt 2 lc 0";

  // now put the command in an gnuplot execute statement
  std::string command = stream.str();
  stream.str( "" );
  stream << "gnuplot -e " << "'" << command << "'";
  return stream.str();
}
