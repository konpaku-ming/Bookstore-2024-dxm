#ifndef ACCOUNT_STORAGE_H
#define ACCOUNT_STORAGE_H

#include "account.h"
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
class AccountDatabase;

// 图书的总数据库
class AccountManage {
  friend AccountDatabase;

private:
  std::string file_name;
  std::fstream account_data;
  int total = 0; // 留一个int存储总本数

public:
  AccountManage() = default;

  ~AccountManage() { account_data.close(); }

  void initialize(string FN = "") {
    if (access(file_name.c_str(), F_OK) == 0) {
      // 检查文件是否存在
      return;
    }
    if (FN != "") {
      file_name = FN;
    }
    account_data.open(file_name, std::ios_base::out);
    account_data.close();
    account_data.open(file_name, std::ios_base::in | std::ios_base::out);
  }

  void Read(Account &dest, const int n) {
    account_data.seekg((n - 1) * sizeof(Account));
    account_data.read(reinterpret_cast<char *>(&dest), sizeof(Account));
  }

  void Update(Account &new_account, const int n) {
    // 更新第n本书的信息
    account_data.seekp((n - 1) * sizeof(Account));
    account_data.write(reinterpret_cast<char *>(&new_account), sizeof(Account));
  }

  int Push(Account &new_account) {
    // 在尾部写入新书
    total++;
    account_data.seekp((total - 1) * sizeof(Account));
    account_data.write(reinterpret_cast<char *>(&new_account), sizeof(Account));
    return total;
  }
};

class AccountDatabase {
private:
  AccountManage account_system;
  std::vector<int> idx;
  int cnt = 0;
  int cur_idx = 0;
  int cur_privilege = 0;

public:
  AccountDatabase() = default;

  ~AccountDatabase() = default;

  void Init() {
    // 初始化，给文件一个名字
    account_system.initialize("accountsystem");
  }

  bool Login(char id[id_len + 1], char password_[password_len + 1] = {}) {
    auto cur = new Account;
    for (int i = 0; i < account_system.total; i++) {
      account_system.Read(*cur, idx[i]);
      if (strcmp(cur->GetId().data(), id) == 0) {
        if (cur_privilege > cur->GetPrivilege()) {
          cur_privilege = cur->GetPrivilege();
          cur_idx = idx[i];
          delete cur;
          return true;
        }
        if (strcmp(password_, cur->GetPassword().data()) == 0) {
          cur_privilege = cur->GetPrivilege();
          cur_idx = idx[i];
          delete cur;
          return true;
        }
        delete cur;
        return false;
      }
    }
    delete cur;
    return false;
  }

  bool Signup(Account &x) {
    char x_id[id_len + 1];
    strcpy(x_id, x.GetId().data());
    auto cur = new Account;
    for (int i = 0; i < account_system.total; i++) {
      account_system.Read(*cur, idx[i]);
      if (strcmp(cur->GetId().data(), x_id) == 0) {
        delete cur;
        return false;
      }
    }
    cnt++;
    account_system.total++;
    account_system.Update(x, cnt);
    idx.push_back(cnt);
    delete cur;
    return true;
  }

  bool ChangePassword(char id[id_len + 1], char password[password_len + 1] = {},
                      char new_password[password_len + 1]) {
    if (cur_privilege < 1)
      return false;
    auto cur = new Account;
    for (int i = 0; i < account_system.total; i++) {
      account_system.Read(*cur, idx[i]);
      if (strcmp(cur->GetId().data(), id) == 0) {
        if (cur_privilege == 7) {
          cur->ModifyPassword(new_password);
          delete cur;
          return true;
        }
        if (strcmp(password, cur->GetPassword().data()) == 0) {
          cur->ModifyPassword(new_password);
          delete cur;
          return true;
        }
        delete cur;
        return false;
      }
    }
    delete cur;
    return false;
  }

  bool AddUser(Account &x) {
    if (cur_privilege < 3)
      return false;
    if (cur_privilege <= x.GetPrivilege())
      return false;
    char x_id[id_len + 1];
    strcpy(x_id, x.GetId().data());
    auto cur = new Account;
    for (int i = 0; i < account_system.total; i++) {
      account_system.Read(*cur, idx[i]);
      if (strcmp(cur->GetId().data(), x_id) == 0) {
        delete cur;
        return false;
      }
    }
    cnt++;
    account_system.total++;
    account_system.Update(x, cnt);
    idx.push_back(cnt);
    delete cur;
    return true;
  }

  bool Delete(char id[id_len + 1]) {
    if (cur_privilege < 7)
      return false;
    auto cur = new Account;
    for (auto it = idx.begin(); it != idx.end(); ++it) {
      account_system.Read(*cur, *it);
      if (strcmp(cur->GetId().data(), id) == 0) {
        idx.erase(it);
        account_system.total--;
        delete cur;
        return true;
      }
    }
    delete cur;
    return false;
  }
};

#endif //ACCOUNT_STORAGE_H
