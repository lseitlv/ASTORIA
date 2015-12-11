
/*******************************************************************/
/*     Modbus TCP/IP Application Data Unit (ADU)
/*
/*  - Transaction ID (16 bits): identification field used for transaction pairing
/*  - Protocol ID (16 bits): it is always 0 for Modbus services
/*  - Lenght (16 bits): lenght of modbus packet (max value is 65536)
/*  - Unit ID (8 bits): used to identify a remote server located on a non TCP/IP network (default FF)
/*  - Function Code (8 bits)
/*  - Modbus Data
/*      - Min: 1 byte
/*      - Max: 65527 bytes (65535 bytes - 8 bytes (MBAP Header))
/*      - 65527/47(size packet) = 1394,1914 packets
/*      - Request: nodeId (uint32_t), value (uint16_t)
/* 
/*   
/*
/*
/*******************************************************************/


#ifndef MODBUS_STATION_H
#define MODBUS_STATION_H

#include "scada-protocol.h"


namespace ns3 {

typedef struct ModbusMessageStruct {
	uint32_t nodeId;
	uint16_t value;
} ModbusMessageFrame;

class ModbusProtocol : public ScadaProtocol {

	public:
		ModbusProtocol();
		std::string createRequest(uint16_t functionCode, uint32_t param1);
		std::string createReply(std::string message, std::vector<float> registers, uint32_t nodeId);
		std::vector<std::string> createReply(std::string message, std::vector<float> registers, uint32_t nodeId, std::vector<std::pair<uint32_t, std::pair<std::string, uint16_t>>> extraData);
		std::vector<std::pair<uint32_t,uint16_t>> getMessageData(std::string message);
		std::string createHeader(uint16_t transactionId, uint16_t protocolId, uint16_t length, uint16_t unitId, uint16_t functionCode);
		uint16_t getPacketLength();



	private:
		
		uint32_t numPacketsSent;
		uint8_t headerSize;

		uint16_t h_transactionId;
		uint16_t h_protocolId;
		uint16_t h_length;
		uint16_t h_unitId;
		uint16_t h_functionCode;

		std::string createRequest2(uint16_t functionCode, uint32_t param1);
		
		std::string getInputRegisters(std::string message,std::vector<float> registers);

		std::string removeHeader(std::string message);



		//std::string getInputRegisters(std::string message,std::vector<float> registers);

};

}

#endif
