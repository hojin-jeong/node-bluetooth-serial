// Type definitions for bluetooth-serial-port v2.2.8
// Project: https://github.com/hojin-jeong/node-bluetooth-serial
// Definitions: https://github.com/DefinitelyTyped/DefinitelyTyped

import { EventEmitter } from "events";

declare module BluetoothSerial {
  class BluetoothSerial extends EventEmitter {
    constructor();
    listPairedDevices(): Promise<object>;
    inquire(): void;
    inquireSync(): void;
    findSerialPortChannel(
        address: string
    ): Promise<number>;
    connect(
        address: string,
        channel?: number
    ): Promise<void>;
    write(
        buffer: Buffer
    ): Promise<number>;
    close(): void;
    isOpen(): boolean;
  }
  class BluetoothSerialServer extends EventEmitter {
    constructor();
    listen(
        options?: {uuid?: string; channel: number;}
    ): Promise<string>;
    write(
        buffer: Buffer
    ): Promise<number>;
    disconnectClient(): void;
    close(): void;
    isOpen(): boolean;
  }
}

export = BluetoothSerial;
