#ifndef H_Fed9USpyApvData
#define H_Fed9USpyApvData
#include "TypeDefs.hh"
#include "Fed9USpyApvDataException.hh"
#include "Fed9UAddress.hh"

namespace Fed9U {
  
  /**
   * \brief Class to hold the data for one APV from the spy channel 
   *        to be used to transfer between xdaq applications.
   */
  class Fed9USpyApvData {
  public:
    
    /**
     * \brief  Default constructor which sets apvNumber to an invalid number.
     */
    Fed9USpyApvData() {}
    /*
     * \brief  Constructor
     * \param  fedId FedId of the FED from which the spy data will come.
     * \param  fedApvNumber Internal FedApv number of the APV from which the spy 
     *                      data will come.
     */
    Fed9USpyApvData(u16 fedId, u8 fedApvNumber) : fedId_(fedId), apv_(fedApvNumber) {}
    /**
     * \brief  Constructor using address for FedApv number
     * \param  fedId FedId of the FED from which the spy data will come.
     * \param  apvAddress Address of APV from which the spy data will come. 
     */
    Fed9USpyApvData(u16 fedId, Fed9UAddress apvAddress) : fedId_(fedId), apv_(apvAddress.getFedApv()) {}
    
    //geters and seters for member variables
    /**
     * \brief  Get FedId of the FED from which the spy data comes.
     */
    u16 getFedId() const { return fedId_; }
    /**
     * \brief  Set FedId of the FED from which the spy data comes.
     * \param  fedId New FedId.
     * \return Self reference.
     */
    Fed9USpyApvData& setFedId(u16 fedId) { fedId_ = fedId; return *this; }
    /**
     * \brief  Get the internal FedApv number of the APV from which the spy 
     *         data comes.
     */
    u8 getApv() const { return apv_; }
    /**
     * \brief  Set the internal FedApv number of the APV from which the spy data
     *         comes.
     * \param  fedApvNumber New internal FedApv number.
     * \return Self reference.
     */
    Fed9USpyApvData& setApv(u8 fedApvNumber) { apv_ = fedApvNumber; return *this; }
    /**
     * \brief  Set the APV from which the spy data comes.
     * \param  apvAddress Address of the APV on the FED.
     * \return Self reference.
     */
    Fed9USpyApvData& setApv(Fed9UAddress apvAddress) { apv_ = apvAddress.getFedApv(); return *this; }
    /**
     * \brief  Get the address of the APV from which the spy data comes.
     * \return Fed9UAddress the APV's address.
     */
    Fed9UAddress getApvAddress() { return Fed9UAddress().setFedApv(apv_); }
    /**
     * \brief  Get a sample from the spy data.
     * \param  index Index of sample in spy data (not apv data frame).
     * \throw  Fed9USpyApvDataException index is out of range.
     * \return The value of the sample in ADC counts. 
     */
    u16 getSample(u16 index) const noexcept(false);
    /**
     * \brief  Set a sample from the spy data.
     * \param  index Index of sample in spy data (not apv data frame).
     * \throw  Fed9USpyApvDataException index is out of range.
     * \return The value of the sample in ADC counts.
     */
    Fed9USpyApvData& setSample(u16 index, i16 value) noexcept(false);
    
    //member functions to extract values from the APV header
    /**
     * \brief  Get the number of samples before the start of the tick mark.
     * \param  threshold The threshold for a high value in the APV header.
     * \throw  Fed9USpyApvDataException Tick mark isn't found.
     * \return Number of samples before the start of the tick mark.
     */
    u8 getTickOffset(i16 threshold) const noexcept(false);
    /*
     * \brief  Get the pipeline address APV header.
     * \param  threshold The threshold for a high value in the APV header.
     * \throw  Fed9USpyApvDataException Tick mark isn't found.
     * \return Pipeline address from the APV header.
     */
    u8 getPipelineAddress(i16 threshold) const noexcept(false);
    /**
     * \brief  Get APV header error bit.
     * \param  threshold The threshold for a high value in the APV header.
     * \throw  Fed9USpyApvDataException Tick mark isn't found.
     * \return True if the error bit was high (ie no error) or 
     *         false if it was low (ie there was an error).
     */
    bool getErrorBit(i16 threshold) const noexcept(false);
    
    
    /**
     * \brief  Get a sample from the spy data without range checking. 
     * \param  index Index of sample in spy data (not apv data frame).
     * \return The value of the sample in ADC counts. 
     */
    u16 operator [] (u16 index) const { return samples_[index]; }
    
    static const int SAMPLES_PER_APV = 150;	//!< Number of samples for each APV.
    
  private:

    u16 fedId_;		//!< FedId of the FED from which the spy data comes.
    u8 apv_;	//!< Internal FedApv number of the APV from which the spy data comes.
    i16 samples_[SAMPLES_PER_APV];	//!< Buffer to hold the samples. 
  };
  
} //namespace Fed9U

#endif //ifndef H_Fed9USpyApvData
