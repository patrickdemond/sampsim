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
#include "population.h"
#include "trend.h"
#include "utilities.h"

#include <cstdlib>
#include <ctime>
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
clock_t sampsim::utilities::start_time = clock();

// main function
int main( int argc, char** argv )
{
  int status = EXIT_FAILURE;
  AnyOption *opt = NULL;
  std::string filename;

  double mean_household_pop = 4;
  sampsim::trend *mean_income = new trend;
  sampsim::trend *sd_income = new trend;
  sampsim::trend *popdens = new trend;
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
    opt->addUsage( " -c  --config           A configuration file containing population parameters" );
    opt->addUsage( " -h  --help             Prints this help" );
    opt->addUsage( " -v  --verbose          Be verbose when generating population" );
    opt->addUsage( " -f  --flat_file        Whether to output data in two CSV files instead of JSON data" );
    opt->addUsage( "" );
    opt->addUsage( "Population parameters (overrides config files):" );
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
    opt->addUsage( " --popdens_b00          The population density trend's independent coefficient" );
    opt->addUsage( " --popdens_b01          The population density trend's X coefficient" );
    opt->addUsage( " --popdens_b10          The population density trend's Y coefficient" );
    opt->addUsage( " --popdens_b02          The population density trend's X^2 coefficient" );
    opt->addUsage( " --popdens_b20          The population density trend's Y^2 coefficient" );
    opt->addUsage( " --popdens_b11          The population density trend's XY coefficient" );
    opt->addUsage( " --tile_x               The number of tiles in the horizontal direction" );
    opt->addUsage( " --tile_y               The number of tiles in the vertical direction" );
    opt->addUsage( " --tile_width           The width of a tile in kilometers" );

    // runtime arguments
    opt->setCommandFlag( "help", 'h' );
    opt->setCommandFlag( "verbose", 'v' );
    opt->setCommandFlag( "flat_file", 'f' );
    opt->setOption( "config", 'c' );

    // population parameters
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
    opt->setOption( "popdens_b00" );
    opt->setOption( "popdens_b01" );
    opt->setOption( "popdens_b10" );
    opt->setOption( "popdens_b02" );
    opt->setOption( "popdens_b20" );
    opt->setOption( "popdens_b11" );
    opt->setOption( "tile_x" );
    opt->setOption( "tile_y" );
    opt->setOption( "tile_width" );

    opt->useCommandArgs( argc, argv );
    opt->processCommandArgs();

    bool show_help = false;
    bool flat_file = false;

    // make sure there is a file argument
    if( 1 != opt->getArgc() ) show_help = true;
    else filename = opt->getArgv( 0 );

    // runtime arguments
    if( opt->getFlag( "help" ) || opt->getFlag( 'h' ) ) show_help = true;
    if( opt->getFlag( "verbose" ) || opt->getFlag( 'v' ) ) sampsim::utilities::verbose = true;
    if( opt->getFlag( "flat_file" ) || opt->getFlag( 'c' ) ) flat_file = true;

    // load config file population parameters
    if( opt->getValue( "config" ) || opt->getValue( 'c' ) )
      opt->useFileName( opt->getValue( "config" ) );

    // process the file, then command arguments
    opt->processFile();
    opt->processCommandArgs();

    // process population parameters
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

    if( opt->getValue( "popdens_b00" ) )
      popdens->set_b00( atof( opt->getValue( "popdens_b00" ) ) );
    if( opt->getValue( "popdens_b01" ) )
      popdens->set_b01( atof( opt->getValue( "popdens_b01" ) ) );
    if( opt->getValue( "popdens_b10" ) )
      popdens->set_b10( atof( opt->getValue( "popdens_b10" ) ) );
    if( opt->getValue( "popdens_b02" ) )
      popdens->set_b02( atof( opt->getValue( "popdens_b02" ) ) );
    if( opt->getValue( "popdens_b20" ) )
      popdens->set_b20( atof( opt->getValue( "popdens_b20" ) ) );
    if( opt->getValue( "popdens_b11" ) )
      popdens->set_b11( atof( opt->getValue( "popdens_b11" ) ) );

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
      sampsim::population *pop = new population;
      pop->set_mean_household_population( mean_household_pop );
      pop->set_income( mean_income, sd_income );
      pop->set_population_density( popdens );
      pop->set_number_tiles_x( tile_x );
      pop->set_number_tiles_y( tile_y );
      pop->set_tile_width( tile_width );
      pop->generate();
      pop->write( filename, flat_file );
      sampsim::utilities::safe_delete( pop );
    }

    status = EXIT_SUCCESS;
  }
  catch( std::exception &e )
  {
    std::cerr << "Uncaught exception: " << e.what() << std::endl;
  }

  sampsim::utilities::safe_delete( mean_income );
  sampsim::utilities::safe_delete( sd_income );
  sampsim::utilities::safe_delete( popdens );
  sampsim::utilities::safe_delete( opt );
  return status;
}
