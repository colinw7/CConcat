#include <std_c++.h>
#include "CConcat.h"

int
main(int argc, char **argv)
{
  CConcat concat;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-')
      cerr << "Invalid option " << argv[i] << endl;
    else
      concat.addFile(argv[i]);
  }

  if (concat.getNumFiles() <= 1) {
    cerr << "Usage - " << argv[0] << " <file1> <file2> ..." << endl;
    exit(1);
  }

  if (! concat.exec())
    exit(1);

  return 0;
}

CConcat::
CConcat() :
 check_pos_(0)
{
  char *env = getenv("CONCAT_ID");

  if (env)
    id_ = env;

  if (id_ == "")
    id_ = getDefId();
}

bool
CConcat::
exec()
{
  fprintf(stdout, "CONCAT_ID=%s\n", id_.c_str());

  uint num_files = files_.size();

  for (uint i = 0; i < num_files; ++i) {
    FILE *fp = fopen(files_[i].c_str(), "rb");

    if (fp == NULL) {
      cerr << "Can't Open Input File " << files_[i] << endl;
      continue;
    }

    fprintf(stdout, "%s%s\n", id_.c_str(), files_[i].c_str());

    int c;

    while ((c = fgetc(fp)) != EOF) {
      if (check_match(c)) {
        cerr << "Concat Id found in input file " << files_[i] << endl;
        fclose(fp);
        return false;
      }

      fputc(c, stdout);
    }

    fclose(fp);
  }

  return true;
}

bool
CConcat::
check_match(int c)
{
  uint len = id_.size();

  if (c != id_[check_pos_]) {
    check_pos_ = 0;
    return false;
  }

  ++check_pos_;

  return (check_pos_ >= len);
}

const string &
CConcat::
getDefId()
{
  static string id = "##concat##";

  return id;
}
