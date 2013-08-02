/*=========================================================================

  Program:  sampsim
  Module:   generate.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// An executable which generates a population
//

#include "AnyOption/anyoption.h"
#include "population.h"
#include "trend.h"
#include "utilities.h"

// main function
int main( int argc, char** argv )
{
  int status = EXIT_FAILURE;
  AnyOption *opt = NULL;
  std::string filename;

  double mean_household_pop = 4;
  sampsim::trend *mean_income = new sampsim::trend;
  sampsim::trend *sd_income = new sampsim::trend;
  sampsim::trend *mean_disease = new sampsim::trend;
  sampsim::trend *sd_disease = new sampsim::trend;
  sampsim::trend *popdens = new sampsim::trend;
  int tile_x = 10, tile_y = 10;
  double dweight_population = 1.0,
         dweight_income = 1.0,
         dweight_risk = 1.0,
         dweight_age = 1.0,
         dweight_sex = 1.0;
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
    opt->addUsage( " --seed                 Seed used by the random generator" );
    opt->addUsage( " --mean_household_pop   Mean number of individuals per household" );
    opt->addUsage( " --mean_income_b00      Mean income trend's independent coefficient" );
    opt->addUsage( " --mean_income_b01      Mean income trend's X coefficient" );
    opt->addUsage( " --mean_income_b10      Mean income trend's Y coefficient" );
    opt->addUsage( " --mean_income_b02      Mean income trend's X^2 coefficient" );
    opt->addUsage( " --mean_income_b20      Mean income trend's Y^2 coefficient" );
    opt->addUsage( " --mean_income_b11      Mean income trend's XY coefficient" );
    opt->addUsage( " --sd_income_b00        SD income trend's independent coefficient" );
    opt->addUsage( " --sd_income_b01        SD income trend's X coefficient" );
    opt->addUsage( " --sd_income_b10        SD income trend's Y coefficient" );
    opt->addUsage( " --sd_income_b02        SD income trend's X^2 coefficient" );
    opt->addUsage( " --sd_income_b20        SD income trend's Y^2 coefficient" );
    opt->addUsage( " --sd_income_b11        SD income trend's XY coefficient" );
    opt->addUsage( " --mean_disease_b00     Mean disease trend's independent coefficient" );
    opt->addUsage( " --mean_disease_b01     Mean disease trend's X coefficient" );
    opt->addUsage( " --mean_disease_b10     Mean disease trend's Y coefficient" );
    opt->addUsage( " --mean_disease_b02     Mean disease trend's X^2 coefficient" );
    opt->addUsage( " --mean_disease_b20     Mean disease trend's Y^2 coefficient" );
    opt->addUsage( " --mean_disease_b11     Mean disease trend's XY coefficient" );
    opt->addUsage( " --sd_disease_b00       SD disease trend's independent coefficient" );
    opt->addUsage( " --sd_disease_b01       SD disease trend's X coefficient" );
    opt->addUsage( " --sd_disease_b10       SD disease trend's Y coefficient" );
    opt->addUsage( " --sd_disease_b02       SD disease trend's X^2 coefficient" );
    opt->addUsage( " --sd_disease_b20       SD disease trend's Y^2 coefficient" );
    opt->addUsage( " --sd_disease_b11       SD disease trend's XY coefficient" );
    opt->addUsage( " --popdens_b00          Population density trend's independent coefficient" );
    opt->addUsage( " --popdens_b01          Population density trend's X coefficient" );
    opt->addUsage( " --popdens_b10          Population density trend's Y coefficient" );
    opt->addUsage( " --popdens_b02          Population density trend's X^2 coefficient" );
    opt->addUsage( " --popdens_b20          Population density trend's Y^2 coefficient" );
    opt->addUsage( " --popdens_b11          Population density trend's XY coefficient" );
    opt->addUsage( " --tile_x               Number of tiles in the horizontal direction" );
    opt->addUsage( " --tile_y               Number of tiles in the vertical direction" );
    opt->addUsage( " --tile_width           Width of a tile in kilometers" );
    opt->addUsage( "" );
    opt->addUsage( "Disease status weighting parameters (overrides config files):" );
    opt->addUsage( "" );
    opt->addUsage( " --dweight_population   Disease weight for household population" );
    opt->addUsage( " --dweight_income       Disease weight for household income" );
    opt->addUsage( " --dweight_risk         Disease weight for household risk" );
    opt->addUsage( " --dweight_age          Disease weight for household age" );
    opt->addUsage( " --dweight_sex          Disease weight for household sex" );

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
    opt->setOption( "mean_disease_b00" );
    opt->setOption( "mean_disease_b01" );
    opt->setOption( "mean_disease_b10" );
    opt->setOption( "mean_disease_b02" );
    opt->setOption( "mean_disease_b20" );
    opt->setOption( "mean_disease_b11" );
    opt->setOption( "sd_disease_b00" );
    opt->setOption( "sd_disease_b01" );
    opt->setOption( "sd_disease_b10" );
    opt->setOption( "sd_disease_b02" );
    opt->setOption( "sd_disease_b20" );
    opt->setOption( "sd_disease_b11" );
    opt->setOption( "popdens_b00" );
    opt->setOption( "popdens_b01" );
    opt->setOption( "popdens_b10" );
    opt->setOption( "popdens_b02" );
    opt->setOption( "popdens_b20" );
    opt->setOption( "popdens_b11" );
    opt->setOption( "tile_x" );
    opt->setOption( "tile_y" );
    opt->setOption( "tile_width" );
    opt->setOption( "dweight_population" );
    opt->setOption( "dweight_income" );
    opt->setOption( "dweight_risk" );
    opt->setOption( "dweight_age" );
    opt->setOption( "dweight_sex" );

    opt->useCommandArgs( argc, argv );
    opt->processCommandArgs();

    bool show_help = false;
    bool flat_file = false;
    std::string seed = "";

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
    if( opt->getValue( "seed" ) ) seed = opt->getValue( "seed" );

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

    if( opt->getValue( "mean_disease_b00" ) )
      mean_disease->set_b00( atof( opt->getValue( "mean_disease_b00" ) ) );
    if( opt->getValue( "mean_disease_b01" ) )
      mean_disease->set_b01( atof( opt->getValue( "mean_disease_b01" ) ) );
    if( opt->getValue( "mean_disease_b10" ) )
      mean_disease->set_b10( atof( opt->getValue( "mean_disease_b10" ) ) );
    if( opt->getValue( "mean_disease_b02" ) )
      mean_disease->set_b02( atof( opt->getValue( "mean_disease_b02" ) ) );
    if( opt->getValue( "mean_disease_b20" ) )
      mean_disease->set_b20( atof( opt->getValue( "mean_disease_b20" ) ) );
    if( opt->getValue( "mean_disease_b11" ) )
      mean_disease->set_b11( atof( opt->getValue( "mean_disease_b11" ) ) );
    if( opt->getValue( "sd_disease_b00" ) )
      sd_disease->set_b00( atof( opt->getValue( "sd_disease_b00" ) ) );
    if( opt->getValue( "sd_disease_b01" ) )
      sd_disease->set_b01( atof( opt->getValue( "sd_disease_b01" ) ) );
    if( opt->getValue( "sd_disease_b10" ) )
      sd_disease->set_b10( atof( opt->getValue( "sd_disease_b10" ) ) );
    if( opt->getValue( "sd_disease_b02" ) )
      sd_disease->set_b02( atof( opt->getValue( "sd_disease_b02" ) ) );
    if( opt->getValue( "sd_disease_b20" ) )
      sd_disease->set_b20( atof( opt->getValue( "sd_disease_b20" ) ) );
    if( opt->getValue( "sd_disease_b11" ) )
      sd_disease->set_b11( atof( opt->getValue( "sd_disease_b11" ) ) );

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

    if( opt->getValue( "dweight_population" ) )
      dweight_population = atof( opt->getValue( "dweight_population" ) );
    if( opt->getValue( "dweight_income" ) )
      dweight_income = atof( opt->getValue( "dweight_income" ) );
    if( opt->getValue( "dweight_risk" ) )
      dweight_risk = atof( opt->getValue( "dweight_risk" ) );
    if( opt->getValue( "dweight_age" ) )
      dweight_age = atof( opt->getValue( "dweight_age" ) );
    if( opt->getValue( "dweight_sex" ) )
      dweight_sex = atof( opt->getValue( "dweight_sex" ) );

    // now either show the help or run the application
    if( show_help )
    {
      opt->printUsage();
    }
    else
    {
      // launch application
      sampsim::population *pop = new sampsim::population;
      pop->set_seed( seed );
      pop->set_mean_household_population( mean_household_pop );
      pop->set_income( mean_income, sd_income );
      pop->set_disease( mean_disease, sd_disease );
      pop->set_population_density( popdens );
      pop->set_number_tiles_x( tile_x );
      pop->set_number_tiles_y( tile_y );
      pop->set_tile_width( tile_width );
      pop->set_disease_weights(
        dweight_population,
        dweight_income,
        dweight_risk,
        dweight_age,
        dweight_sex );
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
  sampsim::utilities::safe_delete( mean_disease );
  sampsim::utilities::safe_delete( sd_disease );
  sampsim::utilities::safe_delete( popdens );
  sampsim::utilities::safe_delete( opt );
  return status;
}
