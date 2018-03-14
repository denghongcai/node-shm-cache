#ifndef BSON_WRAPPER_H
#define BSON_WRAPPER_H

#include <nan.h>
#include <stack>
#include <bson.h>

#define SHM_INTERNAL_KEY "__SHM__VALUE__"
#define SHM_INTERNAL_KEY_LEN 14

class BSONWrapper {
  public:
    BSONWrapper(v8::Local<v8::Value>);
    BSONWrapper(const char*, int);
    ~BSONWrapper();

    const char* getBuffer();
    int getBufferLen();
    v8::Local<v8::Value> getValue();

  private:
    bson_t *b;
};

#endif
