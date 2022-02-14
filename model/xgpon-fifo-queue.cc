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
 *         Jerome Arokkiam <jerome@4c.ucc.ie>
 */
#include "ns3/log.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "xgpon-fifo-queue.h"
#include "ns3/simulator.h"



NS_LOG_COMPONENT_DEFINE ("XgponFifoQueue");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponFifoQueue);

TypeId
XgponFifoQueue::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponFifoQueue")
    .SetParent<XgponQueue> ()
    .AddConstructor<XgponFifoQueue> ()
  ;

  return tid;
}
TypeId
XgponFifoQueue::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponFifoQueue::XgponFifoQueue (): XgponQueue ()
{
}
XgponFifoQueue::~XgponFifoQueue ()
{
}




bool 
XgponFifoQueue::DoEnqueue (const Ptr<Packet>& p)
{
  NS_LOG_FUNCTION (this << p);
  double now = Simulator::Now().GetSeconds();

  if (m_mode == XGPON_QUEUE_MODE_PACKETS && m_nPackets >= m_maxPackets)
    {
      NS_LOG_LOGIC ("Queue full (at max packets) -- droppping pkt");
      Drop (p);
      return false;
    }

  if (m_mode == XGPON_QUEUE_MODE_BYTES && (m_nBytes + p->GetSize () >= m_maxBytes)) 
    {
      NS_LOG_LOGIC ("Queue full (packet would exceed max bytes) -- droppping pkt");
      Drop (p);
      return false;
    }

  m_packets.push(p);

  if (m_allocId >= 1024)
  {  
  //this excerpt is used to store the time each packet is stored in the ONU buffer to analyse the delay of packet in each ONU buffer.
  //3 elements are added each time packet is in queue.
    m_allPacket.push_back(m_packetIndex);   //index of the packet arrived in queue
    //std::cout << "AllocId: " << m_allocId << ", pushed index " << m_packetIndex << " & Time " << now << " index in vector-" << m_allPacket.back() <<std::endl;
    m_allPacket.push_back(now);         //time the packet arrived in queue
    m_allPacket.push_back(0);           //null value for pop time here. This will be modified at DoDequeue
    m_allPacket.push_back(p->GetSize());
    m_allPacket.push_back(m_allocId);

    NS_LOG_LOGIC ("Packet added to queue");
  }
    m_packetIndex++;                    //increment packetIndex for every packet RECEIVED, regardless of the packet being logged.

  NS_LOG_LOGIC ("Number packets " << m_nPackets);
  NS_LOG_LOGIC ("Number bytes " <<  m_nBytes);

  return true;
}



const Ptr<Packet>
XgponFifoQueue::DoDequeue (void)
{
  NS_LOG_FUNCTION (this);
  Ptr<Packet> p;

  if (m_remainingSegment==0 && m_packets.empty ())
    {
      NS_LOG_LOGIC ("Queue empty");
      return 0;
    }

  if (m_remainingSegment==0)
    {
			double now = Simulator::Now().GetSeconds();

      p = m_packets.front ();
      m_packets.pop();

/* this excerpt is used to store the time each packet is deleted from the ONU buffer
 * for dba delay based analysis
 */
      if ((m_allocId >= 1024) && (m_popIndex < m_allPacket.size())) 
      //to make sure that pop happens at the same frequency as push
      {
				m_allPacket.at(m_popIndex+2) = now; 	// time the packet is deleted from queue (US Tx time)
        m_popIndex = m_popIndex + 5;		// 5 elements are added above each time packet is in queue.
        //std::cout << "m_popIndex " << m_popIndex << " AllocId "<< m_allocId << std::endl;
				/* once 100 packets have been transferred, vector is reset to prevent the memory size from growing
				* m_popIndex is also reset, but packetIndex continue to grow to know the no of pkets arrived in buffer
				*/    
				
        if (m_popIndex % 500 == 0)
        {
        	/* enable this block if need per packet delay stats. 
          // The packets are stored in m_allPacket vector as blocks of 500 packets and then printed thereafter to minimise processing delay on printing the stats
        
					for(int i = (m_popIndex-500) ; i < m_popIndex ; i=i+5)
	        {
						std::cout 
							<< "xgponFifoQdelay" 
							<< ",pktId," << m_allPacket.at(i) 
							<< ",xgponQInT," << m_allPacket.at(i+1) 
							<< ",xgponQOutT," << m_allPacket.at(i+2) 
							<< ",size," << m_allPacket.at(i+3) 
							<< ",allocId," << m_allPacket.at(i+4) << std::endl;
					}
        	*/
        	
					//erase the elements once written to the file
					m_allPacket.erase(m_allPacket.begin(), m_allPacket.begin() + m_popIndex);
					m_popIndex = 0; 			//reset popIndex
	        }
      }


    }
    else
    {
      p = m_remainingSegment;
      m_remainingSegment = 0;
    }  

  NS_LOG_LOGIC ("Popped " << p);

  NS_LOG_LOGIC ("Number packets " << m_packets.size ());
  NS_LOG_LOGIC ("Number bytes " << m_nBytes);

  return p;
}





const Ptr<const Packet>
XgponFifoQueue::DoPeek (void) const
{
  NS_LOG_FUNCTION (this);

  if (m_remainingSegment==0 && m_packets.empty ())
    {
      NS_LOG_LOGIC ("Queue empty");
      return 0;
    }

  if (m_remainingSegment==0)
    {
      return m_packets.front ();
    }
  else
    {
      return m_remainingSegment;
    }
}







}; // namespace ns3
