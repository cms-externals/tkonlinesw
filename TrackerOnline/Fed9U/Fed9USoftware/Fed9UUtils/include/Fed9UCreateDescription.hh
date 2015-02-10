#ifndef _Fed9UCreateDescription_H
#define _Fed9UCreateDescription_H

#include "TypeDefs.hh"
#include "Fed9UDescription.hh"

namespace Fed9U {

  //using.*std::string;

  /**
   * \brief  This class can be used to create a complete description object, with settings other than
   *         the default description settings. It has been designed to be used with the soak test
   *         which requires a range of description settings.
   * \author Gareth Rogers, RAL
   * \date   05/2004
   *
   * The contents of the descripition files created are not controlled by the caller of the class.
   * The description will be created with predefined settings. A single instace of Fed9UCreateDescription
   * will keep track of the number of times it has been called and will create a description with
   * different settings each time. The maximum value of unique descriptions that can be generated is 4294967295.
   */
  class Fed9UCreateDescription {

  public:

    /**
     * \brief Fed9UCreateDescription constructor.
     * \param fed9UDescriptionNumber Each description corresponding to this number is different, by default it is zero
     *        but giving a different value will cause an offset into the created descriptions.
     *
     * A Fed9UCreateDescription will return a different description depending on how many times it
     * has been called.
     */
    Fed9UCreateDescription(u32 fed9UDescriptionNumber = 0);

    /**
     * \brief  Makes a FedUDescription object with predefined settings depending on the description number
     * \return Fed9UDescription A copy of the description object that has been created.
     *
     * The crate number, base address of the FED and the location of the HAL address table must be specified.
     */
    Fed9UDescription makeFed9UDescription(u16 crateNumber, u32 baseAddress, std::string halAddressTable);

  private:

    /**
     * \brief Loads all the FE settings into the description.
     */
    Fed9UCreateDescription& loadFeFpgaSettings();

    /**
     * \brief Loads all the BE settings into the description.
     */
    Fed9UCreateDescription& loadBeFpgaSettings();

    /**
     * \brief Loads all the VME settings into the description.
     */
    Fed9UCreateDescription& loadVmeFpgaSettings();

    /**
     * \brief Loads all the TTCrx settings into the description.
     */
    Fed9UCreateDescription& loadTtcrxDeviceSettings();

    /**
     * \brief Loads all the Voltage Monitor settings into the description.
     */
    Fed9UCreateDescription& loadVoltageMonitorSettings();

    /**
     * \brief Loads the serial EPROM into the description.
     */
    Fed9UCreateDescription& loadEpromSettings();

    /**
     * \brief Loads all the higher level settings into the description.
     */
    Fed9UCreateDescription& loadHigherLevelSettings();

    /**
     * \brief This holds the iteration number. It is used to vary the settings in the returned FED description.
     */
    u32 fed9UDescriptionNumber_;

    /**
     * \brief Holds the settings that are to be returned.
     */
    Fed9UDescription fedDescription;

  };// End of Fed9UCreateDescription class

}// End of Fed9U namespace

#endif// _H_Fed9UCreateDescription
