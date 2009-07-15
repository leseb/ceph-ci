#ifndef __S3ACCESS_H
#define __S3ACCESS_H

#include <time.h>
#include <string>
#include <vector>
#include <include/types.h>

#include <openssl/md5.h>

#define SERVER_NAME "S3FS"

#define S3_ATTR_PREFIX  "user.s3."

#define S3_ATTR_ACL		S3_ATTR_PREFIX "acl"
#define S3_ATTR_ETAG    	S3_ATTR_PREFIX "etag"
#define S3_ATTR_BUCKETS		S3_ATTR_PREFIX "buckets"
#define S3_ATTR_META_PREFIX	S3_ATTR_PREFIX "x-amz-meta-"

typedef void *S3AccessHandle;

struct s3_err {
  const char *num;
  const char *code;
  const char *message;

  s3_err() : num(NULL), code(NULL), message(NULL) {}
};


struct S3ObjEnt {
  std::string name;
  size_t size;
  time_t mtime;
  char etag[MD5_DIGEST_LENGTH * 2 + 1];

  void encode(bufferlist& bl) const {
     ::encode(name, bl);
     ::encode(size, bl);
     ::encode(mtime, bl);
  }
  void decode(bufferlist::iterator& bl) {
    ::decode(name, bl);
    ::decode(size, bl);
    ::decode(mtime, bl);
  }
};
WRITE_CLASS_ENCODER(S3ObjEnt)

class S3Access {
public:
  virtual int initialize(int argc, char *argv[]) { return 0; }
  virtual int list_buckets_init(std::string& id, S3AccessHandle *handle) = 0;
  virtual int list_buckets_next(std::string& id, S3ObjEnt& obj, S3AccessHandle *handle) = 0;

  virtual int list_objects(std::string& id, std::string& bucket, int max, std::string& prefix, std::string& marker, std::vector<S3ObjEnt>& result) = 0;

  virtual int create_bucket(std::string& id, std::string& bucket, map<nstring, bufferlist>& attrs) = 0;
  virtual int put_obj(std::string& id, std::string& bucket, std::string& obj, const char *data, size_t size,
                      time_t *mtime,
                      map<nstring, bufferlist>& attrs) = 0;

  virtual int copy_obj(std::string& id, std::string& dest_bucket, std::string& dest_obj,
                      std::string& src_bucket, std::string& src_obj,
                      time_t *mtime,
                      const time_t *mod_ptr,
                      const time_t *unmod_ptr,
                      const char *if_match,
                      const char *if_nomatch,
                      map<nstring, bufferlist>& attrs,
                      struct s3_err *err) = 0;
  virtual int delete_bucket(std::string& id, std::string& bucket) = 0;
  virtual int delete_obj(std::string& id, std::string& bucket, std::string& obj) = 0;

  virtual int get_obj(std::string& bucket, std::string& obj, 
            char **data, off_t ofs, off_t end,
            map<nstring, bufferlist> *attrs,
            const time_t *mod_ptr,
            const time_t *unmod_ptr,
            const char *if_match,
            const char *if_nomatch,
            bool get_data,
            struct s3_err *err) = 0;

  virtual int get_attr(std::string& bucket, std::string& obj,
                       const char *name, bufferlist& dest) = 0;
  virtual int set_attr(std::string& bucket, std::string& obj,
                       const char *name, bufferlist& bl) = 0;

  static S3Access *init_storage_provider(const char *type, int argc, char *argv[]);
  static S3Access *store;
};

#define s3store S3Access::store



#endif
