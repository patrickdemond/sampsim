/*=========================================================================

  Program:  sampsim
  Module:   population.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_population_h
#define __sampsim_population_h

#include "model_object.h"

#include "coordinate.h"
#include "distribution.h"
#include "utilities.h"

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class tile;
  class trend;

  /**
   * @class population
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief A population which contains X by Y tiles
   * @details
   * Populations are organized into a tree such that all nodes are responsible for creating,
   * generating and deleting their children.  The structure is as follows:
   * - population
   *   + list of n by m tiles
   *     - list of buildings in tile
   *       + list of households in building
   *         - list of individuals belonging to household
   * 
   * Generating a population will cause a cascade effect which will generate all tiles, all
   * buildings, all households and all individuals.
   */
  class population : public model_object
  {
  public:
    /**
     * Constructor
     */
    population();

    /**
     * Destructor
     */
    ~population();

    /**
     * Iterator access to child tiles
     * 
     * These methods provide iterator access to the list of tiles belonging to the population.
     */
    tile_list_type::iterator get_tile_list_begin()
    { return this->tile_list.begin(); }

    /**
     * Iterator access to child tiles
     * 
     * These methods provide iterator access to the list of tiles belonging to the population.
     */
    tile_list_type::iterator get_tile_list_end()
    { return this->tile_list.end(); }

    /**
     * Constant iterator access to child tiles
     * 
     * These methods provide constant iterator access to the list of tiles belonging to the population.
     */
    tile_list_type::const_iterator get_tile_list_cbegin() const
    { return this->tile_list.cbegin(); }

    /**
     * Constant iterator access to child tiles
     * 
     * These methods provide constant iterator access to the list of tiles belonging to the population.
     */
    tile_list_type::const_iterator get_tile_list_cend() const
    { return this->tile_list.cend(); }

    /**
     * Generate the population and create all tiles belonging to it
     * 
     * This method will generate the population according to its internal parameters.  A grid of N by M
     * tiles will be created forming a tesselation of squares all of equal size.
     */
    void generate();

    /**
     * Reads a population from disk
     * 
     * This method opens and reads a text file containing a serialized population object stored in JSON
     * format.  It returns true if the population has been successuflly unserialized or false if an
     * error occurred.
     */
    bool read( const std::string filename );

    /**
     * Writes the population to disk
     * 
     * This method opens and writes a serialization of the population to a text file.  If the flat_file
     * parameter is true then the population will be written as two CSV files (one for households and
     * the other for individuals), otherwise a single file is written in JSON format.
     */
    void write( const std::string filename, const bool flat_file = false ) const;

    /**
     * Returns whether the population is in sample mode or not
     * 
     * When a population is in sample mode then only selected buildings, households and individuals are
     * included in all count_population(), to_json() and to_csv() methods (including this class' write()
     * method).
     */
    bool get_sample_mode() { return this->sample_mode; }

    /**
     * Sets the sample mode
     * 
     * When a population is in sample mode then only selected buildings, households and individuals are
     * included in all count_population(), to_json() and to_csv() methods (including this class' write()
     * method).
     */
    void set_sample_mode( bool sample_mode ) { this->sample_mode = sample_mode; }

    /**
     * Sets the random generator's seed
     */
    void set_seed( const std::string seed );

    /**
     * Sets the number of tiles in the longitudinal (X) direction
     */
    void set_number_tiles_x( const int );

    /**
     * Sets the number of tiles in the latitudinal (Y) direction
     */
    void set_number_tiles_y( const int );

    /**
     * Gets the width of the population's tiles
     */
    double get_tile_width() const { return this->tile_width; }

    /**
     * Sets the width of the population's tiles
     */
    void set_tile_width( const double );

    /**
     * Sets the disease weights
     * 
     * Disease status is assigned in a standard generalized-linear-model way: that is, a linear function
     * of the various contributing factors (income, spatial disease effect, household disease effect,
     * individual disease effect, age/adult vs. child) is constructed.  In order to make this into a
     * probability the linear disease score is logistic-transformed (called a linear predictor in
     * statistical contexts), and a Bernoulli random variable is chosen based on this score.
     * 
     * This is done in the following manner:
     * 
     * - standardize all of the predictors (income, household latent variable, etc.) to have mean zero
     *   and standard deviation 1 (the latent variables are already scaled this way)
     * - scale each variable by an “importance parameter” and add them together
     * - add an intercept to set the desired median prevalence
     */
    void set_disease_weights(
      const double population,
      const double income,
      const double risk,
      const double age,
      const double sex )
    {
      this->disease_weights[0] = population;
      this->disease_weights[1] = income;
      this->disease_weights[2] = risk;
      this->disease_weights[3] = age;
      this->disease_weights[4] = sex;
    }

    /**
     * Returns the population's mean household population
     */
    double get_mean_household_population() const { return this->mean_household_population; }

    /**
     * Sets the population's mean household population
     */
    void set_mean_household_population( const double );

    /**
     * Returns a reference to the trend defining the population's mean income
     */
    trend* get_mean_income() { return this->mean_income; }

    /**
     * Returns a reference to the trend defining the standard deviation of the population's income
     */
    trend* get_sd_income() { return this->sd_income; }

    /**
     * Sets the trends defining the population's mean and standard deviation income
     */
    void set_income( const trend *mean, const trend *sd );

    /**
     * Returns a reference to the trend defining the population's mean disease risk factor
     */
    trend* get_mean_disease() { return this->mean_disease; }

    /**
     * Returns a reference to the trend defining the standard deviation of the population's
     * disease risk factor
     */
    trend* get_sd_disease() { return this->sd_disease; }

    /**
     * Sets the trends defining the population's mean and standard deviation disease risk factor
     */
    void set_disease( const trend *mean, const trend *sd );

    /**
     * Returns a reference to the trend defining the population's population density
     */
    trend* get_population_density() { return this->population_density; }

    /**
     * Sets the trend defining the population's population density
     */
    void set_population_density( const trend *population_density );

    /**
     * Get the number of individuals in the population
     * 
     * Returns a sum of all individuals in all households in all buildings in all tiles in the population.
     * This method iterates over all tiles (and all buildings in those tiles, etc) every time it is called,
     * so it should only be used when re-counting is necessary.  A population contains no tiles (so no
     * population) until its generate() method is called.
     */
    int count_population() const;

    /**
     * Returns a coordinate at the centre of the population
     * 
     * This coordinate is based on the number of tiles in the X and Y directions as well as tile width.
     */
    coordinate get_centroid() const;

    /**
     * Returns the surface area of the population
     * 
     * The surface area is determined by the area of each tile multiplied by the number of tiles
     * in the X and Y directions.
     */
    double get_area() const;

    /**
     * Deserialize the population
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void from_json( const Json::Value& );

    /**
     * Serialize the population
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void to_json( Json::Value& ) const;

    /**
     * Output the population to two CSV files (households and individuals)
     * 
     * All objects must provide an implementation for printing to a CSV (comma separated value) format.
     * Two streams are expected, the first is for household data and the second for individual data.
     */
    virtual void to_csv( std::ostream&, std::ostream& ) const;

    /**
     * 
     */
    distribution* get_population_distribution() { return &( this->population_distribution ); }

  private:
    /**
     * The number of weights included in determining disease status
     * 
     * See the set_disease_weights() method for more details.
     */
    static const unsigned int NUMBER_OF_WEIGHTS = 5;

    /**
     * Whether the population is in sample mode or not
     */
    bool sample_mode;

    /**
     * The population's current seed value
     */
    std::string seed;

    /**
     * The number of tiles to generate in the longitudinal (X) direction
     */
    int number_tiles_x;

    /**
     * The number of tiles to generate in the latitudinal (Y) direction
     */
    int number_tiles_y;

    /**
     * The width of all (square) tiles
     */
    double tile_width;

    /**
     * The disease weights used to determine disease status
     * 
     * See the set_disease_weights() method for more details.
     */
    double disease_weights[NUMBER_OF_WEIGHTS];

    /**
     * The population's mean household population (size)
     */
    double mean_household_population;

    /**
     * The trend defining the population's mean income
     * 
     * Mean income is determined by tile position according to this trend.
     */
    trend *mean_income;

    /**
     * The trend defining the standard deviation of the population's income
     * 
     * Standard deviation of income is determined by tile position according to this trend.
     */
    trend *sd_income;

    /**
     * The trend defining the population's mean disease risk factor
     * 
     * Mean disease risk factor is determined by tile position according to this trend.
     */
    trend *mean_disease;

    /**
     * The trend defining the standard deviation of the population's disease risk factor
     * 
     * Standard deviation of disease risk factor is determined by tile position according to this trend.
     */
    trend *sd_disease;

    /**
     * The trend defining the population's population density
     * 
     * Population density is determined by tile position according to this trend.
     */
    trend *population_density;

    /**
     * A container holding all tiles belonging to the population.  The population is responsible
     * for managing the memory needed for all of its child tiles.
     */
    tile_list_type tile_list;

    /**
     * The population distribution to use when creating individuals within households
     */
    distribution population_distribution;
  };
}

/** @} end of doxygen group */

#endif
