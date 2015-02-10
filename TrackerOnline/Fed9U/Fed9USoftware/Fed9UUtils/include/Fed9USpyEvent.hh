#ifndef H_Fed9USpyEvent
#define H_Fed9USpyEvent
#include <vector>
#include <iostream>
#include <fstream>
#include "TypeDefs.hh"
#include "Fed9UAddress.hh"
#include "Fed9UDescription.hh"
#include "ICAssert.hh"
#include "Fed9USpyEventException.hh"
namespace Fed9U {
  
  /**
   * \brief  Class to hold and access data from the spy channel.
   */
  class Fed9USpyEvent {
  public:
    /**
     * \brief Construct Fed9USpyEvent by giving a value for thresholds.
     *        The buffer is allocated and deallocated by Fed9USpyEvent.
     * \param thresholdValue Value to use for all channel thresholds.
     */
    Fed9USpyEvent(u16 thresholdValue=500);
    /**
     * \brief Construct Fed9USpyEvent with a description to take the thresholds
     *        from. The buffer is allocated and deallocated by Fed9USpyEvent.
     * \param theFedDescription Description containing thresholds.
     */
    Fed9USpyEvent(const Fed9UDescription& theFedDescription);
    /**
     * \brief Construct Fed9USpyEvent by giving a buffer and a description to 
     *        take the thresholds from. The buffer is not deallocated by 
     *        Fed9USpyEvent.
     * \param bufferPointer Pointer to buffer large enough for data.
     * \param theFedDescription Description containing thresholds.
     */
    Fed9USpyEvent(u32* bufferPointer, const Fed9UDescription& theFedDescription);
    /**
     * \brief Construct Fed9USpyEvent from an input stream and a description to
     *        take the thresholds from. The buffer is allocated and 
     *        deallocated by Fed9USpyEvent.
     * \param theStream Pointer to input stream to read from.
     * \param theFedDescription Description containing thresholds.
     */
    Fed9USpyEvent(std::istream* theStream, const Fed9UDescription& theFedDescription);
    /**
     * \brief Construct Fed9USpyEvent from the file name of a file written to 
     *        with write and a description to take the thresholds from. The 
     *        buffer is allocated and deallocated by Fed9USpyEvent.
     * \param fileName Name of the file to read from.
     * \param theFedDescription Description containing thresholds.
     */
    Fed9USpyEvent(const char*const filename, const Fed9UDescription& theFedDescription);
    /**
     * \brief Construct Fed9USpyEvent from an input stream. The buffer is 
     *        allocated and deallocated by Fed9USpyEvent.
     * \param theStream Pointer to input stream to read from.
     * \param thresholdValue Value to use for all channel thresholds.
     */
    Fed9USpyEvent(std::istream* theStream, u16 thresholdValue=500);
    /**
     * \brief Construct Fed9USpyEvent from the file name of a file written to.
     *        The buffer is allocated and deallocated by Fed9USpyEvent.
     * \param fileName Name of the file to read from.
     * \param thresholdValue Value to use for all channel thresholds.
     */
    Fed9USpyEvent(const char*const filename, u16 thresholdValue=500);
    /**
     * \brief Construct Fed9USpyEvent from a buffer dumped with writeToBuffer.
     *        The buffer is not allocated or deallocated by Fed9USpyEvent.
     * \param readBuffer Buffer to read from.
     * \param thresholdValue Value to use for all channel thresholds.
     */
    Fed9USpyEvent(u32* readBuffer, u16 thresholdValue=500);
    /**
     * \brief Read SpyEvernt from a binary buffer dumped with writeToBuffer
     * \param readBuffer Buffer to read from.
     */
    void readFromBuffer(u32* inBuffer);
    /**
     * \brief Deallocates the buffer if it is managed by Fed9USpyEvent.
     */
    ~Fed9USpyEvent();
    /**
     * \brief Read SpyEvent from stream from write.
     * \param theStream Input stream to read from.
     */
    void readFromStream(std::istream* theStream);
    /**
     * \brief  Get a new buffer to be passed to Fed9uVmeDevice::fireSpy. 
     * \param  address Address of delay chip which captured this data.
     * \return Pointer to the begining of a buffer large enough for 
     *         one delay chip's spy channel data including offset.
     */
    u32* delayChipBuffer(Fed9UAddress address) const;
    /**
     * \brief  Get the value of a sample.
     * \param  channelAddress The address of the channel from which the sample 
     *         comes.
     * \param  sample The index of the sample.
     * \return unsigned short int The value of the sample.
     */
    u16 getSample(Fed9UAddress channelAddress, u16 sample) const;
    /**
     * \brief  Get the value of a payload sample.
     * \param  channelAddress The address of the channel from which the sample 
     *         comes.
     * \param  sample The index of the sample in the payload.
     * \return unsigned short int The value of the sample.
     */
    u16 getPayloadSample(Fed9UAddress channelAddress, u16 sample) const;
    /**
     * \brief  Returns the number of samples per channel
     * \return unsigned short int Number of samples per channel.
     */
    static u16 samplesPerChannel() { return static_cast<u16>(spyDelayChipBufferSize*32.0/10.0)/CHANNELS_PER_DELAY_CHIP; }
    /**
     * \brief  Returns the size of a full spy event buffer in bytes
     * \return size of full spy event buffer.
     */
    static size_t fullEventSize() { return spyDelayChipBufferSize*4*(CHANNELS_PER_FED/CHANNELS_PER_DELAY_CHIP); }
    /**
     * \brief  Get the pipeline address of the APV frame in then event.
     * \return unsigned char The pipeline address.
     */
    u8 getPipelineAddress() const;
    /**
     * \brief  Get the pipeline address from a single APV in a single channel.
     * \param  theAPV The Address of the APV.
     * \return unsigned char The APV pipeline address.
     */
    u8 getAPVPipelineAddress(Fed9UAddress apvAddress) const;
    /**
     * \brief  Get the number of samples before the begining of the tick mark.
     * \param  channelAddress The address of the channel.
     * \return unsigned char The offset (in data words) of the begining of the 
     *         tick mark.
     */
    u8 getChannelTickOffset(Fed9UAddress channelAddress) const;
    /**
     * \brief Write SpyEvent to output stream.
     * \param theStream Pointer to output stream to write to.
     */
    void writeToStream(std::ostream* theStream) const;
    /**
     * \brief  Get required size of buffer to write to with writeToBuffer.
     * \return unsigned long int buffer size in bytes
     */
    u32 getDumpBufferSize() const;
    /**
     * \brief Write SpyEvent to a buffer.
     * \param buffer Buffer to write to.
     */
    void writeToBuffer(u32* buffer) const;
    /**
     * \brief Write SpyEvent to file.
     * \param filename Name of file to write to.
     */
    void writeToFile(const char*const filename) const;
    /**
     * \brief Print SpyEvent summary to output stream.
     * \param os Output stream to print to.
     */
    void print(std::ostream& os) const;
    /**
     * \brief  Get the error bit from a single APV in a single channel.
     * \param  theAPV The Address of the APV.
     * \return bool The APV error bit.
     */
    bool getAPVErrorBit(Fed9UAddress apvAddress) const;
    /**
     * \brief Set the thresholds for bits in APV headers.
     * \param theFedDescription Description containing thresholds.
     */
    void setThresholds(const Fed9UDescription& theFedDescription);
    /**
     * \brief Set the thresholds for bits in APV headers for all channels 
     *        at once
     * \param thresholdValue Value for threshold.
     */
    void setAllThresholds(u16 thresholdValue);
    /**
     * \brief  Check if the tick marke could be found.
     * \param  channelAddress The address o0f the channel.
     * \return bool Was it found.
     */
    bool tickFound(Fed9UAddress channelAddress) const;
    void setSample(u8 delayChip, u8 channel, u16 sampleNumber, u16 value);
  private:
    /**
     * \brief  Get the value of a sample.
     * \param  delayChip The delay chip.
     * \param  channel The channel in the delay chip from which the sample comes.
     * \param  sample The index of the sample.
     * \return unsigned short int The value of the sample.
     */
    u16 getSample(u8 delayChip, u8 channel, u16 sample) const;
    /**
     * \brief  Get a pointer to the buffer holding delay chip data.
     * \param  delayChip The delay chip.
     * \return unsigned long int pointer Pointer to the begining of the buffer 
     *         holding a data.
     */
    u32* delayChipBuffer(u8 delayChip) const;
    /**
     * \brief  Get the pipeline address from a single APV in a single channel/
     * \param  delayChip The delay chip buffer number containing the APV's 
     *         channel in the buffer.
     * \param  channel Channel in delay chip. 
     * \param  apv The apv in the channel.
     * \return unsigned char The APV pipeline address.
     */
    u8 getAPVPipelineAddress(u8 delayChip, u8 channel, u8 apv) const;
    /**
     * \brief  Get the error bit from a single APV in a single channel.
     * \param  delayChip The delay chip containing the APV's 
     *         channel in the buffer.
     * \param  channel Channel in delay chip. 
     * \param  apv The apv in the channel.
     * \return bool The APV error bit.
     */
    bool getAPVErrorBit(u8 delayChip, u8 channel, u8 apv) const;
    /**
     * \brief  Get the number of samples before the begining of the tick mark.
     * \param  delayChip The delay chip number.
     * \param  channel The number of the channel in the delay chip.
     * \return unsigned char The offset (in data words) of the begining of the 
     *         tick mark.
     */
    u8 getChannelTickOffset(u8 delayChip, u8 channel) const;
    /**
     * \brief  Check if the tick marke could be found.
     * \param  delayChip The delay chip number.
     * \param  channel The number of the channel in the delay chip.
     * \return bool Was it found.
     */
    bool tickFound(u8 delayChip, u8 channel) const;
    /**
     * \brief Unimplemented constructors to prevent copying.
     */
    Fed9USpyEvent(const Fed9USpyEvent&);
    Fed9USpyEvent& operator = (const Fed9USpyEvent&);
    
    /**
     * \brief  Class used to signal internal errors.
     */
    struct Fed9USpyEventInternalException {};
    
    u32* buffer;		//!< Buffer which holds data.
    bool manageBuffer;		//!< Bool controlling if buffer should be deallocated.
    std::vector<u16> channelThresholds;	//<! Thresholds for header information (position in vector is internal channel number).
  public:  
    static const u16 spyDelayChipBufferSize = 376; //!< Number of u32s in one delay chip's data.
  protected:
    static const u16 offsetOfSpyData = 44; //!< Offset in bits of first bit of spy data in buffer from FED
    
  };
  
  inline std::ostream& operator<<(std::ostream& os, const Fed9USpyEvent & sev) { sev.print(os); return os; }
  
}
#endif // H_Fed9USpyEvent
