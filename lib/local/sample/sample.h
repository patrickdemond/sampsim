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
class building_tree;
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
   * All sampling methods work by selecting buildings until the sample size has been met.
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

    /**
     * Generates the sample by calling select_next_building() until sample size has been met
     */
    virtual void generate();

    /**
     * Writes the sample to disk
     * 
     * This method opens and writes a serialization of the selected inviduals in the sample's population.
     * If the flat_file parameter is true then the population will be written as two CSV files (one for
     * households and the other for individuals), otherwise a single file is written in JSON format.
     */
    void write( const std::string filename, const bool flat_file = false ) const;

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
     * Sets the number of times to sample the population
     */
    void set_number_of_samples( const unsigned int number_of_samples );

    /**
     * Returns the number of times to sample the population
     */
    unsigned int get_number_of_samples() const { return this->number_of_samples; }

    /**
     * Sets the number of individuals to select per sample
     */
    void set_size( const unsigned int size );

    /**
     * Returns the number of individuals to select per sample
     */
    unsigned int get_size() const { return this->size; }

    /**
     * Sets whether to sample one individual per household
     */
    void set_one_per_household( const bool one_per_household );

    /**
     * Returns whether sampling is restricted to one individual per household
     */
    bool get_one_per_household() const { return this->one_per_household; }

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
     * Deserialize the sample
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void from_json( const Json::Value& );

    /**
     * Serialize the sample
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void to_json( Json::Value& ) const;

    /**
     * Output the sample to two CSV files (households and individuals)
     * 
     * All objects must provide an implementation for printing to a CSV (comma separated value) format.
     * Two streams are expected, the first is for household data and the second for individual data.
     */
    virtual void to_csv( std::ostream&, std::ostream& ) const;

    /**
     * Returns the header for generated CSV files
     * 
     * The header includes all parameters used by the sampling method
     */
    virtual std::string get_csv_header() const;

  protected:
    /**
     * Algorithm which selects buildings based on the sampling method
     * 
     * This pure virtual method must be defined by all child classes.  It allows each sampling method
     * to define how buildings are selected (in sequence).
     */
    virtual building* select_next_building( sampsim::building_tree& ) = 0;

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
     * Defines whether this class is reponsible for deleting the memory used by the population object
     */
    bool owns_population;

    /**
     * The random generator's seed
     */
    std::string seed;

    /**
     * The number of times to sample the population
     */
    unsigned int number_of_samples;

    /**
     * How many individuals to select in each sample
     */
    unsigned int size;

    /**
     * Whether to sample one individual per household
     */
    bool one_per_household;

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
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
