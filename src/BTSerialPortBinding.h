#ifndef NODE_BTSP_SRC_SERIAL_PORT_BINDING_H
#define NODE_BTSP_SRC_SERIAL_PORT_BINDING_H

#define READ_BUFFER_SIZE 4096

#include <node.h>
#include <uv.h>
#include <nan.h>
#include "ngx-queue.h"

#ifdef __APPLE__
#import <Foundation/NSObject.h>
#import <IOBluetooth/objc/IOBluetoothDevice.h>
#import <IOBluetooth/objc/IOBluetoothDeviceInquiry.h>
#import "pipe.h"
#endif

class BTSerialPortBinding : public Nan::ObjectWrap {
    private:
#ifdef _WIN32
        bool initialized;

        bool GetInitializedProperty() {
            return initialized;
        }
#endif

    public:
#ifdef _WIN32
        __declspec(property(get = GetInitializedProperty)) bool Initialized;
#endif

        static Nan::Persistent<v8::FunctionTemplate> s_ct;
        static void Init(v8::Local<v8::Object> exports);
        static NAN_METHOD(Write);
        static NAN_METHOD(Close);
        static NAN_METHOD(Read);

    private:
        struct connect_baton_t {
            BTSerialPortBinding *rfcomm;
            uv_work_t request;
            Nan::Callback* cb;
            Nan::Callback* ecb;
            char address[40];
            int status;
            int channelID;
        };

        struct read_baton_t {
            BTSerialPortBinding *rfcomm;
            uv_work_t request;
            Nan::Callback* cb;
            unsigned char result[READ_BUFFER_SIZE];
            int errorno;
            int size;
        };

        struct write_baton_t {
            BTSerialPortBinding *rfcomm;
            char address[40];
            void* bufferData;
            int bufferLength;
            Nan::Persistent<v8::Object> buffer;
            Nan::Callback* callback;
            size_t result;
            char errorString[1024];
        };

        struct queued_write_t {
            uv_work_t req;
            ngx_queue_t queue;
            write_baton_t* baton;
        };


#ifdef __APPLE__
        pipe_consumer_t *consumer;
#else
#ifdef _WIN32
        SOCKET s;
#else
        int s;
        int rep[2];
#endif
#endif

        BTSerialPortBinding();
        ~BTSerialPortBinding();

        static NAN_METHOD(New);
        static void EIO_Connect(uv_work_t *req);
        static void EIO_AfterConnect(uv_work_t *req);
        static void EIO_Write(uv_work_t *req);
        static void EIO_AfterWrite(uv_work_t *req);
        static void EIO_Read(uv_work_t *req);
        static void EIO_AfterRead(uv_work_t *req);
};

#endif
