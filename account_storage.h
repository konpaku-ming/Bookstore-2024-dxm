#ifndef ACCOUNT_STORAGE_H
#define ACCOUNT_STORAGE_H

#include "account.h"
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>
class AccountDatabase;
using std::unordered_map;

// 图书的总数据库
class AccountManage {
  friend AccountDatabase;

private:
  std::string file_name;
  std::fstream account_data;
  int total = 0; // 留一个int存储用户总数

public:
  AccountManage() = default;

  ~AccountManage() { account_data.close(); }

  void initialize(string FN = "") {
    /*
    if (access(file_name.c_str(), F_OK) == 0) {
      // 检查文件是否存在
      return;
    }
    */
    if (FN != "") {
      file_name = FN;
    }
    account_data.open(file_name, std::ios_base::out);
    account_data.close();
    account_data.open(file_name, std::ios_base::in | std::ios_base::out);
    Account administer("root", "sjtu", "root", 7);
    Push(administer);
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

struct HashPair {
  char id[id_len + 1];
  int value;
};

class AccountDatabase {
private:
  AccountManage account_system;
  std::fstream back_up;
  unordered_map<string, int> hash_map;
  // std::vector<int> idx = {1};
  int cnt = 1;
  int total = 1;

public:
  int cur_privilege = 0;
  int cur_idx = 0;

  AccountDatabase() = default;

  ~AccountDatabase() = default;

  void Init() {
    // 初始化，给文件一个名字
    account_system.initialize("accountsystem");
  }

  void Restore() {
    // 把一部分外存信息写回内存
    if (access("backup", F_OK) != 0) {
      // 检查文件是否存在
      back_up.open("backup", std::ios::out);
      back_up.close();
      hash_map["root"] = cnt;
      return;
    }
    back_up.open("backup", std::ios::in | std::ios::out);
    back_up.seekg(0);
    back_up.read(reinterpret_cast<char *>(&cnt), sizeof(int));
    back_up.seekg(sizeof(int));
    back_up.read(reinterpret_cast<char *>(&total), sizeof(int));
    HashPair temp{};
    for (int i = 0; i < total; i++) {
      back_up.seekg(2 * sizeof(int) + i * sizeof(HashPair));
      back_up.read(reinterpret_cast<char *>(&temp), sizeof(HashPair));
      hash_map[temp.id] = temp.value;
    }
    back_up.close();
  }

  void Save() {
    // 把内存信息转到文件里
    total = hash_map.size();
    back_up.open("backup", std::ios::in | std::ios::out);
    back_up.seekp(0);
    back_up.write(reinterpret_cast<char *>(&cnt), sizeof(int));
    back_up.seekp(sizeof(int));
    back_up.write(reinterpret_cast<char *>(&total), sizeof(int));
    HashPair temp{};
    int i = 0;
    for (const auto &it : hash_map) {
      strcpy(temp.id, it.first.c_str());
      temp.value = it.second;
      back_up.seekp(2 * sizeof(int) + i * sizeof(HashPair));
      back_up.write(reinterpret_cast<char *>(&temp), sizeof(HashPair));
      i++;
    }
    back_up.close();
    hash_map.clear();
  }

  bool Login(const char id[id_len + 1],
             const char password_[password_len + 1]) {
    if (hash_map.find(id) == hash_map.end()) {
      return false;
    }
    int number = hash_map[id];
    auto cur = new Account;
    account_system.Read(*cur, number);
    if (cur_privilege > cur->GetPrivilege() && password_ == "") {
      cur_privilege = cur->GetPrivilege();
      cur_idx = number;
      delete cur;
      return true;
    }
    if (strcmp(password_, cur->GetPassword().data()) == 0) {
      cur_privilege = cur->GetPrivilege();
      cur_idx = number;
      delete cur;
      return true;
    }
    delete cur;
    return false;
  }

  void EnforcingLogin(const char id[id_len + 1]) {
    int number = hash_map[id];
    auto cur = new Account;
    account_system.Read(*cur, number);
    cur_privilege = cur->GetPrivilege();
    cur_idx = number;
    delete cur;
  }

  bool Signup(Account &x) {
    char x_id[id_len + 1];
    strcpy(x_id, x.GetId().data());
    if (hash_map.find(x_id) != hash_map.end()) {
      return false;
    }
    cnt++;
    account_system.total++;
    account_system.Update(x, cnt);
    hash_map[x_id] = cnt;
    return true;
  }

  bool ChangePassword(const char id[id_len + 1],
                      const char password[password_len + 1],
                      char new_password[password_len + 1]) {
    if (cur_privilege < 1) {
      return false;
    }
    if (hash_map.find(id) == hash_map.end()) {
      return false;
    }
    int number = hash_map[id];
    auto cur = new Account;
    account_system.Read(*cur, number);
    if (cur_privilege == 7 && password == "") {
      cur->ModifyPassword(new_password);
      account_system.Update(*cur, number);
      delete cur;
      return true;
    }
    if (strcmp(password, cur->GetPassword().data()) == 0) {
      cur->ModifyPassword(new_password);
      account_system.Update(*cur, number);
      delete cur;
      return true;
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
    if (hash_map.find(x_id) != hash_map.end()) {
      return false;
    }
    cnt++;
    account_system.total++;
    account_system.Update(x, cnt);
    hash_map[x_id] = cnt;
    return true;
  }

  bool Delete(const char id[id_len + 1]) {
    if (cur_privilege < 7)
      return false;
    auto it = hash_map.find(id);
    if (it == hash_map.end()) {
      return false;
    }
    hash_map.erase(it);
    return true;
  }
};

#endif //ACCOUNT_STORAGE_H
