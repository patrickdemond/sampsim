/*=========================================================================

  Program:  sampsim
  Module:   arc_epi_sample.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// An executable which samples a population using the arc EPI method
//

#include "arc_epi_sample.h"

#include "common.h"

#include <stdexcept>

// main function
int main( const int argc, const char** argv )
{
  int status = EXIT_FAILURE;
  sampsim::options opts( argv[0] );
  sampsim::sample::arc_epi *sample = new sampsim::sample::arc_epi;

  // define inputs
  opts.add_input( "population_file" );
  opts.add_input( "output_file" );
  opts.add_flag( 'f', "flat_file", "Whether to output data in two CSV files instead of JSON data" );
  opts.add_flag( 's', "summary_file", "Whether to output summary data of the sample" );
  if( GNUPLOT_AVAILABLE )
    opts.add_flag( 'p', "plot", "Whether to create a plot of the sample (will create a flat-file)" );
  opts.add_flag( 'v', "verbose", "Be verbose when generating sample" );
  setup_arc_epi_sample( opts );
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
        std::cout << "sampsim arc_epi_sample version " << sampsim::utilities::get_version() << std::endl;
        std::string population_filename = opts.get_input( "population_file" );
        std::string sample_filename = opts.get_input( "output_file" );
        sampsim::utilities::verbose = opts.get_flag( "verbose" );
        sample->set_seed( opts.get_option( "seed" ) );
        parse_arc_epi_sample( opts, sample );

        if( sample->set_population( population_filename ) )
        {
          bool flat = opts.get_flag( "flat_file" );
          bool summary = opts.get_flag( "summary_file" );
          bool plot = GNUPLOT_AVAILABLE ? opts.get_flag( "plot" ) : false;

          sample->generate();

          // create a json file no flat file was requested
          if( !flat ) sample->write( sample_filename, false );

          // create a flat file if a flat file or plot was requested
          if( flat || plot ) sample->write( sample_filename, true );

          // create a summary file if requested
          if( summary ) sample->write_summary( sample_filename );

          // plot the flat file if requested to
          if( plot )
          {
            sampsim::population *population = sample->get_population();

            if( 1 == population->get_number_of_towns() )
            {
              std::string result = sampsim::utilities::exec(
                gnuplot( *( population->get_town_list_cbegin() ), population_filename, sample_filename ) );
              if( "ERROR" == result ) sampsim::utilities::output( "warning: failed to create plot" );
              else sampsim::utilities::output( "creating plot file" );
            }
            else
            {
              std::stringstream stream;
              unsigned int index = 0;
              for( auto it = population->get_town_list_cbegin();
                   it != population->get_town_list_cend();
                   ++it, ++index )
              {
                sampsim::town *town = *it;
                std::string result = sampsim::utilities::exec(
                  gnuplot( town, population_filename, index, sample_filename ) );

                stream.str( "" );
                stream << sample_filename << ".t"
                       << std::setw( log( population->get_number_of_towns()+1 ) )
                       << std::setfill( '0' ) << index << ".png";
                std::string image_filename = stream.str();

                stream.str( "" );
                if( "ERROR" == result ) stream << "warning: failed to create plot";
                else stream << "creating plot file \"" << image_filename << "\"";
                sampsim::utilities::output( stream.str() );
              }
            }
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
