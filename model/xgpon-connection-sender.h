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

#ifndef XGPON_CONNECTION_SENDER_H
#define XGPON_CONNECTION_SENDER_H

#include <deque>

#include "ns3/packet.h"

#include "xgpon-connection.h"
#include "xgpon-queue.h"
#include "xgpon-service-record.h"


namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The connection maintained at sender-side. It has a queue to hold the packets from upper layers.
 *        It also maintains a list of service records (at most 8) used for scheduling.
 *        Note that most of its functions are forwarded to XgponQueue.
 *
 */
class XgponConnectionSender : public XgponConnection
{
  const static uint16_t MAXIMAL_NUM_SERVICE_RECORDS = 8;   //maximal service records maintained for each connection.
                                                           //in the future, we may delete old records based on time.
public:

  /**
   * \brief Constructor
   */
  XgponConnectionSender ();
  virtual ~XgponConnectionSender ();



  ////////////////////////////////////////////////////////////////////////Main interfaces: most of the functions are carried out by Xgpon-Queue
  
  /**
   * \brief receive one packet from upper layer and put it into its queue.
   * \param pkt the packet from upper layer
   * \return true: put into the queue; false: dropped due to buffer overflow.
   */
  bool ReceiveUpperLayerSdu (const Ptr<Packet>& pkt);



  /**
   * \brief  return one packet to be encapsulated into one XGEM frame and send out.
   * \return the packet to be encapsulated and transmited.
   * Note that we cannot return one reference since the queue may be empty.
   */
  const Ptr<Packet> GetOnePacketForTransmit ();



  /**
   * \brief  put the remaining part of a segmented packet into the queue.
   * \param  pkt the remaining segment of a packet.
   */
  void PutRemainingSegmentIntoQueue (const Ptr<Packet>& pkt);


  /**
   * \brief  to check whether segmentation is carrying out for this connection.
   * \return true: segment;  false: not.
   */  
  bool IsSegmentationRunning ( );






  /**
   * \brief  get buffer occupancy of this connection.
   * \return the exact amount of data in the queue. unit: byte
   */
  uint32_t  GetQueueStatus ();  

  /**
   * \brief get the amount of data in queue used for DBA and Schedulers;
   * \return the amount of data (padded + xgem header). unit: word
   */
  uint32_t GetBufOccupancy4Scheduling ( );

  /**
   * \brief get the size of the remaining part of the segmented packet (used for DBA and Schedulers). 
   * \return the size of the segment (padded + xgem header). Unit: word
   */
  uint32_t GetFragBufOccupancy4Scheduling ( ); //just the remaining part when a packet segmenting.








  ///////////////////////////////////////////////////////////////Member variable accessors
  void SetXgponQueue (const Ptr<XgponQueue>& queue);
  const Ptr<XgponQueue>& GetXgponQueue ( ) const;


  /**
   * \brief add one service record to the scheduling history
   */  
  void AddServiceRecord (const Ptr<XgponServiceRecord>& sRecord);

  /**
   * \brief get all service records until now (note that very old records had been deleted).
   */ 
  const std::deque< Ptr<XgponServiceRecord> >& GetAllServiceRecords () const;







  /////////////////////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;




private:
  Ptr<XgponQueue> m_txQueue;                //the queue for holding the packets to be transmitted.
                                            //we should use the helper to configure the queue (type, queue size, etc.) for each connection 

  //a list of service records (at most 8) for this connection. used for scheduling purpose
  std::deque< Ptr<XgponServiceRecord> > m_serviceRecords;   
};






/////////////////////////////////////////////////////////////////INLINE Functions
inline bool 
XgponConnectionSender::ReceiveUpperLayerSdu (const Ptr<Packet>& pkt)
{
  return m_txQueue->Enqueue (pkt);
}

inline const Ptr<Packet> 
XgponConnectionSender::GetOnePacketForTransmit () 
{
  return m_txQueue->Dequeue();
}

inline void 
XgponConnectionSender::PutRemainingSegmentIntoQueue (const Ptr<Packet>& pkt)
{
  m_txQueue->PushFrontRemainingSegment (pkt);
}
 
inline bool 
XgponConnectionSender::IsSegmentationRunning ( )
{
  return m_txQueue->IsSegmentationRunning ( );
}




inline uint32_t  
XgponConnectionSender::GetQueueStatus ()
{
  return m_txQueue->GetNBytes ();  
}

inline uint32_t 
XgponConnectionSender::GetBufOccupancy4Scheduling ( )
{
  return m_txQueue->GetBufOccupancy4Scheduling ( );
}

inline uint32_t 
XgponConnectionSender::GetFragBufOccupancy4Scheduling ( )
{
  return m_txQueue->GetFragBufOccupancy4Scheduling ( );
}





inline void 
XgponConnectionSender::SetXgponQueue (const Ptr<XgponQueue>& queue)
{
  m_txQueue = queue;
}
inline const Ptr<XgponQueue>& 
XgponConnectionSender::GetXgponQueue ( ) const
{
  NS_ASSERT_MSG((m_txQueue!=0), "Tx-queue has not been set yet.");
  return m_txQueue;
}





inline void 
XgponConnectionSender::AddServiceRecord (const Ptr<XgponServiceRecord>& sRecord)
{  
  m_serviceRecords.push_back (sRecord);
  if(m_serviceRecords.size() > MAXIMAL_NUM_SERVICE_RECORDS) m_serviceRecords.pop_front();
}
inline const std::deque< Ptr<XgponServiceRecord> >&
XgponConnectionSender::GetAllServiceRecords () const
{
  return m_serviceRecords;
}




}; // namespace ns3

#endif // XGPON_CONNECTION_SENDER_H
