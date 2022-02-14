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

#include "xgpon-xgtc-ds-frame.h"




NS_LOG_COMPONENT_DEFINE ("XgponXgtcDsFrame");

namespace ns3 {

XgponXgtcDsFrame::XgponXgtcDsFrame ()
  :m_burst (0), m_broadcastBurst(0), meta_burstSize (0), m_bitmap(1024, 0) 
{
  m_burst.reserve(XGPON1_MAX_XGEM_FRAMES_PER_DS_FRAME);
  m_broadcastBurst.reserve(XGPON1_MAX_BROADCAST_XGEM_FRAMES_PER_DS_FRAME);
}
XgponXgtcDsFrame::~XgponXgtcDsFrame ()
{
}







void
XgponXgtcDsFrame::Print (std::ostream &os)  const
{
  int frameNum = GetNBroadcastXgemFrames () + GetNUnicastXgemFrames ();

  os << " DSFRAME-HEADER= ";
  m_header.Print(os);

  os << " XGEM-FRAME-NUM= " << frameNum;

  std::vector<Ptr<XgponXgemFrame> >::const_iterator iterator4Print, end;
  iterator4Print = m_broadcastBurst.begin();
  end = m_broadcastBurst.end();
  int i = 1;
  while (iterator4Print != end)
  {
    os << " PACKET "<<i<<": ";
    (*iterator4Print)->Print(os);
    iterator4Print++;
    i++;
  }


  iterator4Print = m_burst.begin();
  end = m_burst.end();
  while (iterator4Print != end)
  {
    os << " PACKET "<<i<<": ";
    (*iterator4Print)->Print(os);
    iterator4Print++;
    i++;
  }

  os << std::endl;

  return;
}

uint32_t XgponXgtcDsFrame::GetSerializedSize (void) const
{
  uint32_t len;

  len = m_header.GetSerializedSize();

  std::vector<Ptr<XgponXgemFrame> >::const_iterator it, end;
  it = m_broadcastBurst.begin();
  end = m_broadcastBurst.end();
  while(it!=end) { len += (*it)->GetSerializedSize (); }


  it = m_burst.begin();
  end = m_burst.end();
  while(it!=end) { len += (*it)->GetSerializedSize (); }


  return len;
}

void XgponXgtcDsFrame::Serialize (Buffer::Iterator start) const
{
  std::vector<Ptr<Packet> >::const_iterator iteratorSerialize, end;

  m_header.Serialize(start);

  //TODO: packet burst serialization;
  //iteratorSerialize = m_burst.Begin();
  //end = m_burst.End();
  //while (iteratorSerialize != end)
  //{
  //  (*iteratorSerialize)->Serialize(start);
  //  iteratorSerialize++;
  //}
  return;
}


uint32_t XgponXgtcDsFrame::Deserialize (Buffer::Iterator start)
{
  //uint32_t size;

  //size = m_header.Deserialize(start);

  //TODO: packet burst deserialization;  
  //while (size < meta_burstSize)
  //{
  //  Ptr<Packet> pkt = Create<Packet>();
  //  size = size + pkt->Deserialize(start);
  //  AddPacket(pkt);
  //}
  return GetSerializedSize ();
}

}; // namespace ns3
