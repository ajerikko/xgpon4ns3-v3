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

#include "xgpon-psbu.h"



NS_LOG_COMPONENT_DEFINE ("XgponPsbu");

namespace ns3 {


XgponPsbu::XgponPsbu ()
{
  //for(int i=0; i<XGPON_PSBU_MAX_LEN; i++) { m_preamble[i] = 0; m_delimiter[i] = 0; } 
}
XgponPsbu::~XgponPsbu ()
{
}




void 
XgponPsbu::SetPreamble (const uint8_t *preamble, uint8_t len)
{
  int i;

  meta_preambleLen = len;
  for(i=0; i<len; i++) { m_preamble[i] = *(preamble+i); }  
}
uint8_t
XgponPsbu::GetPreamble (uint8_t *dst) const
{
  int i;

  for(i=0; i<meta_preambleLen; i++) { *(dst+i) = m_preamble[i]; }  

  return meta_preambleLen;
}


void 
XgponPsbu::SetDelimiter (const uint8_t *delimiter, uint8_t len)
{
  int i;
  meta_delimiterLen = len;
  for(i=0; i<len; i++) { m_delimiter[i] = *(delimiter+i); } 
}
uint8_t
XgponPsbu::GetDelimiter (uint8_t *dst) const
{
  int i;

  for(i=0; i<meta_delimiterLen; i++) { *(dst+i) = m_delimiter[i]; }  
  
  return meta_delimiterLen;
}








XgponPsbu& 
XgponPsbu::operator=(const XgponPsbu &rhs)
{
  SetPreamble(rhs.m_preamble, rhs.meta_preambleLen);
  SetDelimiter(rhs.m_delimiter, rhs.meta_delimiterLen);
  
  return *this;
}





void
XgponPsbu::Print (std::ostream &os)  const
{
  int i;

  os << " PREAMBLE (";
  os << meta_preambleLen;
  os << " byte): ";
  os.setf(std::ios::hex, std::ios::basefield);
  for(i=0; i<meta_preambleLen; i++) { os << std::hex << (uint16_t) m_preamble[i]; }  
  os.unsetf(std::ios::hex);

  os << "\n DELIMITER (";
  os << meta_delimiterLen;
  os << " byte): ";
  os.setf(std::ios::hex, std::ios::basefield);
  for(i=0; i<meta_delimiterLen; i++) { os << (uint16_t)m_delimiter[i]; } 
  os.unsetf(std::ios::hex);

  os << std::endl;
}

uint32_t 
XgponPsbu::GetSerializedSize (void) const
{
  return (meta_preambleLen + meta_delimiterLen);
}

void 
XgponPsbu::Serialize (Buffer::Iterator start) const
{
  //leave alone in first phase
  //uint8_t psbu[PSBU_PREAMBLE_LEN+PSBU_DELIMITER_LEN];
  int i;

  for(i=0; i<meta_preambleLen; i++) 
  {
    start.WriteU8(m_preamble[i]); 
  }

  for(i=0; i<meta_delimiterLen; i++) 
  {
    start.WriteU8(m_delimiter[i]); 
  } 
  
  return;
}

uint32_t 
XgponPsbu::Deserialize (Buffer::Iterator start)
{
  //leave alone in first phase
  int i;

  for(i=0; i<meta_preambleLen; i++) 
  {
    m_preamble[i]=start.ReadU8(); 
  }

  for(i=0; i<meta_delimiterLen; i++) 
  {
    m_delimiter[i]=start.ReadU8(); 
  } 

  return GetSerializedSize ();
}

}; // namespace ns3
