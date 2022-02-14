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

#ifndef XGPON_CONFIG_DB_H
#define XGPON_CONFIG_DB_H

#include <stdint.h>
#include <string>
#include <map>

#define DEFAULT_XGPON_OLT_DBA_ENGINE_TYPEID_STR          "ns3::XgponOltDbaEngineRoundRobin"
#define DEFAULT_XGPON_OLT_DS_SCHEDULER_TYPEID_STR        "ns3::XgponOltDsSchedulerRoundRobin"
#define XGPON_OLT_CONN_MANAGER_FLEXIBLE_TYPEID_STR       "ns3::XgponOltConnManagerFlexible"
#define XGPON_OLT_CONN_MANAGER_SPEED_TYPEID_STR          "ns3::XgponOltConnManagerSpeed"

#define XGPON_ONU_CONN_MANAGER_FLEXIBLE_TYPEID_STR       "ns3::XgponOnuConnManagerFlexible"
#define XGPON_ONU_CONN_MANAGER_SPEED_TYPEID_STR          "ns3::XgponOnuConnManagerSpeed"

#define DEFAULT_XGPON_ONU_US_SCHEDULER_TYPEID_STR        "ns3::XgponOnuUsSchedulerRoundRobin"

#define DEFAULT_XGPON_CHANNEL_TYPEID_STR                 "ns3::XgponChannel"

#define DEFAULT_XGPON_QUEUE_TYPEID_STR                   "ns3::XgponFifoQueue"
#define DEFAULT_XGPON_QOS_PARAMETERS_TYPEID_STR          "ns3::XgponQosParameters"

#define DEFAULT_OLT_NETMASK_LEN                          16
#define DEFAULT_ONU_NETMASK_LEN                          24
#define DEFAULT_IP_ADDRESS_FIRST_BYTE_XGPON              10   //"10.0.*.*"
#define DEFAULT_IP_ADDRESS_FIRST_BYTE_ONUS               172  //"172.onuid.*"




namespace ns3 {

/**
 * \brief a database used to hold the configuration of the xgpon network
 *
 */
class XgponConfigDb
{

friend class XgponHelper;

private:
  const static uint64_t DEFAULT_PON_ID = 1;


public:
  /**
   * \brief constructor
   */
  XgponConfigDb ();
  virtual ~XgponConfigDb ();

  void SetPonId (uint64_t ponId);
  void SetOltDbaEngineTypeIdStr (std::string typeId);
  void SetOltDsSchedulerTypeIdStr (std::string typeId);

  void SetAllocateIds4Speed (bool speed);

  void SetOnuUsSchedulerTypeIdStr (std::string typeId);

  void SetProfilePreambleLen (uint16_t len);
  void SetProfileDelimiterLen (uint16_t len);
  void SetProfileFec (bool fec);


  void SetChannelTypeIdStr (std::string typeId);

  void SetQueueTypeIdStr (std::string typeId);

  void SetQosParametersTypeIdStr (std::string typeId);


  void SetOltNetmaskLen (uint8_t len);
  void SetOnuNetmaskLen (uint8_t len);

  void SetIpAddressFirstByteForOnus (uint8_t firstByte);

  void SetIpAddressFirstByteForXgpon (uint8_t firstByte);
  



private:

  uint64_t m_ponId;                                   //Pon-ID

  std::string m_oltDbaEngineTypeIdStr;                //Type Id string of the dba engine used by the OLT
  std::string m_oltDsSchedulerTypeIdStr;              //Type Id string of the ds scheduler used by the OLT


  bool m_allocateIds4Speed;


  std::string m_onuUsSchedulerTypeIdStr;              //Type Id string of the per Alloc-ID us scheduler used by the ONU

  uint16_t m_profilePreambleLen;                      //burst profile parameters
  uint16_t m_profileDelimiterLen;
  bool m_profileFec;

  std::string m_channelTypeIdStr;                     //Type Id string of the xgpon channel

  std::string m_queueTypeIdStr;                       //Type Id string of the tx-queue used by a conection at sender-side

  std::string m_qosParametersTypeIdStr;               //Type Id string of the qos parameters used by a conection


  uint8_t m_oltNetmaskLen;                            //Netmask length for the whole Xgpon network (OLT, ONUs, and computers connected to ONUs.)
  uint8_t m_onuNetmaskLen;                            //Netmask length for the network that are composed by one ONU and computers connected to this ONU.

  uint16_t m_addressFirstByteXgpon;                    //First byte of the IP address for Xgpon network (Olt + Onus).
  uint16_t m_addressFirstByteOnus;                     //First byte of the IP address for networks connected to the Internet through ONUs. (FirstByte.onu_id.computer)

};




}//namespace ns3

#endif /* XGPON_CONFIG_DB_H */
