/*=========================================================================

  Program:  sampsim
  Module:   sampsim.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// The main function which launches the application.
//

#include "AnyOption/anyoption.h"
#include "simulation.h"
#include "trend.h"
#include "utilities.h"

#include <cstdlib>
#include <map>
#include <random>
#include <stdexcept>
#include <string>

using namespace sampsim;

// instantiate static variables in utilities class
std::mt19937 sampsim::utilities::random_engine;
sampsim::utilities::safe_delete_type sampsim::utilities::safe_delete;
bool sampsim::utilities::verbose = false;
int sampsim::utilities::household_index = 0;

// main function
int main( int argc, char** argv )
{
  int status = EXIT_FAILURE;
  AnyOption *opt = NULL;
  std::string filename;

  double mean_household_pop = 4;
  sampsim::trend *mean_income = new trend;
  sampsim::trend *sd_income = new trend;
  sampsim::trend *population = new trend;
  int tile_x = 10, tile_y = 10;
  double tile_width = 3.5;

  try
  {
    // parse the command line arguments
    std::string usage = "Usage: ";
    usage += argv[0];
    usage += " [options...] <file>";
    opt = new AnyOption();
    opt->addUsage( usage.c_str() );
    opt->addUsage( "" );
    opt->addUsage( " -c  --config           A configuration file containing simulation parameters" );
    opt->addUsage( " -h  --help             Prints this help" );
    opt->addUsage( " -v  --verbose          Be verbose when generating simulation" );
    opt->addUsage( " -f  --flat_file        Whether to output data in two CSV files instead of JSON data" );
    opt->addUsage( "" );
    opt->addUsage( "Simulation parameters (overrides config files):" );
    opt->addUsage( "" );
    opt->addUsage( " --seed                 The seed used by the random generator" );
    opt->addUsage( " --mean_household_pop   The mean number of individuals per household" );
    opt->addUsage( " --mean_income_b00      The mean income trend's independent coefficient" );
    opt->addUsage( " --mean_income_b01      The mean income trend's X coefficient" );
    opt->addUsage( " --mean_income_b10      The mean income trend's Y coefficient" );
    opt->addUsage( " --mean_income_b02      The mean income trend's X^2 coefficient" );
    opt->addUsage( " --mean_income_b20      The mean income trend's Y^2 coefficient" );
    opt->addUsage( " --mean_income_b11      The mean income trend's XY coefficient" );
    opt->addUsage( " --sd_income_b00        The SD income trend's independent coefficient" );
    opt->addUsage( " --sd_income_b01        The SD income trend's X coefficient" );
    opt->addUsage( " --sd_income_b10        The SD income trend's Y coefficient" );
    opt->addUsage( " --sd_income_b02        The SD income trend's X^2 coefficient" );
    opt->addUsage( " --sd_income_b20        The SD income trend's Y^2 coefficient" );
    opt->addUsage( " --sd_income_b11        The SD income trend's XY coefficient" );
    opt->addUsage( " --population_b00       The population density trend's independent coefficient" );
    opt->addUsage( " --population_b01       The population density trend's X coefficient" );
    opt->addUsage( " --population_b10       The population density trend's Y coefficient" );
    opt->addUsage( " --population_b02       The population density trend's X^2 coefficient" );
    opt->addUsage( " --population_b20       The population density trend's Y^2 coefficient" );
    opt->addUsage( " --population_b11       The population density trend's XY coefficient" );
    opt->addUsage( " --tile_x               The number of tiles in the horizontal direction" );
    opt->addUsage( " --tile_y               The number of tiles in the vertical direction" );
    opt->addUsage( " --tile_width           The width of a tile in kilometers" );

    // runtime arguments
    opt->setOption( "config", 'c' );
    opt->setFlag( "help", 'h' );
    opt->setFlag( "verbose", 'v' );
    opt->setFlag( "flat_file", 'f' );

    // simulation parameters
    opt->setOption( "seed" );
    opt->setOption( "mean_household_pop" );
    opt->setOption( "mean_income_b00" );
    opt->setOption( "mean_income_b01" );
    opt->setOption( "mean_income_b10" );
    opt->setOption( "mean_income_b02" );
    opt->setOption( "mean_income_b20" );
    opt->setOption( "mean_income_b11" );
    opt->setOption( "sd_income_b00" );
    opt->setOption( "sd_income_b01" );
    opt->setOption( "sd_income_b10" );
    opt->setOption( "sd_income_b02" );
    opt->setOption( "sd_income_b20" );
    opt->setOption( "sd_income_b11" );
    opt->setOption( "population_b00" );
    opt->setOption( "population_b01" );
    opt->setOption( "population_b10" );
    opt->setOption( "population_b02" );
    opt->setOption( "population_b20" );
    opt->setOption( "population_b11" );
    opt->setOption( "tile_x" );
    opt->setOption( "tile_y" );
    opt->setOption( "tile_width" );

    opt->processCommandArgs( argc, argv );

    bool show_help = false;
    bool flat_file = false;

    // make sure there is a file argument
    if( 1 != opt->getArgc() ) show_help = true;
    else filename = opt->getArgv( 0 );

    // runtime arguments
    if( opt->getFlag( "help" ) || opt->getFlag( 'h' ) ) show_help = true;
    if( opt->getFlag( "verbose" ) || opt->getFlag( 'v' ) ) sampsim::utilities::verbose = true;
    if( opt->getFlag( "flat_file" ) || opt->getFlag( 'c' ) ) flat_file = true;

    // load config file simulation parameters
    if( opt->getValue( "config" ) || opt->getValue( 'c' ) )
      opt->processFile( opt->getValue( "config" ) );

    // to override the config file parameters we need to re-process the command line arguments
    opt->processCommandArgs( argc, argv );

    // process simulation parameters
    if( opt->getValue( "seed" ) )
      sampsim::utilities::random_engine.seed( atoi( opt->getValue( "seed" ) ) );

    if( opt->getValue( "mean_household_pop" ) )
      mean_household_pop = atof( opt->getValue( "mean_household_pop" ) );

    if( opt->getValue( "mean_income_b00" ) )
      mean_income->set_b00( atof( opt->getValue( "mean_income_b00" ) ) );
    if( opt->getValue( "mean_income_b01" ) )
      mean_income->set_b01( atof( opt->getValue( "mean_income_b01" ) ) );
    if( opt->getValue( "mean_income_b10" ) )
      mean_income->set_b10( atof( opt->getValue( "mean_income_b10" ) ) );
    if( opt->getValue( "mean_income_b02" ) )
      mean_income->set_b02( atof( opt->getValue( "mean_income_b02" ) ) );
    if( opt->getValue( "mean_income_b20" ) )
      mean_income->set_b20( atof( opt->getValue( "mean_income_b20" ) ) );
    if( opt->getValue( "mean_income_b11" ) )
      mean_income->set_b11( atof( opt->getValue( "mean_income_b11" ) ) );
    if( opt->getValue( "sd_income_b00" ) )
      sd_income->set_b00( atof( opt->getValue( "sd_income_b00" ) ) );
    if( opt->getValue( "sd_income_b01" ) )
      sd_income->set_b01( atof( opt->getValue( "sd_income_b01" ) ) );
    if( opt->getValue( "sd_income_b10" ) )
      sd_income->set_b10( atof( opt->getValue( "sd_income_b10" ) ) );
    if( opt->getValue( "sd_income_b02" ) )
      sd_income->set_b02( atof( opt->getValue( "sd_income_b02" ) ) );
    if( opt->getValue( "sd_income_b20" ) )
      sd_income->set_b20( atof( opt->getValue( "sd_income_b20" ) ) );
    if( opt->getValue( "sd_income_b11" ) )
      sd_income->set_b11( atof( opt->getValue( "sd_income_b11" ) ) );

    if( opt->getValue( "population_b00" ) )
      population->set_b00( atof( opt->getValue( "population_b00" ) ) );
    if( opt->getValue( "population_b01" ) )
      population->set_b01( atof( opt->getValue( "population_b01" ) ) );
    if( opt->getValue( "population_b10" ) )
      population->set_b10( atof( opt->getValue( "population_b10" ) ) );
    if( opt->getValue( "population_b02" ) )
      population->set_b02( atof( opt->getValue( "population_b02" ) ) );
    if( opt->getValue( "population_b20" ) )
      population->set_b20( atof( opt->getValue( "population_b20" ) ) );
    if( opt->getValue( "population_b11" ) )
      population->set_b11( atof( opt->getValue( "population_b11" ) ) );

    if( opt->getValue( "tile_x" ) )
      tile_x = atoi( opt->getValue( "tile_x" ) );
    if( opt->getValue( "tile_y" ) )
      tile_y = atoi( opt->getValue( "tile_y" ) );
    if( opt->getValue( "tile_width" ) )
      tile_width = atof( opt->getValue( "tile_width" ) );

    // now either show the help or run the application
    if( show_help )
    {
      opt->printUsage();
    }
    else
    {
      // launch application
      sampsim::simulation *sim = new simulation;
      sim->set_mean_household_population( mean_household_pop );
      sim->set_income( mean_income, sd_income );
      sim->set_population_density( population );
      sim->set_number_tiles_x( tile_x );
      sim->set_number_tiles_y( tile_y );
      sim->set_tile_width( tile_width );
      sim->generate();
      sim->write( filename, flat_file );
      sampsim::utilities::safe_delete( sim );
    }

    status = EXIT_SUCCESS;
  }
  catch( std::exception &e )
  {
    std::cerr << "Uncaught exception: " << e.what() << std::endl;
  }

  sampsim::utilities::safe_delete( mean_income );
  sampsim::utilities::safe_delete( sd_income );
  sampsim::utilities::safe_delete( population );
  sampsim::utilities::safe_delete( opt );
  return status;
}
