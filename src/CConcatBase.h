#ifndef CCONCAT_BASE_H
#define CCONCAT_BASE_H

#include <string>
#include <sys/types.h>

class CConcatBase {
 public:
  CConcatBase();

  const std::string &fileName() const { return filename_; }
  void setFileName(const std::string &v) { filename_ = v; }

 protected:
  bool readId(FILE *fp);

  const std::string &getDefId() const;

 protected:
  std::string id_;
  std::string filename_;
  uint        check_pos_;
};

#endif
