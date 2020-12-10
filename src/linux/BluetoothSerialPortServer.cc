#include <node.h>
#include "DeviceINQ.h"
#include "BTSerialPortBindingServer.h"

using namespace v8;

void InitAllServer(Local<Object> exports) {
	BTSerialPortBindingServer::Init(exports);
}

NODE_MODULE(BluetoothSerialPortServer, InitAllServer)
