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

#ifndef __individual_h
#define __individual_h

#include "base_object.h"

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

  class individual : public base_object
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

    virtual void to_json( Json::Value& ) const;
    virtual void to_csv( std::ostream& ) const;

    bool is_male() const { return this->male; }
    void set_male( const bool male ) { this->male = male; }
    bool is_adult() const { return this->adult; }
    void set_adult( const bool adult ) { this->adult = adult; }
    bool is_disease() const { return this->disease; }
    void set_disease( const bool disease ) { this->disease = disease; }

  protected:

  private:
    household *parent;
    bool male;
    bool adult;
    bool disease;
  };
}

/** @} end of doxygen group */

#endif
