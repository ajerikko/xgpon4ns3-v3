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

#include "xgpon-xgem-header.h"




NS_LOG_COMPONENT_DEFINE ("XgponXgemHeader");

namespace ns3 {


XgponXgemHeader::XgponXgemHeader ()
  : m_pli (0x0000),
    m_keyIndex (0x00),
    m_xgemPortId (0x0000),
    m_options (0x00000),
    m_lastFragment (0x01),
    m_hec (0x0000)
{
}
XgponXgemHeader::XgponXgemHeader (uint16_t pli, uint8_t keyIndex, uint16_t xgemPortId, uint32_t options, uint8_t lastFragment)
{
  m_pli = pli & 0x3fff;                  //14bits
  m_keyIndex = keyIndex & 0x03;          //2bits
  m_xgemPortId = xgemPortId;             //16bits
  m_options = options & 0x3ffff;         //18bits
  m_lastFragment = lastFragment & 0x01;  //1bit

  CalculateHec ();
}
XgponXgemHeader::~XgponXgemHeader ()
{
}








void
XgponXgemHeader::Print (std::ostream &os)  const
{
  os << " PLI=" << (uint16_t)m_pli;
  os << " KI=" << (uint16_t)m_keyIndex;
  os << " PORT=" << (uint16_t)m_xgemPortId;

  os.setf(std::ios::hex, std::ios::basefield);
  os << " OPTS=" << (uint32_t)m_options;
  os.unsetf(std::ios::hex);

  os << " LF=" << (uint16_t)m_lastFragment;

  os.setf(std::ios::hex, std::ios::basefield);
  os << " HEC=" << (uint16_t)m_hec;
  os.unsetf(std::ios::hex);
  
  os<<std::endl;
}

uint32_t XgponXgemHeader::GetSerializedSize (void) const
{
  return XGPON_XGEM_HEADER_LENGTH;  //8 bytes;
}

void XgponXgemHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (((m_pli >> 6) & 0x00FF));
  i.WriteU8 (((m_pli << 2) & 0x00FC) | (m_keyIndex & 0x03));
  i.WriteU8 (((m_xgemPortId >> 8) & 0x00FF));
  i.WriteU8 ((m_xgemPortId & 0x00FF));

  i.WriteU8 (((m_options >> 10) & 0x000000FF));
  i.WriteU8 (((m_options >> 2) & 0x000000FF));
  i.WriteU8 (((m_options << 6) & 0x000000C0) |
             ((m_lastFragment << 5) & 0x20) |
             ((m_hec >> 8) & 0x001F));
  i.WriteU8 ((m_hec & 0x000000FF));
}

uint32_t XgponXgemHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  uint8_t byte_1;
  uint8_t byte_2;
  uint8_t byte_3;
  uint8_t byte_4;


  byte_1 = i.ReadU8 ();
  byte_2 = i.ReadU8 ();
  byte_3 = i.ReadU8 ();
  byte_4 = i.ReadU8 ();

  m_pli = (((byte_1 & 0x00FF) << 6) | ((byte_2 >> 2) & 0x3F));
  m_keyIndex = (byte_2 & 0x03);
  m_xgemPortId = (((byte_3 & 0x00FF) << 8) | (byte_4 & 0x00FF));



  byte_1 = i.ReadU8 ();
  byte_2 = i.ReadU8 ();
  byte_3 = i.ReadU8 ();
  byte_4 = i.ReadU8 ();

  m_options = ((byte_1 << 10) & 0x0003FC00) | ((byte_2 << 2) & 0x000003FC) | ((byte_3 >> 6) & 0x00000003);
  m_lastFragment = ((byte_3 >> 5) & 0x01);
  m_hec = ((byte_3 << 8) & 0x001F00) | (byte_4 & 0x00FF);

  return GetSerializedSize ();
}

}; // namespace ns3
