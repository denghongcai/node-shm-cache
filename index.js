var binary = require('node-pre-gyp');
var path = require('path');
var binding_path = binary.find(path.resolve(path.join(__dirname,'./package.json')));
var binding = require(binding_path);
var ShmCacheWrapper = binding.ShmCacheWrapper;

var defaultConfig = {
  type: "shm",
  filename: "/tmp/shmcache",
  maxMemory: 64 * 1024 * 1024,
  segmentSize: 4 * 1024 * 1024,
  maxKeyCount: 100000,
  maxValueSize: 256 * 1024,
  recycleKeyOnce: 128,
  vaPolicy: {
    avgKeyTTL: 900,
    discardMemorySize: 128,
    maxFailTimes: 5,
    sleepUsWhenRecycleValidEntries: 1000
  },
  lockPolicy: {
    tryLockIntervalUs: 200,
    detectDeadLockIntervalMs: 1000
  }
};

var ShmCache = function (config) {
  var c = Object.assign({}, defaultConfig);
  Object.assign(c, config);
  switch (c.type) {
    case 'shm':
      c.type = 1;
      break;
    case 'mmap':
      c.type = 2;
      break;
    default:
      c.type = 1;
      break;
  }
  var wrapper = new ShmCacheWrapper(c);
  Object.defineProperty(this, 'wrapper', {
    value: wrapper,
    enumerable: false,
    configurable: false,
    writable: false
  });
};

ShmCache.prototype.set = function (key, value, ttl) {
  if (typeof key !== 'string') {
    throw new Error('key must be string');
  }
  var parsedTTL = parseInt(ttl, 10);
  if (isNaN(parsedTTL) || parsedTTL <= 0) {
    throw new Error('ttl must be number and greater than zero');
  }
  return this.wrapper.Set(key, value, parsedTTL);
};

ShmCache.prototype.get = function (key) {
  if (typeof key !== 'string') {
    throw new Error('key must be string');
  }
  return this.wrapper.Get(key);
};

ShmCache.prototype.remove = function (key) {
  if (typeof key !== 'string') {
    throw new Error('key must be string');
  }
  return this.wrapper.Remove(key);
};

ShmCache.prototype.stats = function () {
  return this.wrapper.Stats();
};

ShmCache.prototype.clear = function () {
  return this.wrapper.Clear();
};

module.exports = ShmCache;
