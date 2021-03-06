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

// function to generate gnuplot commands
std::string gnuplot(
  sampsim::town *town,
  std::string population_name,
  int town_index,
  std::string sample_name = "" )
{
  if( GNUPLOT_AVAILABLE ) return "";

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
    <<   "using ($1 == " << town_index << " ? $3 : 1/0):4:($8/4):10 palette pt 6 ps variable "
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

void setup_sample( sampsim::options &opts )
{
  // define inputs
  opts.add_input( "population_file" );
  opts.add_input( "output_file" );
  opts.add_flag( 'f', "flat_file", "Whether to output data in two CSV \"flat\" files" );
  opts.add_flag( 'F', "flat_file_only", "Whether to output data in CSV format, omitting the usual JSON file" );
  opts.add_flag( 's', "summary_file", "Whether to output summary data of the sample" );
  opts.add_flag( 'S', "summary_file_only",
    "Whether to output summary data of the sample only, with no data output" );
  if( GNUPLOT_AVAILABLE ) opts.add_flag( 'p', "plot", "Plot all samples (will create a flat-file)" );
  opts.add_flag( 'v', "verbose", "Be verbose when generating sample" );
  opts.add_flag( 'q', "quiet", "Be quiet when generating sample" );

  static const std::string array[] = { "1", "1" };
  std::vector< std::string > vector( array, array + sizeof( array ) / sizeof( array[0] ) );
  opts.add_option( 'p', "part", vector, "Divide samples into sub-batches, where \"part,total\"" );

  opts.add_heading( "" );
  opts.add_heading( "Sampling parameters (overrides config files):" );
  opts.add_heading( "" );
  opts.add_option( "seed", "", "Seed used by the random generator" );
  opts.add_flag( "use_sample_weights", "Whether to calculate and use sample weights" );
  opts.add_option( "age", "either", "Restricts sample by age (\"adult\", \"child\" or \"either\")" );
  opts.add_flag( "one_per_household", "Only sample one individual per household" );
  opts.add_option( "sex", "either", "Restricts sample by sex (\"male\", \"female\" or \"either\")" );
  opts.add_option( "samples", "1", "The number of times to sample the population" );
  opts.add_option( "towns", "1", "For multi-town populations, the number of towns to sample" );
  opts.add_option( "size", "1000", "How many individuals to select in each sample" );
  opts.add_flag( "resample_towns", "Resample towns with every sample iteration" );
}

void process_sample( sampsim::options &opts, sampsim::sample::sized_sample *sample )
{
  bool flat = opts.get_flag( "flat_file" );
  bool flat_only = opts.get_flag( "flat_file_only" );
  if( flat_only ) flat = true;
  bool summary = opts.get_flag( "summary_file" );
  bool summary_only = opts.get_flag( "summary_file_only" );
  if( summary_only ) summary = true;
  bool plot = GNUPLOT_AVAILABLE ? opts.get_flag( "plot" ) : false;
  int samples = opts.get_option_as_int( "samples" );
  std::vector< int > part_list = opts.get_option_as_int_list( "part" );

  // can't get a summary if run is done in parts
  if( 1 < part_list[1] && ( summary || summary_only ) )
  {
    std::cout << "ERROR: generating summaries and dividing samples into sub-batches are mutually exclusive. "
              << "If you wish to get a summary of a sample which was generated using sub-batches run the "
              << "summary program on the generated sample files."
              << std::endl;
  }
  else if( samples < part_list[1] )
  {
    std::cout << "ERROR: cannot divide " << samples << " samples into " << part_list[1] << " parts" << std::endl;
  }
  else if( 0 >= part_list[0] || part_list[0] > part_list[1] )
  {
    std::cout << "ERROR: requested part " << part_list[0] << " must be between 1 and the total number of parts ("
              << part_list[1] << ")" << std::endl;
  }
  else
  {
    std::string population_filename = opts.get_input( "population_file" );
    std::string output_filename = opts.get_input( "output_file" );
    sampsim::utilities::verbose = opts.get_flag( "verbose" );
    sampsim::utilities::quiet = opts.get_flag( "quiet" );

    if( !sampsim::utilities::quiet )
      std::cout << "sampsim strip_epi_sample version " << sampsim::utilities::get_version() << std::endl;

    sample->set_seed( opts.get_option( "seed" ) );
    sample->set_use_sample_weights( opts.get_flag( "use_sample_weights" ) );
    sample->set_age( sampsim::get_age_type( opts.get_option( "age" ) ) );
    sample->set_one_per_household( opts.get_flag( "one_per_household" ) );
    sample->set_sex( sampsim::get_sex_type( opts.get_option( "sex" ) ) );
    sample->set_number_of_samples( samples );
    sample->set_sample_part( part_list.at( 0 ) );
    sample->set_number_of_sample_parts( part_list.at( 1 ) );
    sample->set_number_of_towns( opts.get_option_as_int( "towns" ) );
    sample->set_size( opts.get_option_as_int( "size" ) );
    sample->set_resample_towns( opts.get_flag( "resample_towns" ) );

    if( sample->set_population( population_filename ) )
    {
      sample->generate();

      // create a json file unless a flat file only was requested
      if( !flat_only && !summary_only ) sample->write( output_filename, false );

      // create a summary file if requested
      if( summary )
      {
        sample->write_summary( output_filename );
        sample->write_variance( output_filename );
      }

      // create a flat file if a flat file or plot was requested
      if( !summary_only && ( flat || plot ) ) sample->write( output_filename, true );
        
      /*
      NOTE: plotting is currently disabled

      if( plot )
      {
        sampsim::population *population = sample->get_population();
        unsigned int number_of_towns = population->get_number_of_towns();
        int town_width = floor( log10( number_of_towns ) ) + 1;
        unsigned int number_of_samples = sample->get_number_of_samples();

        std::stringstream stream;
        for( unsigned int s = 1; s <= number_of_samples; s++ )
        {
          // plot the flat file
          unsigned int index = 0;
          for( auto it = population->get_town_list_cbegin();
               it != population->get_town_list_cend();
               ++it, ++index )
          {
            sampsim::town *town = *it;
            std::string command = 1 < number_of_towns
                                ? gnuplot( town, population_filename, index, sample_filename )
                                : gnuplot( town, population_filename, sample_filename );
            std::string result = sampsim::utilities::exec( command );

            // create the image filename and create the plot file
            stream.str( "" );
            stream << sample_filename;
            if( 1 < number_of_towns )
              stream << ".t" << std::setw( town_width ) << std::setfill( '0' ) << ( index + 1 )
                     << ".png";
            std::string image_filename = stream.str();

            stream.str( "" );
            if( "ERROR" == result ) stream << "warning: failed to create plot";
            else stream << "creating plot file \"" << image_filename << "\"";
            sampsim::utilities::output( stream.str() );
          }
        }
      }
      */
    }
  }
}
