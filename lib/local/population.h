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
  class household;
  class individual;
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

    // defining pure abstract methods
    std::string get_name() const { return "population"; }
    void copy( const base_object* o ) { this->copy( static_cast<const population*>( o ) ); }
    void copy( const population* );
    void from_json( const Json::Value& );
    void to_json( Json::Value& ) const;
    void to_csv( std::ostream&, std::ostream& ) const;
    unsigned int get_number_of_individuals() const { return this->number_of_individuals; }
    household* get_household_by_index( const unsigned int index ) const
    { return this->household_map.at( index ); }
    individual* get_individual_by_index( const unsigned int index ) const
    { return this->individual_map.at( index ); }
    void assert_summary();
    void rebuild_summary();
    void expire_summary() { this->expired = true; }
    void select();
    void unselect();
    void select_all();

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
     * Writes a summary of the population to disk
     * 
     * This method opens and writes a brief summary of the population
     */
    void write_summary( const std::string filename );

    /**
     * Returns whether the population is in sample mode or not
     * 
     * When a population is in sample mode then only selected buildings, households and individuals are
     * included in all get_summary(), to_json() and to_csv() methods (including this class' write()
     * method).
     */
    bool get_sample_mode() { return this->sample_mode; }

    /**
     * Sets the sample mode
     * 
     * When a population is in sample mode then only selected buildings, households and individuals are
     * included in all copy(), get_summary(), to_json() and to_csv() methods (including this class' write()
     * method).
     */
    void set_sample_mode( const bool sample_mode );

    /**
     * Stores a reference to the household and returns the household's index
     * 
     * This should only be used when creating a new household, not when reading them from an existing
     * population.
     */
    unsigned int add_household( household *h, int predefined_index = -1 )
    {
      unsigned int index = 0 > predefined_index
                         ? this->current_household_index++
                         : (unsigned int) predefined_index;
      this->household_map[index] = h;
      return index;
    }

    /**
     * Removes a household from the household map
     * 
     * Note: this should only be used by the tile's create method when removing a building after it has
     * been created because the population density has been met (see tile::create)
     */
    void remove_household( unsigned int index ) { this->household_map.erase( index ); }

    /**
     * Stores a reference to the individual and returns the individual's index
     * 
     * This should only be used when creating a new individual, not when reading them from an existing
     * population.
     */
    unsigned int add_individual( individual *i, int predefined_index = -1 )
    {
      unsigned int index = 0 > predefined_index
                         ? this->current_individual_index++
                         : (unsigned int) predefined_index;
      this->individual_map[index] = i;
      return index;
    }

    /**
     * Removes a individual from the individual map
     * 
     * Note: this should only be used by the tile's create method when removing a building after it has
     * been created because the population density has been met (see tile::create)
     */
    void remove_individual( unsigned int index ) { this->individual_map.erase( index ); }

    /**
     * Sets the random generator's seed
     */
    void set_seed( const std::string );

    /**
     * Sets whether to calculate sample weights
     */
    void set_use_sample_weights( const bool );

    /**
     * Gets whether to calculate sample weights
     */
    bool get_use_sample_weights() const { return this->use_sample_weights; }

    /**
     * Gets the number of towns to create
     */
    unsigned int get_number_of_towns() const { return this->number_of_towns; }

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
     * Gets the target mean disease prevalence of the population
     */
    double get_target_prevalence() const { return this->target_prevalence; }

    /**
     * Sets the target mean disease prevalence of the population
     */
    void set_target_prevalence( const double );

    /**
     * Sets the x and y slopes of the population density
     * 
     * Note that both values must be in the range [-1, 1].  Trying to set values outside of this
     * range will show a warning and constrain the value to within the valid range.
     */
    void set_population_density_slope( double mx, double my );

    /**
     * Gets the probability that a town has a river.
     * 
     * If a river is added to a town then a random line is drawn across the town and all buildings
     * that are within half the river's width from that line are moved to the closest "shore" from
     * their initial position.
     */
    double get_river_probability() const { return this->river_probability; }

    /**
     * Sets the probability that a town has a river.
     * 
     * If a river is added to a town then a random line is drawn across the town and all buildings
     * that are within half the river's width from that line are moved to the closest "shore" from
     * their initial position.
     */
    void set_river_probability( const double );

    /**
     * Gets the width to make rivers.
     * 
     * If a river is added to a town then a random line is drawn across the town and all buildings
     * that are within half the river's width from that line are moved to the closest "shore" from
     * their initial position.
     */
    double get_river_width() const { return this->river_width; }

    /**
     * Sets the width to make rivers.
     * 
     * If a river is added to a town then a random line is drawn across the town and all buildings
     * that are within half the river's width from that line are moved to the closest "shore" from
     * their initial position.
     */
    void set_river_width( const double );

    /**
     * Gets the number of disease pockets.
     * 
     * Disease pockets affect the overall chance that an individual has a disease.  The factor is
     * determined as a sum of the inverse square distance of an individual from each pocket.
     */
    unsigned int get_number_of_disease_pockets() const { return this->number_of_disease_pockets; }

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
     * - scale each variable by an ???importance parameter??? and add them together
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
     * - scale each variable by an ???importance parameter??? and add them together
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
     * Returns a reference to the trend defining the population's mean exposure risk factor
     */
    trend* get_mean_exposure() { return this->mean_exposure; }

    /**
     * Returns a reference to the trend defining the standard deviation of the population's
     * exposure risk factor
     */
    trend* get_sd_exposure() { return this->sd_exposure; }

    /**
     * Sets the trends defining the population's mean and standard deviation disease risk factor
     */
    void set_disease( trend *mean, trend *sd );

    /**
     * Gets the proportion and variance for this population
     * 
     * Note that this is only valid when individuals have been selected
     */
    std::pair< double, double > get_variance( unsigned int index ) const;

  protected:
    void create();
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
     * Used to provide unique indeces for all households in the population
     */
    unsigned int current_household_index;

    /**
     * Used to provide unique indeces for all individuals in the population
     */
    unsigned int current_individual_index;

    /**
     * The population's current seed value
     */
    std::string seed;

    /**
     * Whether to calculate sample weights
     */
    bool use_sample_weights;

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
     * The target mean disease prevalence of the population
     */
    double target_prevalence;

    /**
     * The x and y slope of the population density
     */
    double population_density_slope[2];

    /**
     * The probability that a town has a river.
     */
    double river_probability;

    /**
     * The width to make rivers.
     */
    double river_width;

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
     * The trend defining the population's mean exposure risk factor
     * 
     * Mean exposure risk factor is determined by tile position according to this trend.
     */
    trend *mean_exposure;

    /**
     * The trend defining the standard deviation of the population's exposure risk factor
     * 
     * Standard deviation of exposure risk factor is determined by tile position according to this trend.
     */
    trend *sd_exposure;

    /**
     * A container holding all towns belonging to the population.  The population is responsible
     * for managing the memory needed for all of its child towns.
     */
    town_list_type town_list;

    /**
     * A reference map to all households in the population.  The population is NOT responsible
     * for managing the memory needed for these references (this is done by their buidings)
     */
    household_map_type household_map;

    /**
     * A reference map to all individuals in the population.  The population is NOT responsible
     * for managing the memory needed for these references (this is done by their household)
     */
  public:
    individual_map_type individual_map;

    /**
     * The number of individuals in the population.
     */
    unsigned int number_of_individuals;

    /**
     * The master expired variable for the population's summaries
     */
    bool expired;
  };
}

/** @} end of doxygen group */

#endif
