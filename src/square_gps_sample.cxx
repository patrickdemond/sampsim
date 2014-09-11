/*=========================================================================

  Program:  sampsim
  Module:   square_gps_sample.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// An executable which square_gpsly samples a population
//

#include "square_gps_sample.h"

#include <stdexcept>

// main function
int main( const int argc, const char** argv )
{
  int status = EXIT_FAILURE;
  sampsim::options opts( argv[0] );
  sampsim::sample::square_gps *sample = new sampsim::sample::square_gps;

  // define inputs
  opts.add_input( "population_file" );
  opts.add_input( "output_file" );
  opts.add_flag( 'f', "flat_file", "Whether to output data in two CSV files instead of JSON data" );
  if( GNUPLOT_AVAILABLE )
    opts.add_flag( 'p', "plot", "Whether to create a plot of the sample (will create a flat-file)" );
  opts.add_flag( 'v', "verbose", "Be verbose when generating sample" );
  setup_square_gps_sample( opts );
  opts.add_option( "seed", "", "Seed used by the random generator" );

  try
  {
    // parse the command line arguments
    opts.set_arguments( argc, argv );
    if( opts.process() )
    {
      // now either show the help or run the application
      if( opts.get_flag( "help" ) )
      {
        opts.print_usage();
      }
      else
      {
        std::cout << "sampsim square_gps_sample version " << sampsim::utilities::get_version() << std::endl;
        std::string population_filename = opts.get_input( "population_file" );
        std::string sample_filename = opts.get_input( "output_file" );
        sampsim::utilities::verbose = opts.get_flag( "verbose" );
        sample->set_seed( opts.get_option( "seed" ) );
        parse_square_gps_sample( opts, sample );

        if( sample->set_population( opts.get_input( "population_file" ) ) )
        {
          bool flat = opts.get_flag( "flat_file" );
          bool plot = opts.get_flag( "plot" );

          sample->generate();

          // create a json file no flat file was requested
          if( !flat ) sample->write( sample_filename, false );

          // create a flat file if a flat file or plot was requested
          if( flat || plot ) sample->write( sample_filename, true );

          // plot the flat file if requested to
          if( plot )
          {
            std::string population_name =
              population_filename.substr( 0, population_filename.find_last_of( '.' ) );
            std::stringstream stream;
            stream
               << "gnuplot -p -e "
               <<   "'set title \"Household plot of \\\"" << sample_filename << "\\\"\" "
               <<      "font \"sans, 18\"; "
               <<    "set xlabel \"Position (km)\"; "
               <<    "set ylabel \"Position (km)\"; "
               <<    "set datafile separator \",\"; "
               <<    "set key at 35.5, 36.5; "
               <<    "unset colorbox; "
               <<    "set palette model RGB defined ( 0 0 0 1, 1 1 0 0 ); "
               <<    "set term pngcairo size 1200,1230; "
               <<    "set output \"" << sample_filename << ".png\"; "
               <<    "plot \"" << population_name << ".household.csv\" using 3:4:($8/2):10 "
               <<      "palette pt 6 ps variable title \"household\", "
               <<    "\"" << sample_filename << ".household.csv\" using 3:4 "
               <<      "lc rgb \"black\" pt 3 ps 1.5 title \"sampled\"'";

            std::string result = sampsim::utilities::exec( stream.str() );

            stream.str( "" );
            if( "ERROR" == result )
              stream << "warning: failed to create plot";
            else
              stream << "creating plot file \"" << sample_filename << ".png\"";
            sampsim::utilities::output( stream.str() );
          }
        }
      }

      status = EXIT_SUCCESS;
    }
  }
  catch( std::exception &e )
  {
    std::cerr << "Uncaught exception: " << e.what() << std::endl;
  }

  sampsim::utilities::safe_delete( sample );
  return status;
}
