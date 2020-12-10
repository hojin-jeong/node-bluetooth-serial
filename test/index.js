const BT = require('../lib/bluetooth-serial')

const client = new BT.BluetoothSerial()
const clientTestFunctions = [
    'inquire', 'findSerialPortChannel', 'connect', 'write', 'on', 'close'
]
console.log('Checking client...')
clientTestFunctions
    .forEach(fun => {
        if (typeof client[fun] !== 'function')
            throw new Error("Assert failed: " + fun +
                "should be a function but is " + (typeof client[fun]))
    })

console.log('Ok!')

if (process.platform === 'linux') {
    console.log('Checking server...')

    const server = new BT.BluetoothSerialServer()
    const serverTestFunctions = [
        'listen', 'write', 'on', 'close'
    ]
    serverTestFunctions
        .forEach(fun => {
            if (typeof server[fun] !== 'function')
                throw new Error("Assert failed: " + fun +
                    "should be a function but is " + (typeof server[fun]))
        })

    console.log('Ok!')
}

process.exit(0)

