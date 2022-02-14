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

#include "ns3/log.h"

#include "xgpon-olt-ploam-engine.h"
#include "xgpon-channel.h"   //the constant for maximal onus per xgpon



NS_LOG_COMPONENT_DEFINE ("XgponOltPloamEngine");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponOltPloamEngine);

TypeId 
XgponOltPloamEngine::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltPloamEngine")
    .SetParent<XgponOltEngine> ()
    .AddConstructor<XgponOltPloamEngine> ()
  ;
  return tid;
}
TypeId 
XgponOltPloamEngine::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


XgponOltPloamEngine::XgponOltPloamEngine ():
  m_linkInfos(XgponChannel::MAXIMAL_NODES_PER_XGPON),
  m_ploamMsgsToBroadcast(0)
{
}
XgponOltPloamEngine::~XgponOltPloamEngine ()
{
}







void 
XgponOltPloamEngine::ReceivePloamMessage (const Ptr<XgponXgtcPloam>& msg, uint16_t onuId)
{
  NS_LOG_FUNCTION(this);

  return;
}
void
XgponOltPloamEngine::GeneratePloamMessage ( )
{
  NS_LOG_FUNCTION(this);

  //Below is just one example.

  Ptr<XgponXgtcPloam> ploam = Create<XgponXgtcPloam>();

  //TODO: add to corresponding queues.

  return;
}

std::list< Ptr<XgponXgtcPloam> >& 
XgponOltPloamEngine::GetPloamsForTransmit(void)
{
  NS_LOG_FUNCTION(this);

  //Below is just one example (empty list).
  //When Ploam related functions are implemented, we need consider the maximal ploam messages in one frame header.

  //ja:update:ns-3.35 return value for empty() is defined, though not used
  auto is_empty __attribute__((unused)) = m_ploamMsgsToBroadcastInCurrentFrame.empty();

  return m_ploamMsgsToBroadcastInCurrentFrame;
}








}; // namespace ns3

