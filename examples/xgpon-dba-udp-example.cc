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
 * Author: Jerome A Arokkiam
 * Co-Authors in earlier versions of the code: Xiuchao Wu, Pedro Alvarez
 */

/**********************************************************************
* This example simulates application traffic from servers (traffic sources) to clients (traffic sinks), between which an XG-PON network lies.
* Depending on where the application servers/clients are placed, the traffic can be considered downstream/upstream through XG-PON
  - upstream (servers closer to the ONU side and clients near the OLTs)
  - downstream (servers closer to the OLT side and clients near the ONUs)
* Logical Network setup for upstream:
    serverNode1 |                                |--- ONU1------ userNodes1
                ======== routerNode ===== OLT-----
    serverNode2 |                                |--- ONU2------ userNodes2

*  CoreDevices (serverNode + routerNode)
*  MetroDevices (routerNode + OLT)
*  LastmileDevices (ONU + userNode)
*
*
* More details of the XG-PON module (equivalent to v2 of the code in sourceforge) can be found at the reference:
* Arokkiam, J. A., Alvarez, P., Wu, X., Brown, K. N., Sreenan, C. J., Ruffini, M., … Payne, D. (2017). Design, implementation, and evaluation of an XG-PON module for the ns-3 network simulator. SIMULATION, 93(5), 409–426. https://doi.org/10.1177/0037549716682093
*
* The latest version (v3) of the XG-PON code can be downloaded at https://github.com
* The earlier versions (v2 and v1) of the XG-PON code can be downloaded at https://sourceforge.net/projects/xgpon4ns3/
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

//global parameters
static const uint16_t nOnus = 5; //number of ONUs to be used in the XGPON
static const uint32_t timeIntervalToPrint = 100000000; //1,000,000,000 nanoseconds per second; so this prints the traces every 100ms
static uint64_t time2printOnu[nOnus] = {timeIntervalToPrint};
static uint64_t time2printOlt = timeIntervalToPrint; 
static uint16_t incrementingOnuId = 0;
static uint64_t totalOnuReceivedBytes = 0;
static const double XGPON_UPSTREAM_CAPACITY = 2.24; // unit: Gbps
static const double XGPON_DOWNSTREAM_CAPACITY = 9.9; // unit: Gbps

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("xgpon-dba-udp-test");


//trace at the OLT for upstream (counters are updated at the receiver)
void
DeviceStatisticsTrace (const XgponNetDeviceStatistics& stat)
{
  if(stat.m_currentTime > time2printOlt)
  {
    uint64_t totalRxFromXgponBytes = 0;
    //std::cout << "stat.m_currentTime: " << stat.m_currentTime << std::endl;
    for(uint16_t i = 0; i < nOnus; i++){ 
      totalRxFromXgponBytes += stat.m_usOltBytes[i];
      //RoundRobin DBA only increases the counters for T4. QoS aware DBAs (all others except RoundRobin) generate traffic in all TCONTS; but since the traffic is generated for T2 - T4 in this example by default, only three types of counters can be seen incremented in the upstream when using the QoS-aware DBAs.
      std::cout << (stat.m_currentTime / 1000000L) << ",ms," 
          << "From ONU," << i << ",total-Upstream," << stat.m_usOltBytes[i] << "," 
          << "alloc," << i+1024 << ",T1," << stat.m_usT1oltBytes[i] << "," 
          << "alloc," << i+2048 << ",T2," << stat.m_usT2oltBytes[i] << "," 
          << "alloc," << i+3072 << ",T3," << stat.m_usT3oltBytes[i] << "," 
          << "alloc," << i+4096 << ",T4," << stat.m_usT4oltBytes[i] << "," << std::endl;
    }   
    std::cout << "\tTotal-ONU-Upstream," << totalRxFromXgponBytes << ",(Bytes)" << std::endl;
    time2printOlt += 100000000;
  }
}

//trace at the ONUs for the downstream (counters are updated at the receiver)
void
DeviceStatisticsTraceOnu (const XgponNetDeviceStatistics& stat)
{
  if(stat.m_currentTime > time2printOnu[incrementingOnuId])
  {
    totalOnuReceivedBytes += stat.m_dsOnuBytes; 
    std::cout << (stat.m_currentTime / 1000000L) << ",ms,ONU," << incrementingOnuId << ",total-downstream-thisOnu," << stat.m_dsOnuBytes << ",Bytes" << std::endl;
    time2printOnu[incrementingOnuId] += 100000000;
    incrementingOnuId++;
    if(incrementingOnuId == nOnus){
      std::cout << (stat.m_currentTime / 1000000L) << ",ms,total-downstream-allOnus," << totalOnuReceivedBytes << std::endl;
      totalOnuReceivedBytes = 0;
      incrementingOnuId = 0;
    }
  }
}

int 
main (int argc, char *argv[])
{

  /////////////////////////////////////SETTING DYNAMIC PARAMETERS (with default values) FOR THE OVERALL SIMULATION
  //these are example values that may need to be changed often, and may impact multiple configuration locations in this example file. 
  std::string traffic_direction = "upstream"; //traffic direction: downstream/upstream; default is upstream
  std::string upstream_dba = "RoundRobin"; //DBA to be used for upstream bandwidth allocation
  std::string per_app_rate = "100Mbps"; //Datarate of an application traffic source
  //uint16_t dtqSize=800; //queue size for the net devices used in this example. Per AllocID Queues needs to be set at xgpon-queue.cc
  
  /*  
   *  DBA mechanisms that can be used are: RoundRobin, Giant, Ebu, Xgiant, XgiantDeficit, XgiantProp
      
      NOTE: RoundRobin is a non-QoS aware DBA mechanism (therefore, not compliant with the G987.3 standard). This DBA is there for verification purposes.
      Parameters in section 'CONFIGURATIONS FOR QOS-AWARE DBAS ONLY' do not impact the behaviour of RoundRobin DBA.
      The rest of the DBAs are QoS-aware DBA mechanisms (section CONFIGURATIONS FOR QOS-AWARE DBAS ONLY is responsible only for the outcome of these QoS-aware DBAs)
      One can look at the relevant implemenation files of RoundRobin and Xgiant to see the difference in implementations between a non-QoS aware and QoS-aware DBAs.
  */
  
  CommandLine cmd;
  //COMMAND LINE OPTIONS FOR THE USER TO OVERRIDE THE DEFAULT OPTIONS ABOVE
  cmd.AddValue ("traffic-direction", "The direction of the traffic flow in XGPON (values: downstream, upstream)", traffic_direction);
  cmd.AddValue ("upstreamDBA", "DBA to be used for XGPON upstream; a simple RoundRobin is used for downstream (values: RoundRobin, Giant, Ebu, Xgiant, XgiantDeficit, XgiantProp)", upstream_dba);
  cmd.AddValue("app-rate", "Datarate of an application traffice source (values: 10Mbps, 1Gbps, 254kbps, etc)", per_app_rate);
  cmd.Parse (argc, argv);

  std::string xgponDba = "ns3::XgponOltDbaEngine";
  xgponDba.append(upstream_dba);

  ////////////////////////////////////////////////////////CONFIGURATIONS FOR QOS-AWARE DBAS ONLY
  
  /* 2.15 is the ideal max capacity, as the DBAoverhead(~4Mbps) + datarate(692Mbps) for T4 equals that of T2 and T3 datarates(696.7Mbps) individually. 
   * The additional ~0.7Mbps can be attributed to XGPON overhead in the seperate grant given to T4 every cycle, in the firstT3round
   * One is allowed to set higher values here, but the outcome will be an overprovisioning DBA mechanism. 
   * Note that this configuration is only for DBA algorithm operation, and does not impact the actural capacity of XG-PON which is controlled through how much of bytes packed in each XGPON frame.
   * Indirectly for Upstream however, DBA controlls how much payload goes into each Upstream frame. Hence one must take care in altering values set here when using QoS-aware DBAs. 
   * RoundRobin DBA is not affected by these values.
   */
  double max_bandwidth = traffic_direction == "upstream" ? XGPON_UPSTREAM_CAPACITY : XGPON_DOWNSTREAM_CAPACITY; //Capacity of XGPON in Gbps 2.3 for XGPON upstream, 9.9 for XGPON downstream 
  std::cout << "max_bandwidth of XGPON: " << max_bandwidth << " Gbps" << std::endl;
  uint32_t siValue=1;//service interval for GIR. 
 
  std::vector<uint64_t> fixedBw(nOnus);
  std::vector<uint64_t> assuredBw(nOnus);
  std::vector<uint64_t> nonAssuredBw(nOnus);
  std::vector<uint64_t> bestEffortBw(nOnus);
  std::vector<uint64_t> maxBw(nOnus);
  std::vector<uint16_t> siMax(nOnus);
  std::vector<uint16_t> siMin(nOnus);

  //these are aggregate network load values for each TCONT type for the entire XG-PON.
  //each tcont in each ONU will get its propotional value, as can be seen in the corresponding DBA implementation files
  uint32_t fixedBwValue=0.0*max_bandwidth*1e9; //unit: bps, e.g. 0.7*max_bandwidth*1e9 means 0.7*2.15Gbps
  uint32_t assuredBwValue=0.7*max_bandwidth*1e9;	
  uint32_t nonAssuredBwValue=0.8*max_bandwidth*1e9;	
  uint32_t bestEffortBwValue=0.67*max_bandwidth*1e9;

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
 
  // XG-PON is the only bottleneck; hence the rest of the network capacity and the application traffic are overpro    visioned 
  double corePerNodeP2PDataRate = max_bandwidth*5;
  double metroNodesP2PDataRate = max_bandwidth*5;
  double lastmilePerNodeP2PDataRate = max_bandwidth*5; 

  //p2p datarate for core nodes
  std::stringstream corePerNodeP2PDataRateString;
  corePerNodeP2PDataRateString << corePerNodeP2PDataRate << "Gbps";
  //p2p datarate for the metro nodes
  std::stringstream metroNodesP2PDataRateString;
  metroNodesP2PDataRateString << metroNodesP2PDataRate << "Gbps"; 
  //p2p datarate for and lastmile nodes
  std::stringstream lastmilePerNodeP2PDataRateString;
  lastmilePerNodeP2PDataRateString << lastmilePerNodeP2PDataRate << "Gbps";
    /* 
      //if an autonomous method is needed to set the application datarate, here's an example
      //siValue and load are necessesary multipliers
      double load=0.5; //loading of the XGPON by the application, in terms of the ratio againts the XGPON capacity. this can be anywhere between 0 and 2.0
      double perAppDataRate = siValue*max_bandwidth*load/(nOnus*3); //due to having three tconts per ONU
      std::stringstream perAppDataRateString;
      perAppDataRateString << perAppDataRate << "Gbps";
     */
  std::cout << "datarate(Gbps),metro," << metroNodesP2PDataRateString.str() << ",core," << corePerNodeP2PDataRateString.str() << ",lastmile," << lastmilePerNodeP2PDataRateString.str() << ",perApplication," << per_app_rate << std::endl;
  

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
  std::string p2pDelay = "1ms"; //p2p delay beween diffent network segments used in this example. This is not the XG-PON OLT-ONU delay. For setting the XG-PON propagatio    n dealy, one should look at xgpon-channel.h
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

  ////////////////////////////////////////////////////CONFIGURE THE XGPON WITH QUEUE VALUE, QOS CLASSES, UPSTREAM/DOWNSTREAM CONNECTIONS AND IP-ALLOCID-PORTID-TCONT MAPPINGS

  //calculate and set queue size
  uint16_t min_queue_size; //unit: KBytes, set the min queue size required in the ONUs and OLTs as per the capacity of and direction of traffic in XGPON
  if (traffic_direction == "downstream"){
    min_queue_size = max_bandwidth * 1000 * 5; //there's only one queue in the downstream, hence no impact by the no. of ONUs. But maximum expected queue dealy at OLT is 5ms. Any higher values of the queue size will have a higher maximum queue delay at OLT
    std::cout << "queue size at OLT for a 5ms max delay: " << min_queue_size << std::endl;
  }else{
    min_queue_size = (upstream_dba == "RoundRobin") ? (max_bandwidth * 1000 * 5 / (nOnus * 3)) : 
              (max_bandwidth * 1000 * 5 / nOnus) ; //min_queue_size is based on a maximum expected queue dealy at ONU of 5ms and 3 TCONTs per ONU; hence there's no division by 3 if the DBA is not RoundRobin
    std::cout << "queue size at ONU for a 5ms max delay: " << min_queue_size << std::endl;
  }
	Config::SetDefault ("ns3::XgponQueue::MaxBytes", UintegerValue(min_queue_size*1000)); //set the queue size. min_queue_size (unit: KBytes), MaxBytes (unit: Bytes)

  Ptr<XgponOltNetDevice> oltDevice = DynamicCast<XgponOltNetDevice, NetDevice> (xgponDevices.Get(0));
  if(traffic_direction == "upstream"){
    std::cout << "Connecting OLT to upstream statistics " << std::endl;
    oltDevice->TraceConnectWithoutContext ("DeviceStatistics", MakeCallback(&DeviceStatisticsTrace));
  }
  //add xgem ports for user nodes connected to ONUs
  //each onu-olt pair would have a single downstream port and multiple upstream ports depending on the no. of tconts (by default 4 tconts, hence 4 upstream ports)
  int nTconts = 4;
  for(int i=0; i< nOnus; i++)
  {
    
    Address addr = p2pLastmileInterfaces[i].GetAddress(1);
    Ptr<XgponOnuNetDevice> onuDevice = DynamicCast<XgponOnuNetDevice, NetDevice> (xgponDevices.Get(i+1));
      if(traffic_direction == "downstream"){
        std::cout << "Connecting ONU " << i << " to device downstream statistics " << std::endl;
        onuDevice->TraceConnectWithoutContext ("DeviceStatistics", MakeCallback(&DeviceStatisticsTraceOnu));
      }
    uint16_t downPortId = xgponHelper.AddOneDownstreamConnectionForOnu (onuDevice, oltDevice, addr);
    xgponHelper.SetQosParametersAttribute ("FixedBandwidth", UintegerValue (fixedBw[i]) );
    xgponHelper.SetQosParametersAttribute ("AssuredBandwidth", UintegerValue (assuredBw[i]) );
    xgponHelper.SetQosParametersAttribute ("NonAssuredBandwidth", UintegerValue(nonAssuredBw[i]) );
    xgponHelper.SetQosParametersAttribute ("BestEffortBandwidth", UintegerValue (bestEffortBw[i]) );
    xgponHelper.SetQosParametersAttribute ("MaxServiceInterval", UintegerValue (siMax[i]));
    xgponHelper.SetQosParametersAttribute ("MinServiceInterval", UintegerValue (siMin[i]));
    
    //tconts 1-4 are here.
    for (uint8_t tcont=1; tcont<= nTconts; tcont++)
    {
      
      XgponQosParameters::XgponTcontType tcontType = static_cast<XgponQosParameters::XgponTcontType>(tcont);
      uint16_t allocId = xgponHelper.AddOneTcontForOnu (onuDevice, oltDevice, tcontType);
      uint16_t upPortId = xgponHelper.AddOneUpstreamConnectionForOnu (onuDevice, oltDevice, allocId, addr);
      
      std::cout << "\t\tUP/DOWNSTREAM ONU-ID = "    << onuDevice->GetOnuId()
      << "\tALLOC-ID = "  << allocId 
      << "\tUP-PORT-ID= "   << upPortId 
      << "\tDOWN-PORT-ID = "  << downPortId
      << "\tTCONT-"   << tcontType << std::endl;
      
      allocIdList.push_back(allocId);
    }
  }

std::cout << std::endl;
    std::cout << "===========================================================================================" << std::endl;

  //POPULATE ROUTING TABLE
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  //IF RR DBA IS ENABLED, ONLY BEST EFFORT TRAFFIC IS GENERATED WITH TCONT TYPE T4
  //IF A QOS-BASED DBA IS USED, THEN TRAFFIC FOR TCONT TYPES T2, T3 AND T4 ARE GENERATED
  int serverPort[nTconts];
  for (int i=0; i<nOnus; i++){
    int p = upstream_dba == "RoundRobin" ? 4 : 2; // if the DBA is RoundRobin, only one type of traffic is suffient; t1 traffic is skipped for its predictability 
    for(; p <= nTconts; p++){
      serverPort[p-1] = 9000 + p; //9002-9004, for tcont2,3,4
      PacketSinkHelper sink ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), serverPort[p-1]));
      ApplicationContainer sinkApp;
      InetSocketAddress addr = InetSocketAddress(p2pCoreInterfaces[i].GetAddress(0), serverPort[p-1]);
        if(traffic_direction == "upstream"){
          sinkApp = sink.Install (serverNodes.Get(i));
          addr = InetSocketAddress(p2pCoreInterfaces[i].GetAddress(0), serverPort[p-1]);
        }else{
          sinkApp = sink.Install (userNodes.Get(i));
          addr = InetSocketAddress(p2pLastmileInterfaces[i].GetAddress(0), serverPort[p-1]);
        }
      sinkApp.Start (Seconds (0.000001));
      sinkApp.Stop (Seconds (APP_STOP + 0.1));

      addr.SetTos(p); //set the tcont type, which will then be carried through the IP header and into the XG-PON

      OnOffHelper onOff ("ns3::UdpSocketFactory", addr);
      onOff.SetAttribute ("OffTime",  StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
      onOff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
      //onOff.SetAttribute ("DataRate", DataRateValue(DataRate(perAppDataRateString.str()))); //if using the automated app rate calculation
      onOff.SetAttribute ("DataRate", StringValue(per_app_rate));
      onOff.SetAttribute ("PacketSize", UintegerValue(1447));
      onOff.SetAttribute ("MaxBytes", UintegerValue(0));
      ApplicationContainer sourceApp;
        if(traffic_direction == "upstream"){sourceApp = onOff.Install (userNodes.Get(i));}
        else{sourceApp = onOff.Install (serverNodes.Get(i));}
      sourceApp.Start (Seconds (0.005));
      sourceApp.Stop (Seconds (APP_STOP));
    }
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
