/*=========================================================================

  Program:  sampsim
  Module:   random.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// An executable which randomly samples a population
//

#include "AnyOption/anyoption.h"
#include "utilities.h"

#include <stdexcept>

// main function
int main( int argc, char** argv )
{
  int status = EXIT_FAILURE;
  AnyOption *opt = NULL;
  std::string filename;

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
    opt->addUsage( "Sampling parameters (overrides config files):" );
    opt->addUsage( "" );
    opt->addUsage( " --seed                 Seed used by the random generator" );
    opt->addUsage( " --arc_width            Width of the arc used to find buildings (in radians)" );

    // runtime arguments
    opt->setCommandFlag( "help", 'h' );
    opt->setCommandFlag( "verbose", 'v' );
    opt->setCommandFlag( "flat_file", 'f' );
    opt->setOption( "config", 'c' );

    // population parameters
    opt->setOption( "seed" );
    opt->setOption( "arc_width" );

    opt->useCommandArgs( argc, argv );
    opt->processCommandArgs();

    bool show_help = false;
    bool flat_file = false;
    std::string seed = "";
    double arc_width = 0;

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
    if( opt->getValue( "arc_width" ) ) arc_width = atof( opt->getValue( "arc_width" ) );

    // now either show the help or run the application
    if( show_help )
    {
      opt->printUsage();
    }
    else
    {
      // launch application
    }

    status = EXIT_SUCCESS;
  }
  catch( std::exception &e )
  {
    std::cerr << "Uncaught exception: " << e.what() << std::endl;
  }

  sampsim::utilities::safe_delete( opt );
  return status;
}
