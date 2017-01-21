#ifndef NATIVE_EXTENSION_GRAB_H
#define NATIVE_EXTENSION_GRAB_H

#include <nan.h>
#include "logger.h"
#include "shmcache.h"

// Based on https://nodejs.org/api/addons.html#addons_wrapping_c_objects but using NAN
class ShmCacheWrapper : public Nan::ObjectWrap {
  public:
    static NAN_MODULE_INIT(Init);

  private:
    explicit ShmCacheWrapper(struct shmcache_config);
    ~ShmCacheWrapper();

    static NAN_METHOD(New);
    static NAN_METHOD(Set);
    static NAN_METHOD(Get);
    static NAN_METHOD(Remove);
    static NAN_METHOD(Stats);
    static NAN_METHOD(Clear);
    static Nan::Persistent<v8::Function> constructor;
    
    struct shmcache_context context;
    struct shmcache_config config;
};

#endif
