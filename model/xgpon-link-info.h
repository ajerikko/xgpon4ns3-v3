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

#ifndef XGPON_LINK_INFO_H
#define XGPON_LINK_INFO_H

#include <queue>

#include "ns3/object.h"

#include "xgpon-key.h"
#include "xgpon-burst-profile.h"
#include "xgpon-xgtc-ploam.h"


namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The class used to represent the link between OLT and one ONU.
 *        It contains the keys, burst profiles, equalization delay, and ploam messages to be sent.
 *        Note that keys and burst profiles should be configured at both side in one vector through OMCI / PLOAM.
 *        The index of the used key/profile is embeded in the xgem frame header.
 *
 */
class XgponLinkInfo : public Object
{
  const static uint8_t MAX_NUMBER_OF_KEYS  = 4;
  const static uint8_t MAX_NUMBER_OF_PROFILES  = 4;
public:

  /**
   * \brief Constructor
   */
  XgponLinkInfo ();
  virtual ~XgponLinkInfo ();



  ///////////////////////////////////////////////////Main Interfaces
  //key related
  void AddNewDsKey (const Ptr<XgponKey>& key, uint8_t index);
  void AddNewUsKey (const Ptr<XgponKey>& key, uint8_t index);  

  void DeleteDsKey (uint8_t index);
  void DeleteUsKey (uint8_t index);  

  const Ptr<XgponKey>& GetDsKeyByIndex (uint8_t index) const;
  const Ptr<XgponKey>& GetUsKeyByIndex (uint8_t index) const;


  const Ptr<XgponKey>& GetCurrentDsKey ( ) const;
  const Ptr<XgponKey>& GetCurrentUsKey ( ) const;

  void SetCurrentDsKeyIndex (uint8_t index);
  void SetCurrentUsKeyIndex (uint8_t index);

  uint8_t GetCurrentDsKeyIndex ( ) const;
  uint8_t GetCurrentUsKeyIndex ( ) const;



  //profile related
  void AddNewProfile (const Ptr<XgponBurstProfile>& profile, uint8_t index);
  void DeleteProfile (uint8_t index);
  const Ptr<XgponBurstProfile>& GetCurrentProfile ( ) const;
  const Ptr<XgponBurstProfile>& GetProfileByIndex (uint8_t index) const;
  void SetCurrentProfileIndex (uint8_t index);
  uint8_t GetCurrentProfileIndex ( ) const;




  //Equalization delay used by ONU to scheduling the upstream bursts
  uint64_t GetEqualizeDelay () const;
  void SetEqualizeDelay (uint64_t delay);


  //Dying Gasp status
  void SetDyingGasp(bool state);
  bool GetDyingGasp(void) const;








  //ploam message queue
  void AddPloam2TxQueue (const Ptr<XgponXgtcPloam>& msg);
  const Ptr<XgponXgtcPloam> GetPloam2Transmit(void);


  //used by ONU only when reporting ploam queue status to OLT
  bool HasPloam2Transmit4ONU(void) const;   


  //used by OLT only to decide the queue status at ONU
  void SetPloamExistAtOnu4OLT(bool state);      
  bool GetPloamExistAtOnu4OLT(void) const;      






  //Copy all information except PLOAM message queue; used by the helper to construct the same linkinfo for both OLT and ONU
  void DeepCopy(const Ptr<XgponLinkInfo>& linkInfo);



  //onu-id
  void SetOnuId (uint16_t onuId);
  uint16_t GetOnuId () const;  


  ///////////////////////////////////////////////////required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

private:
  uint16_t m_onuId;

  std::vector< Ptr<XgponKey> > m_activeDsKeys;
  uint8_t m_curDsKeyIndex;


  std::vector< Ptr<XgponKey> > m_activeUsKeys;
  uint8_t m_curUsKeyIndex;



  std::vector< Ptr<XgponBurstProfile> > m_profiles;
  uint8_t m_curProfileIndex;

  

  uint64_t m_eqDelay;    //equalization delay for avoiding collision in upstream direction. unit: nanosecond
                         //in the first case, the same value will be used by all ONUs and ranging won't be carried out.

  //ploam to be transmitted from/to this onu
  std::queue< Ptr<XgponXgtcPloam> > m_ploamMsgQueue;


  bool m_dyingGasp;         //Dying Gasp state at ONU side; maintained at both side


  bool m_ploamExistAtOnu;   //whether this ONU has PLOAM message to be sent to the OLT; maintained at OLT side based on burst headers from ONU


  /* more variables may be needed, such as activation/registration/ranging state, etc. */

};






////////////////////////////////////////////////////////INLINE Functions

inline void 
XgponLinkInfo::SetOnuId (uint16_t onuId)
{
  m_onuId = onuId;
}
inline uint16_t 
XgponLinkInfo::GetOnuId () const
{
  return m_onuId;
}



inline void 
XgponLinkInfo::AddNewDsKey (const Ptr<XgponKey>& key, uint8_t index)
{
  NS_ASSERT_MSG((index<MAX_NUMBER_OF_KEYS), "Key index is too large.");
  m_activeDsKeys[index] = key;
}
inline void 
XgponLinkInfo::AddNewUsKey (const Ptr<XgponKey>& key, uint8_t index)
{
  NS_ASSERT_MSG((index<MAX_NUMBER_OF_KEYS), "Key index is too large.");
  m_activeUsKeys[index] = key;
}

inline void 
XgponLinkInfo::DeleteDsKey (uint8_t index)
{
  NS_ASSERT_MSG((index<MAX_NUMBER_OF_KEYS), "Key index is too large.");
  m_activeDsKeys[index] = 0;
}
inline void 
XgponLinkInfo::DeleteUsKey (uint8_t index)
{
  NS_ASSERT_MSG((index<MAX_NUMBER_OF_KEYS), "Key index is too large.");
  m_activeUsKeys[index] = 0;
}

inline const Ptr<XgponKey>& 
XgponLinkInfo::GetCurrentDsKey ( ) const
{
  NS_ASSERT_MSG((m_activeDsKeys[m_curDsKeyIndex]!=0), "Try to use one null XgponKey.");
  return m_activeDsKeys[m_curDsKeyIndex];
}
inline const Ptr<XgponKey>& 
XgponLinkInfo::GetCurrentUsKey ( ) const
{
  NS_ASSERT_MSG((m_activeUsKeys[m_curUsKeyIndex]!=0), "Try to use one null XgponKey.");
  return m_activeUsKeys[m_curUsKeyIndex];
}


inline const Ptr<XgponKey>& 
XgponLinkInfo::GetDsKeyByIndex (uint8_t index) const
{
  NS_ASSERT_MSG(((index<MAX_NUMBER_OF_KEYS) && (m_activeDsKeys[index]!=0)), "Try to use one null XgponKey.");
  return m_activeDsKeys[index];
}
inline const Ptr<XgponKey>& 
XgponLinkInfo::GetUsKeyByIndex (uint8_t index) const
{
  NS_ASSERT_MSG(((index<MAX_NUMBER_OF_KEYS) && (m_activeUsKeys[index]!=0)), "Try to use one null XgponKey.");
  return m_activeUsKeys[index];
}

inline void 
XgponLinkInfo::SetCurrentDsKeyIndex (uint8_t index)
{
  NS_ASSERT_MSG(((index<MAX_NUMBER_OF_KEYS) && (m_activeDsKeys[index]!=0)), "Key index is too large.");
  m_curDsKeyIndex = index;
}
inline void 
XgponLinkInfo::SetCurrentUsKeyIndex (uint8_t index)
{
  NS_ASSERT_MSG(((index<MAX_NUMBER_OF_KEYS) && (m_activeUsKeys[index]!=0)), "Key index is too large.");
  m_curUsKeyIndex = index;
}

inline uint8_t 
XgponLinkInfo::GetCurrentDsKeyIndex () const
{
  return m_curDsKeyIndex;
}
inline uint8_t
XgponLinkInfo::GetCurrentUsKeyIndex () const
{
  return m_curUsKeyIndex;
}





inline void 
XgponLinkInfo::AddNewProfile (const Ptr<XgponBurstProfile>& profile, uint8_t index)
{
  NS_ASSERT_MSG((index<MAX_NUMBER_OF_PROFILES), "Profile index is too large.");
  m_profiles[index] = profile;
}
inline void 
XgponLinkInfo::DeleteProfile (uint8_t index)
{
  NS_ASSERT_MSG((index<MAX_NUMBER_OF_PROFILES), "Profile index is too large.");
  m_profiles[index] = 0;
}
inline const Ptr<XgponBurstProfile>& 
XgponLinkInfo::GetCurrentProfile ( ) const
{
  NS_ASSERT_MSG((m_profiles[m_curProfileIndex]!=0), "Try to use one null XgponKey.");
  return m_profiles[m_curProfileIndex];
}
inline const Ptr<XgponBurstProfile>& 
XgponLinkInfo::GetProfileByIndex (uint8_t index) const
{
  NS_ASSERT_MSG(((index<MAX_NUMBER_OF_PROFILES) && (m_profiles[index]!=0)), "Try to use one null XgponKey.");
  return m_profiles[index];
}
inline void 
XgponLinkInfo::SetCurrentProfileIndex (uint8_t index)
{
  NS_ASSERT_MSG(((index<MAX_NUMBER_OF_PROFILES) && (m_profiles[index]!=0)), "Profile index is too large.");
  m_curProfileIndex = index;
}
inline uint8_t 
XgponLinkInfo::GetCurrentProfileIndex ( ) const
{
  return m_curProfileIndex;
}





inline uint64_t 
XgponLinkInfo::GetEqualizeDelay () const
{
  return m_eqDelay;
}
inline void 
XgponLinkInfo::SetEqualizeDelay (uint64_t delay)
{
  m_eqDelay = delay;
}




inline void 
XgponLinkInfo::SetPloamExistAtOnu4OLT(bool state)
{
  m_ploamExistAtOnu = state;
}
inline bool 
XgponLinkInfo::GetPloamExistAtOnu4OLT(void) const
{
  return m_ploamExistAtOnu;
}



inline void 
XgponLinkInfo::SetDyingGasp(bool state)
{
  m_dyingGasp = state;
}
inline bool 
XgponLinkInfo::GetDyingGasp(void) const
{
  return m_dyingGasp;
}



}; // namespace ns3

#endif // XGPON_LINK_INFO_H
