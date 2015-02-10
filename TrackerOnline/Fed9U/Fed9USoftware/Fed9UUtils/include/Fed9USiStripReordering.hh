#ifndef FED9USISTRIPREORDERING_H
#define FED9USISTRIPREORDERING_H

#include<vector>

namespace Fed9U {

  //using.*std::vector;

  /**
   * \brief  Class which can perform reordering on silicon strip data.
   * \author Matthew Pearson, RAL
   * \date   07/2004
   *
   * The data output by the tracker FED in raw data mode is dis-ordered (ie. not in strip order). This class 
   * provides a method to re-order a FED channel worth of data. There is also a method
   * to do the reverse, which dis-orders the data to emulate the ordering done in the APV MUX chip.
   * The re-ordering algorithm implemented here is the same as the re-ordering algorithm in the FED 9U.
   */
  class Fed9USiStripReordering {

  public:

    /**
     * \brief Virtual destructor.
     */
    virtual ~Fed9USiStripReordering() {}
    
    /**
     * \brief Method which contains the re-ordering algorithm for the APV data frame.
     * \param apvOutput Contains an element for each strip, with the strips in the APV-MUX order, but for a single APV (i.e. no interleaving). Size:128.
     * \param fedOutput Contains an element for each strip, with the strips in the FED output (physical) ordering for a single APV. Should be an empty
     *        vector as the contents will be deleted.
     *
     * The apvOutput vector will contain the dis-ordered (MUX ordered) strip data that will be placed into the fedOutput vector in the reordered (physical)
     * strip ordering, which is returned by reference.
     */
    void reOrderingAlgorithm(const std::vector<unsigned short> &apvOutput, std::vector<unsigned short> &fedOutput);

    /**
     * \brief Method which contains the dis-ordering algorithm for of APV strips.
     * \param apvInput Contains an element for each strip, with the strips in the physical strip ordering. Size:128.
     * \param apvOutput Contains an element for each strip, with the strips in the APV-MUX order. It should be an empty vector as it will be filled with
     *        strip data.
     *
     * The apvInput vector will contain the re-ordered (physical/FED output ordering) strip data, which will be dis-ordered (APV-MUX ordering) and placed
     * in the apvOutput vector. This is returned by reference.
     */
    void disOrderingAlgorithm(const std::vector<unsigned short> &apvInput, std::vector<unsigned short> &apvOutput);

  private:

    static const unsigned short NUMBER_OF_APV_STRIPS = 128; //!< Number of strips on an APV.
 
  };
  
} //End of Fed9U namespace.


#endif //FED9USISTRIPREORDERING_H
