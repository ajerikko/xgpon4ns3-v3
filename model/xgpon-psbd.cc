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

#include "ns3/xgpon-psbd.h"

NS_LOG_COMPONENT_DEFINE ("XgponPsbd");

namespace ns3 {

XgponPsbd::XgponPsbd ():
  m_psync(0), m_sfc(0), m_sfcHec(0),
  m_ponId(0), m_ponIdHec(0)
{
}
XgponPsbd::~XgponPsbd ()
{
}







XgponPsbd& 
XgponPsbd::operator=(const XgponPsbd &rhs)
{
  m_psync = rhs.m_psync;
  m_sfc = rhs.m_sfc;
  m_sfcHec = rhs.m_sfcHec;
  m_ponId = rhs.m_ponId;
  m_ponIdHec = rhs.m_ponIdHec;
  
  return *this;
}







void
XgponPsbd::Print (std::ostream &os)  const
{
  os << " PSYC: " << m_psync;
  os << " SFC: " << m_sfc;
  os << " PON-ID: " << m_ponId;
  os << std::endl;
}

uint32_t 
XgponPsbd::GetSerializedSize (void) const
{
  return 24;  //24 bytes;
}

void 
XgponPsbd::Serialize (Buffer::Iterator start) const
{
  uint64_t sfcStructure=(m_sfc<<13)|m_sfcHec;
  uint64_t ponIdStructure=(m_ponId<<13)|m_sfcHec;
  
  start.WriteHtonU64 (m_psync);
  start.WriteHtonU64 (sfcStructure);
  start.WriteHtonU64 (ponIdStructure);
 
  return;
}

uint32_t 
XgponPsbd::Deserialize (Buffer::Iterator start)
{
  uint64_t sfcStructure, ponIdStructure;  
  
  m_psync=start.ReadNtohU64();

  sfcStructure = start.ReadNtohU64();
  m_sfcHec=(sfcStructure & 0x1fff);
  m_sfc=(sfcStructure>>13);

  ponIdStructure = start.ReadNtohU64();
  m_ponIdHec=(ponIdStructure & 0x1fff);
  m_ponId=(ponIdStructure>>13);

  return GetSerializedSize ();
}

}; // namespace ns3
