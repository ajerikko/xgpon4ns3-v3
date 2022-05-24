# xgpon4ns3-v3

VERSION 3.0 
RELEASE DATE 25.04.2022
RELEASED BY JEROME A AROKKIAM

This XG-PON module contains significant changes to v2.0 published a while ago. 
I've added comments where necessary, within the code, to indicate the changes (please beare with me as I've moved my repo from sourceforge to git and I've not figured out everything in git)
A summary of the changes are:
1. Compatibility upgrade to C++17 (return types are more spectific in some cases)
  -unit32_t changed to std::size_t
  -a return value for the call XXX.empty() is added as auto is_empty __attribute__((unused)) == XXX.empty()
2. Compability upgrade to ns-3.35
  -updated .AddTraceSource to include a string as the fourth parameter at definition within the .cc files
3. Specific Changes to the modules
    -example file (xgpon-dba-udp-test.cc) modified significantly
    -xgpon-net-device.h/cc modified with:
      --new parameters introduced for XgponNetDeviceStatistics, to be used in the example file
      --function XgponNetDeviceStatistics::initialize() modified with the new parameters
      --function SendSduToUpperLayer and it's inputs modified with the new parmeters
    -xgpon-onu-xgem-engine.cc and xgpon-olt-xgem-engine.cc modified with
      --function call to SendSduToUpperLayer modified
    -xgpon-id-allocator-speed.cc
      --the process of allocating down/up ports are modified; that is, each OLT-ONU pair would have a single down port and multiple up ports to represent the number of tconts to be used.


To use the XG-PON module with ns-3.35,
1. First, install and build the vanilla ns-3 version 3.35 (without the xgpon module)
2. Then download and extract this code; rename the top folder from 'xgpon4ns3-v3' to 'xgpon'; copy the 'xgpon' folder to ns-3.35/src/. The code would then sit at ns-3.35/src/xgpon/ 
3. Copy the example file xgpon-dba-udp-test.cc, from inside ns-3.35/src/xgpon/examples/, to inside the scratch folder at ns-3.35/scratch/
4. Finally, by following the comments and instructions in this example file, run the file as ./waf --run scratch/xgpon-dba-udp-test.cc
5. Voilà!!!

I'd ask that the users of this module duly cite the developers' scientific papers (as given in the example and DBA implementation files) and the code repository as appropriate. 

Questions and request for contributions in github are most welcome.

Jerome A Arokkiam
