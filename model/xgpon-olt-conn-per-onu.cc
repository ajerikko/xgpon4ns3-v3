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

#include "xgpon-olt-conn-per-onu.h"



NS_LOG_COMPONENT_DEFINE ("XgponOltConnPerOnu");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponOltConnPerOnu);

TypeId 
XgponOltConnPerOnu::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltConnPerOnu")
    .SetParent<Object> ()
    .AddConstructor<XgponOltConnPerOnu> ()
  ;
  return tid;
}
TypeId 
XgponOltConnPerOnu::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponOltConnPerOnu::XgponOltConnPerOnu ():
  m_onuId(0),
  m_tconts(0),
  m_connections(0)
{
}
XgponOltConnPerOnu::~XgponOltConnPerOnu ()
{
}





}; // namespace ns3

