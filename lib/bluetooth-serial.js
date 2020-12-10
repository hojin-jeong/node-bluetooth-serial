
const { inherits } = require('util')
const { EventEmitter } = require('events')
const { DeviceINQ } = require('bindings')('BluetoothSerialPort.node')

inherits(DeviceINQ, EventEmitter)

class BluetoothSerial extends EventEmitter {
    "use strict";
    
    /**
     * Creates an instance of the bluetooth-serial object.
     * @constructor
     * @name Connection
     * @param rpcHandler The RPC handler.
     */
    constructor() {
        super()

        this.$BluetoothBinding = require('bindings')('BluetoothSerialPort.node').BTSerialPortBinding
        this.$DeviceINQ = new DeviceINQ()
        this.$connection = undefined
        this.$address = undefined
    }

    $_found(address, name) {
        this.emit('found', address, name)
    }
    $_finish() {
        this.emit('finished')
    }
    $_onDataReceived(buffer) {
        if (buffer.length <= 0) {
            // we are done reading. The remote device might have closed the device
            // or we have closed it ourself. Lets cleanup our side anyway...
            this.close()
        } else {
            this.$_read()
        }
    }
    $_read() {
        process.nextTick(_ => {
            if (this.$connection) {
                this.$connection.read((err, buffer) => {
                    if(!err && buffer) {
                        this.emit('data', buffer)
                    } else {
                        this.removeListener('data', this.$_onDataReceived)  // remove it to prevent
                        this.close()                                        // calling self.on many
                        this.emit('failure', err)                           // times
                    }
                })
            }
        })
    }

    listPairedDevices() {
        return new Promise(resolve => {
            this.$DeviceINQ.listPairedDevices(pairedDevices => {
                resolve(pairedDevices)
            })
        })
    }
    inquire() {
        this.$DeviceINQ.inquire(this.$_found, this.$_finish)
    }
    inquireSync() {
        this.$DeviceINQ.inquireSync(this.$_found, this.$_finish)
    }
    findSerialPortChannel(address) {
        return new Promise((resolve, reject) => {
            this.$DeviceINQ.findSerialPortChannel(address, channel => {
                if(channel >= 0) {
                    resolve(channel)
                } else {
                    reject()
                }
            })
        })
    }
    connect(address, channel) {
        return new Promise(async (resolve, reject) => {
            if(!channel) {
                channel = await this.findSerialPortChannel(address).catch(reject)
            }

            const onDeviceConnected = _ => {
                this.$address = address
                this.$connection = connection

                this.on('data', this.$_onDataReceived.bind(this)) // add listener to event 'data'
                this.$_read()

                resolve()
            }
            const onDeviceConnectFailed = err => {
                // cleaning up the the failed connection
                if(this.$connection) this.$connection.close(address)
                reject(err)
            }
            const connection = new this.$BluetoothBinding(address, channel, onDeviceConnected.bind(this), onDeviceConnectFailed.bind(this))
        })
    }
    write(buffer) {
        return new Promise((resolve, reject) => {
            if(this.$connection) {
                this.$connection.write(buffer, this.$address, (err, bytesLength) => {
                    if(err) {
                        reject(err)
                    } else {
                        resolve(bytesLength)
                    }
                })
            } else {
                reject(new Error('Not connected'))
            }
        })
    }
    close() {
        if(this.$connection) {
            this.$connection.close(this.$address)
            this.$connection = undefined
        }

        this.emit('closed')
    }
    isOpen() {
        return this.$connection !== undefined
    }
}

class BluetoothSerialServer extends EventEmitter {
    "use strict";

    /**
     * Creates an instance of the bluetooth-serial-server object.
     * @constructor
     */
    constructor() {
        if(process.platform !== 'linux') throw new Error('Server support only on Linux')
        super()

        this.$BTSerialPortBindingServer = require('bindings')('BluetoothSerialPortServer.node').BTSerialPortBindingServer
        this.$constants = {
            SERIAL_PORT_PROFILE_UUID        : '1101',
            DEFAULT_SERVER_CHANNEL          : 1,
            ERROR_CLIENT_CLOSED_CONNECTION  : 'Error: Connection closed by the client'
        }
        this.$server = undefined
        this.$inDisconnect = undefined
    }

    $_read() {
        process.nextTick(_ => {
            if(this.$server) {
                this.$server.read((err, buffer) => {
                    if(!err && buffer) {
                        this.emit('data', buffer)
                        this.$_read()
                    } else if(this.$inDisconnect) {
                        // We were told to disconnect, and now we've disconnected, so emit disconnected
                        this.$inDisconnect = false
                        this.emit('disconnected')
                    } else if(err !== this.$constants.ERROR_CLIENT_CLOSED_CONNECTION) {
                        this.emit('failure', err)
                    } else {
                        this.emit('closed')
                    }
                })
            }
        })
    }

    listen(options) {
        return new Promise((resolve, reject) => {
            options         = options || {}
            options.uuid    = options.uuid || this.$constants.SERIAL_PORT_PROFILE_UUID
            options.channel = options.channel || this.$constants.DEFAULT_SERVER_CHANNEL

            const onDeviceConnected = clientAddress => {
                this.$_read()
                resolve(clientAddress)
            }
            const onDeviceConnectFailed = err => {
                if(this.$server) {
                    this.$server.close()
                } else {
                    reject(err)
                }
            }
            this.$server = new this.$BTSerialPortBindingServer(onDeviceConnected.bind(this), onDeviceConnectFailed.bind(this))
        })
    }
    write(buffer) {
        return new Promise((resolve, reject) => {
            if(this.$server) {
                this.$server.write(buffer, (err, bytesLength) => {
                    if(err) {
                        reject(err)
                    } else {
                        resolve(bytesLength)
                    }
                })
            } else {
                reject(new Error('Not connected'))
            }
        })
    }
    disconnectClient() {
        if(this.$server) {
            this.$inDisconnect = true
            this.$server.disconnectClient()
        }
    }
    close() {
        if(this.$server) {
            this.$server.close()
            this.$server = undefined
        }
    }
    isOpen() {
        return this.$server && this.$server.isOpen()
    }
}

module.exports = {
    BluetoothSerial,
    BluetoothSerialServer,
    DeviceINQ
}