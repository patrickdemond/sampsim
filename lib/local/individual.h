/*=========================================================================

  Program:  sampsim
  Module:   individual.h
  Language: C++

=========================================================================*/

/**
 * @class individual
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief An individual which belongs to a household
 */

#ifndef __sampsim_individual_h
#define __sampsim_individual_h

#include "model_object.h"

#include "utilities.h"

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class building;
  class household;
  class population;
  class tile;

  class individual : public model_object
  {
  public:
    individual( household *parent );
    ~individual();

    /**
     * Lineage methods
     */
    household* get_household() const { return this->parent; }
    building* get_building() const;
    tile* get_tile() const;
    population* get_population() const;

    virtual void from_json( const Json::Value& );
    virtual void to_json( Json::Value& ) const;
    virtual void to_csv( std::ostream&, std::ostream& ) const;

    sex_type get_sex() const { return this->sex; }
    void set_sex( const sex_type sex ) { this->sex = sex; }
    age_type get_age() const { return this->age; }
    void set_age( const age_type age ) { this->age = age; }
    bool is_disease() const { return this->disease; }
    void set_disease( const bool disease ) { this->disease = disease; }
    bool is_selected() const { return this->selected; }
    void set_selected( const bool selected );

  protected:

  private:
    household *parent;
    age_type age;
    sex_type sex;
    bool disease;
    bool selected;
  };
}

/** @} end of doxygen group */

#endif
