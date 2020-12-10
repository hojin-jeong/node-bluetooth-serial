#ifndef NODE_BTSP_SRC_SERIAL_PORT_BINDING_SERVER_H
#define NODE_BTSP_SRC_SERIAL_PORT_BINDING_SERVER_H

#include <node.h>
#include <uv.h>
#include <nan.h>
#include <memory>
#include "ngx-queue.h"
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

class BTSerialPortBindingServer : public Nan::ObjectWrap {
    public:
        static void Init(v8::Local<v8::Object> exports);
        static NAN_METHOD(Write);
        static NAN_METHOD(Close);
        static NAN_METHOD(Read);
        static NAN_METHOD(DisconnectClient);
        static NAN_METHOD(IsOpen);

    private:

        struct listen_baton_t {
            BTSerialPortBindingServer *rfcomm;
            uv_work_t request;
            Nan::Callback* cb;
            Nan::Callback* ecb;
            char clientAddress[40];
            int status;
            int listeningChannelID;
            char errorString[1024];
            uuid_t uuid;
        };

        struct read_baton_t {
            BTSerialPortBindingServer *rfcomm;
            uv_work_t request;
            Nan::Callback* cb;
            unsigned char result[1024];
            int errorno;
            int size;
            bool isDisconnect;
            bool isClose;
        };

        struct write_baton_t {
            BTSerialPortBindingServer *rfcomm;
            char address[40];
            void* bufferData;
            size_t bufferLength;
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

        int s;
        int rep[2];
        int mClientSocket = 0;

        listen_baton_t * mListenBaton = nullptr;
        sdp_session_t * mSdpSession = nullptr;

        uv_mutex_t mWriteQueueMutex;
        ngx_queue_t mWriteQueue;


        BTSerialPortBindingServer();
        ~BTSerialPortBindingServer();

        static NAN_METHOD(New);
        static void EIO_Listen(uv_work_t *req);
        static void EIO_AfterListen(uv_work_t *req);
        static void EIO_Write(uv_work_t *req);
        static void EIO_AfterWrite(uv_work_t *req);
        static void EIO_Read(uv_work_t *req);
        static void EIO_AfterRead(uv_work_t *req);

        void AdvertiseAndAccept();
        void Advertise();
        void CloseClientSocket();


        class ClientWorker : public Nan::AsyncWorker {
            public:
                ClientWorker(Nan::Callback *cb, listen_baton_t *baton);
                ~ClientWorker();
                void Execute() override;
                void HandleOKCallback() override;
            private:
                listen_baton_t * mBaton;

        };
};

#endif
