/*=========================================================================

  Program:  sampsim
  Module:   generate.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// An executable which generates a population
//

#include "arc_epi_sample.h"
#include "random_sample.h"
#include "strip_epi_sample.h"

#include "population.h"
#include "trend.h"
#include "utilities.h"

#include <stdexcept>

// main function
int main( const int argc, const char** argv )
{
  int status = EXIT_FAILURE;
  sampsim::options opts( argv[0] );
  sampsim::trend *mean_income = new sampsim::trend;
  sampsim::trend *sd_income = new sampsim::trend;
  sampsim::trend *mean_disease = new sampsim::trend;
  sampsim::trend *sd_disease = new sampsim::trend;
  sampsim::trend *popdens = new sampsim::trend;
  sampsim::population *population = new sampsim::population;
  sampsim::sample::arc_epi *arc_epi_sample = new sampsim::sample::arc_epi;
  sampsim::sample::random *random_sample = new sampsim::sample::random;
  sampsim::sample::strip_epi *strip_epi_sample = new sampsim::sample::strip_epi;

  // define inputs
  opts.add_input( "output" );

  // define general parameters
  opts.add_flag( 'f', "flat_file", "Whether to output data in two CSV files instead of JSON data" );
  opts.add_flag( 'v', "verbose", "Be verbose when generating population" );

  // define population parameters
  opts.add_heading( "" );
  opts.add_heading( "Population parameters (overrides config files):" );
  opts.add_heading( "" );
  opts.add_option( "seed", "", "Seed used by the random generator" );
  opts.add_option( "mean_household_pop", "4", "Mean number of individuals per household" );
  opts.add_option( "mean_income_b00", "1", "Mean income trend's independent coefficient" );
  opts.add_option( "mean_income_b01", "0", "Mean income trend's X coefficient" );
  opts.add_option( "mean_income_b10", "0", "Mean income trend's Y coefficient" );
  opts.add_option( "mean_income_b02", "0", "Mean income trend's X^2 coefficient" );
  opts.add_option( "mean_income_b20", "0", "Mean income trend's Y^2 coefficient" );
  opts.add_option( "mean_income_b11", "0", "Mean income trend's XY coefficient" );
  opts.add_option( "sd_income_b00", "1", "SD income trend's independent coefficient" );
  opts.add_option( "sd_income_b01", "0", "SD income trend's X coefficient" );
  opts.add_option( "sd_income_b10", "0", "SD income trend's Y coefficient" );
  opts.add_option( "sd_income_b02", "0", "SD income trend's X^2 coefficient" );
  opts.add_option( "sd_income_b20", "0", "SD income trend's Y^2 coefficient" );
  opts.add_option( "sd_income_b11", "0", "SD income trend's XY coefficient" );
  opts.add_option( "mean_disease_b00", "1", "Mean disease trend's independent coefficient" );
  opts.add_option( "mean_disease_b01", "0", "Mean disease trend's X coefficient" );
  opts.add_option( "mean_disease_b10", "0", "Mean disease trend's Y coefficient" );
  opts.add_option( "mean_disease_b02", "0", "Mean disease trend's X^2 coefficient" );
  opts.add_option( "mean_disease_b20", "0", "Mean disease trend's Y^2 coefficient" );
  opts.add_option( "mean_disease_b11", "0", "Mean disease trend's XY coefficient" );
  opts.add_option( "sd_disease_b00", "1", "SD disease trend's independent coefficient" );
  opts.add_option( "sd_disease_b01", "0", "SD disease trend's X coefficient" );
  opts.add_option( "sd_disease_b10", "0", "SD disease trend's Y coefficient" );
  opts.add_option( "sd_disease_b02", "0", "SD disease trend's X^2 coefficient" );
  opts.add_option( "sd_disease_b20", "0", "SD disease trend's Y^2 coefficient" );
  opts.add_option( "sd_disease_b11", "0", "SD disease trend's XY coefficient" );
  opts.add_option( "popdens_b00", "1", "Population density trend's independent coefficient" );
  opts.add_option( "popdens_b01", "0", "Population density trend's X coefficient" );
  opts.add_option( "popdens_b10", "0", "Population density trend's Y coefficient" );
  opts.add_option( "popdens_b02", "0", "Population density trend's X^2 coefficient" );
  opts.add_option( "popdens_b20", "0", "Population density trend's Y^2 coefficient" );
  opts.add_option( "popdens_b11", "0", "Population density trend's XY coefficient" );
  opts.add_option( "tile_x", "1", "Number of tiles in the horizontal direction" );
  opts.add_option( "tile_y", "1", "Number of tiles in the vertical direction" );
  opts.add_option( "tile_width", "1", "Width of a tile in kilometers" );

  // define disease parameters
  opts.add_heading( "" );
  opts.add_heading( "Disease status weighting parameters (overrides config files):" );
  opts.add_heading( "" );
  opts.add_option( "dweight_population", "1.0", "Disease weight for household population" );
  opts.add_option( "dweight_income", "1.0", "Disease weight for household income" );
  opts.add_option( "dweight_risk", "1.0", "Disease weight for household risk" );
  opts.add_option( "dweight_age", "1.0", "Disease weight for household age" );
  opts.add_option( "dweight_sex", "1.0", "Disease weight for household sex" );

  // define batch parameters
  opts.add_heading( "" );
  opts.add_heading( "Batch operation parameters (overrides config files):" );
  opts.add_heading( "" );
  opts.add_option( "batch_sampler", "", "Which sampler to use when batch processing" );
  opts.add_option( "batch_config", "", "Config file containing options to pass to the batch sampler" );
  opts.add_option( "batch_npop", "1", "Number of populations to generate" );
  opts.add_option( "batch_nsamp", "0", "Number of samples to take of each population" );

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
        std::string filename = opts.get_input( "output" );
        sampsim::utilities::verbose = opts.get_flag( "verbose" );

        // build trends
        mean_income->set_b00( opts.get_option_as_double( "mean_income_b00" ) );
        mean_income->set_b01( opts.get_option_as_double( "mean_income_b01" ) );
        mean_income->set_b10( opts.get_option_as_double( "mean_income_b10" ) );
        mean_income->set_b02( opts.get_option_as_double( "mean_income_b02" ) );
        mean_income->set_b20( opts.get_option_as_double( "mean_income_b20" ) );
        mean_income->set_b11( opts.get_option_as_double( "mean_income_b11" ) );
        sd_income->set_b00( opts.get_option_as_double( "sd_income_b00" ) );
        sd_income->set_b01( opts.get_option_as_double( "sd_income_b01" ) );
        sd_income->set_b10( opts.get_option_as_double( "sd_income_b10" ) );
        sd_income->set_b02( opts.get_option_as_double( "sd_income_b02" ) );
        sd_income->set_b20( opts.get_option_as_double( "sd_income_b20" ) );
        sd_income->set_b11( opts.get_option_as_double( "sd_income_b11" ) );

        mean_disease->set_b00( opts.get_option_as_double( "mean_disease_b00" ) );
        mean_disease->set_b01( opts.get_option_as_double( "mean_disease_b01" ) );
        mean_disease->set_b10( opts.get_option_as_double( "mean_disease_b10" ) );
        mean_disease->set_b02( opts.get_option_as_double( "mean_disease_b02" ) );
        mean_disease->set_b20( opts.get_option_as_double( "mean_disease_b20" ) );
        mean_disease->set_b11( opts.get_option_as_double( "mean_disease_b11" ) );
        sd_disease->set_b00( opts.get_option_as_double( "sd_disease_b00" ) );
        sd_disease->set_b01( opts.get_option_as_double( "sd_disease_b01" ) );
        sd_disease->set_b10( opts.get_option_as_double( "sd_disease_b10" ) );
        sd_disease->set_b02( opts.get_option_as_double( "sd_disease_b02" ) );
        sd_disease->set_b20( opts.get_option_as_double( "sd_disease_b20" ) );
        sd_disease->set_b11( opts.get_option_as_double( "sd_disease_b11" ) );

        // work out the batch job details, if requested
        std::string batch_sampler = opts.get_option( "batch_sampler" );
        std::string batch_config = opts.get_option( "batch_config" );
        int batch_npop = opts.get_option_as_int( "batch_npop" );
        int batch_nsamp = opts.get_option_as_int( "batch_nsamp" );

        if( 0 < batch_sampler.length() && 0 >= batch_nsamp )
        {
          std::cout << "ERROR: Batch sampler provided without specifying how many samples to take."
                    << std::endl
                    << "       Make sure to set batch_nsamp when providing a batch_sampler."
                    << std::endl;
        }
        else if( 0 < batch_config.length() && 0 >= batch_nsamp )
        {
          std::cout << "ERROR: Batch config provided without specifying how many samples to take. "
                    << std::endl
                    << "       Make sure to set batch_nsamp when providing a batch_config."
                    << std::endl;
        }
        else if( 0 < batch_config.length() && 0 == batch_sampler.length() )
        {
          std::cout << "ERROR: Batch config provided without specifying which sampler to use. "
                    << std::endl
                    << "       Make sure to set batch_sampler when providing a batch_config."
                    << std::endl;
        }
        else if( 0 < batch_nsamp && 0 == batch_sampler.length() )
        {
          std::cout << "ERROR: Number of batch samples provided without specifying which sampler to use. "
                    << std::endl
                    << "       Make sure to set batch_sampler when providing batch_nsamp."
                    << std::endl;
        }
        else
        {
          std::cout << "sampsim generate version " << sampsim::utilities::get_version() << std::endl;
          popdens->set_b00( opts.get_option_as_double( "popdens_b00" ) );
          popdens->set_b01( opts.get_option_as_double( "popdens_b01" ) );
          popdens->set_b10( opts.get_option_as_double( "popdens_b10" ) );
          popdens->set_b02( opts.get_option_as_double( "popdens_b02" ) );
          popdens->set_b20( opts.get_option_as_double( "popdens_b20" ) );
          popdens->set_b11( opts.get_option_as_double( "popdens_b11" ) );

          population->set_seed( opts.get_option( "seed" ) );
          population->set_mean_household_population( opts.get_option_as_double( "mean_household_pop" ) );
          population->set_income( mean_income, sd_income );
          population->set_disease( mean_disease, sd_disease );
          population->set_population_density( popdens );
          population->set_number_tiles_x( opts.get_option_as_int( "tile_x" ) );
          population->set_number_tiles_y( opts.get_option_as_int( "tile_y" ) );
          population->set_tile_width( opts.get_option_as_double( "tile_width" ) );
          population->set_disease_weights(
            opts.get_option_as_double( "dweight_population" ),
            opts.get_option_as_double( "dweight_income" ),
            opts.get_option_as_double( "dweight_risk" ),
            opts.get_option_as_double( "dweight_age" ),
            opts.get_option_as_double( "dweight_sex" ) );

          std::string population_filename;
          for( int p = 1; p <= batch_npop; p++ )
          {
            // filename depends on whether we are creating a batch of populations or not
            if( 1 < batch_npop )
            {
              std::stringstream stream;
              stream << filename << "."
                     << std::setw( log( batch_npop ) ) << std::setfill( '0' ) << p;
              population_filename = stream.str();
              std::cout << "generating population " << p << " of " << batch_npop << std::endl;
            }
            else population_filename = filename;

            population->generate();

            // only write the population to a file if we are not sampling
            if( 0 >= batch_nsamp )
            {
              population->write( population_filename, opts.get_flag( "flat_file" ) );
            }
            else
            {
              // determine which sampler to use and set up the options for it
              const char* sampler_argv[3];
              sampler_argv[0] = batch_sampler.c_str();
              sampler_argv[1] = "--config";
              sampsim::options sampler_opts( argv[0] );
              sampler_opts.add_option( "seed", "", "Seed used by the random generator" );
              sampsim::sample::sample *sample;
              if( "arc_epi" == batch_sampler || "arc_epi_sample" == batch_sampler )
              {
                setup_arc_epi_sample( sampler_opts );
                if( 0 < batch_config.length() )
                {
                  sampler_argv[2] = batch_config.c_str();
                  sampler_opts.set_arguments( 3, sampler_argv );
                }
                if( !sampler_opts.process() ) throw std::runtime_error( "Error while setting up sampler" );
                parse_arc_epi_sample( sampler_opts, arc_epi_sample );
                arc_epi_sample->set_population( population );
                sample = arc_epi_sample;
              }
              else if( "random" == batch_sampler || "random_sample" == batch_sampler )
              {
                setup_random_sample( sampler_opts );
                parse_random_sample( sampler_opts, random_sample );
                random_sample->set_population( population );
                sample = random_sample;
              }
              else if( "strip_epi" == batch_sampler || "strip_epi_sample" == batch_sampler )
              {
                setup_strip_epi_sample( sampler_opts );
                parse_strip_epi_sample( sampler_opts, strip_epi_sample );
                strip_epi_sample->set_population( population );
                sample = strip_epi_sample;
              }
              else
              {
                std::stringstream stream;
                stream << "Unknown sampler \"" << batch_sampler << "\", must be the same as one of "
                       << "the executables ending in _sample";
                throw std::runtime_error( stream.str() );
              }

              std::string sample_filename;
              for( int s = 1; s <= batch_nsamp; s++ )
              {
                // filename depends on whether we are creating a batch of samples or not
                if( 1 < batch_nsamp )
                {
                  std::stringstream stream;
                  stream << population_filename << "."
                         << std::setw( log( batch_nsamp ) ) << std::setfill( '0' ) << s;
                  sample_filename = stream.str();
                  std::cout << "sampling iteration " << s << " of " << batch_nsamp << std::endl;
                }
                else sample_filename = population_filename;

                sample->generate();
                sample->write( sample_filename, opts.get_flag( "flat_file" ) );
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

  sampsim::utilities::safe_delete( mean_income );
  sampsim::utilities::safe_delete( sd_income );
  sampsim::utilities::safe_delete( mean_disease );
  sampsim::utilities::safe_delete( sd_disease );
  sampsim::utilities::safe_delete( popdens );
  sampsim::utilities::safe_delete( population );
  sampsim::utilities::safe_delete( arc_epi_sample );
  sampsim::utilities::safe_delete( random_sample );
  sampsim::utilities::safe_delete( strip_epi_sample );
  return status;
}
