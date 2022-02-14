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

#include "xgpon-xgtc-us-header.h"

NS_LOG_COMPONENT_DEFINE ("XgponXgtcUsHeader");

namespace ns3 {

XgponXgtcUsHeader::XgponXgtcUsHeader ()
  : m_onuId (0x0000),
    m_ind (0x0000),
    m_hec (0x0000),
    m_ploam(0),
    meta_ploamExist (false)
{
}
XgponXgtcUsHeader::~XgponXgtcUsHeader ()
{
  m_ploam = 0;
}










void
XgponXgtcUsHeader::Print (std::ostream &os)  const
{
  os << " ONU-ID=" << (uint16_t)m_onuId;

  os.setf(std::ios::hex, std::ios::basefield);
  os << " IND=" << (uint16_t)m_ind;
  os << " HEC=" << (uint16_t)m_hec;
  os.unsetf(std::ios::hex);

  if(meta_ploamExist==true)
  {
    os << " PLOAM: ";
    m_ploam->Print (os);
  }

  os << std::endl;

  return;
}

uint32_t XgponXgtcUsHeader::GetSerializedSize (void) const
{
  uint32_t len;

  len = 4; 
  if(meta_ploamExist == true) len = len + 48;

  return len;
}

void XgponXgtcUsHeader::Serialize (Buffer::Iterator start) const
{
  uint32_t rst;

  rst = (m_onuId <<22) | (m_ind << 13) | m_hec;
  start.WriteHtonU32(rst);

  if(meta_ploamExist == true) m_ploam->Serialize(start);
  
  return;
}

uint32_t XgponXgtcUsHeader::Deserialize (Buffer::Iterator start)
{
  uint32_t rst;

  rst = start.ReadNtohU32();
  m_onuId = rst >> 22;
  m_ind = (rst >> 13) & 0x1ff;
  m_hec = rst & 0x1fff;

  //VerifyHec ();

  if(meta_ploamExist == true) m_ploam->Deserialize(start);

  return GetSerializedSize ();
}

}; // namespace ns3
