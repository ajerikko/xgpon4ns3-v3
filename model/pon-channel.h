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

#ifndef PON_CHANNEL_H
#define PON_CHANNEL_H

#include <ns3/ptr.h>
#include "ns3/channel.h"

#include "pon-net-device.h"
#include "pon-frame.h"


namespace ns3 {

/**
 * \ingroup pon
 * \brief A general optical distribution channel for TDMA-based PON. 
 *
 * PonChannel is designed as an abstract class. XgponChannel is the subclass implemented for XG-PON. 
 * It is responsible to maintain the network devices (OLT and ONUs) attached to this channel
 * and simulate propagation delay and data transmission between OLT and ONUs.
 */

class PonChannel : public Channel
{
public:

  /**
   * \brief Constructor
   */
  PonChannel ();
  virtual ~PonChannel ();


  /********************************************************************************
   * Functions used by NetDevices for sending out one frame through a pon network.
   *******************************************************************************/
  /**
   * \brief transmit the upstream burst from one ONU to the OLT
   * \param frame the frame to be transfered from one ONU to the OLT.
   * \param index the index of the onu device that transmits the upstream frame/burst
   *              it is used to find the corresponding propagation delay.
   */
  void SendUpstream (const Ptr<PonFrame>& frame, uint16_t index);

  /**
   * \brief send the downstream frame from OLT to all ONUs
   * \param frame the frame to be transfered from the OLT to all ONUs.
   */
  void SendDownstream (const Ptr<PonFrame>& frame);




  ///////////////////////////////////////////////////////////////////////////////////Operations related with ONUs. 
  //////////////Since different subclasses may organize ONUs in different ways, these functions are pure abstract.
  /**
   * \brief add onu network device to the channel.
   * \param device the ONU NetDevice to be attached to this channel;
   * \return the index of this ONU in this channel. This index will be stored in the ONU network device.
   *         When sending in upstream direction, this index (and the data) will be passed to the channel by the ONU. 
   *         Thus, the channel will be able to find this ONU's propagation delay quickly.
   */
  virtual uint16_t AddOnu (const Ptr<PonNetDevice>& device)=0;

  /**
   * \brief Get the total number of ONUs attached to this channel
   */
  virtual uint16_t GetNOnuDevices ( ) const = 0;

  /**
   * \brief Get Onu device based on index. Note that OLT should not be considered in this index.
   */
  virtual const Ptr<PonNetDevice>& GetOnuByIndex (uint32_t index) const = 0;


  /**
   * \brief Set the one-way propagation delay between OLT and one ONU. The unit of the delay is nanosecond.
   * \param index the index of this ONU in this channel. Unit: nanosecond
   * \param delay the one-way propagation delay between OLT and this ONU.
   */
  virtual void SetOnuPropagationDelay (uint16_t onuIndex, uint32_t delay) = 0;  

  /**
   * \brief  Get the one-way propagation delay between OLT and this ONU
   * \param  index the index of this ONU in this channel.
   * \return the one-way propagation delay between OLT and this ONU.
   */
  virtual uint32_t GetOnuPropagationDelay (uint16_t onuIndex) const = 0;  







  //////////////////////////////////////////////////////Inline Functions for OLT
  void SetOlt (const Ptr<PonNetDevice>& device);
  const Ptr<PonNetDevice>& GetOlt ( ) const;




  //////////////////////////////////////////////////////Inherited from Channel
  /**
   * \brief Get the number of all network devices (OLT + ONUs) on this channel.
   */
  virtual std::size_t GetNDevices (void) const; //ja:update:ns-3.35 uint32_t changed to std::size_t to match ns3/channel.h

  /**
   * \brief Get one network device based on index. Here, 0: olt, i: m_onuDevices[i-1].
   */
  virtual Ptr<NetDevice> GetDevice (std::size_t i) const; //ja:update:ns-3.35 unint32_t changed to std::size_t to match ns3/channel.h




  /////////////////////////////////////////////////////////////required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


protected:
  Ptr<PonNetDevice> m_oltDevice;           //the OLT network device attached to this channel.

};



///////////////////////////////////////////////INLINE FUNCTIONS
inline void
PonChannel::SetOlt (const Ptr<PonNetDevice>& device)
{
  m_oltDevice=device;
}
inline const Ptr<PonNetDevice>& 
PonChannel::GetOlt ( ) const
{
  NS_ASSERT_MSG((m_oltDevice!=0), "OLT NetDevice has not been set in the network!!!");
  return m_oltDevice;
}




inline std::size_t //ja:update:ns-3.35 uint32_t changed to std::size_t to match ns3/channel.h
PonChannel::GetNDevices (void) const
{
  NS_ASSERT_MSG((m_oltDevice!=0), "OLT NetDevice has not been set in the network!!!");
  return GetNOnuDevices()+1;
}
inline Ptr<NetDevice>
PonChannel::GetDevice (std::size_t i) const
{
  if(i==0) { return m_oltDevice; }
  else { return GetOnuByIndex(i-1); }
}


} // namespace ns3

#endif  // PON_CHANNEL_H
