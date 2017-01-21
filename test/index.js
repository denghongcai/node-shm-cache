var ShmCache =  require('../');
var assert = require('assert');
var obj = new ShmCache({});

describe('ShmCache', function() {
  it('should set ok', function() {
    obj.set("test", "test", 60);
  });

  it('should get ok', function() {
    assert.equal(obj.get("test"), "test");
  });

  it('should remove ok', function() {
    obj.remove("test");
    assert.equal(obj.get("test"), undefined);
  });

  it('should stats ok', function() {
    assert.equal(obj.stats().hashTable.currentKeyCount, 0);
  });

  it('should clear ok', function() {
    obj.clear()
  });
});
