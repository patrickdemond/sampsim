/*=========================================================================

  Program:  sampsim
  Module:   generate.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// An executable which generates a population
//

#include "options.h"
#include "population.h"
#include "trend.h"
#include "utilities.h"

// main function
int main( int argc, char** argv )
{
  int status = EXIT_FAILURE;
  sampsim::options opts( argv[0] );
  sampsim::trend *mean_income = new sampsim::trend;
  sampsim::trend *sd_income = new sampsim::trend;
  sampsim::trend *mean_disease = new sampsim::trend;
  sampsim::trend *sd_disease = new sampsim::trend;
  sampsim::trend *popdens = new sampsim::trend;
  sampsim::population *population = new sampsim::population;

  // define inputs
  opts.add_input( "output_file" );

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
        std::string filename = opts.get_input( "output_file" );
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

        popdens->set_b00( opts.get_option_as_double( "popdens_b00" ) );
        popdens->set_b01( opts.get_option_as_double( "popdens_b01" ) );
        popdens->set_b10( opts.get_option_as_double( "popdens_b10" ) );
        popdens->set_b02( opts.get_option_as_double( "popdens_b02" ) );
        popdens->set_b20( opts.get_option_as_double( "popdens_b20" ) );
        popdens->set_b11( opts.get_option_as_double( "popdens_b11" ) );

        // launch application
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
        population->generate();
        population->write( filename, opts.get_flag( "flat_file" ) );
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
  return status;
}
