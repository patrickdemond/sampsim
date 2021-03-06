/*=========================================================================

  Program:  sampsim
  Module:   sample.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_sample_h
#define __sampsim_sample_sample_h

#include "base_object.h"

#include "utilities.h"

#include <list>
#include <string>

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
class individual;
class population;

/**
 * @addtogroup sample
 * @{
 */

/**
 * @namespace sampsim::sample
 * @brief A namespace containing all sampling classes
 */
namespace sample
{
  /**
   * @class sample
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief An abstract base class for all sampling methods
   * @details
   * All sampling methods work by selecting buildings until some pre-defined condition has been met.
   * Implementations of this class define how buildings are selected by defining the 
   * select_next_building() method.
   * Populations used by sample classes are put into sampling mode.
   */
  class sample : public sampsim::base_object
  {
  public:
    /**
     * Constructor
     */
    sample();

    /**
     * Destructor
     */
    ~sample();

    // defining pure abstract methods
    void copy( const base_object* o ) { this->copy( static_cast<const sample*>( o ) ); }
    void copy( const sample* );
    virtual void from_json( const Json::Value& );
    virtual void to_json( Json::Value& ) const;
    virtual void to_csv( std::ostream&, std::ostream& ) const;

    /**
     * Generates the sample by calling select_next_building() until the ending condition is met
     */
    virtual void generate();

    /**
     * Iterator access to child populations
     * 
     * These methods provide iterator access to the list of populations belonging to this building.
     */
    population_list_type::iterator get_sampled_population_list_begin()
    { return this->sampled_population_list.begin(); }

    /**
     * Iterator access to child populations
     * 
     * These methods provide iterator access to the list of populations belonging to this building.
     */
    population_list_type::iterator get_sampled_population_list_end()
    { return this->sampled_population_list.end(); }

    /**
     * Constant iterator access to child populations
     * 
     * These methods provide constant iterator access to the list of populations belonging to this building.
     */
    population_list_type::const_iterator get_sampled_population_list_cbegin() const
    { return this->sampled_population_list.cbegin(); }

    /**
     * Constant iterator access to child populations
     * 
     * These methods provide constant iterator access to the list of populations belonging to this building.
     */
    population_list_type::const_iterator get_sampled_population_list_cend() const
    { return this->sampled_population_list.cend(); }

    /**
     * Reads a sample from disk
     * 
     * This method opens and reads a text file containing a serialized population object stored in JSON
     * format.  It returns true if the sample has been successuflly unserialized or false if an
     * error occurred.
     */
    bool read( const std::string filename );

    /**
     * Writes the sample to disk
     * 
     * This method opens and writes a serialization of the selected inviduals in the sample's population.
     * If the flat_file parameter is true then the population will be written as two CSV files (one for
     * households and the other for individuals), otherwise a single file is written in JSON format.
     */
    void write( const std::string filename, const bool flat_file = false ) const;

    /**
     * Writes a summary of the sample to disk
     * 
     * This method opens and writes a brief summary of the sample.
     */
    void write_summary( const std::string filename ) const;

    /**
     * Writes proportion and variance of the sample to disk
     */
    void write_variance( const std::string filename ) const;

    /**
     * Returns the name of the sampling method
     */
    virtual std::string get_type() const = 0;

    /**
     * Returns a pointer to the population used by this sampler
     */
    sampsim::population* get_population() const { return this->population; }

    /**
     * Loads a population from disk to sample from
     */
    bool set_population( const std::string filename );

    /**
     * Loads a population from memory to sample from
     */
    bool set_population( sampsim::population *population );

    /**
     * Sets the random generator's seed
     */
    void set_seed( const std::string seed );

    /**
     * Returns the random generator's seed
     */
    std::string get_seed() const { return this->seed; }

    /**
     * Sets whether to calculate and use sampling weights
     */
    void set_use_sample_weights( const bool use_sample_weights );

    /**
     * Returns whether sampling weights are used
     */
    bool get_use_sample_weights() const { return this->use_sample_weights; }

    /**
     * Sets the current sample part to process
     */
    void set_sample_part( const unsigned int sample_part );

    /**
     * Returns the current sample part to process
     */
    unsigned int get_sample_part() const { return this->sample_part; }

    /**
     * Sets the total number of parts to divide all samples into
     */
    void set_number_of_sample_parts( const unsigned int number_of_sample_parts );

    /**
     * Returns the total number of parts to divide all samples into
     */
    unsigned int get_number_of_sample_parts() const { return this->number_of_sample_parts; }

    /**
     * Sets the number of times to sample the population
     */
    void set_number_of_samples( const unsigned int number_of_samples );

    /**
     * Returns the number of times to sample the population
     */
    unsigned int get_number_of_samples() const { return this->number_of_samples; }

    /**
     * Sets the number of towns to sample
     */
    void set_number_of_towns( const unsigned int number_of_towns );

    /**
     * Returns the number of town to sample
     */
    unsigned int get_number_of_towns() const { return this->number_of_towns; }

    /**
     * Sets whether to sample one individual per household
     */
    void set_one_per_household( const bool one_per_household );

    /**
     * Returns whether sampling is restricted to one individual per household
     */
    bool get_one_per_household() const { return this->one_per_household; }

    /**
     * Sets whether to resample towns on every sample iteration
     */
    void set_resample_towns( const bool resample_towns );

    /**
     * Returns whether to resample towns on every sample iteration
     */
    bool get_resample_towns() const { return this->resample_towns; }

    /**
     * Sets what age to restrict the sample to
     */
    void set_age( const age_type age );

    /**
     * Returns the age the sampling is restricted to
     */
    age_type get_age() const { return this->age; }

    /**
     * Sets what sex to restrict the sample to
     */
    void set_sex( const sex_type sex );

    /**
     * Returns the sex the sampling is restricted to
     */
    sex_type get_sex() const { return this->sex; }

    /**
     * Returns the first building which was selected by the sampler
     */
    building* get_first_building() { return this->first_building; }

    /**
     * Returns the header for generated CSV files
     * 
     * The header includes all parameters used by the sampling method
     */
    virtual std::string get_csv_header() const;

  protected:
    /**
     * Returns the current number of selected individuals in the sample
     */
    unsigned int get_current_size() const { return this->current_size; }

    /**
     * Returns the current number of selected individuals in the current town
     */
    unsigned int get_current_town_size() const { return this->current_town_size; }

    /**
     * Algorithm which selects buildings based on the sampling method
     * 
     * This pure virtual method must be defined by all child classes.  It allows each sampling method
     * to define how buildings are selected (in sequence).
     */
    virtual building* select_next_building( building_list_type& ) = 0;

    /**
     * TODO: document
     */
    virtual void create_building_list( sampsim::town*, building_list_type& );

    /**
     * Called before each sample is taken
     */
    virtual void reset_for_next_sample( const bool full = true );

    /**
     * Returns whether or not the sampling is complete
     * 
     * This pure virtual method must be defined by all child classes.  It allows each sampling method
     * to determine when to stop selecting buildings.
     */
    virtual bool is_sample_complete() = 0;

    /**
     * Determines an individual's sample weight
     * 
     * Sample weight is the inverse of the probability of selecting an individual.  This base class
     * includes determining the probability of selecting the town the individual belongs to as compared
     * to the total population size.  Additional factors must be implemented by child classes.
     */
    virtual double get_immediate_sample_weight( const sampsim::individual* ) const;

    /**
     * Determines the post-sample weighting factor
     * 
     * Some aspects of sample weight do not need to be calculated on an individual-by-individual basis.
     * Those factors should be calculated in this method instead of get_immediate_sample_weight() to save
     * on processing time.
     */
    virtual double get_post_sample_weight_factor() const;

    /**
     * The population to sample from
     */
    sampsim::population *population;

  private:
    /**
     * Internal method for creating new populations when they are read from disk
     */
    void create_population();

    /**
     * Internal method for deleting populations when they are read from disk
     */
    void delete_population();

    /**
     * Recalculates the first and last sample indeces
     */
    void recalculate_sample_indeces();

    /**
     * Defines whether this class is reponsible for deleting the memory used by the population object
     */
    bool owns_population;

    /**
     * A list of sampled populations which only include selected individuals
     */
    population_list_type sampled_population_list;

    /**
     * The random generator's seed
     */
    std::string seed;

    /**
     * Whether to sample one individual per household
     */
    bool use_sample_weights;

    /**
     * The current sample part to process
     */
    unsigned int sample_part;

    /**
     * The total number of parts to divide all samples into
     */
    unsigned int number_of_sample_parts;

    /**
     * The number of times to sample the population
     */
    unsigned int number_of_samples;

    /**
     * The current run's first sample index
     * (based on sample_part, number_of_sample_parts and number_of_samples)
     */
    unsigned int first_sample_index;

    /**
     * The current run's last sample index
     * (based on sample_part, number_of_sample_parts and number_of_samples)
     */
    unsigned int last_sample_index;

    /**
     * The number of towns to sample
     */
    unsigned int number_of_towns;

    /**
     * The number of individuals which have been selected by the sample so far.
     * 
     * This value is updated after every call to the select_next_building() method
     */
    unsigned int current_size;

    /**
     * The number of individuals which have been selected by the sample for the current town.
     * 
     * This value is updated after every call to the select_next_building() method
     */
    unsigned int current_town_size;

    /**
     * Whether to sample one individual per household
     */
    bool one_per_household;

    /**
     * Whether to resample towns on every sample iteration
     */
    bool resample_towns;

    /**
     * What age to restrict the sample to
     */
    age_type age;

    /**
     * What age to restrict the sample to
     */
    sex_type sex;

    /**
     * A reference to the first building selected by the sampler
     */
    building *first_building;

    /**
     * The fraction of the currently selected town's population to the total population (used for sample weights)
     */
    double current_town_individual_fraction;
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
