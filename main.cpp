#include "account.h"
#include "account_storage.h"
#include "book.h"
#include "file_storage.h"
#include "log_storage.h"
#include <algorithm>
#include <cassert>
#include <vector>

BookDatabase MyBook;
AccountDatabase MyUser;
FinanceDatabase MyFinance("finance_log");
std::vector<string> user_stack;
using std::cin;
using std::vector;

void SpiltString(const string &str, vector<string> &list, char c = ' ') {
  list.clear();
  if (c == ' ' && str.empty())
    return;
  string::size_type last_pos = 0, pos;
  do {
    pos = str.find(c, last_pos);
    if (pos > last_pos || c != ' ')
      list.push_back(str.substr(last_pos, pos - last_pos));
    last_pos = pos + 1;
  } while (last_pos < str.length() && pos != string::npos);
  if (c != ' ' && str.back() == c) {
    list.push_back(string());
  }
}
// 用于分割命令参数

int StringToInt(const string &str) {
  int num = 0;
  bool negative = false;
  int i = 0;
  if (str[0] == '-') {
    negative = true;
    i++;
  }
  for (; i < str.length(); i++) {
    num = num * 10 + (str[i] - '0');
  }
  return negative ? -num : num;
}

int main() {
  MyBook.Init();
  MyUser.Init();
  MyUser.Restore();
  MyFinance.initialise();
  string cmd;
  std::vector<string> list;
  while (true) {
    getline(cin, cmd);
    SpiltString(cmd, list);
    if (list.empty())
      continue;
    if (list[0] == "quit" || list[0] == "exit") {
      if (list.size() > 1) {
        cout << "Invalid\n";
      } else {
        MyUser.Save();
        exit(0);
      }
    } else if (list[0] == "su") {
      char id[id_len + 1]{};
      char password[password_len + 1]{};
      switch (list.size()) {
      case 2: {
        strcpy(id, list[1].data());
        if (MyUser.Login(id, ""))
          user_stack.push_back(list[1]);
        else
          cout << "Invalid\n";
        break;
      }
      case 3: {
        strcpy(id, list[1].data());
        strcpy(password, list[2].data());
        if (MyUser.Login(id, password))
          user_stack.push_back(list[1]);
        else
          cout << "Invalid\n";
        break;
      }
      default:
        cout << "Invalid\n";
      }
      if (user_stack.empty()) {
        cout << "no login"
             << "\n"; // TODO delete this
        continue;
      }
      cout << user_stack.back() << "\n"; // TODO delete this
    } else if (list[0] == "logout") {
      if (list.size() != 1 || MyUser.cur_privilege < 1)
        cout << "Invalid\n";
      else {
        user_stack.pop_back();
        if (user_stack.empty()) {
          MyUser.cur_privilege = 0;
          MyUser.cur_idx = 0;
        } else {
          char id[id_len + 1];
          strcpy(id, user_stack.back().c_str());
          MyUser.EnforcingLogin(id);
        }
        if (user_stack.empty()) {
          cout << "no login"
               << "\n"; // TODO delete this
          continue;
        }
        cout << user_stack.back() << "\n"; // TODO delete this
      }
    } else if (list[0] == "register") {
      if (list.size() != 4) {
        cout << "Invalid\n";
        continue;
      }
      if (IsId(list[1]) && IsPassword(list[2]) && IsUserName(list[3])) {
        auto x = new Account(list[1], list[2], list[3], 1);
        if (MyUser.Signup(*x)) {
          delete x;
          continue;
        }
        delete x;
        cout << "Invalid\n";
      } else {
        cout << "Invalid\n";
      }
    } else if (list[0] == "passwd") {
      char id[id_len + 1]{};
      char password[password_len + 1]{};
      char new_password[password_len + 1]{};
      switch (list.size()) {
      case 3: {
        strcpy(id, list[1].c_str());
        strcpy(new_password, list[2].c_str());
        if (!MyUser.ChangePassword(id, "", new_password)) {
          cout << "Invalid\n";
        }
        break;
      }
      case 4: {
        strcpy(id, list[1].c_str());
        strcpy(password, list[2].c_str());
        strcpy(new_password, list[3].c_str());
        if (!MyUser.ChangePassword(id, password, new_password)) {
          cout << "Invalid\n";
        }
        break;
      }
      default:
        cout << "Invalid\n";
      }
    } else if (list[0] == "useradd") {
      if (list.size() != 5) {
        cout << "Invalid\n";
        continue;
      }
      const int privilege_ = StringToInt(list[3]);
      if (IsId(list[1]) && IsPassword(list[2]) && IsUserName(list[4]) &&
          IsPrivilege(privilege_)) {
        auto x = new Account(list[1], list[2], list[4], privilege_);
        if (MyUser.AddUser(*x)) {
          delete x;
          continue;
        }
        delete x;
        cout << "Invalid\n";
      } else {
        cout << "Invalid\n";
      }
    } else if (list[0] == "delete") {
      if (find(user_stack.begin(), user_stack.end(), list[1]) !=
          user_stack.end()) {
        cout << "Invalid\n";
        continue;
      }
      char id_[id_len + 1]{};
      strcpy(id_, list[1].c_str());
      if (list.size() != 2) {
        cout << "Invalid\n";
        continue;
      }
      if (!MyUser.Delete(id_)) {
        cout << "Invalid\n";
      }
    }
  }
  return 0;
}