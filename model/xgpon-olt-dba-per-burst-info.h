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

#ifndef XGPON_OLT_DBA_PER_BURST_INFO_H
#define XGPON_OLT_DBA_PER_BURST_INFO_H

#include <cstdlib>
#include <stack>
#include <deque>

#include "xgpon-xgtc-bwmap.h"
#include "xgpon-xgtc-bw-allocation.h"
#include "xgpon-tcont-olt.h"
#include "xgpon-burst-profile.h"


namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The class used to maintain the per-burst information when producing BW_MAP. 
 *        It is designed because T-CONTs of the same ONU should put together to form one burst and save PHY overhead. 
 *
 */
class XgponOltDbaPerBurstInfo : public SimpleRefCount<XgponOltDbaPerBurstInfo>
{
public:
  const static uint32_t MAX_TCONT_PER_BURST=16;                       //at most, 16 T-CONTs can be scheduled in one burst of the bwmap.
  const static uint32_t XGTC_USBURST_HEADERTRAILER_INWORD=2;          //xgtc-us-burst header (w/o ploam)+trailer size in word.

private:
  //used to allocate this structure from a pool for saving CPU.
  static bool m_poolEnabled;
  static std::stack<void*> m_pool;

public:

  /**
   * \brief Constructor
   */
  XgponOltDbaPerBurstInfo ();
  virtual ~XgponOltDbaPerBurstInfo ();




  /**
   * \brief Carry out initialization before the first XgponXgtcBwAllocation is scheduled in this burst.
   * \param ploam whether ploam exists in the header of this burst
   * \param profile the burst profile used by this burst
   */
  void Initialize(uint16_t onuId, bool ploam, const Ptr<XgponBurstProfile>& profile, uint16_t guardTime, uint16_t dataBlockSize, uint16_t fecBlockSize);

  /**
   * \brief Add one XgponXgtcBwAllocation into this burst. burst size will be updated
   * \param bwAlloc the bandwidth allocation to be added
   * \param tcontOlt the T-CONT that this bandwidth allocation belongs to
   */
  void AddOneNewBwAlloc(const Ptr<XgponXgtcBwAllocation>& bwAlloc, const Ptr<XgponTcontOlt>& tcontOlt);


  /**
   * \brief Put all XgponXgtcBwAllocation of this burst into BW-MAP. 
   *         It also puts XgponXgtcBwAllocation into the corresponding XgponTcontOlt for scheduling purpose.
   * \param map the bwmap that this burst will be put into
   * \param startTime the start time of this burst
   * \param now current simulation time
   */
  void PutAllBwAllocIntoBwmap(const Ptr<XgponXgtcBwmap>& map, uint16_t startTime, uint64_t now);

  /**
   * \brief Finds if there is a BwAllocation already present in the burst for a particular T-CONT
   * \param the T-CONT to look for in the bwMap
   * \return The BwAllocation corresponding to the T-CONT if that T-CONT was served before, 0 otherwise.
   */
  Ptr<XgponXgtcBwAllocation> FindBwAlloc(Ptr<XgponTcontOlt> tcontOlt);

  /**
   * \brief Adds extra bytes in a BwAllocation already present in a burst.
   * \param bwAlloc The BwAlloction that is getting extra bytes
   * \param extraGrantSize The amount of words (4 bytes) to add to the bwAlloc
   */
  void AddToExistingBwAlloc(Ptr<XgponXgtcBwAllocation> bwAlloc, uint32_t extraGrantSize);




  //////////////////////////////////////member variable accessors
  /**
   * \brief Get onu-id of the ONU that this burst belongs to
   */
  uint32_t GetOnuId( );

  /**
   * \brief Get the final size of the burst (phy overhead, FEC, etc.)
   */
  uint32_t GetFinalBurstSize( );

  /**
   * \brief Get the size of inter-burst gap + physical layer overhead.
   */
  uint32_t GetGapPhyOverhead( ); 


  /**
   * \brief Get the number of XgponXgtcBwAllocation in this burst
   */
  uint32_t GetBwAllocNumber();

  /**
    * \brief Get the Size of the XGTC header+data in this burst.
   */
  uint32_t GetHeaderTrailerDataSize( );


  ///////////////////////////////////////////Override new and delete to use a pool for call malloc/free too many times.
  void* operator new(size_t size) noexcept(false); //throw(const char*);
  void operator delete(void *p);

  //called before the end of simulation to avoid memory leakage.
  static void DisablePoolAllocation();



private:

  /**
   * \brief update overall burst size after each XgponXgtcBwAllocation is added
   */
  void UpdateFinalBurstSize( );

private:
  /////////////////////////////////////////////values are set just inside Initialization()
  uint16_t m_onuId;                //ONU ID
  uint8_t  m_gapPhyOverhead;       //GAP + PREAMBLE + DELIMITER. unit: word
  bool m_fec;                      //whether FEC will be used by this burst
  bool m_ploamExist;               //whether one PLOAM message will be sent in the upstream burst
  uint16_t m_dataBlockSize;        //used to calculate burst size when FEC is used. unit: word
  uint16_t m_fecBlockSize;  
  uint32_t m_headerTrailerDataSize;//XGTC header (ploam included if exist) + BwAllocations. unit: word


  //updated after each XgponXgtcBwAllocation is added
  uint32_t m_finalBurstSize;       //final size (all stuffs after FEC if FEC is used). unit: word



  std::deque< Ptr<XgponXgtcBwAllocation> > m_bwAllocs;  //the list of BwAllocations (of this ONU) to be included in this BW_MAP.

  std::deque< Ptr<XgponTcontOlt> > m_tcontOlts;         //the corresponding XgponTcontOlt. used in the case that FEC is enabled.
                                                         //header+data must be aligned with FEC code size. 
                                                         //With XgponTcontOlt, we can allocate the extra space to T-CONTs in the burst for alignment.



private:

  //disable users to call new[] and delete[].
  void* operator new[](size_t size) noexcept(false) //throw(const char*) 
  {
    void *p = malloc(size);
    if (!p) throw "XgponOltDbaPerBurstInfo::operator new[]() error!!!";
    return p; 
  }
  void operator delete[](void *p) { free(p); }

};




/////////////////////////////////////////////////////INLINE Functions
inline uint32_t 
XgponOltDbaPerBurstInfo::GetOnuId( )
{
  return m_onuId;
}

inline uint32_t 
XgponOltDbaPerBurstInfo::GetGapPhyOverhead( )
{
  return m_gapPhyOverhead;
}


inline uint32_t
XgponOltDbaPerBurstInfo::GetHeaderTrailerDataSize( )
{
  return m_headerTrailerDataSize;
}


inline uint32_t 
XgponOltDbaPerBurstInfo::GetFinalBurstSize( )
{
  return m_finalBurstSize;
}

inline uint32_t
XgponOltDbaPerBurstInfo::GetBwAllocNumber()
{
  return m_bwAllocs.size();
}



}; // namespace ns3

#endif // XGPON_OLT_DBA_PER_BURST_INFO_H
