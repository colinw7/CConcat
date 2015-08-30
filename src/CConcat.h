#ifndef CCONCAT_H
#define CCONCAT_H

#include <CConcatBase.h>
#include <vector>

class CConcat : public CConcatBase {
 public:
  CConcat();

  void addFile(const std::string &fileName) {
    files_.push_back(fileName);
  }

  uint getNumFiles() const { return files_.size(); }

  bool exec();

 private:
  bool check_match(int c);

 private:
  std::vector<std::string> files_;
};

#endif
