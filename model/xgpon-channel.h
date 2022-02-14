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

#ifndef XGPON_CHANNEL_H
#define XGPON_CHANNEL_H

#include <vector>

#include "pon-channel.h"
#include "xgpon-phy.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief A simple XG-PON instance of PON channel.
 *        It has one variable to specify the logic one-way delay of this channel, i.e.,
 *        the maximal propagation delay of ONUs plus various processing delays.
 */
class XgponChannel : public PonChannel
{
public:
  const static uint16_t MAXIMAL_NODES_PER_XGPON = 1024;        //The maximal number of onus supported by the channel.
  const static uint32_t DEFAULT_LOGIC_ONE_WAY_DELAY = 400000;  //Unit: nanosecond;   0.4ms
                                                               //the one way propagation delay (light in optical fibre --- 2*10^8) for a distance of 60km + process delay 


  /**
   * \brief Constructor
   */  
  XgponChannel ( );
  virtual ~XgponChannel ( );




  ///////////////////////////////////////////////////Operations related with ONUs. 
  /**
   * \brief add onu network device to the channel.
   * \param device the ONU NetDevice to be attached to this channel;
   * \return the index of this ONU in this channel. This index will be stored in the ONU network device.
   *         When sending in upstream, this index (and the data) will be passed to the channel by the ONU. 
   *         Thus, the channel will be able to find this ONU's propagation delay quickly.
   */
  virtual uint16_t AddOnu (const Ptr<PonNetDevice>& device);

  /**
   * \brief Get the total number of ONUs attached to this channel
   */
  virtual uint16_t GetNOnuDevices ( ) const;

  /**
   * \brief Get Onu device based on index. Note that OLT should not be considered in this index.
   */
  virtual const Ptr<PonNetDevice>& GetOnuByIndex (uint32_t index) const;


  /**
   * \brief Set the one-way propagation delay between OLT and one ONU. The unit of the delay is nanosecond.
   * \param index the index of this ONU in this channel. Unit: nanosecond
   * \param delay the one-way propagation delay between OLT and this ONU.
   */
  virtual void SetOnuPropagationDelay (uint16_t onuIndex, uint32_t delay);  

  /**
   * \brief  Get the one-way propagation delay between OLT and this ONU
   * \param  index the index of this ONU in this channel.
   * \return the one-way propagation delay between OLT and this ONU.
   */
  virtual uint32_t GetOnuPropagationDelay (uint16_t onuIndex) const;  






  ///////////////////////////////////////////////////Operations for logic propagation delay and physical layer parameters
  /**
   * \brief get the logical one way delay agreed by the whole network (OLT and all ONUs). 
   *        Note that the unit is nanosecond and should be the sum of maximal propagation delay, processing delay, etc.
   *        There is no set operation since the value should be configured through attribute feature of NS-3.
   */
  uint32_t GetLogicOneWayDelay (void) const;   







  /////////////////////////////////////////////////////////////////////////required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;



private:

  //different channels (passive splitters included) may be implemented in the future and different data structures may be used for ONUs.
  std::vector< Ptr<PonNetDevice> > m_onuDevices;    //NetDevices entities of all ONU network devices attached to this channel. 
  std::vector< uint32_t > m_onuPropDelays;          //to simulate different distances to OLT in the future. 


  uint32_t m_logicOneWayDelay;                      //the logic one way delay agreed/observed by OLT and all ONUs to avoid upstream collision.
                                                    //Unit: nanosecond; Note that EPON has no such concept.
};



///////////////////////////////////////////////INLINE Functions
inline uint16_t
XgponChannel::AddOnu (const Ptr<PonNetDevice>& device)
{
  NS_ASSERT_MSG((m_onuDevices.size() < MAXIMAL_NODES_PER_XGPON), "Too many ONUs are attached to this channel.");

  m_onuDevices.push_back (device);

  //The delay is initialized to 0. It will be set later through "SetOnuPropagationDelay".
  m_onuPropDelays.push_back (0);  

  return m_onuDevices.size () - 1;
}


inline const Ptr<PonNetDevice>& 
XgponChannel::GetOnuByIndex (uint32_t index) const
{
  return m_onuDevices[index];
}

inline uint16_t 
XgponChannel::GetNOnuDevices ( ) const
{
  return m_onuDevices.size ();
}


inline void 
XgponChannel::SetOnuPropagationDelay (uint16_t onuIndex, uint32_t delay)
{
  m_onuPropDelays[onuIndex] = delay;
}
inline uint32_t 
XgponChannel::GetOnuPropagationDelay (uint16_t onuIndex) const
{
  NS_ASSERT_MSG((onuIndex < m_onuPropDelays.size()), "The index of ONU is too large!!!");
  NS_ASSERT_MSG ((m_onuPropDelays[onuIndex] > 0), "One Way Delay has not been set yet.");

  return m_onuPropDelays[onuIndex];
}



inline uint32_t 
XgponChannel::GetLogicOneWayDelay (void) const
{
  NS_ASSERT_MSG ((m_logicOneWayDelay > 0), "One Way Logic Delay has not been set yet.");
  return m_logicOneWayDelay;
}



} // namespace ns3

#endif /* XGPON_CHANNEL_H */
