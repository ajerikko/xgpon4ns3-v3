/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c)  2012 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
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

#include "xgpon-config-db.h"

#include "ns3/xgpon-burst-profile.h"


namespace ns3 {

XgponConfigDb::XgponConfigDb()
{
  m_ponId = DEFAULT_PON_ID;

  m_oltDbaEngineTypeIdStr = DEFAULT_XGPON_OLT_DBA_ENGINE_TYPEID_STR;
  m_oltDsSchedulerTypeIdStr = DEFAULT_XGPON_OLT_DS_SCHEDULER_TYPEID_STR;

  m_allocateIds4Speed = true;
  
  m_onuUsSchedulerTypeIdStr = DEFAULT_XGPON_ONU_US_SCHEDULER_TYPEID_STR;

  m_profilePreambleLen = XgponBurstProfile::PSBU_PREAMBLE_DEFAULT_LEN;
  m_profileDelimiterLen = XgponBurstProfile::PSBU_DELIMITER_DEFAULT_LEN;
  m_profileFec = true;

  m_channelTypeIdStr = DEFAULT_XGPON_CHANNEL_TYPEID_STR;

  m_queueTypeIdStr = DEFAULT_XGPON_QUEUE_TYPEID_STR;

  m_qosParametersTypeIdStr = DEFAULT_XGPON_QOS_PARAMETERS_TYPEID_STR;

  m_oltNetmaskLen = DEFAULT_OLT_NETMASK_LEN;
  m_onuNetmaskLen = DEFAULT_ONU_NETMASK_LEN; 

  m_addressFirstByteXgpon = DEFAULT_IP_ADDRESS_FIRST_BYTE_XGPON;
  m_addressFirstByteOnus = DEFAULT_IP_ADDRESS_FIRST_BYTE_ONUS;
}

XgponConfigDb::~XgponConfigDb()
{
}















void 
XgponConfigDb::SetPonId (uint64_t ponId)
{
  m_ponId = ponId;
}

 


void 
XgponConfigDb::SetOltDbaEngineTypeIdStr (std::string typeId)
{
  m_oltDbaEngineTypeIdStr = typeId;
}

void 
XgponConfigDb::SetOltDsSchedulerTypeIdStr (std::string typeId)
{
  m_oltDsSchedulerTypeIdStr = typeId;
}

void 
XgponConfigDb::SetAllocateIds4Speed (bool speed)
{
  m_allocateIds4Speed = speed;
}

void 
XgponConfigDb::SetOnuUsSchedulerTypeIdStr (std::string typeId)
{
  m_onuUsSchedulerTypeIdStr = typeId;
}

void 
XgponConfigDb::SetProfilePreambleLen (uint16_t len)
{
  m_profilePreambleLen = len;
}
void 
XgponConfigDb::SetProfileDelimiterLen (uint16_t len)
{
  m_profileDelimiterLen = len;
}
void 
XgponConfigDb::SetProfileFec (bool fec)
{
  m_profileFec = fec;
}

void 
XgponConfigDb::SetChannelTypeIdStr (std::string typeId)
{
  m_channelTypeIdStr = typeId;
}


void 
XgponConfigDb::SetOltNetmaskLen (uint8_t len)
{
  NS_ASSERT_MSG(((m_oltNetmaskLen >= 8) && (m_oltNetmaskLen<=16)), "OLT Netmask length should be in the range of [8, 16].");
  m_oltNetmaskLen = len;
}
void 
XgponConfigDb::SetOnuNetmaskLen (uint8_t len)
{
  NS_ASSERT_MSG(((m_onuNetmaskLen > 16) && (m_oltNetmaskLen<=28)), "ONU Netmask length should in the range of (16, 28].");
  m_onuNetmaskLen = len;
}


void 
XgponConfigDb::SetIpAddressFirstByteForXgpon (uint8_t firstByte)
{
  m_addressFirstByteXgpon = firstByte;
}

void 
XgponConfigDb::SetIpAddressFirstByteForOnus (uint8_t firstByte)
{
  m_addressFirstByteOnus = firstByte;
}

}//namespace ns3

