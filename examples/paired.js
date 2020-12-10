const BluetoothSerialPort = require('../lib/bluetooth-serial');
const rfcomm = new BluetoothSerialPort.BluetoothSerialPort();
rfcomm.listPairedDevices(function (list) {
		console.log(JSON.stringify(list,null,2));
});