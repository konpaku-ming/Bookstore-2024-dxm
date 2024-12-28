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
    int zero = 0;
    log_data.write(reinterpret_cast<char *>(&zero), sizeof(int));
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

  bool FinanceShow(const long long count) {
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

enum operation {
  kSu,
  kLogout,
  kRegister,
  kPasswd,
  kUseradd,
  kDelete,
  kShow,
  kBuy,
  kSelect,
  kModify,
  kImport
};

struct Log {
  int privilege;
  operation op;
  char id[id_len + 1];
  double finance;
};

class LogDatabase {

private:
  std::fstream log_data;
  int total = 0; // 留一个int存储日志总数

public:
  LogDatabase() = default;

  ~LogDatabase() { log_data.close(); }

  void Restore() {
    log_data.seekg(0);
    log_data.read(reinterpret_cast<char *>(&total), sizeof(int));
  }

  void Save() {
    log_data.seekp(0);
    log_data.write(reinterpret_cast<char *>(&total), sizeof(int));
  }

  void Init() {
    if (access("logsystem", F_OK) == 0) {
      // 检查文件是否存在
      log_data.open("logsystem", std::ios_base::in | std::ios_base::out);
      log_data.seekg(0);
      log_data.read(reinterpret_cast<char *>(&total), sizeof(int));
      return;
    }
    log_data.open("logsystem", std::ios_base::out);
    log_data.close();
    log_data.open("logsystem", std::ios_base::in | std::ios_base::out);
    total = 0;
    log_data.write(reinterpret_cast<char *>(&total), sizeof(int));
  }

  void Read(Log &dest, const int n) {
    log_data.seekg((n - 1) * sizeof(Log) + sizeof(int));
    log_data.read(reinterpret_cast<char *>(&dest), sizeof(Log));
  }

  void Update(Log &new_book, const int n) {
    // 更新第n本书的信息
    log_data.seekp((n - 1) * sizeof(Log) + sizeof(int));
    log_data.write(reinterpret_cast<char *>(&new_book), sizeof(Log));
  }

  int Push(Log &new_book) {
    // 在尾部写入新书
    total++;
    log_data.seekp((total - 1) * sizeof(Log) + sizeof(int));
    log_data.write(reinterpret_cast<char *>(&new_book), sizeof(Log));
    return total;
  }

  void ReportLog() {
    Log temp{};
    for (int i = 1; i <= total; i++) {
      Read(temp, i);
      cout << temp.id << "\t";
      if (temp.op == kSu) {
        cout << "su\n";
      } else if (temp.op == kLogout) {
        cout << "logout\n";
      } else if (temp.op == kRegister) {
        cout << "register\n";
      } else if (temp.op == kPasswd) {
        cout << "passwd\n";
      } else if (temp.op == kUseradd) {
        cout << "useradd\n";
      } else if (temp.op == kDelete) {
        cout << "delete\n";
      } else if (temp.op == kShow) {
        cout << "show\n";
      } else if (temp.op == kBuy) {
        cout << "buy\t" << std::fixed << std::setprecision(2) << temp.finance
             << "\n";
      } else if (temp.op == kSelect) {
        cout << "select\n";
      } else if (temp.op == kModify) {
        cout << " modify\n";
      } else if (temp.op == kImport) {
        cout << "import\t" << std::fixed << std::setprecision(2) << temp.finance
             << "\n";
      }
    }
  }

  void ReportEmployee() {
    Log temp{};
    for (int i = 1; i <= total; i++) {
      Read(temp, i);
      if (temp.privilege != 3) {
        continue;
      }
      cout << temp.id << "\t";
      if (temp.op == kSu) {
        cout << "su\n";
      } else if (temp.op == kLogout) {
        cout << "logout\n";
      } else if (temp.op == kRegister) {
        cout << "register\n";
      } else if (temp.op == kPasswd) {
        cout << "passwd\n";
      } else if (temp.op == kUseradd) {
        cout << "useradd\n";
      } else if (temp.op == kDelete) {
        cout << "delete\n";
      } else if (temp.op == kShow) {
        cout << "show\n";
      } else if (temp.op == kBuy) {
        cout << "buy\t" << std::fixed << std::setprecision(2) << temp.finance
             << "\n";
      } else if (temp.op == kSelect) {
        cout << "select\n";
      } else if (temp.op == kModify) {
        cout << " modify\n";
      } else if (temp.op == kImport) {
        cout << "import\t" << std::fixed << std::setprecision(2) << temp.finance
             << "\n";
      }
    }
  }

  void ReportFinance() {
    Log temp{};
    for (int i = 1; i <= total; i++) {
      Read(temp, i);
      if (temp.op == kBuy) {
        cout << temp.id << "\t"
             << "buy\t" << temp.finance << "\n";
      } else if (temp.op == kImport) {
        cout << temp.id << "\t"
             << "import\t" << temp.finance << "\n";
      }
    }
  }
};
#endif // LOG_STORAGE_H
