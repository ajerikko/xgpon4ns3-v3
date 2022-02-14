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

#ifndef XGPON_QOS_PARAMETERS_H
#define XGPON_QOS_PARAMETERS_H

#include "ns3/object.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The QoS parameters of a connection or tcont (aggregated from its connections), such as the minimum bandwidth, maximal service interval, etc.
 * TODO: change based on XG-PON standard, that is a little different with T-CONTs in G-PON.
 *
 */
class XgponQosParameters : public Object
{
public:

  /**
   * \brief Constructor
   */
  XgponQosParameters ();
  virtual ~XgponQosParameters ();


  enum XgponTcontType
  {
    XGPON_TCONT_TYPE_1 = 1,     /* TCONT 1 --- fixed bandwidth allocation: very important for upstream traffic, but unsuitable for downstream traffic. */
    XGPON_TCONT_TYPE_2 = 2,     /* TCONT 2 --- assured bandwidth */
    XGPON_TCONT_TYPE_3 = 3,     /* TCONT 3 --- assured + nonassured bandwidth */
    XGPON_TCONT_TYPE_4 = 4,     /* TCONT 4 --- best effort service */
    XGPON_TCONT_TYPE_5 = 5,     /* TCONT 5 --- general container with all kinds of bandwidth allocation */
  };



  ///////////////////////////////////////////////////////////////////////Deep Copy and Aggregation Operations
  /**
   * \brief aggregate the qosParameters (the parameter) into itself.
   *        Note that it is called by XgponTcontOlt to produce Qos Parameters for the tcont based on its connections' qos parameters
   */
  void AggregateQosParameters(const Ptr<XgponQosParameters>& qosParameters);

  /**
   * \brief deep copy the qosParameters (the parameter) into itself.
   */
  void DeepCopy(const Ptr<XgponQosParameters>& qosParameters);




  /////////////////////////////////////////////Member variable accessors
  void SetTcontType (XgponQosParameters::XgponTcontType type);
  XgponQosParameters::XgponTcontType GetTcontType () const;

  void SetFixedBw (uint32_t bw);
  uint32_t GetFixedBw () const;

  void SetAssuredBw (uint32_t bw);
  uint32_t GetAssuredBw () const;

  void SetNonAssuredBw (uint32_t bw);
  uint32_t GetNonAssuredBw () const;

  //jerome, C1
  void SetBestEffortBw (uint32_t bw);
  uint32_t GetBestEffortBw () const;

  void SetTotalBwPerOnu (uint32_t bw);
  uint32_t GetTotalBwPerOnu ();

  void SetMaxInterval (uint32_t interval);
  uint32_t GetMaxInterval () const;

  void SetMinInterval (uint32_t interval);
  uint32_t GetMinInterval () const;

  //////////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


private:
  XgponTcontType m_tcontType;    //the T-CONT type of this connection
  uint32_t  m_fixedBw;         //the fixed bandwidth. Unit: Bps (Byte per second), 32 bit integer is large enough for 10Gbps
  uint32_t  m_assuredBw;       //the assured bandwidth.
  uint32_t  m_nonAssuredBw;    //the non-assured bandwidth.
  uint32_t  m_bestEffortBw;    //the best effort bandwidth, jerome, C1
  uint32_t  m_totBwPerOnu;     //the assigned total bandwidth per ONU, jerome, C1
  uint32_t  m_maxInterval;     //the maximal interval between consecutive service. Unit: multiples of 125us
  uint32_t  m_minInterval;     //the minimum interval between consecutive service. Unit: multiples of 125us
  /* more variables may be needed */

  //jerome, C1
  uint32_t CalculateTotalBwPerOnu();

};








//////////////////////////////////////////////////INLINE Functions
inline void 
XgponQosParameters::SetTcontType (XgponQosParameters::XgponTcontType type)
{
  m_tcontType = type;
}

inline XgponQosParameters::XgponTcontType 
XgponQosParameters::GetTcontType () const
{
  return m_tcontType;
}

inline void 
XgponQosParameters::SetFixedBw (uint32_t bw)
{
  m_fixedBw = bw;
}
inline uint32_t 
XgponQosParameters::GetFixedBw () const
{
  return m_fixedBw;
}

inline void 
XgponQosParameters::SetAssuredBw (uint32_t bw)
{
  m_assuredBw = bw;
}
inline uint32_t 
XgponQosParameters::GetAssuredBw () const
{
  return m_assuredBw;
}

inline void 
XgponQosParameters::SetNonAssuredBw (uint32_t bw)
{
  m_nonAssuredBw = bw;
}
inline uint32_t 
XgponQosParameters::GetNonAssuredBw () const
{
  return m_nonAssuredBw;
}

//jerome, C1
inline void 
XgponQosParameters::SetBestEffortBw (uint32_t bw)
{
  m_bestEffortBw = bw;
}
inline uint32_t 
XgponQosParameters::GetBestEffortBw () const
{
  return m_bestEffortBw;
}
inline void 
XgponQosParameters::SetTotalBwPerOnu (uint32_t bw)
{
  m_totBwPerOnu = bw;
}
inline uint32_t 
XgponQosParameters::GetTotalBwPerOnu ()
{
  return CalculateTotalBwPerOnu();
}

inline void 
XgponQosParameters::SetMaxInterval (uint32_t interval)
{
  m_maxInterval = interval;
}
inline uint32_t 
XgponQosParameters::GetMaxInterval () const
{
  return m_maxInterval;
}

/*
 * Added minInterval To Help with GIANT scheduler
 */

inline void
XgponQosParameters::SetMinInterval (uint32_t interval)
{
  m_minInterval = interval;
}
inline uint32_t
XgponQosParameters::GetMinInterval () const
{
  return m_minInterval;
}


}; // namespace ns3

#endif // XGPON_QOS_PARAMETERS_H
