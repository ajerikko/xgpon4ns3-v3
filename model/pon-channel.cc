/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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
 * Author: Pedro Alvarez <pinheirp@tcd.ie>
 */

#include "ns3/simulator.h"
#include "ns3/log.h"

#include "pon-channel.h"


NS_LOG_COMPONENT_DEFINE ("PonChannel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (PonChannel);

TypeId
PonChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PonChannel")
    .SetParent<Channel> ()
  ;
  return tid;
}
TypeId
PonChannel::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


PonChannel::PonChannel () : Channel(),  m_oltDevice(0)
{
}
PonChannel::~PonChannel ()
{
}




void
PonChannel::SendUpstream (const Ptr<PonFrame>& frame, uint16_t index)
{
  NS_LOG_INFO ("Schedule to Send One Upstream Burst to the OLT"); 

  uint32_t delay = GetOnuPropagationDelay(index);

  const Ptr<PonNetDevice>& oltDevice = GetOlt();
  Simulator::ScheduleWithContext (oltDevice->GetNode ()->GetId (), NanoSeconds (delay), &PonNetDevice::ReceivePonFrameFromChannel, oltDevice, frame);

}


void
PonChannel::SendDownstream (const Ptr<PonFrame>& frame)
{
  NS_LOG_INFO ("Schedule to Send One Downstream Frame to all ONUs");

  for (int i = 0; i < GetNOnuDevices(); i++)
  {
    uint32_t delay = GetOnuPropagationDelay(i);

    const Ptr<PonNetDevice>& onuDevice = GetOnuByIndex(i);   
    Simulator::ScheduleWithContext (onuDevice->GetNode ()->GetId (), NanoSeconds (delay), &PonNetDevice::ReceivePonFrameFromChannel, onuDevice, frame);
  }
}




}//namespace ns-3
