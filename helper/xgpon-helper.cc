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
 */
#include <stdint.h>

#include "ns3/log.h"

#include "ns3/xgpon-channel.h"

#include "ns3/xgpon-net-device.h"
#include "ns3/xgpon-olt-net-device.h"
#include "ns3/xgpon-onu-net-device.h"


#include "ns3/xgpon-olt-xgem-engine.h"
#include "ns3/xgpon-olt-framing-engine.h"
#include "ns3/xgpon-olt-phy-adapter.h"
#include "ns3/xgpon-olt-ploam-engine.h"
#include "ns3/xgpon-olt-omci-engine.h"
#include "ns3/xgpon-olt-conn-manager.h"
#include "ns3/xgpon-olt-ds-scheduler.h"
#include "ns3/xgpon-olt-dba-engine.h"

#include "ns3/xgpon-onu-xgem-engine.h"
#include "ns3/xgpon-onu-framing-engine.h"
#include "ns3/xgpon-onu-phy-adapter.h"
#include "ns3/xgpon-onu-ploam-engine.h"
#include "ns3/xgpon-onu-omci-engine.h"
#include "ns3/xgpon-onu-conn-manager.h"
#include "ns3/xgpon-onu-dba-engine.h"
#include "ns3/xgpon-onu-us-scheduler.h"

#include "ns3/xgpon-fifo-queue.h"

#include "ns3/xgpon-tcont-olt.h"
#include "ns3/xgpon-tcont-onu.h"

#include "xgpon-id-allocator-speed.h"
#include "xgpon-id-allocator-flexible.h"

#include "xgpon-helper.h"


NS_LOG_COMPONENT_DEFINE("XgponHelper");

namespace ns3 {

XgponHelper::XgponHelper()
{
}

XgponHelper::~XgponHelper()
{
  delete m_idAllocator;
}



void 
XgponHelper::InitializeObjectFactories (void)
{
  m_channelFactory.SetTypeId(m_configDb.m_channelTypeIdStr);

  m_oltDbaEngineFactory.SetTypeId (m_configDb.m_oltDbaEngineTypeIdStr);
  m_oltDsSchedulerEngineFactory.SetTypeId (m_configDb.m_oltDsSchedulerTypeIdStr);
  
  //if(m_configDb.m_oltDbaEngineTypeIdStr == "XgponOltDbaEngineRoundRobin")
    //
  //else
    //m_oltTcontFactory.SetTypeId (XGPON_TCONT_QOS_TYPEID_STR);

  if(m_configDb.m_allocateIds4Speed)
  {
    m_oltConnManagerEngineFactory.SetTypeId (XGPON_OLT_CONN_MANAGER_SPEED_TYPEID_STR);
    m_onuConnManagerEngineFactory.SetTypeId (XGPON_ONU_CONN_MANAGER_SPEED_TYPEID_STR);
    m_idAllocator = new XgponIdAllocatorSpeed(m_configDb.m_onuNetmaskLen);
  } else
  {
    m_oltConnManagerEngineFactory.SetTypeId (XGPON_OLT_CONN_MANAGER_FLEXIBLE_TYPEID_STR);
    m_onuConnManagerEngineFactory.SetTypeId (XGPON_ONU_CONN_MANAGER_FLEXIBLE_TYPEID_STR);
    m_idAllocator = new XgponIdAllocatorFlexible(m_configDb.m_onuNetmaskLen);
  }


  m_onuUsSchedulerEngineFactory.SetTypeId (m_configDb.m_onuUsSchedulerTypeIdStr);

  m_queueFactory.SetTypeId(m_configDb.m_queueTypeIdStr);
  m_qosParametersFactory.SetTypeId(m_configDb.m_qosParametersTypeIdStr); 
}



void 
XgponHelper::SetChannelAttribute (std::string n1, const AttributeValue &v1)
{
  m_channelFactory.Set (n1, v1);
}

void 
XgponHelper::SetPhyAttribute (std::string n1, const AttributeValue &v1)
{
  //example --- Config::SetDefault ("ns3::XgponPhy::DsLinkRate", UintegerValue(10000));
  Config::SetDefault(n1, v1);
}

void 
XgponHelper::SetQueueAttribute (std::string n1, const AttributeValue &v1)
{
  m_queueFactory.Set (n1, v1);
}

void 
XgponHelper::SetQosParametersAttribute (std::string n1, const AttributeValue &v1)
{
  m_qosParametersFactory.Set (n1, v1);
}





NetDeviceContainer XgponHelper::Install (NodeContainer nodes)
{
  NetDeviceContainer deviceContainer;

  //Set channel attributes before the following call
  Ptr<XgponChannel> xgponChannel = CreateXgponChannel ( );

  
  //Create olt device and add it to the node and channel
  Ptr<XgponOltNetDevice> oltDevice = CreateXgponOltNetDeviceAndEngines ( );
  nodes.Get(0)->AddDevice(oltDevice); 
  AttachOltToPonChannel (xgponChannel, oltDevice);

  deviceContainer.Add (oltDevice);
  
  //Create onu devices and add them to nodes and channel.
  for(uint32_t i=1; i<nodes.GetN(); i++)
  {
    Ptr<XgponOnuNetDevice> onuDevice = CreateXgponOnuNetDeviceAndEngines ( );
    nodes.Get(i)->AddDevice(onuDevice);

    AttachOnuToPonChannel (xgponChannel, onuDevice);
    AddOnuToOlt (onuDevice, oltDevice);

    deviceContainer.Add (onuDevice);
  }

  return deviceContainer;
}








std::string 
XgponHelper::GetOnuAddressNetmask ()
{
  return GetIpAddressNetmask(m_configDb.m_onuNetmaskLen);
}
std::string 
XgponHelper::GetOltAddressNetmask ()
{
  return GetIpAddressNetmask(m_configDb.m_oltNetmaskLen);
}



std::string 
XgponHelper::GetIpAddressNetmask (uint8_t netmaskLen)
{
  uint64_t netmask = 1;

  for(uint8_t i=1; i<netmaskLen; i++)
  {
    netmask = netmask << 1;
    netmask = netmask | 0x01;
  }
  netmask = netmask << (32 - netmaskLen);

  std::ostringstream convertStream;   // stream used for the conversion

  uint16_t tmpByte = (netmask >> 24) & 0xff;
  convertStream << tmpByte << ".";
  tmpByte = (netmask >> 16) & 0xff;
  convertStream << tmpByte << ".";
  tmpByte = (netmask >> 8) & 0xff;
  convertStream << tmpByte << ".";
  tmpByte = netmask & 0xff;
  convertStream << tmpByte;

  std::string netmaskStr = convertStream.str(); // set 'Result' to the contents of the stream

  return netmaskStr;

}




std::string 
XgponHelper::GetOnuIpAddressBase (Ptr<XgponOnuNetDevice> onuDevice)
{
  NS_ASSERT_MSG((m_configDb.m_onuNetmaskLen > 20), "Onu network mask must be long enough to hold the first byte plus onu-id.");

  return GetIpAddressBase (m_configDb.m_addressFirstByteOnus, onuDevice->GetOnuId (), m_configDb.m_onuNetmaskLen);
}

std::string 
XgponHelper::GetXgponIpAddressBase ( )
{
  NS_ASSERT_MSG((m_configDb.m_oltNetmaskLen < 20), "Xgpon network mask should be short enough so that all onus can get one address.");

  return GetIpAddressBase (m_configDb.m_addressFirstByteXgpon, 0, m_configDb.m_oltNetmaskLen);
}


std::string 
XgponHelper::GetIpAddressBase (uint16_t firstByte, uint16_t netid, uint8_t netmaskLen)
{
  uint64_t ipbase = firstByte << 24;
  uint64_t tmpInt = netid << (32 - netmaskLen);
  ipbase  = ipbase | tmpInt;

  std::ostringstream convertStream;   // stream used for the conversion

  uint16_t tmpByte = (ipbase >> 24) & 0xff;
  convertStream << tmpByte << ".";
  tmpByte = (ipbase >> 16) & 0xff;
  convertStream << tmpByte << ".";
  tmpByte = (ipbase >> 8) & 0xff;
  convertStream << tmpByte << ".";
  tmpByte = ipbase & 0xff;
  convertStream << tmpByte;

  std::string ipbaseStr = convertStream.str(); // set 'Result' to the contents of the stream

  return ipbaseStr;
}












uint16_t 
XgponHelper::AddOneTcontForOnu (Ptr<XgponOnuNetDevice> onuDevice, Ptr<XgponOltNetDevice> oltDevice, XgponQosParameters::XgponTcontType tcontType)
{
  uint16_t onuId = onuDevice->GetOnuId ( );
  uint16_t allocId = m_idAllocator->GetOneNewAllocId (onuId);

  AddOneTcontForOnu (onuDevice, oltDevice, allocId, tcontType);

  return allocId;
}


uint16_t 
XgponHelper::AddOneUpstreamConnectionForOnu (Ptr<XgponOnuNetDevice> onuDevice, Ptr<XgponOltNetDevice> oltDevice, uint16_t allocId, const Address& addr)
{
  uint16_t onuId = onuDevice->GetOnuId ( );
  uint16_t portId = m_idAllocator->GetOneNewUpstreamPortId (onuId, addr);

  AddOneUpstreamConnectionForOnu (onuDevice, oltDevice, allocId, addr, portId);

  return portId;
}


uint16_t 
XgponHelper::AddOneDownstreamConnectionForOnu (Ptr<XgponOnuNetDevice> onuDevice, Ptr<XgponOltNetDevice> oltDevice, const Address& addr)
{
  uint16_t onuId = onuDevice->GetOnuId ( );
  uint16_t portId = m_idAllocator->GetOneNewDownstreamPortId (onuId, addr);

  AddOneDownstreamConnectionForOnu (onuDevice, oltDevice, addr, portId);

  return portId;
}

uint16_t 
XgponHelper::AddOneBroadcastDownstreamConnection (Ptr<XgponOltNetDevice> oltDevice, const Address& addr)
{
  Ptr<XgponChannel> ch = DynamicCast<XgponChannel, Channel> (oltDevice->GetChannel ( ));
  int num = ch->GetNOnuDevices ( );

  uint16_t portId = m_idAllocator->GetOneNewBroadcastDownstreamPortId (addr);

  Ptr<XgponConnectionSender> connSender = CreateObject<XgponConnectionSender> ( );
  Ptr<XgponFifoQueue> txQueue = m_queueFactory.Create<ns3::XgponFifoQueue> ( );
  Ptr<XgponQosParameters> qosParameters = m_qosParametersFactory.Create<ns3::XgponQosParameters> ( );

  connSender->SetDirection (XgponConnection::DOWNSTREAM_CONN);
  connSender->SetBroadcast (true);
  connSender->SetXgemPort (portId);
  connSender->SetUpperLayerAddr (addr);
  connSender->SetXgponQueue (txQueue);

  oltDevice->SetQosParameters(qosParameters); //jerome
  Ptr<XgponOltConnManager> connManager = oltDevice->GetConnManager ( );
  connManager->AddOneDsConn (connSender, true, 0);

  Ptr<XgponOltDsScheduler> dsScheduler = oltDevice->GetDsScheduler ( );
  dsScheduler->AddConnToScheduler (connSender);

  for(int i=0; i<num; i++)
  {
    Ptr<XgponOnuNetDevice> onuDevice = DynamicCast<XgponOnuNetDevice, PonNetDevice> (ch->GetOnuByIndex (i));
    Ptr<XgponQosParameters> qosParameters2 = m_qosParametersFactory.Create<ns3::XgponQosParameters> ( );
    qosParameters2->DeepCopy(qosParameters);    
    onuDevice->SetQosParameters(qosParameters2); //jerome

    Ptr<XgponConnectionReceiver> connReceiver = CreateObject<XgponConnectionReceiver> ( );
    connReceiver->SetDirection (XgponConnection::DOWNSTREAM_CONN);
    connReceiver->SetBroadcast (true);
    connReceiver->SetXgemPort (portId);
    connReceiver->SetUpperLayerAddr (addr);

    Ptr<XgponOnuConnManager> onuConnManager = onuDevice->GetConnManager ( );
    onuConnManager->AddOneDsConn (connReceiver);
  }

  return portId;
}




void 
XgponHelper::EnablePcapInternal (std::string prefix, Ptr<NetDevice> nd, bool promiscuous, bool explicitFilename)
{
  //
  // All of the Pcap enable functions vector through here including the ones
  // that are wandering through all of devices on perhaps all of the nodes in
  // the system.  We can only deal with devices of type PointToPointNetDevice.
  //
  Ptr<XgponNetDevice> device = nd->GetObject<XgponNetDevice> ();
  if (device == 0)
    {
      NS_LOG_INFO ("XgponHelper::EnablePcapInternal(): Device " << device << " not of type ns3::XgponNetDevice");
      return;
    }

  PcapHelper pcapHelper;

  std::string filename;
  if (explicitFilename)
    {
      filename = prefix;
    }
  else
    {
      filename = pcapHelper.GetFilenameFromDevice (prefix, device);
    }

  Ptr<PcapFileWrapper> file = pcapHelper.CreateFile (filename, std::ios::out, 
                                                     PcapHelper::DLT_RAW);
  pcapHelper.HookDefaultSink<XgponNetDevice> (device, "PromiscSniffer", file);
}





void 
XgponHelper::EnableAsciiInternal (
  Ptr<OutputStreamWrapper> stream, 
  std::string prefix, 
  Ptr<NetDevice> nd,
  bool explicitFilename)
{
  //
  // All of the ascii enable functions vector through here including the ones
  // that are wandering through all of devices on perhaps all of the nodes in
  // the system.  We can only deal with devices of type PointToPointNetDevice.
  //
  Ptr<XgponNetDevice> device = nd->GetObject<XgponNetDevice> ();
  if (device == 0)
    {
      NS_LOG_INFO ("XgponHelper::EnableAsciiInternal(): Device " << device << 
                   " not of type ns3::XgpontNetDevice");
      return;
    }

  //
  // Our default trace sinks are going to use packet printing, so we have to 
  // make sure that is turned on.
  //
  Packet::EnablePrinting ();

  //
  // If we are not provided an OutputStreamWrapper, we are expected to create 
  // one using the usual trace filename conventions and do a Hook*WithoutContext
  // since there will be one file per context and therefore the context would
  // be redundant.
  //
  if (stream == 0)
    {
      //
      // Set up an output stream object to deal with private ofstream copy 
      // constructor and lifetime issues.  Let the helper decide the actual
      // name of the file given the prefix.
      //
      AsciiTraceHelper asciiTraceHelper;

      std::string filename;
      if (explicitFilename)
        {
          filename = prefix;
        }
      else
        {
          filename = asciiTraceHelper.GetFilenameFromDevice (prefix, device);
        }

      Ptr<OutputStreamWrapper> theStream = asciiTraceHelper.CreateFileStream (filename);

      //
      // The "+", '-', and 'd' events are driven by trace sources actually in the
      // transmit queue.
      //
      asciiTraceHelper.HookDefaultEnqueueSinkWithoutContext<XgponNetDevice> (device, "Enqueue", theStream);
      asciiTraceHelper.HookDefaultDropSinkWithoutContext<XgponNetDevice> (device, "Drop", theStream);
      asciiTraceHelper.HookDefaultDequeueSinkWithoutContext<XgponNetDevice> (device, "Dequeue", theStream);


      return;
    }

  //
  // If we are provided an OutputStreamWrapper, we are expected to use it, and
  // to providd a context.  We are free to come up with our own context if we
  // want, and use the AsciiTraceHelper Hook*WithContext functions, but for 
  // compatibility and simplicity, we just use Config::Connect and let it deal
  // with the context.
  //
  // Note that we are going to use the default trace sinks provided by the 
  // ascii trace helper.  There is actually no AsciiTraceHelper in sight here,
  // but the default trace sinks are actually publicly available static 
  // functions that are always there waiting for just such a case.
  //
  uint32_t nodeid = nd->GetNode ()->GetId ();
  uint32_t deviceid = nd->GetIfIndex ();
  std::ostringstream oss;

  oss << "/NodeList/" << nodeid << "/DeviceList/" << deviceid << "/$ns3::XgponNetDevice/Enqueue";
  Config::Connect (oss.str (), MakeBoundCallback (&AsciiTraceHelper::DefaultEnqueueSinkWithContext, stream));

  oss.str ("");
  oss << "/NodeList/" << nodeid << "/DeviceList/" << deviceid << "/$ns3::XgponNetDevice/Dequeue";
  Config::Connect (oss.str (), MakeBoundCallback (&AsciiTraceHelper::DefaultDequeueSinkWithContext, stream));

  oss.str ("");
  oss << "/NodeList/" << nodeid << "/DeviceList/" << deviceid << "/$ns3::XgponNetDevice/Drop";
  Config::Connect (oss.str (), MakeBoundCallback (&AsciiTraceHelper::DefaultDropSinkWithContext, stream));

}

Ptr<XgponChannel> 
XgponHelper::CreateXgponChannel (void)
{
  Ptr<XgponChannel> ch = m_channelFactory.Create<ns3::XgponChannel> ();
  return ch;
}



Ptr<XgponOltNetDevice> 
XgponHelper::CreateXgponOltNetDeviceAndEngines (void)
{
  Ptr<XgponOltNetDevice> oltDevice = CreateObject<XgponOltNetDevice>();


  Ptr<XgponOltXgemEngine> xgemEngine = CreateObject<XgponOltXgemEngine>();
  oltDevice->SetXgemEngine (xgemEngine);
  xgemEngine->SetXgponOltNetDevice (oltDevice);

  Ptr<XgponOltFramingEngine> framingEngine = CreateObject<XgponOltFramingEngine>();
  oltDevice->SetFramingEngine (framingEngine);
  framingEngine->SetXgponOltNetDevice (oltDevice);

  //Create Phy-adapter and set PON-ID.
  Ptr<XgponOltPhyAdapter> phyAdapter = CreateObject<XgponOltPhyAdapter>();
  oltDevice->SetPhyAdapter (phyAdapter);
  phyAdapter->SetXgponOltNetDevice (oltDevice);
  phyAdapter->SetPonid (m_configDb.m_ponId); 


  Ptr<XgponPhy> commonPhy = CreateObject<XgponPhy>();
  oltDevice->SetXgponPhy (commonPhy);


  Ptr<XgponOltOmciEngine> omciEngine = CreateObject<XgponOltOmciEngine>();
  oltDevice->SetOmciEngine (omciEngine);
  omciEngine->SetXgponOltNetDevice (oltDevice);

  Ptr<XgponOltPloamEngine> ploamEngine = CreateObject<XgponOltPloamEngine>();
  oltDevice->SetPloamEngine (ploamEngine);
  ploamEngine->SetXgponOltNetDevice (oltDevice);


  //Create the other engines that may have different sub-classes.
  Ptr<XgponOltDbaEngine> dbaEngine = m_oltDbaEngineFactory.Create<ns3::XgponOltDbaEngine> ();
  oltDevice->SetDbaEngine (dbaEngine);
  dbaEngine->SetXgponOltNetDevice (oltDevice);

  Ptr<XgponOltDsScheduler> dsScheduler = m_oltDsSchedulerEngineFactory.Create<ns3::XgponOltDsScheduler> ();
  oltDevice->SetDsScheduler (dsScheduler);
  dsScheduler->SetXgponOltNetDevice (oltDevice);


  Ptr<XgponOltConnManager> connManager = m_oltConnManagerEngineFactory.Create<ns3::XgponOltConnManager> ();
  oltDevice->SetConnManager (connManager);
  connManager->SetXgponOltNetDevice (oltDevice);
  connManager->SetOnuNetmaskLen (m_configDb.m_onuNetmaskLen);

  return oltDevice;
}


Ptr<XgponOnuNetDevice> 
XgponHelper::CreateXgponOnuNetDeviceAndEngines (void)
{
  Ptr<XgponOnuNetDevice> onuDevice = CreateObject<XgponOnuNetDevice>();
  uint16_t onuId = m_idAllocator->GetOneNewOnuId ();
  onuDevice->SetOnuId (onuId);


  Ptr<XgponOnuXgemEngine> xgemEngine = CreateObject<XgponOnuXgemEngine>();
  onuDevice->SetXgemEngine (xgemEngine);
  xgemEngine->SetXgponOnuNetDevice (onuDevice);

  Ptr<XgponOnuFramingEngine> framingEngine = CreateObject<XgponOnuFramingEngine>();
  onuDevice->SetFramingEngine (framingEngine);
  framingEngine->SetXgponOnuNetDevice (onuDevice);


  Ptr<XgponOnuPhyAdapter> phyAdapter = CreateObject<XgponOnuPhyAdapter>();
  onuDevice->SetPhyAdapter (phyAdapter);
  phyAdapter->SetXgponOnuNetDevice (onuDevice);
  phyAdapter->SetPonid (m_configDb.m_ponId); 

  Ptr<XgponPhy> commonPhy = CreateObject<XgponPhy>();
  onuDevice->SetXgponPhy (commonPhy);


  Ptr<XgponOnuOmciEngine> omciEngine = CreateObject<XgponOnuOmciEngine>();
  onuDevice->SetOmciEngine (omciEngine);
  omciEngine->SetXgponOnuNetDevice (onuDevice);



  //////////////////////////////Create PloamEngine and the corresponding LinkInfo
  Ptr<XgponOnuPloamEngine> ploamEngine = CreateObject<XgponOnuPloamEngine>();
  onuDevice->SetPloamEngine (ploamEngine);
  ploamEngine->SetXgponOnuNetDevice (onuDevice);
  
  Ptr<XgponLinkInfo> linkInfo = CreateObject<XgponLinkInfo>();
  ploamEngine->SetLinkInfo (linkInfo);
  linkInfo->SetOnuId (onuId);

  Ptr<XgponKey> key1 = CreateObject<XgponKey>();
  Ptr<XgponKey> key2 = CreateObject<XgponKey>();
  linkInfo->AddNewDsKey (key1, 0);
  linkInfo->AddNewUsKey (key2, 0);  
  linkInfo->SetCurrentDsKeyIndex (0);
  linkInfo->SetCurrentUsKeyIndex (0);

  //In the future, profile may depend on propagation delay (distance)
  Ptr<XgponBurstProfile> profile = CreateObject<XgponBurstProfile>();
  profile->SetPreambleLen (m_configDb.m_profilePreambleLen);
  profile->SetDelimiterLen (m_configDb.m_profileDelimiterLen);
  profile->SetFec (m_configDb.m_profileFec);

  linkInfo->AddNewProfile (profile, 0);
  linkInfo->SetCurrentProfileIndex (0);
  //////////////////////////////End of PloamEngine Configuration

  


  Ptr<XgponOnuDbaEngine> dbaEngine = CreateObject<XgponOnuDbaEngine> ();
  onuDevice->SetDbaEngine (dbaEngine);
  dbaEngine->SetXgponOnuNetDevice (onuDevice);


  Ptr<XgponOnuConnManager> connManager = m_onuConnManagerEngineFactory.Create<ns3::XgponOnuConnManager> ();
  onuDevice->SetConnManager (connManager);
  connManager->SetXgponOnuNetDevice (onuDevice);
  connManager->SetOnuNetmaskLen (m_configDb.m_onuNetmaskLen);


  return onuDevice;
}







void
XgponHelper::AttachOltToPonChannel (Ptr<XgponChannel> ch, Ptr<XgponOltNetDevice> oltDevice)
{
  ch->SetOlt (oltDevice);
  oltDevice->SetChannel (ch); 
}


void
XgponHelper::AttachOnuToPonChannel (Ptr<XgponChannel> ch, Ptr<XgponOnuNetDevice> onuDevice)
{
  uint16_t chIndex = ch->AddOnu (onuDevice);
  onuDevice->SetChannel (ch);  
  onuDevice->SetChannelIndex (chIndex);


  //////////////////////Currently, we assume that all onus have the same propagation delay.
  //////////////////////In the future, propagation delay may be generated randomly.
  uint32_t propDelay = ch->GetLogicOneWayDelay ();
  ch->SetOnuPropagationDelay (chIndex, propDelay);  
  Ptr<XgponLinkInfo> linkInfo = onuDevice->GetPloamEngine()->GetLinkInfo();
  linkInfo->SetEqualizeDelay (0); //LogicOneWayDelay - OnuPropagationDelay; 
}


void 
XgponHelper::AddOnuToOlt (Ptr<XgponOnuNetDevice> onuDevice, Ptr<XgponOltNetDevice> oltDevice)
{
  uint16_t onuId = onuDevice->GetOnuId();

  Ptr<XgponOltConnManager> oltConnManager = oltDevice->GetConnManager ( );
  Ptr<XgponOltConnPerOnu> onu4Conns = CreateObject<XgponOltConnPerOnu> ( );
  onu4Conns->SetOnuId (onuId);
  oltConnManager->AddOneOnu4Conns (onu4Conns);  //add based on onu-id

  Ptr<XgponOltPloamEngine> oltPloamEngine = oltDevice->GetPloamEngine ( );
  Ptr<XgponLinkInfo> linkInfo = CreateObject<XgponLinkInfo> ( );
  Ptr<XgponLinkInfo> linkInfo2 = onuDevice->GetPloamEngine()->GetLinkInfo();
  linkInfo->DeepCopy(linkInfo2);
  oltPloamEngine->AddLinkInfo (linkInfo, onuId);
}














void 
XgponHelper::AddOneTcontForOnu (Ptr<XgponOnuNetDevice> onuDevice, Ptr<XgponOltNetDevice> oltDevice, uint16_t allocId, XgponQosParameters::XgponTcontType tcontType) 
{
  uint16_t onuId = onuDevice->GetOnuId ( );

  Ptr<XgponTcontOlt> tcontOlt = CreateObject<ns3::XgponTcontOlt> ();
  Ptr<XgponQosParameters> qosParameters = m_qosParametersFactory.Create<ns3::XgponQosParameters> ( );

  tcontOlt->SetOnuId (onuId);
  tcontOlt->SetAllocId(allocId);
  tcontOlt->SetTcontType(tcontType);
  tcontOlt->SetQosParameters(qosParameters);      //jerome, for the moment, tcontOlt also has a qosParameters object, attached to it. But this should be the object used from the ONU device instead. TODO: have to find a way to bring that qosParameters object from ONU device to the tcontOlt at the time of CalculateTcontParameters

  Ptr<XgponOltConnManager> connManager = oltDevice->GetConnManager ( );
  connManager->AddOneUsTcont (tcontOlt, onuId);

  Ptr<XgponOltDbaEngine> dbaEngine = oltDevice->GetDbaEngine ( );
  dbaEngine->AddTcontToDbaEngine (tcontOlt);


  Ptr<XgponTcontOnu> tcontOnu=CreateObject<XgponTcontOnu>();
  tcontOnu->SetOnuId (onuId);
  tcontOnu->SetAllocId(allocId);
  tcontOnu->SetTcontType(tcontType);  
  tcontOnu->SetQosParameters(qosParameters);
  Ptr<XgponOnuUsScheduler> usScheduler = m_onuUsSchedulerEngineFactory.Create<ns3::XgponOnuUsScheduler> ( );
  usScheduler->SetTcontOnu(tcontOnu);
  tcontOnu->SetOnuUsScheduler(usScheduler);

  Ptr<XgponOnuConnManager> onuConnManager = onuDevice->GetConnManager ( );
  onuConnManager->AddOneUsTcont (tcontOnu);
}


void 
XgponHelper::AddOneUpstreamConnectionForOnu (Ptr<XgponOnuNetDevice> onuDevice, Ptr<XgponOltNetDevice> oltDevice, uint16_t allocId, const Address& addr, uint16_t portId)
{
  uint16_t onuId = onuDevice->GetOnuId ( );

  Ptr<XgponConnectionSender> connSender = CreateObject<XgponConnectionSender> ( );
  Ptr<XgponFifoQueue> txQueue = m_queueFactory.Create<ns3::XgponFifoQueue> ( );
        txQueue->SetAllocId(allocId);
  Ptr<XgponQosParameters> qosParameters = m_qosParametersFactory.Create<ns3::XgponQosParameters> ( );

  //jerome
  onuDevice->SetQosParameters (qosParameters);
  Ptr<XgponQosParameters> qosParameters2 = m_qosParametersFactory.Create<ns3::XgponQosParameters> ( );
  qosParameters2->DeepCopy(qosParameters);
  oltDevice->SetQosParameters (qosParameters2);

  connSender->SetDirection (XgponConnection::UPSTREAM_CONN);
  connSender->SetBroadcast (false);
  connSender->SetXgemPort (portId);
  connSender->SetAllocId (allocId);
  connSender->SetOnuId (onuId);
  connSender->SetUpperLayerAddr (addr);
  connSender->SetXgponQueue (txQueue);

  Ptr<XgponOnuConnManager> onuConnManager = onuDevice->GetConnManager ( );
  onuConnManager->AddOneUsConn (connSender, allocId);


  Ptr<XgponConnectionReceiver> connReceiver = CreateObject<XgponConnectionReceiver> ( );
  connReceiver->SetDirection (XgponConnection::UPSTREAM_CONN);
  connReceiver->SetBroadcast (false);
  connReceiver->SetXgemPort (portId);
  connReceiver->SetAllocId (allocId);
  connReceiver->SetOnuId (onuId);
  connReceiver->SetUpperLayerAddr (addr);

  Ptr<XgponOltConnManager> connManager = oltDevice->GetConnManager ( );
  connManager->AddOneUsConn (connReceiver, allocId);
}


void
XgponHelper::AddOneDownstreamConnectionForOnu (Ptr<XgponOnuNetDevice> onuDevice, Ptr<XgponOltNetDevice> oltDevice, const Address& addr, uint16_t portId)
{
  uint16_t onuId = onuDevice->GetOnuId ( );

  Ptr<XgponConnectionSender> connSender = CreateObject<XgponConnectionSender> ( );
  Ptr<XgponFifoQueue> txQueue = m_queueFactory.Create<ns3::XgponFifoQueue> ( );
  Ptr<XgponQosParameters> qosParameters = m_qosParametersFactory.Create<ns3::XgponQosParameters> ( );


  //jerome, C1
  onuDevice->SetQosParameters (qosParameters);
  Ptr<XgponQosParameters> qosParameters2 = m_qosParametersFactory.Create<ns3::XgponQosParameters> ( );
  qosParameters2->DeepCopy(qosParameters);
  oltDevice->SetQosParameters (qosParameters);
  
  connSender->SetDirection (XgponConnection::DOWNSTREAM_CONN);
  connSender->SetBroadcast (false);
  connSender->SetXgemPort (portId);
  connSender->SetOnuId (onuId);
  connSender->SetUpperLayerAddr (addr);
  connSender->SetXgponQueue (txQueue);

  Ptr<XgponOltConnManager> connManager = oltDevice->GetConnManager ( );
  connManager->AddOneDsConn (connSender, false, onuId);

  Ptr<XgponOltDsScheduler> dsScheduler = oltDevice->GetDsScheduler ( );
  dsScheduler->AddConnToScheduler (connSender);

  Ptr<XgponConnectionReceiver> connReceiver = CreateObject<XgponConnectionReceiver> ( );
  connReceiver->SetDirection (XgponConnection::DOWNSTREAM_CONN);
  connReceiver->SetBroadcast (false);
  connReceiver->SetXgemPort (portId);
  connReceiver->SetOnuId (onuId);
  connReceiver->SetUpperLayerAddr (addr);

  Ptr<XgponOnuConnManager> onuConnManager = onuDevice->GetConnManager ( );
  onuConnManager->AddOneDsConn (connReceiver);
}




}//namespace ns3

