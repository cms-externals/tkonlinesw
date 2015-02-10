//Author: Matthew Pearson, RAL
//Date: May 2003

#include "Fed9UConstructCommand.hh"

#include <iomanip>
#include <iostream>

namespace Fed9U {

  //This method takes as input a set of integers which completely specify
  //the command to be sent to the FED. These include the FPGA to address (1-9,10,15),
  //a read/write flag (read=1 for a read), designator (5-bit), length (16-bit)
  //and a vector of integers which contain the argument list.
  //The boolean leftShift determins if we want to left-shift the
  //last element of the argument vector (false=no, true=yes).
  //The method modifies a reference to the command vector.
  void Fed9UConstructCommand::constructCommand(unsigned short fpga, 
					       bool read, 
					       u32 designator, 
					       u32 length, 
					       std::vector<u32> &arguments,
					       bool leftShift,
					       std::vector<u32> &command) throw (Fed9UVmeBaseException)
  {
    try {
      //Check for input errors
      ICUTILS_VERIFYX(command.size()==0,Fed9UVmeBaseException)(command.size()).code(Fed9UVmeBaseException::ERROR_FED9UCONSTRUCTCOMMAND).error().msg("Non-zero length command in Fed9UConstructCommand::constructCommand.");
      if ((!read)||((read)&&((designator==1)&&(fpga!=16)))) { //Check the arguments vector if we are writing or if we are reading and doing the skew command (designator==1 for any fpga other than 16).
	ICUTILS_VERIFYX(arguments.size()!=0,Fed9UVmeBaseException)(arguments.size()).code(Fed9UVmeBaseException::ERROR_FED9UCONSTRUCTCOMMAND).error().msg("Zero length arguments during a WRITE in Fed9UConstructCommand::constructCommand.");
      }
      //the fpga number 9 is used for the sofware trigger 11/08/03 GJR
      ICUTILS_VERIFYX(((fpga>=1)&&(fpga<=10))||(fpga==15)||(fpga==16),Fed9UVmeBaseException)(fpga).code(Fed9UVmeBaseException::ERROR_FED9UCONSTRUCTCOMMAND).error().msg("fpga not valid in Fed9UConstructCommand::constructCommand.");
      
      //If we are writing, bit-shift the last element of the arguments so that MSB is on left.
      if ((!read)&&(leftShift)) {
	if ((length%32)!=0) { //(do not do bit-shift if length is a multiple of 32 bits)
	  arguments[arguments.size()-1] = arguments[arguments.size()-1] << (32-(length%32));
	} 
      }
      
      //Construct the command
      u32 tempCommand = (1<<29) + (fpga<<24) + (1<<23) + (read<<22) + (designator<<17) + (1<<16) + (length);
      command.push_back(tempCommand);
      
      //Add the arguments to the command vector if we are writing. 
      if (!read) {
	std::vector<u32>::const_iterator i;
	for (i=arguments.begin(); i!=arguments.end(); i++) {
	  command.push_back(*i);
	}
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "std::exception caught in Fed9UConstructCommand::constructCommand."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "Unknown exception caught in Fed9UConstructCommand::constructCommand."));
    }
  }
  
  void Fed9UConstructCommand::constructBlockWriteCommand(unsigned short fpga, 
							 u32 designator, 
							 u32 length, 
							 std::vector<u32> arguments,
							 bool leftShift,
							 u8 * command) throw (Fed9UVmeBaseException) 
  {
    try {
      // note that the block commands created at this stage can be very large and would not be useable in one chunck
      // these block commands must be handled by the writeblockSerialCommand() method in the Fed9UHalInterface, where they
      // are chopped up into smaller chuncks and loaded into the fed one at a time.
      //Check for input errors
      ICUTILS_VERIFYX(length!=0, Fed9UVmeBaseException)(length).code(Fed9UVmeBaseException::ERROR_FED9UCONSTRUCTCOMMAND).error().msg("Zero length arguments during a WRITE in Fed9UConstructCommand::constructBlockWriteCommand.");
      
      //the fpga number 9 is used for the sofware trigger 11/08/03 GJR
      ICUTILS_VERIFYX(((fpga>=1)&&(fpga<=10))||(fpga==15)||(fpga==16),Fed9UVmeBaseException)(fpga).code(Fed9UVmeBaseException::ERROR_FED9UCONSTRUCTCOMMAND).error().msg("fpga not valid in Fed9UConstructCommand::constructCommand.");
      
      //Construct the command
      u32 tempCommand = (1<<29) + (fpga<<24) + (1<<23) + (0<<22) + (designator<<17) + (1<<16) + (length);
      u32 numU32sInPayload = ( length / 32 ) + ( ( (length % 32) > 0) ? 1 : 0);
      /*std::cout << "length = " << length
	<< "total argument vector length = " << arguments.size()
	<< "numU32s in command = " << numU32sInPayload
	<<  std::hex << "block command for next write is: " << std::endl; */
      //Add the arguments to the command buffer 
      
      if (leftShift) {

	u32 tempLength=0;

	for (int i=0; i<arguments.size(); i+=numU32sInPayload) {
	  (reinterpret_cast<u32*>(command))[(i/numU32sInPayload)*(numU32sInPayload+1)] = tempCommand;
	  //std::cout << "CMD: " << std::setw(8) << std::setfill('0') << (reinterpret_cast<u32*>(command))[(i/numU32sInPayload)*(numU32sInPayload+1)] << std::endl;
	  tempLength = length;
	  for (u32 k=1; k<=numU32sInPayload ; k++) {
	    u32 lengthOfKthPayloadWord = ( (tempLength >= 32) ? 32 :tempLength );
	    tempLength -= 32;
	    (reinterpret_cast<u32*>(command))[(i/numU32sInPayload)*(numU32sInPayload+1) + k] = arguments[i+(k-1)]<<(32-lengthOfKthPayloadWord); // chuck the damn payload over to the left !!! innit!
	    // std::cout << std::dec << "KthLength = " << lengthOfKthPayloadWord << std::hex << std::endl;
	    //std::cout << "PL" << k << ": " << std::setw(8) << std::setfill('0') << (reinterpret_cast<u32*>(command))[(i/numU32sInPayload)*(numU32sInPayload+1)+k] << std::endl;
	  }
	  
	  
	  
	}
      } else {

	for (int i=0; i<arguments.size(); i+=numU32sInPayload) {
	  (reinterpret_cast<u32*>(command))[(i/numU32sInPayload)*(numU32sInPayload+1)] = tempCommand;
	  //std::cout << "CMD: " << std::setw(8) << std::setfill('0') << (reinterpret_cast<u32*>(command))[(i/numU32sInPayload)*(numU32sInPayload+1)] << std::endl;
	  for (u32 k=1; k<=numU32sInPayload ; k++) {
	    (reinterpret_cast<u32*>(command))[(i/numU32sInPayload)*(numU32sInPayload+1) + k] = arguments[i+(k-1)];
	    //std::cout << "PL" << k << ": " << std::setw(8) << std::setfill('0') << (reinterpret_cast<u32*>(command))[(i/numU32sInPayload)*(numU32sInPayload+1)+k] << std::endl;
	  }
	}
      }

      std::cout << std::dec;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "std::exception caught in Fed9UConstructCommand::constructCommand."));
    }
    catch (...) {
      std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!unknown exception !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "Unknown exception caught in Fed9UConstructCommand::constructCommand."));
    }
  }
  

  //Note: only works for loadSkew and read only commands at the moment.
  //It adds the normal command to the CDC part and then the arguments.
  //It requires the length of the CDC command, not the length of the total command (27+cdc length).
  void Fed9UConstructCommand::constructDelayChipCommand(std::vector<u32> &command,
							u32 selectedDelayChip,
							bool read, 
							u32 designator, 
							u32 length, 
							std::vector<u32> arguments) throw (Fed9UVmeBaseException)
  {
    try {
      //Check for input errors
      ICUTILS_VERIFYX(arguments.size()!=0,Fed9UVmeBaseException)(arguments.size()).code(Fed9UVmeBaseException::ERROR_FED9UCONSTRUCTCOMMAND).error().msg("Zero length arguments in Fed9UConstructCommand::constructCommandCDC.");
      ICUTILS_VERIFYX(command.size()==2,Fed9UVmeBaseException)(command.size()).code(Fed9UVmeBaseException::ERROR_FED9UCONSTRUCTCOMMAND).error().msg("command must contain only 2 elements in Fed9UConstructCommand::constructDelayChipCommand.");

      //Add the command to the end of the last bit of the CDC command.
      //First OR with some static bits.
      command[1] |= 0x90200000;
      //Now OR in the other relevant parts of the command.
      command[1] |= selectedDelayChip<<29;
      command[1] |= read<<27;
      command[1] |= designator<<22;
      //Appears in documentation, but John does not use the bit. Try it if things don't work.
      command[1] |= 1<<21;
      command[1] |= length<<5;

      //In the event that we are writing then we need to add the first 5 bits of the argument vector
      //to the last 5 bits of the current command vector. Then shuffle the remaing argument vector
      //bit along and add them to the end of the command vector.
      if (!read) {
	std::vector<u32>::const_iterator it(arguments.begin());
	//Add the first 5 bits of the arguments to the last 5 of the command.
	command[1] |= (*it) >> (32-5);
	for (; it != arguments.end(); ++it) {
	  //Now shuffle the rest of the argument bit stream along and add to end of command vector.
	  if ( (it+1) != arguments.end() ) {
	    command.push_back( (*it) << 5 | (*(it+1)) >> (32-5)   );
	  } else {
	    command.push_back( (*it) << 5 );
	  }
	}

	// 	//Add the first 5 bits of the argument list onto the 2nd element of command
	// 	command[1] = command[1] + (arguments[0] >> (32-5));
	// 	//Shift the argument list left by 5-bits, then add to the first element of arguments
	// 	//the 2nd element left-shifted by 1 bit (because it is only 4 bits).
	// 	arguments[0] = arguments[0] << 5;
	// 	arguments[0] = arguments[0] + ((arguments[1]>>(32-(length-32))) << (5-(length-32))); //length is actually 37 due to adding 1 in Fed9UVmeBase::feCommandLoadSkew. Therefore arguments[1] is right-shifted by 32-5 and then left shifted by 0. This works anyway, but If we move back to length=36 then we should keep the above line as it was written for the case length=36. For other commands and lengths this will not work.
	//  	//Now add the rest of the argument list to cdcCommand
	//  	command.push_back(arguments[0]);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "std::exception caught in Fed9UConstructCommand::constructCommandCDC."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "Unknown exception caught in Fed9UConstructCommand::constructCommandCDC."));
    }

  }

  //***************************************************************************************************
  //***************************************************************************************************
  //Method which takes in a vector of 32-bit words, each one containing an n-bit word.
  //The method puts these 32-bit words into a vector of 32-bit words, where each bit is
  //filled up (ie's no gaps between the numbers). The last element will contain less than 32-bits
  //in most cases. Each element will have the MSB on the left. The decimalLength parameter is 
  //the number of bits needed to store the data with no gaps (or redundant bits.).
  std::vector<u32> Fed9UConstructCommand::mergeVectorElements(const std::vector<u32> &arguments, 
									unsigned short decimalLength) throw (Fed9UVmeBaseException)
  {
    try {
      //Check for input errors
      ICUTILS_VERIFYX(arguments.size()!=0,Fed9UVmeBaseException)(arguments.size()).code(Fed9UVmeBaseException::ERROR_FED9UCONSTRUCTCOMMAND).error().msg("Zero length arguments in Fed9UConstructCommand::mergeVectorElements.");

      int k=0;
      int elementLength = decimalLength / arguments.size(); //How many bits in each element of arguments?
      int count = 0;
      int offset = 32;
      int countLength = 0;
      int numLastBits = 0;
      std::vector<u32>::const_iterator i;
      std::vector<u32> newArguments(1,0);//Create first element in the vector.

      //Loop over argument elements
      for (i=arguments.begin(); i!=arguments.end(); i++) {
	countLength = countLength + elementLength; //How many bits will we need to accomodate the next element
	if (countLength <= 32) {
	  newArguments[k] = newArguments[k] + (*i << (offset - ((count+1)*elementLength))); //Add number with offset
	  count++; 
	}
	else if (countLength>32) {  //If we need to use more bits than are available in the 32-bit word.
	  numLastBits = countLength - 32;  //How many bits are left in the current 32-bit word?
	  if (numLastBits!=elementLength) { //If there are more than 0 free bits
	    newArguments[k] = newArguments[k] + (*i >> numLastBits); //Fill up the last free bits with the MSBs.
	  }
	  k++; //Next element
	  newArguments.push_back(0); //Create empty element
	  offset = 32 - numLastBits;  //What is the new offset?
	  countLength = numLastBits; //What will be the new starting point in the 32-bit word to count up from?
	  newArguments[k] = newArguments[k] + (*i << offset); //Take the corresponding LSBs and put in next number.
	  count = 0; 
	}
      }

      return newArguments;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "std::exception caught in Fed9UConstructCommand::mergeVectorElements."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "Unknown exception caught in Fed9UConstructCommand::mergeVectorElements."));
    }
  }  

  //This method takes the vector of arguments as read from a FED and unpacks it into 
  //arbitary sized words. The result is stored in a vector of u32s.
  //readArg contains the data read from the FED, argLength is the number of bits
  //each element will be, and mask is the 'all 1' representation of argLength (ie. argLength 1's, but
  //in decimal format) - this acts as a mask to mask out the unwanted bits.
  //  std::vector<u32> Fed9UConstructCommand::unpackVectorElements(const std::vector<u32> &readArg, 
  //								    u32 argLength,
  //								    u32 mask) throw (Fed9UVmeBaseException)
  //  {
  //    try {
  //Check for input errors
  //      ICUTILS_VERIFYX(readArg.size()!=0,Fed9UVmeBaseException)(readArg.size()).code(Fed9UVmeBaseException::ERROR_FED9UCONSTRUCTCOMMAND).error().msg("Zero length readArg in Fed9UConstructCommand::unpackVectorElements.");

  //      unsigned short diff=0, startPoint=32;
  //      u32 temp = 0, count = 0;
  //      int n=0, j=1;
  //      std::vector<u32>::const_iterator i, i2;
  //      std::vector<u32> unpackedArg(1,0); //Create an initial element

  //      for (i=readArg.begin(); i!=readArg.end(); i++) {
  //	do {
  //	  unpackedArg[n] = ((*i >> (startPoint - (argLength * j))) & mask); //Get the number of bits defined by argLength
  //	  std::cout << "1 unpackArg element " << n << " is " << unpackedArg[n] << std::endl;
  //	  j++;                                                                //(mask them to get rid of unwanted bits)
  //	  n++;
  //	  unpackedArg.push_back(0); //Create the next element
  //	} while (startPoint > (argLength * j));
  //	count++; //count up the elements
  //	if (count!=readArg.size()) { //If we are not at the end....
  //	  diff = (argLength*j) - startPoint; //Number of bits in the next 32 bit word in readArg.
  //	  temp = (*i << (32 - (argLength-diff))); //Shift away unwanted bits
  //	  unpackedArg[n] = temp >> (32 - (argLength)); //Put bits in the right place.
  //	  std::cout << "2 unpackArg element " << n << " is " << unpackedArg[n] << std::endl;
  //Now get the remaining bits
  //	  startPoint = 32-diff;
  //	  j = 0; 
  //	  i2 = i;
  //	  if (startPoint!=32) {  //Only do this when startPoint!=32, since shifting only works for 0-31.
  //	    unpackedArg[n] = unpackedArg[n] + (*(++i2) >> startPoint); 
  //	    std::cout << "3 unpackArg element " << n << " is " << unpackedArg[n] << std::endl;
  //	  } 
  //	  n++;
  //	  j++;
  //	  unpackedArg.push_back(0); //Create the next element
  //	}
  //      }
  //
  //      unpackedArg.pop_back();
  //      return unpackedArg;
  //    }
  //    catch (std::exception &e) {
  //      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "std::exception caught in Fed9UConstructCommand::unpackVectorElements."));
  //    }
  //    catch (...) {
  //      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "Unknown exception caught in Fed9UConstructCommand::unpackVectorElements."));
  //    }
  //  }


  std::vector<u32> Fed9UConstructCommand::unpackVectorElements(const std::vector<u32> &readArg,
									 u32 argLength,
									 u32 mask,
									 u32 commandLength) throw (Fed9UVmeBaseException)
  {    
    try {
      //Check for input errors
      ICUTILS_VERIFYX(readArg.size()!=0,Fed9UVmeBaseException)(readArg.size()).code(Fed9UVmeBaseException::ERROR_FED9UCONSTRUCTCOMMAND).error().msg("Zero length readArg in Fed9UConstructCommand::unpackVectorElements.");
      std::vector<u32> unpackedArg;
      u32 numberOfUnpackedElements = 0;
      u32 numberOfSpareBits = 0;
      u32 startPoint = 0;
      
      u32 totalNumberOfUnpackedElements = commandLength/argLength;
      u32 bitsLeftOver = (32 * readArg.size()) % commandLength;
      
      // loop over the packed vector
      for (u32 loop = 0; loop < readArg.size(); loop++) {
	numberOfUnpackedElements = (32 - startPoint)/argLength;
	numberOfSpareBits = (32 - startPoint) % argLength;
	
	for (u32 unpack = 0; unpack < numberOfUnpackedElements; unpack++) {
	  unpackedArg.push_back((readArg[loop] >> (32 - startPoint - (argLength*(unpack+1)))) & mask);
	}
	
	startPoint = 0;
	// deal with join between packed vector elements in case length is not a factor of 32
	if (numberOfSpareBits != 0) {
	  // end of element
	  u32 endOfArg = ((readArg[loop] << (32 - numberOfSpareBits)) >> (32 - numberOfSpareBits));
	  // start of next element
	  u32 startOfNewArg = (readArg[loop+1] >> (32 - (argLength - numberOfSpareBits)));
	  startPoint = argLength - numberOfSpareBits;
	  // glue the two pieces together and store
	  unpackedArg.push_back((endOfArg << (argLength - numberOfSpareBits)) | startOfNewArg);
	}
      }
      
      //  padding happens around LSB, so tidy up ...
      if (bitsLeftOver != 0) {
	if (unpackedArg.size() == totalNumberOfUnpackedElements) {
	  unpackedArg.back() = unpackedArg.back() >> bitsLeftOver;
	} else {
	  while (unpackedArg.size() > totalNumberOfUnpackedElements) {
	    unpackedArg.pop_back();
	  }
	}
      }
      
      return unpackedArg;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "std::exception caught in Fed9UConstructCommand::unpackVectorElements."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "Unknown exception caught in Fed9UConstructCommand::unpackVectorElements."));
    }  
  }
}













