#ifndef CCONCAT_H
#define CCONCAT_H

class CConcat {
 private:
  string         id_;
  vector<string> files_;
  uint           check_pos_;

 public:
  CConcat();

  void addFile(const string &fileName) {
    files_.push_back(fileName);
  }

  uint getNumFiles() const { return files_.size(); }

  bool exec();

 private:
  bool check_match(int c);

  static const string &getDefId();
};

#endif
