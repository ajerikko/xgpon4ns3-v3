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

#include "xgpon-xgtc-us-burst.h"





NS_LOG_COMPONENT_DEFINE ("XgponXgtcUsBurst");

namespace ns3 {

XgponXgtcUsBurst::XgponXgtcUsBurst ()
  : m_header (), m_allocations (0), m_trailer (0)
{
  //m_allocations.reserve(100);
}
XgponXgtcUsBurst::~XgponXgtcUsBurst ()
{
}








void
XgponXgtcUsBurst::Print (std::ostream &os)  const
{
  uint32_t i, allocNum;

  std::deque< Ptr<XgponXgtcUsAllocation> >::const_iterator iterator4Print, end;

  allocNum = m_allocations.size();

  os << " USBURST-HEADER= ";
  m_header.Print(os);

  os << " ALLOC-NUM= " << allocNum;

  iterator4Print = m_allocations.begin();
  end = m_allocations.end();
  i = 1;
  while (iterator4Print != end)
  {
    os << " US-ALLOCATION "<<i<<": ";
    (*iterator4Print)->Print(os);
    iterator4Print++;
    i++;
  }

  os << std::endl;

  return;
}

uint32_t XgponXgtcUsBurst::GetSerializedSize (void) const
{
  uint32_t i, len, allocNum;

  len = 4 + m_header.GetSerializedSize();  //trailer + header
  
  allocNum = m_allocations.size();
  for(i=0; i<allocNum; i++)
  {
    len = len +  m_allocations[i]->GetSerializedSize(); 
  }

  return len;
}

void XgponXgtcUsBurst::Serialize (Buffer::Iterator start) const
{
  //TODO: serialization;
  return;
}

uint32_t XgponXgtcUsBurst::Deserialize (Buffer::Iterator start)
{
  //TODO: deserialization;
  return GetSerializedSize ();
}

}; // namespace ns3
