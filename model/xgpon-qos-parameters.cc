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

#include "ns3/enum.h"
#include "ns3/uinteger.h"

#include "xgpon-qos-parameters.h"



NS_LOG_COMPONENT_DEFINE ("XgponQosParameters");

namespace ns3 {
NS_OBJECT_ENSURE_REGISTERED (XgponQosParameters);


TypeId
XgponQosParameters::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponQosParameters")
    .SetParent<Object> ()
    .AddConstructor<XgponQosParameters> ()
    .AddAttribute ("FixedBandwidth", 
                   "The fixed bandwidth that is pre-allocated to this connection.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&XgponQosParameters::m_fixedBw),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("AssuredBandwidth", 
                   "The assured bandwidth that is dynamically allocated to this connection.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&XgponQosParameters::m_assuredBw),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("NonAssuredBandwidth", 
                   "The non-assured bandwidth that is dynamically allocated to this connection.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&XgponQosParameters::m_nonAssuredBw),
                   MakeUintegerChecker<uint32_t> ())
     //jerome, C1
    .AddAttribute ("BestEffortBandwidth", 
                   "The non-assured bandwidth that is dynamically allocated to this connection.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&XgponQosParameters::m_bestEffortBw),
                   MakeUintegerChecker<uint32_t> ())

    .AddAttribute ("MaxServiceInterval", 
                   "The maximal interval between the consecutive intervals os a connection. Unit: multiples of 125us",
                   UintegerValue (100),    //jerome, C1, Unit: multiples of 125us
                   MakeUintegerAccessor (&XgponQosParameters::m_maxInterval),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("MinServiceInterval",
                   "The minimum interval between the consecutive intervals of a connection. Unit: multiples of 125us",
                   UintegerValue (100),    //jerome, C1, Unit: multiples of 125us
                   MakeUintegerAccessor (&XgponQosParameters::m_minInterval),
                   MakeUintegerChecker<uint32_t> ())
  ;

  return tid;
}
TypeId
XgponQosParameters::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponQosParameters::XgponQosParameters ():
        m_fixedBw(0),
        m_assuredBw(0),
        m_nonAssuredBw(0),
        m_bestEffortBw(0),
        m_totBwPerOnu(0)
{
}
XgponQosParameters::~XgponQosParameters ()
{
}





/* jerome, C1
 * Aggregiation doesnt have to be done a the qos parameters level.
 * rather, it has to be done at the OLT, to get the aggregated parametes, to calculate the DBA parametes.
 * All we need here is a simple addition of all the assigned bw per device (ONU) to get the m_totBwPerOnu
 */

/*void 
XgponQosParameters::AggregateQosParameters(const Ptr<XgponQosParameters>& qosParameters)
{
  NS_LOG_FUNCTION (this);

  if(m_tcontType != qosParameters->GetTcontType()) 
  {
    NS_LOG_ERROR ("ERROR!!! Connections of the same TCONT should have the same T-CONT type.");
  }

  m_fixedBw += qosParameters->GetFixedBw ();
  m_assuredBw += qosParameters->GetAssuredBw ();
  m_nonAssuredBw += qosParameters->GetNonAssuredBw ();
  m_maxBw += qosParameters->GetMaxBw ();

  if(m_maxInterval <= 0) m_maxInterval = qosParameters->GetMaxInterval ();
  else
  {
    if(m_maxInterval > qosParameters->GetMaxInterval ()) m_maxInterval = qosParameters->GetMaxInterval ();
  }
  //Added minInterval To Help with GIANT scheduler
  if(m_minInterval <= 0) m_minInterval = qosParameters->GetMinInterval ();
  else
  {
    if(m_minInterval < qosParameters->GetMinInterval ()) m_minInterval = qosParameters->GetMinInterval ();
  }

}
*/

//jerome, C1
uint32_t
XgponQosParameters::CalculateTotalBwPerOnu()
{
  NS_LOG_DEBUG("Total bandwidth per ONU = " << (m_fixedBw + m_assuredBw + m_nonAssuredBw + m_bestEffortBw));
  return m_fixedBw + m_assuredBw + m_nonAssuredBw + m_bestEffortBw;
}


void 
XgponQosParameters::DeepCopy(const Ptr<XgponQosParameters>& qosParameters)
{
  NS_LOG_FUNCTION (this);

  m_tcontType = qosParameters->GetTcontType();
  m_fixedBw = qosParameters->GetFixedBw ();
  m_assuredBw = qosParameters->GetAssuredBw ();
  m_nonAssuredBw = qosParameters->GetNonAssuredBw ();
  m_bestEffortBw = qosParameters->GetBestEffortBw ();//jerome, C1
  m_totBwPerOnu = qosParameters->GetTotalBwPerOnu ();//jerome, C1
  m_maxInterval = qosParameters->GetMaxInterval ();
  m_minInterval = qosParameters->GetMinInterval ();
}



}; // namespace ns3
