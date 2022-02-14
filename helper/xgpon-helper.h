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

#ifndef XGPON_HELPER_H
#define XGPON_HELPER_H

#include "ns3/config.h"
#include "ns3/packet.h"
#include "ns3/trace-helper.h"

#include "ns3/xgpon-channel.h"
#include "ns3/xgpon-net-device.h"
#include "ns3/xgpon-olt-net-device.h"
#include "ns3/xgpon-onu-net-device.h"


#include "xgpon-config-db.h"
#include "xgpon-id-allocator.h"

namespace ns3 {

class Node;

/**
 * \brief Build a set of XgponNetDevice objects
 *
 * Normally we eschew multiple inheritance, however, the classes 
 * PcapUserHelperForDevice and AsciiTraceUserHelperForDevice are
 * "mixins".
 */
class XgponHelper : public PcapHelperForDevice, public AsciiTraceHelperForDevice
{
public:
  /**
   * Create a XgponHelper to make life easier when creating point to
   * point networks.
   */
  XgponHelper ();
  virtual ~XgponHelper ();

  XgponConfigDb& GetConfigDb () { return m_configDb; }

  //initialize object factories based on typeid strings in XgponConfigDb
  void InitializeObjectFactories (void);


  //Set attributes of xgpon channel
  void SetChannelAttribute (std::string n1, const AttributeValue &v1);


  //Set attributes of xgpon phy
  void SetPhyAttribute (std::string n1, const AttributeValue &v1);



  ///////////////////////////////////////////////////////////////////////////
  // Before installing xgpon netdevices to nodes, object factories must be 
  // configured through XgponConfigDb and initialized. channel attributes 
  // should also be set before that.
  ///////////////////////////////////////////////////////////////////////////

  /**
   * \brief install xgpon network device on the corresponding nodes
   * \return the container that holds the xgpon network devices. The first one is XgponOltNetDevice.
   * \param nodes that are part of the Xgpon network. The first node acts as the OLT.
   */
  NetDeviceContainer Install (NodeContainer nodes);



  //produce Ip address netmask based on netmask length.
  std::string GetOnuAddressNetmask ();
  std::string GetOltAddressNetmask ();
  //produce Ip address netmask based on netmask length.
  std::string GetIpAddressNetmask (uint8_t netmaskLen);



  //produce Ip address base to assign IP address for OLT and ONUs.
  std::string GetXgponIpAddressBase ( );
  //produce Ip address base to assign IP address for the network segment connected through this onu.
  std::string GetOnuIpAddressBase (Ptr<XgponOnuNetDevice> onuDevice);
  std::string GetIpAddressBase (uint16_t firstByte, uint16_t netid, uint8_t netmaskLen);




  //Set attributes of the per xgem-port tx-queue 
  void SetQueueAttribute (std::string n1, const AttributeValue &v1);

  //Set attributes of the per xgem-port qos parameters 
  void SetQosParametersAttribute (std::string n1, const AttributeValue &v1);


  ///////////////////////////////////////////////////////////////////////////
  // Before calling the following functions to add alloc-id and xgem port, 
  // queue and qos parameters related attributes should be configured first.
  ///////////////////////////////////////////////////////////////////////////

  /**
   * \brief Add one T-CONT between the OLT and one ONU
   * \return Alloc-ID of the just added T-CONT
   * \param onuDevice the ONU that this T-CONT belongs to
   * \param oltDevice the OLT
   */
  uint16_t AddOneTcontForOnu (Ptr<XgponOnuNetDevice> onuDevice, Ptr<XgponOltNetDevice> oltDevice, XgponQosParameters::XgponTcontType type);


  /**
   * \brief Add one upstream xgem-port between the OLT and one ONU for one computer connected to this ONU
   * \return ID of the just added xgem-port
   * \param onuDevice the ONU that this computer is connected
   * \param oltDevice the OLT
   * \param allocId the ID of the T-CONT that this xgem-port belongs to
   * \param addr IP address of this computer
   */
  uint16_t AddOneUpstreamConnectionForOnu (Ptr<XgponOnuNetDevice> onuDevice, Ptr<XgponOltNetDevice> oltDevice, uint16_t allocId, const Address& addr);


  /**
   * \brief Add one downstream xgem-port between the OLT and one ONU for one computer connected to this ONU
   * \return ID of the just added xgem-port
   * \param onuDevice the ONU that this computer is connected
   * \param oltDevice the OLT
   * \param addr IP address of this computer
   */
  uint16_t AddOneDownstreamConnectionForOnu (Ptr<XgponOnuNetDevice> onuDevice, Ptr<XgponOltNetDevice> oltDevice, const Address& addr);


  /**
   * \brief Add one broadcast downstream xgem-port between the OLT and all ONUs. 
   *        Note that broadcast connections must be added after all onus have been added to the network.
   * \return ID of the just added xgem-port
   * \param oltDevice the OLT
   * \param addr IP address for the broadcast traffics
   */
  uint16_t AddOneBroadcastDownstreamConnection (Ptr<XgponOltNetDevice> oltDevice, const Address& addr);







  /**
   * \brief Enable pcap output the indicated net device.
   *
   * NetDevice-specific implementation mechanism for hooking the trace and
   * writing to the trace file.
   *
   * \param prefix Filename prefix to use for pcap files.
   * \param nd Net device for which you want to enable tracing.
   * \param promiscuous If true capture all possible packets available at the device.
   * \param explicitFilename Treat the prefix as an explicit filename if true
   */
  virtual void EnablePcapInternal (std::string prefix, Ptr<NetDevice> nd, bool promiscuous, bool explicitFilename);


  /**
   * \brief Enable ascii trace output on the indicated net device.
   * \internal
   *
   * NetDevice-specific implementation mechanism for hooking the trace and
   * writing to the trace file.
   *
   * \param stream The output stream object to use when logging ascii traces.
   * \param prefix Filename prefix to use for ascii trace files.
   * \param nd Net device for which you want to enable tracing.
   * \param explicitFilename Treat the prefix as an explicit filename if true
   *
   */
  virtual void EnableAsciiInternal (
    Ptr<OutputStreamWrapper> stream,
    std::string prefix,
    Ptr<NetDevice> nd,
    bool explicitFilename);






private:

  //create one xgpon channel
  Ptr<XgponChannel> CreateXgponChannel (void);


  //create XgponOltNetDevice and its engines
  Ptr<XgponOltNetDevice> CreateXgponOltNetDeviceAndEngines (void);

  //create XgponOnuNetDevice and its engines
  Ptr<XgponOnuNetDevice> CreateXgponOnuNetDeviceAndEngines (void);


  //attach XgponOltNetDevice to the channel
  void AttachOltToPonChannel (Ptr<XgponChannel> ch, Ptr<XgponOltNetDevice> oltDevice);

  //attach XgponOnuNetDevice to the channel
  void AttachOnuToPonChannel (Ptr<XgponChannel> ch, Ptr<XgponOnuNetDevice> onuDevice);

  //add the onu-related information at OLT-side
  void AddOnuToOlt (Ptr<XgponOnuNetDevice> onuDevice, Ptr<XgponOltNetDevice> oltDevice);




  //add one ALLOC-ID for one ONU. Note that allocId is from XgponIdAllocator.
  void AddOneTcontForOnu (Ptr<XgponOnuNetDevice> onuDevice, Ptr<XgponOltNetDevice> oltDevice, uint16_t allocId, XgponQosParameters::XgponTcontType type);

  //add one upstream xgem-port for the computer that connects to one ONU. allocId is from previous call for adding T-CONT and portId is from XgponIdAllocator.
  void AddOneUpstreamConnectionForOnu (Ptr<XgponOnuNetDevice> onuDevice, Ptr<XgponOltNetDevice> oltDevice, uint16_t allocId, const Address& addr, uint16_t portId);

  //add one downstream xgem-port for the computer that connects to one ONU. portId is from XgponIdAllocator.
  void AddOneDownstreamConnectionForOnu (Ptr<XgponOnuNetDevice> onuDevice, Ptr<XgponOltNetDevice> oltDevice, const Address& addr, uint16_t portId);  


private:

  XgponConfigDb m_configDb;                //hold the configuration-related information
  XgponIdAllocator* m_idAllocator;          //one singleton used to allocate IDs (xgem-port, alloc-id, onu-id, etc.)



  //Object factories for setting the attributes and creating objects
  ObjectFactory m_channelFactory;

  ObjectFactory m_oltDbaEngineFactory;
  ObjectFactory m_oltDsSchedulerEngineFactory;
  ObjectFactory m_oltConnManagerEngineFactory;

  ObjectFactory m_onuConnManagerEngineFactory;
  ObjectFactory m_onuUsSchedulerEngineFactory;

  ObjectFactory m_queueFactory;  
  ObjectFactory m_qosParametersFactory;  
};

}//namespace ns3

#endif /* XGPON_HELPER_H */
