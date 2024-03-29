## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):

    module = bld.create_ns3_module('xgpon', ['core', 'network', 'internet'])
    module.source = [

        'model/pon-channel.cc',
        'model/pon-net-device.cc',
        'model/xgpon-tcont.cc',
        'model/xgpon-tcont-olt.cc',
        'model/xgpon-tcont-onu.cc',
        'model/xgpon-burst-profile.cc',
        'model/xgpon-channel.cc',
        'model/xgpon-connection.cc',
        'model/xgpon-connection-receiver.cc',
        'model/xgpon-connection-sender.cc',
        'model/xgpon-ds-frame.cc',
        'model/xgpon-fifo-queue.cc',
        'model/xgpon-key.cc',
        'model/xgpon-link-info.cc',
        'model/xgpon-net-device.cc',
        'model/xgpon-olt-conn-manager-flexible.cc',
        'model/xgpon-olt-conn-manager-speed.cc',
        'model/xgpon-olt-conn-manager.cc',
        'model/xgpon-olt-conn-per-onu.cc',
        'model/xgpon-olt-dba-bursts.cc',
        'model/xgpon-olt-dba-engine.cc',
        'model/xgpon-olt-dba-engine-round-robin.cc',
        'model/xgpon-olt-dba-engine-giant.cc',
        'model/xgpon-olt-dba-engine-xgiant.cc',
        'model/xgpon-olt-dba-engine-xgiantdeficit.cc',
        'model/xgpon-olt-dba-engine-xgiantprop.cc',
        'model/xgpon-olt-dba-engine-ebu.cc',
        'model/xgpon-olt-dba-per-burst-info.cc',
        'model/xgpon-olt-ds-scheduler.cc',
        'model/xgpon-olt-ds-scheduler-round-robin.cc',
        'model/xgpon-olt-engine.cc',
        'model/xgpon-olt-framing-engine.cc',
        'model/xgpon-olt-net-device.cc',
        'model/xgpon-olt-omci-engine.cc',
        'model/xgpon-olt-phy-adapter.cc',
        'model/xgpon-olt-ploam-engine.cc',
        'model/xgpon-olt-xgem-engine.cc',
        'model/xgpon-onu-conn-manager-flexible.cc',
        'model/xgpon-onu-conn-manager-speed.cc',
        'model/xgpon-onu-conn-manager.cc',
        'model/xgpon-onu-dba-engine.cc',
        'model/xgpon-onu-engine.cc',
        'model/xgpon-onu-framing-engine.cc',
        'model/xgpon-onu-net-device.cc',
        'model/xgpon-onu-omci-engine.cc',       
        'model/xgpon-onu-ploam-engine.cc',
        'model/xgpon-onu-phy-adapter.cc',
        'model/xgpon-onu-us-scheduler.cc', 
        'model/xgpon-onu-us-scheduler-round-robin.cc', 
        'model/xgpon-onu-xgem-engine.cc',
        'model/xgpon-phy.cc',
        'model/xgpon-psbd.cc',
        'model/xgpon-psbu.cc',
        'model/xgpon-qos-parameters.cc',
        'model/xgpon-queue.cc',
        'model/xgpon-service-record.cc',
        'model/xgpon-us-burst.cc',
        'model/xgpon-xgem-frame.cc',
        'model/xgpon-xgem-header.cc',
        'model/xgpon-xgem-routines.cc',
        'model/xgpon-xgtc-bw-allocation.cc',
        'model/xgpon-xgtc-bwmap.cc',
        'model/xgpon-xgtc-dbru.cc',
        'model/xgpon-xgtc-ds-frame.cc',
        'model/xgpon-xgtc-ds-header.cc',
        'model/xgpon-xgtc-ploam.cc',
        'model/xgpon-xgtc-us-allocation.cc',
        'model/xgpon-xgtc-us-burst.cc',
        'model/xgpon-xgtc-us-header.cc',
        'helper/xgpon-helper.cc',
        'helper/xgpon-config-db.cc',
        'helper/xgpon-id-allocator-flexible.cc',
        'helper/xgpon-id-allocator-speed.cc',
        'helper/xgpon-id-allocator.cc',
        ]

    #module_test = bld.create_ns3_module_test_library('xgpon')
    #module_test.source = [ #no longer available for testing
    #    'test/xgpon-pool-allocation-test.cc',
    #    'test/xgpon-olt-dba-engine-giant-test.cc',
    #    ]

    headers = bld(features='ns3header')
    headers.module = 'xgpon'
    headers.source = [

        'model/pon-channel.h',
        'model/pon-frame.h',
        'model/pon-net-device.h',
        'model/xgpon-tcont.h',
        'model/xgpon-tcont-olt.h',
        'model/xgpon-tcont-onu.h',
        'model/xgpon-burst-profile.h',
        'model/xgpon-channel.h',
        'model/xgpon-connection.h',
        'model/xgpon-connection-receiver.h',
        'model/xgpon-connection-sender.h',
        'model/xgpon-ds-frame.h',
        'model/xgpon-fifo-queue.h',
        'model/xgpon-key.h',
        'model/xgpon-link-info.h',        
        'model/xgpon-net-device.h',
        'model/xgpon-olt-conn-manager-flexible.h',
        'model/xgpon-olt-conn-manager-speed.h',
        'model/xgpon-olt-conn-manager.h',
        'model/xgpon-olt-conn-per-onu.h',
        'model/xgpon-olt-dba-bursts.h',
        'model/xgpon-olt-dba-engine.h',
        'model/xgpon-olt-dba-engine-round-robin.h',
        'model/xgpon-olt-dba-engine-giant.h',
        'model/xgpon-olt-dba-engine-xgiant.h',
        'model/xgpon-olt-dba-engine-xgiantdeficit.h',
        'model/xgpon-olt-dba-engine-xgiantprop.h',
        'model/xgpon-olt-dba-engine-ebu.h',
        'model/xgpon-olt-dba-per-burst-info.h',
        'model/xgpon-olt-ds-scheduler.h',
        'model/xgpon-olt-ds-scheduler-round-robin.h',
        'model/xgpon-olt-engine.h',
        'model/xgpon-olt-framing-engine.h',
        'model/xgpon-olt-net-device.h',
        'model/xgpon-olt-omci-engine.h',
        'model/xgpon-olt-phy-adapter.h',
        'model/xgpon-olt-ploam-engine.h',
        'model/xgpon-olt-xgem-engine.h',
        'model/xgpon-onu-conn-manager-flexible.h',
        'model/xgpon-onu-conn-manager-speed.h',
        'model/xgpon-onu-conn-manager.h',
        'model/xgpon-onu-dba-engine.h',
        'model/xgpon-onu-engine.h',
        'model/xgpon-onu-framing-engine.h',
        'model/xgpon-onu-net-device.h',
        'model/xgpon-onu-omci-engine.h',
        'model/xgpon-onu-ploam-engine.h',
        'model/xgpon-onu-phy-adapter.h',
        'model/xgpon-onu-us-scheduler.h',
        'model/xgpon-onu-us-scheduler-round-robin.h', 
        'model/xgpon-onu-xgem-engine.h',
        'model/xgpon-phy.h',
        'model/xgpon-psbd.h',
        'model/xgpon-psbu.h',
        'model/xgpon-qos-parameters.h',
        'model/xgpon-queue.h',
        'model/xgpon-service-record.h',
        'model/xgpon-us-burst.h',
        'model/xgpon-xgem-frame.h',
        'model/xgpon-xgem-header.h',
        'model/xgpon-xgem-routines.h',
        'model/xgpon-xgtc-bw-allocation.h',
        'model/xgpon-xgtc-bwmap.h',
        'model/xgpon-xgtc-dbru.h',
        'model/xgpon-xgtc-ds-frame.h',
        'model/xgpon-xgtc-ds-header.h',
        'model/xgpon-xgtc-ploam.h',
        'model/xgpon-xgtc-us-allocation.h',
        'model/xgpon-xgtc-us-burst.h',
        'model/xgpon-xgtc-us-header.h',
        'helper/xgpon-helper.h',
        'helper/xgpon-config-db.h',
        'helper/xgpon-id-allocator-flexible.h',
        'helper/xgpon-id-allocator-speed.h',
        'helper/xgpon-id-allocator.h',
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')
        #End of Pedro's files
        #bld.add_subdirs('')

    #bld.ns3_python_bindings()
