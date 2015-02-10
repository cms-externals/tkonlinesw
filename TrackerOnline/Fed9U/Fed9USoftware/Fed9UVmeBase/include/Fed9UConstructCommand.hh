/**Matthew Pearson, RAL - May 2003*/

/**This class is a utility class used by Fed9UVmeBase. It contains
   methods which are used to produce a vector of integers which can
   be written to the FED. This class should be used by the Fed9UVmeDevice class
   in order to provide the Fed9UHalInterface class with compatible arguments. 
   It also contains a method to unpack the numbers read from the FED into a more manageable format (the
   format which in which the data is passed to the Fed9UVmeBase methods).*/

#ifndef _Fed9UConstructCommand_H_
#define _Fed9UConstructCommand_H_

#include"Fed9UVmeBaseException.hh"
#include"TypeDefs.hh"

#include<vector>

namespace Fed9U {

  //using.*std::vector;

  class Fed9UConstructCommand
  {
  public:
    /**This method takes as input a set of integers which completely specify
       the command to be sent to the FED. These include the FPGA to address (1-9),
       a read/write flag (read=1 for a read), designator (5-bit), length (16-bit)
       and a vector of integers which contain the argument list.
       It also takes in a boolean that if true will left-shift the last element
       of the argument list (so that the MSB is on the left).
       The method modifies a reference to the command vector.*/
    void constructCommand(unsigned short fpga, 
			  bool read, 
			  u32 designator, 
			  u32 length, 
			  std::vector<u32> &arguments,
			  bool leftShift,
			  std::vector<u32> &command) throw (Fed9UVmeBaseException);


    /**This method takes as input a set of integers which completely specify
       the command to be sent to the FED. These include the FPGA to address (1-9),
       a read/write flag (read=1 for a read), designator (5-bit), length (16-bit)
       and an arguments buffer containing the reduced set of data to be written in the serial command.
       The method modifies command and fills it with the complete command.*/
    void constructBlockWriteCommand(unsigned short fpga, 
				    u32 designator, 
				    u32 length, 
				    std::vector<u32> arguments,
				    bool leftShift,
				    u8 * command) throw (Fed9UVmeBaseException);
 

    /**Note: only works for loadSkew and read only commands at the moment.
       It adds the normal command to the CDC part and then the arguments.
       It requires the length of the CDC command, not the length of the total command (27+cdc length).*/
    void constructDelayChipCommand(std::vector<u32> &command,
				   u32 selectedDelayChip,
				   bool read, 
				   u32 designator, 
				   u32 length, 
				   std::vector<u32> arguments) throw (Fed9UVmeBaseException);
    
    /**This method merges the elements of a vector into another, getting rid of any redundant bits.
       It also shifts the MSBs to the left of each element of the result.
       The method takes in a vector of 32-bit words, each one containing an n-bit word.
       The method puts these 32-bit words into a vector of 32-bit words, where each bit is
       filled up (ie's no gaps between the numbers). The last element will contain less than 32-bits
       in most cases. Each element will have the MSB on the left. The decimalLength parameter is 
       the number of bits needed to store the data with no gaps (or redundant bits.).*/
    std::vector<u32> mergeVectorElements(const std::vector<u32> &arguments, 
						   unsigned short decimalLength) throw (Fed9UVmeBaseException);

    /**This method takes the vector of arguments as read from a FED and unpacks it into 
       arbitary sized words. The result is stored in a vector of u32s.
       readArg contains the data read from the FED, argLength is the number of bits
       each element will be, and mask is the 'all 1' representation of argLength (ie. argLength 1's, but
       in decimal format) - this acts as a mask to mask out the unwanted bits.*/
    std::vector<u32> unpackVectorElements(const std::vector<u32> &readArg, 
						    u32 argLength,
						    u32 mask,
						    u32 commandLength) throw (Fed9UVmeBaseException);
  };

}

#endif








