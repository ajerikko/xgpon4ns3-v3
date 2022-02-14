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

#include "xgpon-burst-profile.h"



NS_LOG_COMPONENT_DEFINE ("XgponBurstProfile");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponBurstProfile);

TypeId 
XgponBurstProfile::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponBurstProfile")
    .SetParent<Object> ()
    .AddConstructor<XgponBurstProfile> ()
  ;
  return tid;
}
TypeId 
XgponBurstProfile::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponBurstProfile::XgponBurstProfile ():
  m_preambleLen(PSBU_PREAMBLE_DEFAULT_LEN),
  m_delimiterLen(PSBU_DELIMITER_DEFAULT_LEN),
  m_fec (true)
{
}
XgponBurstProfile::~XgponBurstProfile ()
{
}






void 
XgponBurstProfile::DeepCopy(const Ptr<XgponBurstProfile>& profile)
{
  m_preambleLen = profile->m_preambleLen;
  m_delimiterLen = profile->m_delimiterLen;
  m_fec = profile->m_fec;
}



}; // namespace ns3

