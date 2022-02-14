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

#include "xgpon-tcont.h"

NS_LOG_COMPONENT_DEFINE ("XgponTcont");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponTcont);

TypeId 
XgponTcont::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponTcont")
    .SetParent<Object> ()
  ;
  return tid;
}
TypeId 
XgponTcont::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

XgponTcont::XgponTcont ():
  m_allocId(0),
  m_onuId(0),
  m_qosParameters(0),
  m_allocatedRate(0),
  m_pirSI(10), //10*125us of initial timer
  m_bufOccupancyReports(0),
  m_bwAllocations(0),
  m_nullDbru(0),
  m_nullBwAlloc(0)
{
}
XgponTcont::~XgponTcont ()
{
}



}; // namespace ns3

