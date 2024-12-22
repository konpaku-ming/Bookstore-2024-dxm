#ifndef LOG_STORAGE_H
#define LOG_STORAGE_H
#include <cmath>

class FinanceDatabase {
private:
  std::fstream log_data;
  string file_name;

public:
  int info_len = 0;

  FinanceDatabase() = default;

  FinanceDatabase(const string &file_name) : file_name(file_name) {}

  ~FinanceDatabase() { log_data.close(); }

  void Restore() {
    log_data.seekg(0);
    log_data.read(reinterpret_cast<char *>(&info_len), sizeof(int));
  }

  void Save() {
    log_data.seekp(0);
    log_data.write(reinterpret_cast<char *>(&info_len), sizeof(int));
  }

  void Init(string FN = "") {
    if (access(file_name.c_str(), F_OK) == 0) {
      log_data.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
      // 检查文件是否存在
      return;
    }
    if (FN != "") {
      file_name = FN;
    }
    log_data.open(file_name, std::ios::out | std::ios::binary);
    log_data.close();
    log_data.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
  }

  void Read(double &tmp, int n) {
    log_data.seekg(sizeof(int) + (n - 1) * sizeof(double));
    log_data.read(reinterpret_cast<char *>(&tmp), sizeof(double));
  }

  void Write(double tmp) {
    info_len++;
    log_data.seekp(sizeof(int) + (info_len - 1) * sizeof(double));
    log_data.write(reinterpret_cast<char *>(&tmp), sizeof(double));
  }

  bool FinanceReport(const long long count) {
    if (count > info_len || count < 0)
      return false;
    if (count == 0) {
      cout << "\n";
      return true;
    }
    double temp;
    double in = 0;
    double out = 0;
    for (int i = info_len - count + 1; i <= info_len; i++) {
      Read(temp, i);
      if (temp > 0) {
        in += temp;
      } else {
        out += temp;
      }
    }
    std::cout << "+ " << std::fixed << std::setprecision(2) << in << " - "
              << std::fixed << std::setprecision(2) << fabs(out) << "\n";
    return true;
  }
};
#endif // LOG_STORAGE_H
