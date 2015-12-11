#include "MTU.h"

#define PORTA_SRV 2023 // porta do servidor
#define STR_IPSERVER "127.0.0.1"

namespace ns3 {

Server::Server(std::vector<std::pair<uint32_t, std::pair<std::string,Address> >> t, const Address &l, uint32_t nodeId, std::string nodeName, uint32_t interval,std::string protocolType):ScadaApplication(l,nodeId,nodeName){

    m_interval = interval;
    m_local = l;
    targets=t;  // deves ser algo como os possíveis destinos pra onde ele encaminharia os dados
    m_nextStation=0;
    packetsSent = 0;
    m_local = l;

    if(protocolType.compare("Modbus")==0)
    	messageProtocol = new ModbusProtocol();

}

Server::~Server() {
	
	m_socket=0;
	
}

void Server::StartApplication(void) {

	for(uint32_t i=0;i<targets.size();++i){
		Ptr<Socket> newSocket = Socket::CreateSocket (GetNode(),TcpSocketFactory::GetTypeId());
		newSocket->Bind();
		newSocket->Connect(targets.at(i).second.second);
		m_socketSendList.push_back(newSocket);
	  	
		stationData.push_back( new TransmissionData(m_interval,targets.at(i).second.first,targets.at(i).first));	  
	}
	
	ScadaApplication::StartApplication();
	
	ScheduleNextTx();
}

void Server::StopApplication() {

	ScadaApplication::StopApplication();

	if (sendEvent.IsRunning()){
		Simulator::Cancel (sendEvent);
	}

	for(uint32_t i=0;i<m_socketSendList.size();++i)
		m_socketSendList.at(i)->Close();  

	m_socketSendList.clear();
}

void Server::HandleRead(Ptr<Socket> sock) {

	ScadaApplication::HandleRead(sock);

	if(packetDataSize>0) {
/*
		// TODO Em algum lugar dessa caralha, pega os dados que precisa e envia pela rede pra aplicação que tá esperando
		// 	- precisa incluir a bib de socket
		//	- pode ser uma boa fazer um outro método privte pra lidar com isso e não piorar esse macarrão
		struct sockaddr_in peer;
		int peerlen;
		char buffer[100];
	 
		// Cria o socket na familia AF_INET (Internet) e do tipo UDP (SOCK_DGRAM)
		int s = socket(AF_INET, SOCK_DGRAM, 0);
		if(s >= 0) {

			// Cria a estrutura com quem vai conversar 
			peer.sin_family = AF_INET;
			peer.sin_port = htons(PORTA_SRV);
			peer.sin_addr.s_addr = inet_addr(STR_IPSERVER); 
			peerlen = sizeof(peer);
	
			// Envia pacotes Hello e aguarda resposta
			strcpy(buffer,"Hello");
			sendto(s, buffer, sizeof(buffer), 0, (struct sockaddr *)&peer, peerlen);
			close(s);
		}
		// ----------------------------------------------------

*/
		//std::cout << "MTU packet: " << std::string((char*)packetData) << std::endl;
		std::vector<std::pair<uint32_t,uint16_t>> data = messageProtocol->getMessageData(std::string((char*)packetData));
		std::string nodeName = "";
		uint32_t nodeId;
		for( uint32_t i = 0; i<targets.size(); ++i) {
			if(InetSocketAddress::ConvertFrom(targets.at(i).second.second).GetIpv4()==InetSocketAddress::ConvertFrom(from).GetIpv4()){
				nodeName = targets.at(i).second.first;
				nodeId = targets.at(i).first;
				//std::cout << "(MTU-R-RTU) At time " << Simulator::Now().GetSeconds() << "s Server received data from " << nodeName << " " << (InetSocketAddress::ConvertFrom (targets.at(i).second.second)).GetIpv4() <<" !" << std::endl;
				i = targets.size();
			}	
		}

		for(TransmissionData* tData : stationData) {
			if(tData->nodeName == nodeName) {
				tData->addTransmissionData(Simulator::Now().GetSeconds(), data.at(0).second);
				break;
			}
		}

		// Sensors data
		for(int j = 1; j < data.size(); j++) {

			bool find = false;

			//std::cout << j << " " << data[j].first << " " << data[j].second << std::endl;

			for(TransmissionData* sData : sensorData) {
				if(sData->getNodeId() == data[j].first) {
					sData->addTransmissionData(Simulator::Now().GetSeconds(), data.at(j).second);
					find = true;
					break;
				}
			}

			if(!find) {
				sensorData.push_back(new TransmissionData(m_interval,std::to_string(data[j].first),data[j].first));	
			}

		}

	}

}

void Server::ScheduleNextTx() {
	
    	sendEvent = Simulator::Schedule(Seconds(m_interval),		
                                    &Server::Send,
                                    this);
}

void Server::Send() {

	std::string message = messageProtocol->createRequest(4, packetsSent);//functionCode = 4 e qual Reg lê, no caso o ultimo

	m_data = (uint8_t*) message.c_str();
	packetSize=message.length();
	Ptr<Packet> p =Create<Packet>(m_data,packetSize);
	
	for(uint32_t i = 0; i<m_socketSendList.size(); ++i)
		m_socketSendList.at(i)->Send(p);		//Broadcast
	
	Address addr;
	m_socketSendList.at(0)->GetSockName (addr);
	/*std::cout<<"(MTU-S-RTU) At time " << Simulator::Now().GetSeconds()
		<< "s Server "<<(InetSocketAddress::ConvertFrom (addr)).GetIpv4()
		<< " sent "<<  p->GetSize () 
		<< " bytes to ALL Stations "
		<<std::endl;
*/
	packetSize=0;
	packetsSent++;

	//if(++m_numPacketsSent*Interval<SimTime)  por enquanto tirei limtação
		ScheduleNextTx();

	if(++m_nextStation==targets.size())
		m_nextStation=0;
}

void Server::SendBroadcast() {	// Broadcast do Modbus não tem resposta
					// para pegar dados de todos ->fazer get individual

}

std::vector<TransmissionData*> Server::getStationData(){ return stationData; }

std::vector<TransmissionData*> Server::getSensorData(){ return sensorData; }


}
