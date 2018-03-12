#include "bson_wrapper.h"

int bson_append_from_v8_primitive(bson_t *b, v8::Local<v8::Value> src, const char* key, const int keyLen) {
    if (src->IsNumber()) {
        return bson_append_double(b, key, keyLen, Nan::To<double>(src).FromJust());
    }
    if (src->IsBoolean()) {
        return bson_append_bool(b, key, keyLen, Nan::To<bool>(src).FromJust());
    }
    if (src->IsNull()) {
        return bson_append_null(b, key, keyLen);
    }
    if (src->IsString()) {
        v8::String::Utf8Value valueString(Nan::To<v8::String>(src).ToLocalChecked());
        return bson_append_utf8(b, key, keyLen,
                *valueString, valueString.length());
    }
    if (node::Buffer::HasInstance(src)) {
        return bson_append_binary(b, key, keyLen, BSON_SUBTYPE_BINARY, (uint8_t*)node::Buffer::Data(src), node::Buffer::Length(src));
    }

    return -1;
}

v8::Local<v8::Value> v8_value_from_bson_value(const bson_value_t* v) {
    switch(v->value_type) {
        case BSON_TYPE_DOUBLE:
            return Nan::New<v8::Number>(v->value.v_double);
        case BSON_TYPE_NULL:
            return Nan::Null();
        case BSON_TYPE_BOOL:
            return Nan::New<v8::Boolean>(v->value.v_bool);
        case BSON_TYPE_UTF8:
            return Nan::New<v8::String>(v->value.v_utf8.str, v->value.v_utf8.len).ToLocalChecked();
        case BSON_TYPE_BINARY:
            return Nan::CopyBuffer((const char*)v->value.v_binary.data, v->value.v_binary.data_len).ToLocalChecked();
        default:
            return Nan::Undefined();
    }
}

bson_t bson_new_from_v8_value(v8::Local<v8::Value> src, const char* key = NULL, int keyLen = 0, bson_t* parent = NULL) {
    bson_t b;
    if (parent == NULL) {
        bson_init(&b);
        if (!src->IsObject() || src->IsNull()) {
            bson_append_from_v8_primitive(&b, src, SHM_INTERNAL_KEY, SHM_INTERNAL_KEY_LEN);
            return b;
        }
        parent = &b;
    }

    bson_t child;

    if (src->IsArray()) {
        bson_append_array_begin(parent, key == NULL ? SHM_INTERNAL_KEY : key, key == NULL ? SHM_INTERNAL_KEY_LEN : keyLen, &child);
    } else {
        bson_append_document_begin(parent, key == NULL ? SHM_INTERNAL_KEY : key, key == NULL ? SHM_INTERNAL_KEY_LEN : keyLen, &child);
    }
        
    v8::Local<v8::Object> obj = Nan::To<v8::Object>(src).ToLocalChecked();
    v8::Local<v8::Array> keys = Nan::GetOwnPropertyNames(obj).ToLocalChecked();
    uint32_t length = keys->Length();
    for(uint32_t i = 0; i < length; i++) {
        v8::Local<v8::Value> key = Nan::Get(keys, i).ToLocalChecked();
        v8::String::Utf8Value keyString(Nan::To<v8::String>(key).ToLocalChecked());
        v8::Local<v8::Value> value = Nan::Get(obj, key).ToLocalChecked();
        if (!value->IsObject() || value->IsNull() || node::Buffer::HasInstance(value)) {
            bson_append_from_v8_primitive(&child, value, *keyString, keyString.length());
        } else if (value->IsObject()) {
            bson_new_from_v8_value(value, *keyString, keyString.length(), &child);
        } 
    }
    if (src->IsArray()) {
        bson_append_array_end(parent, &child);
    } else {
        bson_append_document_end(parent, &child);
    }
    return *parent;
}

v8::Local<v8::Value> v8_value_from_bson_iter(bson_iter_t* iter, v8::Local<v8::Value> t) {
    while(bson_iter_next(iter)) {
        const char* key = bson_iter_key(iter);
        const bson_value_t* value = bson_iter_value(iter);
        bson_iter_t child;
        v8::Local<v8::Value> target;
        if (value->value_type == BSON_TYPE_ARRAY) {
            target = Nan::New<v8::Array>();
        } else if (value->value_type == BSON_TYPE_DOCUMENT) {
            target = Nan::New<v8::Object>();
        } else {
            Nan::Set(Nan::To<v8::Object>(t).ToLocalChecked(),
                    Nan::New<v8::String>(key).ToLocalChecked(),
                    Nan::New<v8::Value>(v8_value_from_bson_value(value)));
            continue;
        }
        Nan::Set(Nan::To<v8::Object>(t).ToLocalChecked(),
                Nan::New<v8::String>(key).ToLocalChecked(),
                target);
        bson_iter_recurse(iter, &child);
        v8_value_from_bson_iter(&child, target);
    }    
    return t;
}

v8::Local<v8::Value> v8_value_from_bson(bson_t* b) {
    bson_iter_t iter;
    bson_iter_init(&iter, b);
    bson_iter_next(&iter);
    const char* key = bson_iter_key(&iter);
    const bson_value_t* value = bson_iter_value(&iter);
    v8::Local<v8::Value> target;
    if (strncmp(key, SHM_INTERNAL_KEY, SHM_INTERNAL_KEY_LEN) == 0) {
        if (value->value_type == BSON_TYPE_ARRAY) {
            target = Nan::New<v8::Array>();
        } else if (value->value_type == BSON_TYPE_DOCUMENT) {
            target = Nan::New<v8::Object>();
        } else {
            return v8_value_from_bson_value(value);
        }
        bson_iter_t child;
        bson_iter_recurse(&iter, &child);
        return v8_value_from_bson_iter(&child, target);
    }
    // TODO
    return Nan::Undefined();
}

BSONWrapper::BSONWrapper(v8::Local<v8::Value> src) {
    b = bson_new_from_v8_value(src, NULL, 0, NULL);
}

BSONWrapper::BSONWrapper(const char* data, int length) {
    const uint8_t* udata = reinterpret_cast<const uint8_t*>(data);
    bson_init_static(&b, udata, length);
}

const char* BSONWrapper::getBuffer() {
    return reinterpret_cast<const char*>(bson_get_data(&b));
}

int BSONWrapper::getBufferLen() {
    return b.len;
}

v8::Local<v8::Value> BSONWrapper::getValue() {
    return v8_value_from_bson(&b);
}

BSONWrapper::~BSONWrapper() {
    bson_destroy(&b);
}
