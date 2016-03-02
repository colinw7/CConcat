#ifndef CCONCAT_BASE_H
#define CCONCAT_BASE_H

#include <string>
#include <sys/types.h>

class CConcatBase {
 public:
  CConcatBase();

  const std::string &id() const { return id_; }
  void setId(const std::string &s) { id_ = s; }

  const std::string &filename() const { return filename_; }
  void setFilename(const std::string &s) { filename_ = s; }

 protected:
  bool readId(FILE *fp);

  const std::string &getDefId() const;

 protected:
  std::string id_;
  std::string filename_;
  uint        checkPos_ { 0 };
};

#endif
