#ifndef CCONCAT_H
#define CCONCAT_H

#include <CConcatBase.h>
#include <vector>

class CConcat : public CConcatBase {
 public:
  CConcat();

  bool isSymlink() const { return symlink_; }
  void setSymlink(bool b) { symlink_ = b; }

  void addFile(const std::string &s) {
    files_.push_back(s);
  }

  uint getNumFiles() const { return files_.size(); }

  bool exec();

 private:
  bool check_match(int c);

 private:
  bool                     symlink_ { true };
  std::vector<std::string> files_;
};

#endif
