#ifndef NODE_BTSP_SRC_BLUETOOTH_WORKER_H
#define NODE_BTSP_SRC_BLUETOOTH_WORKER_H

#import <Foundation/NSObject.h>
#import <IOBluetooth/objc/IOBluetoothRFCOMMChannel.h>
#import <IOBluetooth/objc/IOBluetoothDevice.h>
#import <IOBluetooth/objc/IOBluetoothDeviceInquiry.h>
#import "BluetoothDeviceResources.h"
#import "pipe.h"
#include <v8.h>
#include <node.h>

struct device_info_t {
    char address[19];
    char name[248];
};

@interface BluetoothWorker: NSObject {
    @private
BluetoothDeviceResources *res;
    NSThread *worker;
    pipe_producer_t *inquiryProducer;
    NSLock *sdpLock;
    NSLock *connectLock;
    NSLock *deviceLock;
    IOReturn connectResult;
    int lastChannelID;
    NSLock *writeLock;
    IOReturn writeResult;
    NSTimer *keepAliveTimer;
}

+ (id)getInstance: (NSString *) address;
- (void)disconnectFromDevice: (NSString *) address;
- (IOReturn)connectDevice: (NSString *) address onChannel: (int) channel withPipe: (pipe_t *)pipe;
- (IOReturn)writeAsync:(void *)data length:(UInt16)length toDevice: (NSString *)address;
- (void)inquireWithPipe: (pipe_t *)pipe;
- (int)getRFCOMMChannelID: (NSString *) address;

- (void)rfcommChannelData:(IOBluetoothRFCOMMChannel*)rfcommChannel data:(void *)dataPointer length:(size_t)dataLength;
- (void)rfcommChannelClosed:(IOBluetoothRFCOMMChannel*)rfcommChannel;

- (void)deviceInquiryComplete: (IOBluetoothDeviceInquiry*) sender
    error: (IOReturn) error
    aborted: (BOOL) aborted;
- (void)deviceInquiryDeviceFound: (IOBluetoothDeviceInquiry*) sender
    device: (IOBluetoothDevice*) device;

- (void) rfcommChannelWriteComplete:(IOBluetoothRFCOMMChannel*)rfcommChannel refcon:(void*)refcon status:(IOReturn)error;

@end

#endif
