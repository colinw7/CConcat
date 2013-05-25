#ifndef CCONCAT_H
#define CCONCAT_H

#include <sys/types.h>
#include <string>
#include <vector>

class CConcat {
 public:
  CConcat();

  void addFile(const std::string &fileName) {
    files_.push_back(fileName);
  }

  uint getNumFiles() const { return files_.size(); }

  bool exec();

 private:
  bool check_match(int c);

  static const std::string &getDefId();

 private:
  std::string              id_;
  std::vector<std::string> files_;
  uint                     check_pos_;
};

#endif
