#include <CConcat.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>

int
main(int argc, char **argv)
{
  CConcat concat;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-')
      std::cerr << "Invalid option " << argv[i] << std::endl;
    else
      concat.addFile(argv[i]);
  }

  if (concat.getNumFiles() <= 1) {
    std::cerr << "Usage - " << argv[0] << " <file1> <file2> ..." << std::endl;
    exit(1);
  }

  if (! concat.exec())
    exit(1);

  return 0;
}

CConcat::
CConcat()
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

    if (! fp) {
      std::cerr << "Can't Open Input File " << files_[i] << std::endl;
      continue;
    }

    fprintf(stdout, "%s%s\n", id_.c_str(), files_[i].c_str());

    int c;

    while ((c = fgetc(fp)) != EOF) {
      if (check_match(c)) {
        std::cerr << "Concat Id found in input file " << files_[i] << std::endl;
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
  if (c != id_[check_pos_]) {
    check_pos_ = 0;
    return false;
  }

  ++check_pos_;

  uint len = id_.size();

  return (check_pos_ >= len);
}
