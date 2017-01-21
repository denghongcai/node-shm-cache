# node-shm-cache

[![Build Status](https://travis-ci.org/denghongcai/node-shm-cache.svg)](https://travis-ci.org/denghongcai/node-shm-cache)

A wrapper for libshmcache

## Building

To compile the extension, you need to install [libshmcache](https://github.com/happyfish100/libshmcache) first, then run 

```
$ npm i
$ npm run configure
$ npm run build
```

All subsequent builds only need `npm run build`

You can confirm everything built correctly by [running the test suite](#to-run-tests).

### To run tests:

```
$ npm test
```

or to run test continuously 

```
$ npm test -- watch
```
