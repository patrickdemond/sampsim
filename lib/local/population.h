/*=========================================================================

  Program:  sampsim
  Module:   population.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_population_h
#define __sampsim_population_h

#include "model_object.h"

#include "distribution.h"
#include "utilities.h"

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class town;
  class trend;

  /**
   * @class population
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief A population which contains one or more towns
   * @details
   * Populations are organized into a tree such that all nodes are responsible for creating,
   * generating and deleting their children.  The structure is as follows:
   * - population
   *   + list of towns in population
   *     - list of n by m tiles
   *       + list of buildings in tile
   *         - list of households in building
   *           + list of individuals belonging to household
   * 
   * Creating a population will cause a cascade effect which will create all towns, all tiles,
   * all buildings, all households and all individuals.
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
     * Returns the name of the object's class
     */
    std::string get_name() const { return "population"; }

    /**
     * Iterator access to child towns
     * 
     * These methods provide iterator access to the list of towns belonging to the population.
     */
    town_list_type::iterator get_town_list_begin()
    { return this->town_list.begin(); }

    /**
     * Iterator access to child towns
     * 
     * These methods provide iterator access to the list of towns belonging to the population.
     */
    town_list_type::iterator get_town_list_end()
    { return this->town_list.end(); }

    /**
     * Constant iterator access to child towns
     * 
     * These methods provide constant iterator access to the list of towns belonging to the population.
     */
    town_list_type::const_iterator get_town_list_cbegin() const
    { return this->town_list.cbegin(); }

    /**
     * Constant iterator access to child towns
     * 
     * These methods provide constant iterator access to the list of towns belonging to the population.
     */
    town_list_type::const_iterator get_town_list_cend() const
    { return this->town_list.cend(); }

    /**
     * Generates the population.
     */
    void generate()
    {
      this->create();
      this->define();
    }

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
     * included in all count_individuals(), to_json() and to_csv() methods (including this class' write()
     * method).
     */
    bool get_sample_mode() { return this->sample_mode; }

    /**
     * Sets the sample mode
     * 
     * When a population is in sample mode then only selected buildings, households and individuals are
     * included in all count_individuals(), to_json() and to_csv() methods (including this class' write()
     * method).
     */
    void set_sample_mode( bool sample_mode ) { this->sample_mode = sample_mode; }

    /**
     * Sets the random generator's seed
     */
    void set_seed( const std::string seed );

    /**
     * Sets the number of towns to create
     */
    void set_number_of_towns( const unsigned int );

    /**
     * Set the minimum town size
     */
    void set_town_size_min( const double );

    /**
     * Set the maximum town size
     */
    void set_town_size_max( const double );

    /**
     * Set the town size pareto distribution's shape factor
     */
    void set_town_size_shape( const double );

    /**
     * Sets the number of tiles in a town in the longitudinal (X) direction
     */
    void set_number_of_tiles_x( const unsigned int );

    /**
     * Sets the number of tiles in a town in the latitudinal (Y) direction
     */
    void set_number_of_tiles_y( const unsigned int );

    /**
     * Gets the width of the tiles in a town
     */
    double get_tile_width() const { return this->tile_width; }

    /**
     * Sets the width of the tiles in a town
     */
    void set_tile_width( const double );

    /**
     * Sets the x and y slopes of the population density
     */
    void set_population_density_slope( const double mx, const double my );

    /**
     * Creates the provided number of disease pockets.
     * 
     * Disease pockets affect the overall chance that an individual has a disease.  The factor is
     * determined as a sum of the inverse square distance of an individual from each pocket.
     */
    void set_number_of_disease_pockets( const unsigned int );

    /**
     * Gets which type of kernel disease pockets use.
     * 
     * The effect a disease pocket has on disease is based on the distance between an individual
     * and the disease pocket.  The pocket kernel type determines how that effect varies with
     * distance.
     */
    std::string get_pocket_kernel_type() const { return this->pocket_kernel_type; }

    /**
     * Determines which type of kernel disease pockets use.
     * 
     * The effect a disease pocket has on disease is based on the distance between an individual
     * and the disease pocket.  The pocket kernel type determines how that effect varies with
     * distance.
     */
    void set_pocket_kernel_type( const std::string );

    /**
     * Gets the scaling factor to use for disease pockets.
     * 
     * The distance an individual is from a pocket is divided by this factor.
     */
    double get_pocket_scaling() const { return this->pocket_scaling; }

    /**
     * Sets the scaling factor to use for disease pockets.
     * 
     * The distance an individual is from a pocket is divided by this factor.
     */
    void set_pocket_scaling( const double );

    /**
     * Gets the total number of disease weights used for determining disease status
     */
    unsigned int get_number_of_disease_weights()
    { return NUMBER_OF_DISEASE_WEIGHTS; }

    /**
     * Gets a disease weight by index
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
    double get_disease_weight_by_index( unsigned int );

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
      const double sex,
      const double pocket )
    {
      this->disease_weights[0] = population;
      this->disease_weights[1] = income;
      this->disease_weights[2] = risk;
      this->disease_weights[3] = age;
      this->disease_weights[4] = sex;
      this->disease_weights[5] = pocket;
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
    void set_income( trend *mean, trend *sd );

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
    void set_disease( trend *mean, trend *sd );

    /**
     * Get the number of individuals in the population
     * 
     * Returns a sum of all individuals in all households in all buildings in all tiles in all towns in the
     * population.  This method iterates over all towns (and all tiles in those towns, etc) every time it is
     * called, so it should only be used when re-counting is necessary.  A population contains no towns (so
     * no individuals) until its create() method is called.
     */
    unsigned int count_individuals() const;

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

  protected:
    /**
     * Create all towns belonging to the population
     * 
     * This method will create the population according to its internal parameters.  The method
     * creates towns but does not define their properties.  After calling this function all individuals
     * belonging to the population will exist but without parameters such as income, disease status,
     * disease risk, etc.
     */
    void create();

    /**
     * Define all parameters for all towns belonging to the population
     * 
     * This method will determine all factors such as income, disease status, disease risk, etc for
     * all individuals belonging to the population.  If this method is called before the individuals
     * have been created nothing will happen.
     */
    void define();

  private:
    /**
     * The number of weights included in determining disease status
     * 
     * See the set_disease_weights() method for more details.
     */
    static const unsigned int NUMBER_OF_DISEASE_WEIGHTS = 6;

    /**
     * Whether the population is in sample mode or not
     */
    bool sample_mode;

    /**
     * The population's current seed value
     */
    std::string seed;

    /**
     * The total number of towns in the population
     */
    unsigned int number_of_towns;

    /**
     * The town size distribution to use when creating towns
     */
    distribution town_size_distribution;

    /**
     * The minimum town size
     */
    double town_size_min;

    /**
     * The maximum town size
     */
    double town_size_max;

    /**
     * The town size pareto distribution's shape factor
     */
    double town_size_shape;

    /**
     * The number of tiles in a town to create in the longitudinal (X) direction
     */
    unsigned int number_of_tiles_x;

    /**
     * The number of tiles in a town to create in the latitudinal (Y) direction
     */
    unsigned int number_of_tiles_y;

    /**
     * The width of all (square) tiles in a town
     */
    double tile_width;

    /**
     * The x and y slope of the population density
     */
    double population_density_slope[2];

    /**
     * The number of disease pockets in each town
     */
    unsigned int number_of_disease_pockets;

    /**
     * The type of kernel to use when determining the effect of disease pockets
     */
    std::string pocket_kernel_type;

    /**
     * The amount to scale the effect of disease pocketing
     */
    double pocket_scaling;

    /**
     * The disease weights used to determine disease status
     * 
     * See the set_disease_weights() method for more details.
     */
    double disease_weights[NUMBER_OF_DISEASE_WEIGHTS];

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
     * A container holding all towns belonging to the population.  The population is responsible
     * for managing the memory needed for all of its child towns.
     */
    town_list_type town_list;
  };
}

/** @} end of doxygen group */

#endif
