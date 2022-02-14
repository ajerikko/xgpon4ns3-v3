# xgpon4ns3-v3
+++++++++++++++++++++++++++++++++++++++++++++
RELEASE NOTES of the NS-3 XG-PON module v3.0
+++++++++++++++++++++++++++++++++++++++++++++

VERSION 3.0 
RELEASE DATE 14.02.2022
RELEASED BY JEROME A AROKKIAM (jerom2005raj@gmail.com)

This XG-PON module contains significant changes to v2.0 published a while ago. 
I've added comments where necessary, within the code, to indicate the changes (please beare with me as I've moved my repo from sourceforge to git and I've not figured out everything in git)
A summary of the changes are:
1. Compatibility upgrade to C++17 (return types are more spectific in some cases)
  --unit32_t changed to std::size_t
  --a return value for the call XXX.empty() is added as auto is_empty __attribute__((unused)) == XXX.empty()
2. Compability upgrade to ns-3.35
  --updated .AddTraceSource to include a string as the fourth parameter at definition within the .cc files


To use the XG-PON module with ns-3.35,
>> First, install and build the vanilla ns-3 version 3.35 (without the xgpon module)
>> Then download this code and copy it inside a folder named 'xgpon' under ns-3.35/src. The code would then sit at ns-3.35/src/xgpon/ 
>> Copy the example file xgpon-dba-udp-test.cc, from inside ns-3.35/src/xgpon/examples/, to inside the scratch folder at ns-3.35/scratch/
>> Finally, by following the comments and instructions in this example file, runn the file as ./waf --run scratch/xgpon-dba-udp-test.cc (stay at ns-3.35/ for running this command)
>> Voilà!!!

I dont want to place any restrictions on the usage except that whoever uses this module in ns3 is kind enough to cite the developers' scientific papers (as given in the example and DBA implementation files) as appropriate. 
Questions and appreciations are most welcome.

Thank you for taking the time to explore this module. My sincere thanks to the ns3 team for their excellent work with the overall ns3 code base.


  

-Jerome
14/02/2022
