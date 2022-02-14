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

#include "xgpon-xgtc-ds-header.h"



NS_LOG_COMPONENT_DEFINE ("XgponXgtcDsHeader");

namespace ns3 {

XgponXgtcDsHeader::XgponXgtcDsHeader ()
  : m_bwmapLen (0x0000),
    m_ploamCount (0x0000),
    m_hec(0), m_bwmap(0), m_ploams(0)
{
}
XgponXgtcDsHeader::~XgponXgtcDsHeader ()
{
}






void
XgponXgtcDsHeader::Print (std::ostream &os)  const
{
  uint16_t i;

  uint32_t m_bwmapLen2 = m_bwmap->GetNumberOfBwAllocation();
  uint32_t m_ploamCount2 = m_ploams.size();

  os << " BWMAP-LEN=" << (uint16_t)m_bwmapLen2;
  os << " PLOAM-COUNT=" << (uint16_t)m_ploamCount2;

  os.setf(std::ios::hex, std::ios::basefield);
  os << " HEC=" << (uint16_t)m_hec;
  os.unsetf(std::ios::hex);

  os << " BWMAP: ";
  m_bwmap->Print(os);

  os << " PLOAM: ";
  for(i=0; i<m_ploamCount2; i++)
  {
    os << " PLOAM " << i << ": ";
    GetPloamByIndex(i)->Print(os);
  }

  os << std::endl;

  return;
}

uint32_t XgponXgtcDsHeader::GetSerializedSize (void) const
{
  uint32_t len, i;

  uint32_t m_ploamCount2 = m_ploams.size();

  len = 4; 

  len = len + m_bwmap->GetSerializedSize();

  for(i=0; i<m_ploamCount2; i++)
  {
    len = len + GetPloamByIndex(i)->GetSerializedSize();
  }

  return len;
}

void XgponXgtcDsHeader::Serialize (Buffer::Iterator start) const
{

  uint32_t hlend, i;

  Ptr<XgponXgtcBwAllocation> tmpBwAllocation;


  uint32_t m_bwmapLen2 = m_bwmap->GetNumberOfBwAllocation();
  uint32_t m_ploamCount2 = m_ploams.size();


  hlend=(m_bwmapLen2<<21)|(m_ploamCount2<<13)|m_hec;
  start.WriteHtonU32 (hlend);
  for(i=0;i<m_bwmapLen2;i++)
  {
    tmpBwAllocation=m_bwmap->GetBwAllocationByIndex(i);
    tmpBwAllocation->Serialize(start);
  }
  for(i=0;i<m_ploamCount2;i++)
  {
    m_ploams[i]->Serialize(start);
  }
  
  return;
}

uint32_t XgponXgtcDsHeader::Deserialize (Buffer::Iterator start)
{

  uint32_t i;
  uint32_t hlend;
  uint64_t serializedAllocation;

  Ptr<XgponXgtcPloam> ploam;

  hlend=start.ReadNtohU32();  
  
  m_hec=hlend & 0x1fff;
  m_bwmapLen=(hlend>>21);
  m_ploamCount=(hlend>>13) & 0xff;
  
  //std::cout<<"In deserialize -> Bw map len: "<<m_bwmapLen<<"Ploam count: "<<m_ploamCount<<std::endl;

  for(i=0;i<m_bwmapLen;i++)
  {
    serializedAllocation=start.ReadNtohU64();
    m_bwmap->AddOneSerializedBwAllocation(serializedAllocation);
  }
  for(i=0;i<m_ploamCount;i++)
  {
    //std::cout<<"Ploam count again: "<<m_ploamCount<<std::endl;
    ploam=Create<XgponXgtcPloam>();
    ploam->Deserialize(start);
    AddPloam(ploam);
  }

  return GetSerializedSize ();
  //return 0;
}

}; // namespace ns3
