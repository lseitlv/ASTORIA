

#ifndef SCADA_PROTOCOL_H
#define SCADA_PROTOCOL_H

#include <vector>
#include <unistd.h> 
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include <iomanip>


namespace ns3{

class ScadaProtocol {
	
	public:
		ScadaProtocol();
		virtual std::string createRequest(uint16_t functionCode, uint32_t param1);
		virtual std::string createReply(std::string message, std::vector<float> registers, uint32_t nodeId);
		virtual std::vector<std::string> createReply(std::string message, std::vector<float> registers, uint32_t nodeId, std::vector<std::pair<uint32_t, std::pair<std::string, uint16_t>>> extraData);
		virtual std::vector<std::pair<uint32_t,uint16_t>> getMessageData(std::string message);
		virtual uint16_t getPacketLength();
		

};

}

#endif
