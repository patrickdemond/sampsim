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
#include "options.h"
#include "population.h"
#include "sample/sized_sample.h"
#include "town.h"
#include "utilities.h"

void setup_sample( sampsim::options &opts )
{
  // define inputs
  opts.add_input( "population_file" );
  opts.add_input( "output_file" );
  opts.add_flag( 'f', "flat_file", "Whether to output data in two CSV files instead of JSON data" );
  opts.add_flag( 's', "summary_file", "Whether to output summary data of the sample" );
  if( GNUPLOT_AVAILABLE )
  {
    opts.add_flag( 'p', "plot", "Whether to create a plot of the sample (will create a flat-file)" );
    opts.add_option( 'P', "plot_sample_number", "", "Whether to only plot a specific sample" );
  }
  opts.add_flag( 'v', "verbose", "Be verbose when generating sample" );
  opts.add_heading( "" );
  opts.add_heading( "Sampling parameters (overrides config files):" );
  opts.add_heading( "" );
  opts.add_option( "seed", "", "Seed used by the random generator" );
  opts.add_option( "age", "either", "Restricts sample by age (\"adult\", \"child\" or \"either\")" );
  opts.add_flag( "one_per_household", "Only sample one individual per household" );
  opts.add_option( "sex", "either", "Restricts sample by sex (\"male\", \"female\" or \"either\")" );
  opts.add_option( "samples", "1", "The number of times to sample the population" );
  opts.add_option( "size", "1000", "How many individuals to select in each sample" );
}

void parse_sample(
  sampsim::options &opts, sampsim::sample::sized_sample *sample, bool &flat, bool &summary, bool &plot )
{
  flat = opts.get_flag( "flat_file" );
  summary = opts.get_flag( "summary_file" );
  plot = GNUPLOT_AVAILABLE ? opts.get_flag( "plot" ) : false;

  sample->set_seed( opts.get_option( "seed" ) );
  sample->set_age( sampsim::get_age_type( opts.get_option( "age" ) ) );
  sample->set_one_per_household( opts.get_flag( "one_per_household" ) );
  sample->set_sex( sampsim::get_sex_type( opts.get_option( "sex" ) ) );
  sample->set_number_of_samples( opts.get_option_as_int( "samples" ) );
  sample->set_size( opts.get_option_as_int( "size" ) );
}

// function to generate gnuplot commands
std::string gnuplot(
  sampsim::town *town,
  std::string population_name,
  int town_index,
  std::string sample_name = "" )
{
  bool multitown = 0 <= town_index;
  if( !multitown ) town_index = 0; // there is only one town so its index is 0
  sampsim::population *population = town->get_population();

  // the name is the sample name if one is provided, the population name if not
  std::string name = 0 == sample_name.length() ? population_name : sample_name;
  std::stringstream stream;

  stream << name;
  if( multitown ) stream << ".t" << std::setw( log( population->get_number_of_towns()+1 ) )
                         << std::setfill( '0' ) << ( town_index + 1 );
  stream << ".png";

  std::string image_filename = stream.str();
  stream.str( "" );
  stream << "Household plot of \\\"" << name << "\\\"";
  if( multitown ) stream << ", town " << ( town_index + 1 );
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

  // remove .json from the population name
  if( 5 < population_name.size() &&
      0 == population_name.compare( population_name.size()-5, 5, ".json" ) )
    population_name = population_name.substr( 0, population_name.size()-5 );

  stream
    << "set termoption dashed; "
    << "plot \"" << population_name << ".household.csv\" "
    <<   "using ($1 == " << town_index << " ? $3 : 1/0):4:($8/2):10 palette pt 6 ps variable "
    <<   "title \"town " << ( town_index + 1 ) << "\"";

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

// convenience function to generate gnuplot commands (for one town only)
std::string gnuplot(
  sampsim::town *town,
  std::string population_name,
  std::string sample_name = "" )
{
  return gnuplot( town, population_name, -1, sample_name );
}
