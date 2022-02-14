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

#ifndef PON_NET_DEVICE_H
#define PON_NET_DEVICE_H

#include "ns3/net-device.h"
#include "ns3/address.h"
#include "ns3/node.h"

#include "pon-frame.h"


namespace ns3{

class PonChannel;

/**
 * \defgroup pon PON (TDMA-based Passive Optical Network) models
 *
 */

/**
 * \ingroup pon
 * \brief The PON net device. It is the main class that interacts with upper layers and the optical distribution channel.
 *
 */
class PonNetDevice: public NetDevice
{
public:

  /**
   * \brief Constructor
   */
  PonNetDevice ( );
  virtual ~PonNetDevice ( ) ;


  //////////////////////////////////////////////////////////////////////Main functions
  /**
   * \brief receive a PonFrame from the channel. As for sending a frame to PonChannel, its subclasses will call PonChannel interface directly.
   * \param frame the frame received from the channel. 
   */
  virtual void ReceivePonFrameFromChannel (const Ptr<PonFrame>& frame)=0;




  /**
   * \brief set the channel that this network device is attached.
   *        Note that GetChannel is inherited from NetDevice and is listed below
   */
  void SetChannel (const Ptr<PonChannel>& channel);  


  /////////////////////////////////////////////////////INLINE Functions
  /**
   * \brief set the index of this device on the channel. used for ONU only
   */
  void SetChannelIndex (uint16_t index);      

  /**
   * \brief get the index of this device on the channel. 
   *        When sending in upstream, this index is passed to the channel.
   *        The channel can get the propagation delay of this ONU and dispatch data to the OLT accordingly.
   */
  uint16_t GetChannelIndex ( );







  //////////////////////////////////////////////////////////inherit from NetDevice. 
  //////////////Defined here to reduce replication of code and make easier to design the sub-classes of PonNetDevice
  //Get packets from upper layers (Defined in Onu/Olt NetDevices) to transmit in the future
  virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) = 0;
  virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber) = 0;


  virtual void SetIfIndex (const uint32_t index);
  virtual uint32_t GetIfIndex (void) const;

  virtual Ptr<Channel> GetChannel (void) const;

  virtual void SetAddress (Address address);
  virtual Address GetAddress (void) const;

  virtual bool SetMtu (const uint16_t mtu);  //MTU should be set through one attribute
  virtual uint16_t GetMtu (void) const;

  virtual bool IsLinkUp (void) const;

  virtual void AddLinkChangeCallback (Callback<void> callback);

  virtual bool IsBroadcast (void) const;
  virtual Address GetBroadcast (void) const;

  virtual bool IsMulticast (void) const;
  virtual Address GetMulticast (Ipv4Address multicastGroup) const;
  virtual Address GetMulticast (Ipv6Address addr) const;

  virtual bool IsPointToPoint (void) const;
  virtual bool IsBridge (void) const;



  virtual Ptr<Node> GetNode (void) const;
  virtual void SetNode (Ptr<Node> node);

  virtual bool NeedsArp (void) const;

  virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);
  virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb);


  virtual bool SupportsSendFrom (void) const;


  ////////////////////////////////////////////////////////////////////////required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


protected:
  //set by upper layers for receiving packets from this network device (promisc or not)
  NetDevice::ReceiveCallback m_rxCallback;                 
  NetDevice::PromiscReceiveCallback m_promiscCallback; 

  Ptr<PonChannel> m_channel; //the channel that this netdevice is attached to
  uint16_t m_channelIndex;   //index of this interface on the channel. (used by onu only)
                             //when passing one upstream burst to the channel, 
                             //ONU uses this index to allow the channel to get the corresponding propagation delay quickly.

  Address m_addr;            //(IP) address of this interface


  Ptr<Node> m_node;          //The node that this network device is installed.
  uint32_t m_ifIndex;        //index of this interface on the node
  bool     m_linkUp;         //state of this interface
  uint16_t m_mtu;            //maximal transmission unit


  virtual void DoDispose (void);

};






////////////////////////////////////////////INLINE Functions
inline void 
PonNetDevice::SetChannelIndex (uint16_t index)
{
  m_channelIndex = index;
}
inline uint16_t 
PonNetDevice::GetChannelIndex ( )
{
  return m_channelIndex;
}




inline void
PonNetDevice::SetAddress (Address address)
{
  m_addr = address;
}
inline Address 
PonNetDevice::GetAddress (void) const
{
  return m_addr;
}


inline void 
PonNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
  return;
}


inline bool 
PonNetDevice::IsBroadcast (void) const
{
  return true;
}
inline Address
PonNetDevice::GetBroadcast (void) const
{
  return Ipv4Address ("255.255.255.255");
}


inline bool 
PonNetDevice::IsMulticast (void) const
{
  return false;
}
inline Address 
PonNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  return m_addr;
}
inline Address PonNetDevice::GetMulticast (Ipv6Address addr) const
{
  return m_addr;
}


inline void 
PonNetDevice::SetIfIndex (const uint32_t index)
{
  m_ifIndex = index;
}
inline uint32_t 
PonNetDevice::GetIfIndex (void) const
{
  return m_ifIndex;
}


inline bool 
PonNetDevice::SetMtu (const uint16_t mtu)
{
  m_mtu = mtu;
  return true;
}
inline uint16_t 
PonNetDevice::GetMtu (void) const
{
  return m_mtu;
}


inline bool 
PonNetDevice::IsLinkUp (void) const
{
  return true;
}


inline bool 
PonNetDevice::IsPointToPoint (void) const
{
  return false;
}


inline bool 
PonNetDevice::IsBridge (void) const
{
  return false;
}


inline Ptr<Node> 
PonNetDevice::GetNode (void) const
{
  return m_node;
}
inline void 
PonNetDevice::SetNode (Ptr<Node> node)
{
  m_node = node;
}


inline bool 
PonNetDevice::NeedsArp (void) const
{
  return false;
}


inline void 
PonNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{
  m_rxCallback = cb;
}

inline void
PonNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
  m_promiscCallback = cb;
}


inline bool
PonNetDevice::SupportsSendFrom (void) const
{
  return true;
}




} //namespace ns3
#endif /* PON_NET_DEVICE_H */
