const { inherits } = require('util')
const { EventEmitter } = require('events')
const { DeviceINQ } = require('bindings')('BluetoothSerialPort.node')

class DeviceInquiry extends EventEmitter {}
inherits(DeviceInquiry, DeviceINQ)

module.exports = DeviceInquiry
