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

#include "xgpon-olt-engine.h"
#include "xgpon-olt-net-device.h"


NS_LOG_COMPONENT_DEFINE ("XgponOltEngine");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponOltEngine);

TypeId 
XgponOltEngine::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltEngine")
    .SetParent<Object> ()
  ;
  return tid;
}
TypeId 
XgponOltEngine::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponOltEngine::XgponOltEngine ()
{
}
XgponOltEngine::~XgponOltEngine ()
{
}




void 
XgponOltEngine::SetXgponOltNetDevice (const Ptr<XgponOltNetDevice>& device)
{
  m_device = device;
}
const Ptr<XgponOltNetDevice>& 
XgponOltEngine::GetXgponOltNetDevice ( )
{
  NS_ASSERT_MSG((m_device!=0), "Network device has not been set yet.");
  return m_device;
}




}; // namespace ns3

