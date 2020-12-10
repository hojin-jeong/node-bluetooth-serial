#ifndef NODE_BTSP_SRC_DEVICE_INQ_H
#define NODE_BTSP_SRC_DEVICE_INQ_H

#include <node.h>
#include <uv.h>
#include <nan.h>

#ifdef __APPLE__
#import <Foundation/NSArray.h>
#endif

struct bt_device {
    char address[19];
    char name[248];
};

#ifndef __APPLE__
struct bt_inquiry {
    int num_rsp;
    bt_device *devices;
};
#endif

class DeviceINQ : public Nan::ObjectWrap {
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
        static void Init(v8::Local<v8::Object> exports);
        static void EIO_SdpSearch(uv_work_t *req);
        static void EIO_AfterSdpSearch(uv_work_t *req);
#ifdef __APPLE__
        static NSArray *doInquire();
#else
        static bt_inquiry doInquire();
#endif

    private:
        struct sdp_baton_t {
            DeviceINQ *inquire;
            uv_work_t request;
            Nan::Callback* cb;
            int channelID;
            char address[40];
        };

        DeviceINQ();
        ~DeviceINQ();

        static NAN_METHOD(New);
        static NAN_METHOD(Inquire);
        static NAN_METHOD(InquireSync);
        static NAN_METHOD(SdpSearch);
        static NAN_METHOD(ListPairedDevices);

};

#endif
