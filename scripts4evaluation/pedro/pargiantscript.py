'''
 Copyright (c)  2012 The Provost, Fellows and Scholars of the 
 College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 as
 published by the Free Software Foundation;

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 Author: Pedro Alvarez <pinheirp@tcd.ie>
'''

import os
from multiprocessing import Pool
from operator import sub
import operator
import numpy as np
import matplotlib.pyplot as plt
import time
import functools


'''Constants'''
VERBOSE=True
DEBUGGING=False
RUN_SIMULATIONS=True
PARSE_RESULTS=True

IP_HEADER_SIZE=20 #In Bytes
UDP_HEADER_SIZE=8 #In Bytes

def main():
  
  nProcesses=8
    
  print ""
  print "Running Parallel Simulations for GIANT Scheduler with", nProcesses, "Processes"
  print ""
  
  '''Simulation Parameters'''
  nOnus=16
  appPacketSize = 1472 # In Bytes
  numPackets = 0
  applicationTxDataRate = 2048000000 #Application rate when transmitting (Bps)
  simTime = 5
  appTime = 5

  fixedRate=128000
  assuredRate=140928000
  nonAssuredRate=0
  bestEffortRate=0
  
  print "Simulation Running with the following parameters:"
  print "    Number of ONUS:", nOnus
  print "    Packet Size at IP Level:", appPacketSize+IP_HEADER_SIZE+UDP_HEADER_SIZE, "Bytes", "or analogously:", (appPacketSize+IP_HEADER_SIZE+UDP_HEADER_SIZE)*8, "Bits"
  print "    Number of Packets to Simulate:", numPackets
  print "    Data Rate of Application when ON:", applicationTxDataRate*1e-6,"Mbps"
  print "    Simulated Seconds:", simTime
  print "    Application Run Time", appTime
  print ""
  
  
  loadOfferedToXgpon=np.array([80, 90, 100, 110, 120, 130, 140, 150])*1e6
  
  appMaxBytes=appPacketSize*numPackets
  
  if(VERBOSE):
    print "Application Parameters:"  
    print "    Maximum amount of bytes transmitted by application:", appMaxBytes
    print ""

  
  if(RUN_SIMULATIONS):
    clearData()
    pool=Pool(processes=nProcesses)
    processingTimes=runSimulations(loadOfferedToXgpon, appPacketSize, appMaxBytes,fixedRate, assuredRate, nonAssuredRate, bestEffortRate, nOnus, simTime, appTime, pool)
    pool.close()
    pool.join()
    
  if(PARSE_RESULTS):
    parseResults(loadOfferedToXgpon)
    #print processingTimes
    #plotProcessingTimes(loadOfferedToXgpon, processingTimes)
    
def clearData():

  print "Clearing Simulation Files From:", os.environ['NS3']+"data/giant-data"
  if (os.listdir(os.environ['NS3']+"data/giant-data") == []): 
    print "Warning: Directory is empty. Nothing to be removed." 
  else: 
    print "Warning: Deleting files from previous simulations." 
    os.system('rm $NS3/data/giant-data/*')

def runSimulations(load, appPacketSize, appMaxBytes, fixedRate, assuredRate, nonAssuredRate, bestEffortRate, nOnus, simTime, appTime, pool):
  
  print "Running GIANT Simulations. Load:", load
  print ""
  
  runningTimes=np.empty(0)
  
  params = [(load[it], appPacketSize, appMaxBytes, fixedRate, assuredRate, nonAssuredRate, bestEffortRate, nOnus,simTime,appTime, it) for it in range(len(load)) ] 
  runningTimes=pool.map_async(fireSimulationProcess,params)
  
def fireSimulationProcess(params):
  
  print "Firing a new simulation"
  
  (load, appPacketSize, appMaxBytes, fixedRate, assuredRate, nonAssuredRate, bestEffortRate, nOnus,simTime,appTime, it)=params
  
  print "Running GIANT Simulation", it, "Offered Load:", load
  print ""
    
  tic = time.time()
  
  os.system('cd $NS3; ./waf --run \"src/xgpon/examples/xgpon-example-multiple-onus-udp-giant-cbr'
    ' --FilenameSuffix='+str(it)+
    ' --PacketSize='+str(appPacketSize)+
    ' --MaxBytes='+str(appMaxBytes)+
    ' --DataRate='+str(load)+
    ' --NumOnus='+str(nOnus)+
    ' --SimTime='+str(simTime)+
    ' --AppTime='+str(appTime)+
    ' --FixedBandwidth='+str(fixedRate)+
    ' --AssuredBandwidth='+str(assuredRate)+
    ' --NonAssuredBandwidth='+str(nonAssuredRate)+
    ' --BestEffortBandwidth='+str(bestEffortRate)+
    ' --verbose=0'
    '\"'
  )
  toc = time.time()
  #simulationTimes.append(toc - tic)
  print "Finished Running GIANT Simulations."
  print ""
    
  return toc-tic
  
def parseResults(load):
  
  print "Parsing GIANT Simulation Results. Load:", load
  print ""  
  
  (average, lostPackets)=getAverageDelayAndLostPacketsFromFiles(load)

  print "Average is:", average
  print "Lost packets:", lostPackets
  
  plotAverageDelayResults(load, average)
  plotLostPacketsResults(load, lostPackets)


def getAverageDelayAndLostPacketsFromFiles(load):
    
  average=np.empty(0)
  lostPackets=np.empty(0)

  for i in range(0, len(load)):
    
    print "Getting delay for load: ", load[i]
    
    sentFile = openFile(os.environ['NS3']+"data/giant-data/OnOffTx"+str(i), "r") 
    recvFile = openFile(os.environ['NS3']+"data/giant-data/PktSinkRx"+str(i), "r") 
    diffFile = openFile(os.environ['NS3']+"data/giant-data/Diff"+str(i), "w")
      
    average=np.append(average, getAverageDelayFromFiles(sentFile,recvFile,diffFile))
    
    diffFile.close()
    diffFile = openFile(os.environ['NS3']+"data/giant-data/Diff"+str(i), "r")
    
    lostPackets=np.append(lostPackets, getNumLostPackets(diffFile))
    
    recvFile.close()
    sentFile.close()
    diffFile.close()
    
    print "Delay for ", load[i]," is: ", average[i]
    
  return (average, lostPackets)

  
def getAverageDelayFromFiles(sentFile, recvFile, diffFile):

  """
  Getting the average delay from the files saved by the simulations.
  Packet Info Saved in the following form:
      Tx: "Tx Time: +xxx.0ns Pkt UId *yy& Payload (size=zz)"
      Rx: "Rx Time: +xxx.0ns Pkt UId *yy& Payload (size=zz)"
  """

  #sentList = []
  #recvList = []
  
  delayArray=np.empty(0)
  sentContents = sentFile.readlines()
  recvContents = recvFile.readlines()

  for txItem in sentContents:
    txUid=(txItem.split("*")[1].split("&")[0])       
    for rxItem in recvContents:
      rxUid=(rxItem.split("*")[1].split("&")[0])
      if(txUid==rxUid):
        #Tx packet found in the received file =). Just save the delay
        rxTime=rxItem.split("+")[1].split(".")[0]
        txTime=txItem.split("+")[1].split(".")[0]
        delayArray=np.append(delayArray,float(rxTime)-float(txTime))
        break
    else:
      diffFile.write(txItem) #Packet was not found in Rx File. Probably dropped in the queue. Write to diff file to count lost packets.     
  

  return np.mean(delayArray*1e-6) #returns mean delay in ms

def getNumLostPackets(diffFile):
  diffContents = diffFile.readlines()
  return len(diffContents)

def plotAverageDelayResults(x,y):
  #convert to Mbps
  #xMbps = tuple(i*1e-6 for i in x)
  xMbps=x*1e-6
  plt.plot(xMbps,y,marker='o', linestyle='-')
  plt.suptitle(' Delay vs Offered Load for Giant Engine')
  plt.xlabel('Load (Mbps)')
  plt.ylabel('Delay (ms)')
  plt.savefig('giant-average-delay.pdf')
  plt.show()

def plotLostPacketsResults(x,y):
  #convert to Mbps
  xMbytes = x*1e-6
  plt.plot(xMbytes,y,marker='o', linestyle='-')
  plt.suptitle(' Number of Lost Packets vs Offered Load for Giant Engine')
  plt.xlabel('Load (Mbps)')
  plt.ylabel('Packets Lost')
  plt.savefig('giant-lost-packets.pdf')
  plt.show()
  
def plotProcessingTimes(x,y):
  #convert to Mbps
  xMbytes = x*1e-6
  plt.plot(xMbytes,y,marker='o', linestyle='-')
  plt.suptitle(' Processing Time vs Offered Load for Giant Engine')
  plt.xlabel('Load (Mbps)')
  plt.ylabel('Processing Times (s)')
  plt.savefig('giant-processing-times.pdf')
  plt.show()
  
  
def openFile(filename, mode):
    
    try:
      file = open(filename, mode) #Data sent from ONU 0, file 0
    except IOError:
      print "Could not open file:", filename
      quit()

    return file


if __name__=='__main__':
  main()
