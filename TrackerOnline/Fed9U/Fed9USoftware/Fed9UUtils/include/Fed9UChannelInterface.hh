#ifndef H_Fed9UChannelInterface
#define H_Fed9UChannelInterface

#include "TypeDefs.hh"

#define FED9U_CHANNEL_INTERFACE_COUNT_FROM_TOP_DEFAULT false //!< Default behaviour whether to count from the top, internal numbering first channel at top.
                                                             //!< or to count from the bottom, external number last channel at the top.
#define FED9U_CHANNEL_INTERFACE_COUNT_BASE_DEFAULT 1         //!< Default behaviour whether to start counting from one or zero.

namespace Fed9U {

  class Fed9UAddress;
#ifdef EVENT_STREAMLINE
  class Fed9UEventStreamLine;
#else
  class Fed9UEvent;
#endif

  /**
   * \brief FED Channel Interface class.
   *
   * This class simply provides an abstracted interface for the user to set the desired channel. The channel can be set on a
   * variety of granularities. Setting the FED channel, that also defines the FE unit channel number and FE unit number, which
   * can be done either on construction or using setter methods. The FE unit channel and unit number can also be set individually
   * after default construction using setter methods. It also allows the user to count in 0 or 1 based and from top to bottom or
   * bottom to top, depending on whether they use the internal (zeroth/first channel at the top) or external (first channel at the
   * bottom) numbering schemes. Top and bottom refer to the physical top and bottom of the FED.
   */
  class Fed9UChannelInterface {
  public:

    /**\name Constructors*/
    //@{

    /**
     * \brief Default constructor.
     *
     * Sets the channel to zero and the counting to FED9U_CHANNEL_INTERFACE_COUNT_FROM_TOP_DEFAULT and the counting base
     * to FED9U_CHANNEL_INTERFACE_COUNT_BASE_DEFAULT.
     */
    Fed9UChannelInterface();

    /**
     * \brief Constructor that allows FED channel, counting base and count direction to be set upon construction.
     * \param fedChannel Channel on the FED that is to be set, in either external or internal number scheme, this is defined by next two arguments.
     * \param countingBase Zero or one are the two allowed numbers to start counting from. Defaulted to class default as defined by
     *        FED9U_CHANNEL_INTERFACE_COUNT_BASE_DEFAULT.
     * \param fromTop True if counting starts from the top, as in the internal numbering system, or false if the counting starts from the bottom
     *        as in the external numbering scheme. 
     */
    Fed9UChannelInterface(u8 fedChannel, u8 countingBase = FED9U_CHANNEL_INTERFACE_COUNT_BASE_DEFAULT, bool fromTop = FED9U_CHANNEL_INTERFACE_COUNT_FROM_TOP_DEFAULT);
    //@}

    /**\name Getters*/
    //@{

    /**
     * \brief  Each FED has 96 channels that are individually addressable.
     * \return unsigned char FED channel number selected.
     */
    u8 getChannel() const;

    /**
     * \brief  Each FED has 8 FE units that are individually addressable.
     * \return unsigned char FE unit number.
     */
    u8 getFeUnit() const;

    /**
     * \brief  Within each FE unit there are 12 channels that can be selected.
     * \return unsigned char FE unit channel number.
     */
    u8 getFeUnitChannel() const;

    /**
     * \brief  Counting can either begin from zero or one.
     * \return unsigned char Returns the base used for counting.
     */
    u8 getBase();

    /**
     * \brief  Counting can either start from the top of the FED or the bottom. Top is internal number, bottom external numbering.
     * \return bool True indicates counting is from the top and false from the bottom.
     */
    bool getCountFromTop();
    //@}

    /**\name Setters*/
    //@{

    /**
     * \brief Each FED has 96 channels that are individually addressable.
     * \param channel FED channel number to be selected.
     */
    Fed9UChannelInterface & setChannel(u8 channel);

    /**
     * \brief Each FED has 8 FE units that are individually addressable.
     * \param feUnit FE unit number to be used.
     */
    Fed9UChannelInterface & setFeUnit(u8 feUnit);

    /**
     * \brief Within each FE unit there are 12 channels that can be selected.
     * \param feUnitChannel FE unit channel number to be used.
     */
    Fed9UChannelInterface & setFeUnitChannel(u8 feUnitChannel);

    /**
     * \brief Counting can either begin from zero or one.
     * \param base Sets the base used for counting.
     */
    Fed9UChannelInterface & setBase(u8 base);

    /**
     * \brief Counting can either start from the top of the FED or the bottom. Top is internal number, bottom external numbering.
     * \param fromTop True indicates counting is from the top and false from the bottom.
     */
    Fed9UChannelInterface & setCountFromTop(bool fromTop);
    //@}


    friend class Fed9UAddress;
#ifdef EVENT_STREAMLINE
    friend class Fed9UEventStreamLine;
#else
    friend class Fed9UEvent;
#endif

  private:

    /**
     * \brief  There are 96 channels on a FED and the internal number scheme starts counting from zero at the top of the FED.
     * \return unsigned char The channel number selected according to the internal number scheme.
     */
    u8  getChannelInternal() const;

    /**
     * \brief There are 96 channels on a FED and the internal number scheme starts counting from zero at the top of the FED.
     * \param channel The channel number to be selected according to the internal number scheme.
     */
    Fed9UChannelInterface & setChannelInternal(u8 channel);

    u8 _channel;   //!< FED channel number, stored in internal numbering format (from top, base 0).
    u8 _base;      //!< Base the User has selected channel counting to start from.
    bool _fromTop; //!< Whether the counting should begin from the top or bottom of the FED channels.
  };

}

#endif // H_Fed9UChannelInterface
