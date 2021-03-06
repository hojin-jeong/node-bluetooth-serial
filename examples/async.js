const BluetoothSerialPort = require('../lib/bluetooth-serial');

const rfcomm = new BluetoothSerialPort.BluetoothSerialPort();

rfcomm.on('found', function (address, name) {
	console.log('found device:', name, 'with address:', address);
});

rfcomm.on('finished', function () {
  console.log('inquiry finished');
});

console.log('start inquiry');
rfcomm.inquire();
console.log('should be displayed before the end of inquiry');