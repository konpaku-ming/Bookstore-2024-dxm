#ifndef LOG_STORAGE_H
#define LOG_STORAGE_H

class FinanceDatabase {
private:
  std::fstream log_data;
  string file_name;
  int count = 0;

public:
  FinanceDatabase() = default;

  FinanceDatabase(const string &file_name) : file_name(file_name) {}

  void initialise(string FN = "") {
    if (access(file_name.c_str(), F_OK) == 0) {
      // 检查文件是否存在
      return;
    }
    if (FN != "") {
      file_name = FN;
    }
    log_data.open(file_name, std::ios_base::out);
    log_data.close();
    log_data.open(file_name, std::ios_base::in | std::ios_base::out);
  }

  void get_info(int &tmp, int n) {
    log_data.seekg((n - 1) * sizeof(int));
    log_data.read(reinterpret_cast<char *>(&tmp), sizeof(int));
  }

  void write_info(int tmp, int n) {
    log_data.seekp((n - 1) * sizeof(int));
    log_data.write(reinterpret_cast<char *>(&tmp), sizeof(int));
  }
};
#endif // LOG_STORAGE_H
