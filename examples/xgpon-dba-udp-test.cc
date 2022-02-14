/* Version Control:
 * V1: Creating the comibination of udp traffic and GIANT MAC via XGPON_GIANT_BW_ASSURED
 */


/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c)  2012 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Xiuchao Wu <xw2@cs.ucc.ie>
 * CoAuthor: Jerome A Arokkiam <jerom2005raj@gmail.com>
 */

/**********************************************************************
*
* This script will create a client and server application running in the OLT and the ONU respectively. 
* These ONUs and the OLT will be in the same subnet; to make end-to-end network similar to a practical network, a metro router is added between core and OLT
* Logical Network setup:
* serverNodes <---> routerNode <---> xgponNodes <---> userNodes
*  > CoreDevices (server + router)
*  > MetroDevices (router + OLT)
*  > LastmileDevices (ONU + userNode)
*
* More details of this XG-PON module can be found at the reference:
* Arokkiam, J. A., Alvarez, P., Wu, X., Brown, K. N., Sreenan, C. J., Ruffini, M., … Payne, D. (2017). Design, implementation, and evaluation of an XG-PON module for the ns-3 network simulator. SIMULATION, 93(5), 409–426. https://doi.org/10.1177/0037549716682093
*
* The XG-PON code can be downloaded at https://sourceforge.net/projects/xgpon4ns3/
**************************************************************/

#include <ctime>
#include <iostream>
#include <fstream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/object-factory.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/packet-sink.h"
#include "ns3/tcp-socket.h"

#include "ns3/xgpon-helper.h"
#include "ns3/xgpon-config-db.h"

#include "ns3/xgpon-channel.h"
#include "ns3/xgpon-onu-net-device.h"
#include "ns3/xgpon-olt-net-device.h"

#include "ns3/xgpon-module.h"

//Stats module for tracing
#include "ns3/stats-module.h"



#define APP_START 0
#define APP_STOP 10 // for total no of packets > (20 * 6x10^5)  
#define SIM_STOP 12

static uint16_t nOnus = 1; //defined in the global scope
static uint64_t totalRxFromXgponBytes = 0;
  
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("xgpon-dba-udp-test");


/*static void
TracePacketSinkRx (std::string context, uint64_t totalRxBytes, uint64_t milliSecondsPassed,  Ptr<const Socket> appSocket)
{
  std::cout << milliSecondsPassed << ",MilliSeconds" << ",AppSinkTotalRx," << totalRxBytes*8 << ",bits,fromNodeId," << (appSocket->GetNode())->GetId() << std::endl;
}
*/

void
DeviceStatisticsTrace (const XgponNetDeviceStatistics& stat)
{
  static uint64_t time2print = 100000000;    //1,000,000,000 nanoseconds per second.
  if(stat.m_currentTime > time2print)
  {
	std::cout << "stat.m_currentTime: " << stat.m_currentTime << std::endl;
	for(uint16_t i = 0; i < nOnus; i++)
		{ 
			totalRxFromXgponBytes += stat.m_rxFromXgponBytes[i]; 
			std::cout << (stat.m_currentTime / 1000000L) << ",ms," 
					<< "ONU," << i << ",total-Upstream," << stat.m_rxFromXgponBytes[i] << "," 
					<< "alloc," << i+1024 << ",T1US," << stat.m_rxT1FromXgponBytes[i] << "," 
					<< "alloc," << i+2048 << ",T2US," << stat.m_rxT2FromXgponBytes[i] << "," 
					<< "alloc," << i+3072 << ",T3US," << stat.m_rxT3FromXgponBytes[i] << "," 
					<< "alloc," << i+4096 << ",T4US," << stat.m_rxT4FromXgponBytes[i] << "," 
					<< "Total-Xgpon-Upstream," << totalRxFromXgponBytes << ",(Bytes)" << std::endl;
					
		}
		time2print += 100000000;
  }
}


int 
main (int argc, char *argv[])
{

  /////////////////////////////////////SETTING DYNAMIC PARAMETERS FOR THE OVERALL SIMULATION
  //these are example values that may need to be changed often, and may impact multiple configuration locations in this example file. 
  
  //p2p delay beween diffent network segments used in this example. This is not the XG-PON OLT-ONU delay. For setting the XG-PON propagation dealy, one should look at xgpon-channel.h 
  std::string p2pDelay = "1ms"; 
  
  //queue size for the net devices used in this example. Per AllocID Queues needs to be set at xgpon-queue.cc
  //uint16_t dtqSize=800; 
  
  //ratio of the load over the XG-PON against the XG-PON capacity. This is a useful parameter when running simulations for different network load
  double load=1.5; 

  //service interval for GIR. 
  uint32_t siValue=1;

  //DBA mechanism to be used for upstream bandwidth allocation
  std::string xgponDba = "ns3::XgponOltDbaEngineXgiant"; 
  /*
   * DBA mechanisms that can be used are:
      ns3::XgponOltDbaEngineRoundRobin  
      ns3::XgponOltDbaEngineGiant
      ns3::XgponOltDbaEngineEbu
      ns3::XgponOltDbaEngineXgiant
      ns3::XgponOltDbaEngineXgiantDeficit
      ns3::XgponOltDbaEngineXgiantProp
      
      NOTE: RoundRobin is a non-QoS aware DBA mechanism (therefore, non XG-PON compliant). This DBA is there for verification purposes.
      Parameters in section 'CONFIGURATIONS FOR QOS-AWARE DBAS ONLY' do not impact the behaviour of RoundRobin DBA.
      The rest of the DBAs are QoS-aware DBA mechanisms (section CONFIGURATIONS FOR QOS-AWARE DBAS ONLY is responsible only for the outcome of these QoS-aware DBAs)
      Also, even though the DeviceStatisticsTrace above gives zeros values for datarates from three other TCONT types for RoundRobin, one should note that RoundRobin is strictly a non-QoS aware DBA.
      One can look at the relevant implemenation files of RoundRobin and Xgiant to see the difference in implementations between a non-QoS aware and QoS-aware DBAs.
  */
  
  CommandLine cmd;
  //COMMAND LINE OPTIONS FOR THE USER TO OVERRIDE THE DEFAULT OPTIONS ABOVE
  //An example is given below. Additional values such as dtqSize, siValue, xgponDba, and any other, can be added using cmd.AddValue, if need be.
  cmd.AddValue ("load", "Percentage of load for data through xg-pon (default 1.0)", load);
  
  cmd.Parse (argc, argv);
  
  ////////////////////////////////////////////////////////CONFIGURATIONS FOR QOS-AWARE DBAS ONLY
  
  /* 2.15 is the ideal max capacity, as the DBAoverhead(~4Mbps) + datarate(692Mbps) for T4 equals that of T2 and T3 datarates(696.7Mbps) individually. 
   * The additional ~0.7Mbps can be attributed to XGPON overhead in the seperate grant given to T4 every cycle, in the firstT3round
   * One is allowed to set higher values here, but the outcome will be an overprovisioning DBA mechanism. 
   * Note that this configuration is only for DBA algorithm operation, and does not impact the actural capacity of XG-PON which is controlled through how much of bytes packed in each XGPON frame.
   * Indirectly for Upstream however, DBA controlls how much payload goes into each Upstream frame. Hence one must take care in altering values set here when using QoS-aware DBAs. 
   * RoundRobin DBA is not affected by these values.
   */
  double maxUSbandwidth = 2.15; //in Gbps
  
  std::vector<uint64_t> fixedBw(nOnus);
  std::vector<uint64_t> assuredBw(nOnus);
  std::vector<uint64_t> nonAssuredBw(nOnus);
  std::vector<uint64_t> bestEffortBw(nOnus);
  std::vector<uint64_t> maxBw(nOnus);
  std::vector<uint16_t> siMax(nOnus);
  std::vector<uint16_t> siMin(nOnus);

  //these are aggregate network load values for each TCONT type for the entire XG-PON.
  //each tcont in each ONU will get its propotional value, as can be seen in the corresponding DBA implementation files
  uint32_t fixedBwValue=0.0*maxUSbandwidth*1e9; //unit: bps, e.g. 0.7*maxUSbandwidth*1e9 means 0.7*2.15Gbps
  uint32_t assuredBwValue=0.7*maxUSbandwidth*1e9;	
  uint32_t nonAssuredBwValue=0.8*maxUSbandwidth*1e9;	
  uint32_t bestEffortBwValue=0.67*maxUSbandwidth*1e9;

  for(uint32_t i=0; i<nOnus; i++)
  {
    fixedBw[i]=fixedBwValue/nOnus;
    assuredBw[i]=assuredBwValue/nOnus;
    nonAssuredBw[i]=nonAssuredBwValue/nOnus;
    bestEffortBw[i]=bestEffortBwValue/nOnus;
    siMax[i]=siValue;
    siMin[i]=2*siValue; //conversion from GIR to PIR service interval
  }
  
  //////////////////////////////////////////////////////////////////////////CONFIGURATIONS FOR XGPON HELPER
  XgponHelper xgponHelper;
  XgponConfigDb& xgponConfigDb = xgponHelper.GetConfigDb ( );

  xgponConfigDb.SetOltNetmaskLen (8);
  xgponConfigDb.SetOnuNetmaskLen (24);
  //XGPON IPBASE
  xgponConfigDb.SetIpAddressFirstByteForXgpon (10);
  //LASTMILE IPBASE
  xgponConfigDb.SetIpAddressFirstByteForOnus (173);
  xgponConfigDb.SetAllocateIds4Speed (true);
  xgponConfigDb.SetOltDbaEngineTypeIdStr (xgponDba); 
  
  //Set TypeId String and other configuration related information through XgponConfigDb before the following call.
  xgponHelper.InitializeObjectFactories ( );


  ////////////////////////////////////////////////////////CONFIGURATIONS FOR POSSIBLE DATARATE VALUES FOR P2P LINKS AND APPLICATION (NOT FOR XGPON)
  
  double metroNodesP2PDataRate = maxUSbandwidth*5; // XG-PON is the only bottleneck; hence the rest of the network capacity and the application traffic are overprovisioned
  std::stringstream metroNodesP2PDataRateString;
  metroNodesP2PDataRateString << metroNodesP2PDataRate << "Gbps";
  //p2p datarate for core nodes
  double corePerNodeP2PDataRate = metroNodesP2PDataRate / nOnus;
  std::stringstream corePerNodeP2PDataRateString;
  corePerNodeP2PDataRateString << corePerNodeP2PDataRate << "Gbps";
  //p2p datarate for and lastmile nodes
  double lastmilePerNodeP2PDataRate = metroNodesP2PDataRate / nOnus; 
  std::stringstream lastmilePerNodeP2PDataRateString;
  lastmilePerNodeP2PDataRateString << lastmilePerNodeP2PDataRate << "Gbps";
  //siValue and load are necessesary multipliers
  double perAppDataRate = siValue*maxUSbandwidth*load/(nOnus*3);
  std::stringstream perAppDataRateString;
  perAppDataRateString << perAppDataRate << "Gbps";
  std::cout << "datarate(Gbps),metro," << metroNodesP2PDataRate << ",core," << corePerNodeP2PDataRate << ",lastmile," << lastmilePerNodeP2PDataRate << ",perApplication," << perAppDataRate << std::endl;
  

  //////////////////////////////////////////CREATE the ONU AND OLT NODES
  Packet::EnablePrinting ();
  
  NodeContainer oltNode, onuNodes;
  //uint16_t nOnus = noOfOnus;  //Number Of Onus in the PON
  oltNode.Create (1);
  onuNodes.Create (nOnus);

  NodeContainer xgponNodes;
  xgponNodes.Add(oltNode.Get(0));
  for(int i=0; i<nOnus; i++) { xgponNodes.Add (onuNodes.Get(i)); }
  
  //0: olt; i (>0): onu
  NetDeviceContainer xgponDevices = xgponHelper.Install (xgponNodes);

  //////////////////////////////////////////SETTING UP OTHER P2P LINKS IN THE NETWORK
  //ARCHITECTURE:
  //	     CORE-------METRO-------------LASTMILE
  // [SERVER]----[ROUTER]---[OLT]-------[ONU--USER]
  //                       	+-------[ONU--USER]  
  NodeContainer serverNodes, userNodes, routerNode;
  userNodes.Create (nOnus);
  serverNodes.Create (nOnus);
  routerNode.Create(1);
  
  NetDeviceContainer p2pLastmileDevices[nOnus], p2pMetroDevices, p2pCoreDevices[nOnus];
  NodeContainer p2pUserNodes[nOnus], p2pMetroNodes, p2pCoreNodes[nOnus];
  Ipv4InterfaceContainer p2pLastmileInterfaces[nOnus], p2pMetroInterfaces, p2pCoreInterfaces[nOnus];
  std::vector<uint16_t > allocIdList;


  PointToPointHelper pointToPoint;

  //set the maximum no of packets in p2p tx queue
  //Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue(800));
  pointToPoint.SetChannelAttribute ("Delay", StringValue (p2pDelay));
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue(lastmilePerNodeP2PDataRateString.str()));
  //lastmile containers
  for(int i=0; i<nOnus; i++)
  {
    p2pUserNodes[i].Add(onuNodes.Get(i));
    p2pUserNodes[i].Add(userNodes.Get(i));
    p2pLastmileDevices[i] = pointToPoint.Install (p2pUserNodes[i]);
  }
  
  
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue(corePerNodeP2PDataRateString.str()));
  //core containers
  for(int i=0; i<nOnus; i++) 
  { 
    p2pCoreNodes[i].Add(serverNodes.Get(i));
    p2pCoreNodes[i].Add(routerNode.Get(0));
    p2pCoreDevices[i] = pointToPoint.Install(p2pCoreNodes[i]);
  }
   
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue(metroNodesP2PDataRateString.str()));
  //metro containers
  //Config::SetDefault ("ns3::PointToPointNetDevice::Mtu", UintegerValue (1500)); //default is 1500
  p2pMetroNodes.Add(routerNode.Get(0));
  p2pMetroNodes.Add(oltNode.Get(0));
  p2pMetroDevices = pointToPoint.Install (p2pMetroNodes);

  //INSTALL INTERNET PROTOCOL STACK
  InternetStackHelper stack;
  stack.Install (xgponNodes);
  stack.Install (routerNode);
  stack.Install (serverNodes);
  stack.Install (userNodes);
      
  /////////////////////////////////////////////////////ASSIGN IP ADDRESSES
  
  //Ip addresses for OLT and ONUs
  Ipv4AddressHelper addressHelper;
  Ptr<XgponOltNetDevice> tmpDevice = DynamicCast<XgponOltNetDevice, NetDevice> (xgponDevices.Get(0));
  std::string xgponIpbase = xgponHelper.GetXgponIpAddressBase ( );
  std::string xgponNetmask = xgponHelper.GetOltAddressNetmask();
  addressHelper.SetBase (xgponIpbase.c_str(), xgponNetmask.c_str());
  std::cout << "xgponIpbase:" << xgponIpbase << ", oltNetMask:" << xgponNetmask << std::endl;


  Ipv4InterfaceContainer xgponInterfaces = addressHelper.Assign (xgponDevices);
  for(int i=0; i<(nOnus+1);i++)
  {
    Ptr<XgponNetDevice> tmpDevice = DynamicCast<XgponNetDevice, NetDevice> (xgponDevices.Get(i));
    Ipv4Address addr = xgponInterfaces.GetAddress(i);
    tmpDevice->SetAddress (addr);
    if(i==0)
      {std::cout << "olt addr: " ; addr.Print(std::cout); std::cout << std::endl;}
    else
      {std::cout << "onu " << i-1 << " addr:" ; addr.Print(std::cout); std::cout << std::endl;}
  }
 
  Ipv4Address addr;
  
  //assign ip address for metro interfaces
  std::string metroIpbase = xgponHelper.GetIpAddressBase (172, 0, 24);
  std::string metroNetmask = xgponHelper.GetIpAddressNetmask (24); 
  addressHelper.SetBase (metroIpbase.c_str(), metroNetmask.c_str());
  p2pMetroInterfaces = addressHelper.Assign (p2pMetroDevices);

  //assign ip address for core and lastmile interfaces
  for(int i=0; i<nOnus; i++) 
  { //core interfaces
    std::string serverIpbase = xgponHelper.GetIpAddressBase (171, i, 24);
    std::string serverNetmask = xgponHelper.GetIpAddressNetmask (24); 
    addressHelper.SetBase (serverIpbase.c_str(), serverNetmask.c_str());
    p2pCoreInterfaces[i] = addressHelper.Assign (p2pCoreDevices[i]);
    //lastmile interfaces
    Ptr<XgponOnuNetDevice> tmpDevice = DynamicCast<XgponOnuNetDevice, NetDevice> (xgponDevices.Get(i+1));
    std::string onuIpbase = xgponHelper.GetOnuIpAddressBase (tmpDevice);
    std::string onuNetmask = xgponHelper.GetOnuAddressNetmask();     
    addressHelper.SetBase (onuIpbase.c_str(), onuNetmask.c_str());
    p2pLastmileInterfaces[i] = addressHelper.Assign (p2pLastmileDevices[i]);
  }

  ////////////////////////////////////////////////////CONFIGURE THE XGPON WITH QOS CLASSES, UPSTREAM/DOWNSTREAM CONNECTIONS AND IP-ALLOCID-PORTID-TCONT MAPPINGS
  Ptr<XgponOltNetDevice> oltDevice = DynamicCast<XgponOltNetDevice, NetDevice> (xgponDevices.Get(0));
		oltDevice->TraceConnectWithoutContext ("DeviceStatistics", MakeCallback(&DeviceStatisticsTrace));
	//add xgem ports for user nodes connected to ONUs
  for(int i=0; i< nOnus; i++) 
  {
    xgponHelper.SetQosParametersAttribute ("FixedBandwidth", UintegerValue (fixedBw[i]) );
    xgponHelper.SetQosParametersAttribute ("AssuredBandwidth", UintegerValue (assuredBw[i]) );
    xgponHelper.SetQosParametersAttribute ("NonAssuredBandwidth", UintegerValue(nonAssuredBw[i]) );
    xgponHelper.SetQosParametersAttribute ("BestEffortBandwidth", UintegerValue (bestEffortBw[i]) );
    xgponHelper.SetQosParametersAttribute ("MaxServiceInterval", UintegerValue (siMax[i]));
    
    xgponHelper.SetQosParametersAttribute ("MinServiceInterval", UintegerValue (siMin[i]));
		
		//only tconts 2-4 are here.
    for (int tcont=0; tcont<4; tcont++)
    {
      Address addr = p2pLastmileInterfaces[i].GetAddress(1);
      Ptr<XgponOnuNetDevice> onuDevice = DynamicCast<XgponOnuNetDevice, NetDevice> (xgponDevices.Get(i+1));

      XgponQosParameters::XgponTcontType tcontType = static_cast<XgponQosParameters::XgponTcontType>(tcont+1);
      uint16_t allocId = xgponHelper.AddOneTcontForOnu (onuDevice, oltDevice, tcontType);
      uint16_t upPortId = xgponHelper.AddOneUpstreamConnectionForOnu (onuDevice, oltDevice, allocId, addr);
      uint16_t downPortId = xgponHelper.AddOneDownstreamConnectionForOnu (onuDevice, oltDevice, addr); 

      std::cout << "ONU-ID = "		<< onuDevice->GetOnuId() 
			<< "\tALLOC-ID = " 	<< allocId 
  		<< "\tUP-PORT-ID= " 	<< upPortId 
  		<< "\tDOWN-PORT-ID = " 	<< downPortId
			<< "\tTCONT-"		<< tcontType << std::endl;

  
      allocIdList.push_back(allocId);
    }
  }
    std::cout << std::endl;
    std::cout << "===========================================================================================" << std::endl;

  //POPULATE ROUTING TABLE
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  //SET-UP APP SINKS AT THE RECEIVERS FOR DATA RX
  uint16_t serverPort = 9000; //9002-9004, for tcont2,3,4
  PacketSinkHelper sinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), serverPort));
  ApplicationContainer sinkApp = sinkHelper.Install (serverNodes);
  sinkApp.Start (Seconds (0.001));
  sinkApp.Stop (Seconds (APP_STOP + 0.1));

  //SET-UP APP SOURCES AT THE TRANSMITTTER
  /* IN THIS CASE THE USER NODE TRANSMITS CONTINUOUS AND DETERMINISTIC TRAFFIC AND THE SERVER NODE RECEIVES, FOR UPSTREAM TRAFFIC
   * FOR DOWNSTREAM TRAFFIC, SERVER NODE SHOULD TRANSMIT AND USER NODE SHOULD RECEIVE
   *
   * Note that the output from DeviceStatisticsTrace will show only traffic upstream in T4 TCONT type. 
   * This is because there is a fixed TCONT (T4) TYPE used for the connection in xgpon-onu-net-device.cc.
   * In order to SEE data traffic across all four TCONT TYPES
   * 1. Create four (UDP) flows with different serverPort, in this example file
   * 2. In file /src/internet/module/udp-socket-impl.cc, map the four serverPort (assuming serverPort values 9001, 9002, 9003 and 9004) to four different DSCP Values (assuming DSCP values 0x00, 0x20, 0x40, and 0x60). An example code is given below:
    
      uint8_t dscpVal=0x00;
		
			if (port == 9001)
				dscpVal = 0x00; //DSCP_CS1
			else if (port == 9002)
				dscpVal = 0x20; //DSCP_CS1
			else if (port == 9003)
				dscpVal = 0x40; //DSCP_CS2
			else if (port == 9004)
				dscpVal = 0x60; //DSCP_CS3
	
			SocketIpTosTag lteTosTag;
			lteTosTag.SetTos(dscpVal);
			p->AddPacketTag (lteTosTag);
		
	 * 3. In file xgpon-onu-net-device.cc, enable the example code for converting DSCP to TCONT type.
	 * 4. Now you can see DeviceStatisticsTrace showing traffic in all four TCONT types
	 */ 
  for(int i=0; i<nOnus; i++)
  {
    OnOffHelper onOff ("ns3::UdpSocketFactory", InetSocketAddress (p2pCoreInterfaces[i].GetAddress (0), serverPort));
    onOff.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    onOff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onOff.SetAttribute ("DataRate", DataRateValue(DataRate(perAppDataRateString.str())));
    onOff.SetAttribute ("PacketSize", UintegerValue(1447));
    onOff.SetAttribute ("MaxBytes", UintegerValue(0));
    ApplicationContainer userApp = onOff.Install (userNodes.Get (i));
    userApp.Start (Seconds (0.002));
    userApp.Stop (Seconds (APP_STOP));
  }


  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  //TRACING
  //pointToPoint.EnablePcap("p2p-user-pcap", userNodes);
  //pointToPoint.EnablePcap("p2p-metro-pcap", p2pMetroNodes);
  //pointToPoint.EnablePcap("p2p-server-pcap", serverNodes);

  Simulator::Stop(Seconds(APP_STOP + 0.2));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;


}
