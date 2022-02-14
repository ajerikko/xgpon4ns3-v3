/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 University College Cork (UCC), Ireland
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

#ifndef XGPON_NET_DEVICE_H
#define XGPON_NET_DEVICE_H

#include "ns3/traced-callback.h"
#include "ns3/packet.h"

#include "pon-net-device.h"
#include "xgpon-phy.h"
#include "xgpon-qos-parameters.h"



namespace ns3{

/**
 * \ingroup  pon
 * \defgroup xgpon XG-PON models
 *
 */

/////////////////////////////////////Xgpon-Interface Statistics
class XgponNetDeviceStatistics
{
public:
  uint64_t m_currentTime;  //current simulation time: nanosecond

  uint64_t m_rxFromUpperLayerPkts;
  uint64_t m_rxFromXgponPkts;

  uint64_t m_passToUpperLayerPkts;
  uint64_t m_passToXgponPkts;

  uint64_t m_overallQueueDropPkts;


  uint64_t m_rxFromUpperLayerBytes;
  uint64_t m_rxFromXgponBytes[1024];
        uint64_t m_rxT1FromXgponBytes[1024];
        uint64_t m_rxT2FromXgponBytes[1024];
        uint64_t m_rxT3FromXgponBytes[1024];
        uint64_t m_rxT4FromXgponBytes[1024];

  uint64_t m_passToUpperLayerBytes;
  uint64_t m_passToXgponBytes;

  uint64_t m_overallQueueDropBytes;

  void initialize ();
};



/**
 * \ingroup xgpon
 * \brief The XG-PON net device. It is the main class that interacts with upper layers and the optical distribution channel.
 *
 */

class XgponNetDevice: public PonNetDevice
{
  const static uint16_t DEFAULT_XGPON_MTU = 16383;           //Unit: byte; For Xgpon (14-bits payload length indicator).

public:
  const static uint64_t HISTORY_2_MAINTAIN = 1000000000;     //unit: nanosecond;   1 seconds

  /**
   * \brief Constructor
   */
  XgponNetDevice ( );
  virtual ~XgponNetDevice ( ) ;

 
  /**
   * \brief receive sdu from upper layers
   */
  //process one packet from upper layers to be transmitted in the future. 
  //Note that we cannot use the addresses in parameters since they are MAC-layer addresses.
  //Otherwise, we cannot assign multiple XGEM ports to one network interface.
  //The main function is to put the packet into the corresponding queue. return false if the queue is full.
  //They mainly implement packet traces. DoSend and DoSendFrom will do the real work.
  virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);    
  virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);





  /**
   * \brief pass sdu to the upper layer. It will be called by XgemEngine after one SDU is received (and reassembed).
   * \param sdu the data received from the peer through xgpon channel
   */
  void SendSduToUpperLayer (const Ptr<Packet>& sdu, uint16_t tcontType, uint16_t onuId);


  /**
   * \brief trace the event that one packet will be trnsmitted on xgpon
   *        called by Xgem engines when they get one packet from a connection for transmitting.
   */
  void TraceVirtualQueueDequeueEvent (const Ptr<Packet>& packet);


  /**
   * \brief trace the packet for supporting pcap capability.
   * called by XgponNetDevice (sdu from upper layer) and Xgem engines (sdu from the peer).
   */
  void TraceForSniffers (const Ptr<Packet>& packet);



  /**
   * \brief get the cumulative statistics of one XgponNetDevice
   */
  XgponNetDeviceStatistics& GetStatistics ();



  /**
   * \brief Set physical layer parameters/routines that are common for both OLT and ONU. 
   */
  void SetXgponPhy (const Ptr<XgponPhy>& phy);

  /**
   * \brief get physical layer parameters/routines that are common for both OLT and ONU. 
   */
  const Ptr<XgponPhy>& GetXgponPhy ( ) const;

  //jerome, C1
  void SetQosParameters (const Ptr<XgponQosParameters>& qosParameters);
  const Ptr<XgponQosParameters>& GetQosParameters () const;

  //////////////////////////////////required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


protected:  
  XgponNetDeviceStatistics m_stat;  //per netdevice statistics

  Ptr<XgponPhy> m_commonPhy;    //physical layer parameters/routines that are common for both OLT and ONU.


  //Trace sources for the whole network device statistics
  TracedCallback<const XgponNetDeviceStatistics& > m_deviceStatisticsTrace;
  Ptr<XgponQosParameters> m_qosParameters;  //jerome, C1, qos parameters associated with the net device

private:
  //process one packet from upper layers. Note that we cannot use the addresses in parameters since they are MAC-layer addresses.
  //Otherwise, we cannot assign multiple XGEM ports to one network interface.
  //The main function is to put the packet into the corresponding queue. return false if the queue is full.
  virtual bool DoSend (const Ptr<Packet>& packet, const Address& dest, uint16_t protocolNumber) = 0;    
  virtual bool DoSendFrom (const Ptr<Packet>& packet, const Address& source, const Address& dest, uint16_t protocolNumber) = 0;



  //Trace sources for simulating a virtual per-device queue: enqueue, dequeue, drop events
  TracedCallback< Ptr<const Packet> > m_traceEnqueueVirtual;
  TracedCallback< Ptr<const Packet> > m_traceDequeueVirtual;
  TracedCallback< Ptr<const Packet> > m_traceDropVirtual;

  //Trace source designed to simulate a packet sniffer facility (tcpdump).
  //Note that ONU does not reassemble packets for other ONUs. 
  //Thus, there is no difference between the following two trace sources.
  TracedCallback< Ptr<const Packet> > m_snifferTrace;
  TracedCallback< Ptr<const Packet> > m_promiscSnifferTrace;

};




///////////////////////////////INLINE functions
inline XgponNetDeviceStatistics& 
XgponNetDevice::GetStatistics ()
{
  return m_stat;
}



inline void 
XgponNetDevice::SetXgponPhy (const Ptr<XgponPhy>& phy)
{
  m_commonPhy = phy;
}
inline const Ptr<XgponPhy>& 
XgponNetDevice::GetXgponPhy ( ) const
{
  NS_ASSERT_MSG ((m_commonPhy !=0), "phy has not been set yet.");
  return m_commonPhy;
}

//jerome, C1
inline void 
XgponNetDevice::SetQosParameters (const Ptr<XgponQosParameters>& qosParameters)
{
  m_qosParameters = qosParameters;
}
inline const Ptr<XgponQosParameters>& 
XgponNetDevice::GetQosParameters () const
{
  NS_ASSERT_MSG((m_qosParameters!=0), "QoS parameter has not been set yet.");

  return m_qosParameters;
}

} //namespace ns3
#endif /* XGPON_NET_DEVICE_H */
