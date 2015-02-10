#include "Fed9USiStripReordering.hh"

#include <stdexcept>

namespace Fed9U {

  //using std::invalid_argument;
  
  const unsigned short Fed9USiStripReordering::NUMBER_OF_APV_STRIPS;


  void Fed9USiStripReordering::reOrderingAlgorithm(const std::vector<unsigned short> &apvOutput, std::vector<unsigned short> &fedOutput) {
    //Check input vector
    if (apvOutput.size()!=NUMBER_OF_APV_STRIPS) {
      throw std::invalid_argument("ERROR. apvOutput is wrong size in Fed9USiStripReordering::reOrderingAlgorithm.");
    }
    
    fedOutput.clear();
    fedOutput.reserve(NUMBER_OF_APV_STRIPS);

    //Calculate the FED re-ordering and transfer data into output vector.
    //In the following the loop i represents the real strip number.
    int newPlace = 0;
    for (int i=0; i<NUMBER_OF_APV_STRIPS; ++i) {
      newPlace = 4*((static_cast<int>((static_cast<float>(i)/8.0)))%4) + static_cast<int>(static_cast<float>(i)/32.0) + 16*(i%8);
      fedOutput.push_back(apvOutput[newPlace]); 
    }
  }
  

  void Fed9USiStripReordering::disOrderingAlgorithm(const std::vector<unsigned short> &apvInput, std::vector<unsigned short> &apvOutput) {
    //Check input vector
    if (apvInput.size()!=NUMBER_OF_APV_STRIPS) {
      throw std::invalid_argument("ERROR. apvInput is wrong size in Fed9USiStripReordering::disOrderingAlgorithm.");
    }
    
    apvOutput.clear();
    apvOutput.reserve(NUMBER_OF_APV_STRIPS);

    //Calculate the channel mapping and transfer data into output vector.
    //In the following the loop i represents the output from the APV.
    int newPlace = 0;
    for (int i=0; i<=(NUMBER_OF_APV_STRIPS-1); ++i) {
      newPlace = (32 * (i%4)) + (8 * static_cast<int>(static_cast<float>(i)/4.0)) - (31 * static_cast<int>(static_cast<float>(i)/16.0));
      apvOutput.push_back(apvInput[newPlace]);   
    }
  }
  
  
} //End of Fed9U namespace.
