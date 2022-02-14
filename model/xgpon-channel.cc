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

#include "ns3/log.h"
#include "ns3/uinteger.h"

#include "xgpon-channel.h"


NS_LOG_COMPONENT_DEFINE ("XgponChannel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponChannel);

TypeId 
XgponChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponChannel")
    .SetParent<PonChannel> ()
    .AddConstructor<XgponChannel> ()
    .AddAttribute ("LogicOneWayPropDelay", 
                   "The logical one way delay agreed by the whole network (OLT and all ONUs). Its unit is nanosecond and should be the sum of maximal propagation delay, processing delay, etc.",
                   UintegerValue (XgponChannel::DEFAULT_LOGIC_ONE_WAY_DELAY),
                   MakeUintegerAccessor (&XgponChannel::m_logicOneWayDelay),
                   MakeUintegerChecker<uint32_t> ())


  ;
  return tid;
}
TypeId 
XgponChannel::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


XgponChannel::XgponChannel () : PonChannel(), m_onuDevices(0), m_onuPropDelays(0)
{
}
XgponChannel::~XgponChannel ()
{
}




} // namespace ns3
