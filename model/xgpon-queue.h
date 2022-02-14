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

#ifndef XGPON_QUEUE_H
#define XGPON_QUEUE_H

#include "ns3/object.h"
#include "ns3/traced-callback.h"
#include "ns3/packet.h"

#include "xgpon-xgem-frame.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The queue used for each XG-PON connection at the sender side. 
 *        Except holding the packets to be transmitted, segmentation is also considered in this class.
 *        Hence, we cannot use DropTailQueue directly. Since some variables in ns3::Queue are private, we also cannot inherit from Queue. 
 */

class XgponQueue : public Object
{

public:

  //Enumeration of the modes supported in the class.
  enum XgponQueueMode
  {
    XGPON_QUEUE_MODE_PACKETS,     /**< Use number of packets for maximum queue size */
    XGPON_QUEUE_MODE_BYTES,       /**< Use number of bytes for maximum queue size */
  };

  /**
   * \brief the default constructor
   */
  XgponQueue ();
  virtual ~XgponQueue ();




  ////////////////////////////////////////////Main Interfaces

  /**
   * \brief place a packet into the rear of the Queue
   * \param p packet to enqueue
   * \return True if the operation was successful; false otherwise
   */
  bool Enqueue (const Ptr<Packet>& p);

  /**
   * \brief remove a packet from the front of the Queue
   * \return 0 if the operation was not successful; the packet otherwise.
   */
  const Ptr<Packet> Dequeue (void);

  /**
   * \brief get a copy of the item at the front of the queue without removing it
   * \return 0 if the operation was not successful; the packet otherwise.
   */
  const Ptr<const Packet> Peek (void) const;




  /**
   * \brief push the remaining segment of a packet back to the queue for transmiting in the future;
   * \param pkt the remaining segment
   */
  void PushFrontRemainingSegment (const Ptr<Packet>& pkt);

  /**
   * \brief return whether segmentation is currently carrying out
   * \return true: segmentation in progress. false: not
   */
  bool IsSegmentationRunning ( ) const;



  /**
   * \brief set the operating mode of this device. 
   *        Note that it should be configured through attribute feature of NS-3 and should not be changed during the simulation
   * \param mode The operating mode of this device.
   */
  void SetMode (XgponQueue::XgponQueueMode mode);

  /**
   * \brief get the encapsulation mode of this device.
   * \return the operating mode of this device.
   */
  XgponQueue::XgponQueueMode GetMode (void) const;





  /**
   * \brief check whether the queue is empty.
   * \return true: if the queue is empty; false: otherwise
   */
  bool IsEmpty (void) const;

  /**
   * \brief flush/empty the queue.
   */
  void DequeueAll (void);




  /**
   * \brief get the number of packets currently stored in the Queue
   */
  uint32_t GetNPackets (void) const;

  /**
   * \brief get the number of packets currently stored in the Queue
   */
  uint32_t GetNBytes (void) const;


  /**
   * \brief the amount of data in queue to be used by DBA and schedulers
   * \return the amount of data (padded + Xgem Frame header). unit: word / 4Bytes
   */
  uint32_t GetBufOccupancy4Scheduling ( );


  /**
   * \brief return the size of the remaining fragment (padded + Xgem Frame header). unit: word / 4Bytes
   */
  uint32_t GetFragBufOccupancy4Scheduling ( );





  ////////////////////////////////////////////////////////////////////////statistics-related functions

  /**
   * \brief get the total number of bytes received by this Queue since the
   * simulation began, or since ResetStatistics was called, according to 
   * whichever happened more recently
   */
  uint32_t GetTotalReceivedBytes (void) const;

  /**
   * \brief get the total number of packets received by this Queue since the
   * simulation began, or since ResetStatistics was called, according to 
   * whichever happened more recently
   */
  uint32_t GetTotalReceivedPackets (void) const;

  /**
   * \brief get the total number of bytes dropped by this Queue since the
   * simulation began, or since ResetStatistics was called, according to 
   * whichever happened more recently
   */
  uint32_t GetTotalDroppedBytes (void) const;

  /**
   * \brief get the total number of bytes dropped by this Queue since the
   * simulation began, or since ResetStatistics was called, according to 
   * whichever happened more recently
   */
  uint32_t GetTotalDroppedPackets (void) const;

  /**
   * \brief resets the counts for dropped packets, dropped bytes, received packets, and
   * received bytes.
   */
  void ResetStatistics (void);

  //jerome, Apr 21
  void SetAllocId(uint16_t id);
  uint16_t GetAllocId(void) const;

  /////////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;





private:

  virtual bool DoEnqueue (const Ptr<Packet>& p) = 0;
  virtual const Ptr<Packet> DoDequeue (void) = 0;
  virtual const Ptr<const Packet> DoPeek (void) const = 0;




protected:  

  XgponQueueMode m_mode;           //Queue mode: packet/byte
  uint32_t m_maxPackets;           //Queue size in packet
  uint32_t m_maxBytes;             //Queue size in byte


  //current queue status
  uint32_t m_nPackets;             //The amount of data (unit: packet) in queue
  uint32_t m_nBytes;               //The amount of data (unit: byte) in queue
  uint32_t m_nWords4Scheduling;    //the amount of data (unit: word / 4bytes) in queue with the assumption that each packet is padded if needed.

  //segmentation related variables
  Ptr<Packet> m_remainingSegment;

  //jerome, Apr 21
  std::vector<double> m_allPacket; // vector to store the index and time of the packet queued
  uint64_t m_packetIndex;          // indices for packet DoEnqueue and DoDequeue respectively
  uint16_t m_popIndex;             
  uint16_t m_allocId;              // to store the alloc Id

  //its main function is to maintain the statistics when a packet is dropped and trigger the trace source related with drop event.
  //when DoEnqueue fails, XgponQueue::Drop is called by the subclass.
  void Drop (const Ptr<Packet>& packet); 




private:
  TracedCallback<const Ptr<const Packet>& > m_traceEnqueue;
  TracedCallback<const Ptr<const Packet>& > m_traceDequeue;
  TracedCallback<const Ptr<const Packet>& > m_traceDrop;

  uint32_t m_nTotalReceivedBytes;
  uint32_t m_nTotalReceivedPackets;
  uint32_t m_nTotalDroppedBytes;
  uint32_t m_nTotalDroppedPackets;



  //to calculate the packet size in word after padding (if needed) + Xgem Frame header
  uint32_t CalculatePacketSize4Scheduling (uint32_t pktSizeInByte);
};





/////////////////////////////////////////////INLINE Functions
//jerome, Apr 21
inline void
XgponQueue::SetAllocId (uint16_t id)
{
  m_allocId = id;
}
inline uint16_t 
XgponQueue::GetAllocId(void) const
{
 return m_allocId;
}

inline void 
XgponQueue::SetMode (XgponQueue::XgponQueueMode mode) 
{ 
  m_mode = mode; 
}
inline XgponQueue::XgponQueueMode 
XgponQueue::GetMode (void) const
{
  return m_mode;
}

inline bool 
XgponQueue::IsSegmentationRunning ( ) const
{
  return (m_remainingSegment!=0);
}

inline const Ptr<const Packet>
XgponQueue::Peek (void) const
{
  return DoPeek ();
}

inline void
XgponQueue::DequeueAll (void)
{
  while (!IsEmpty ()) { Dequeue (); }
}

inline bool
XgponQueue::IsEmpty (void) const
{
  return m_nPackets == 0;
}



inline uint32_t 
XgponQueue::GetNPackets (void) const
{
  return m_nPackets;
}

inline uint32_t
XgponQueue::GetNBytes (void) const
{
  return m_nBytes;
}

inline uint32_t 
XgponQueue::GetBufOccupancy4Scheduling ( )
{
  return m_nWords4Scheduling;
}


inline uint32_t 
XgponQueue::GetFragBufOccupancy4Scheduling ( )
{
  if(m_remainingSegment!=0) return CalculatePacketSize4Scheduling(m_remainingSegment->GetSize());
  else return 0;
}





inline uint32_t
XgponQueue::GetTotalReceivedBytes (void) const
{
  return m_nTotalReceivedBytes;
}

inline uint32_t
XgponQueue::GetTotalReceivedPackets (void) const
{
  return m_nTotalReceivedPackets;
}

inline uint32_t
XgponQueue:: GetTotalDroppedBytes (void) const
{
  return m_nTotalDroppedBytes;
}

inline uint32_t
XgponQueue::GetTotalDroppedPackets (void) const
{
  return m_nTotalDroppedPackets;
}

inline void 
XgponQueue::ResetStatistics (void)
{
  m_nTotalReceivedBytes = 0;
  m_nTotalReceivedPackets = 0;
  m_nTotalDroppedBytes = 0;
  m_nTotalDroppedPackets = 0;
}







inline uint32_t 
XgponQueue::CalculatePacketSize4Scheduling (uint32_t pktSizeInByte)
{
  uint32_t sizeInByte = XgponXgemFrame::GetPaddedPayloadSize(pktSizeInByte); //after padding
  sizeInByte += XgponXgemHeader::XGPON_XGEM_HEADER_LENGTH;    //Xgem header overhead in word

  return (sizeInByte / 4);
}




} // namespace ns3

#endif /* XGPON_QUEUE_H */
