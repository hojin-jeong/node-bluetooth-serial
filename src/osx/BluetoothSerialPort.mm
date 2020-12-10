#include <node.h>
#include "DeviceINQ.h"
#include "BTSerialPortBinding.h"

using namespace v8;

void InitAll(Local<Object> exports) {
	DeviceINQ::Init(exports);
	BTSerialPortBinding::Init(exports);
}

NODE_MODULE(BluetoothSerialPort, InitAll)
