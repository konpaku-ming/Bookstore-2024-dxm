#include "../include/account.h"
#include "../include/account_storage.h"
#include "../include/book.h"
#include "../include/file_storage.h"
#include "../include/log_storage.h"
#include <algorithm>
#include <cassert>
#include <set>
#include <unordered_set>
#include <valarray>
#include <vector>

BookDatabase MyBook;
AccountDatabase MyUser;
FinanceDatabase MyFinance("finance_log");
LogDatabase MyLog;
std::vector<string> user_stack;
std::vector<int> selected_book_stack;
using std::cin;
using std::vector;

void SpiltCommand(const string &str, vector<string> &list, char c = ' ') {
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

bool SpiltString(const string &str, std::pair<string, string> &p,
                 char c = '=') {
  string::size_type pos = str.find(c);
  if (pos == string::npos) {
    return false;
  }
  string s1 = str.substr(0, pos), s2 = str.substr(pos + 1);
  p = std::make_pair(s1, s2);
  return true;
}

long long StringToInt(const string &str) {
  long long num = 0;
  bool negative = false;
  int i = 0;
  if (str[0] == '-') {
    negative = true;
    i++;
  }
  for (; i < str.length(); i++) {
    num = num * 10 + (str[i] - '0');
  }
  if (negative) {
    num = -num;
  }
  return num;
}

double StringToDouble(const string &str) {
  double num = 0;
  bool negative = false;
  int i = 0, pos = -1;
  if (str[0] == '-') {
    negative = true;
    i++;
  }
  for (; i < str.length(); i++) {
    if (str[i] == '.') {
      pos = i;
      continue;
    }
    num = num * 10 + (str[i] - '0');
  }
  if (pos != -1) {
    for (int k = pos + 1; k < str.length(); k++) {
      num /= 10;
    }
  }
  if (negative) {
    num = -num;
  }
  return num;
}

int main() {
  MyBook.Init();
  MyBook.Restore();
  MyUser.Init();
  MyUser.Restore();
  MyLog.Init();
  MyLog.Restore();
  MyFinance.Init("finance_log");
  MyFinance.Restore();
  string cmd;
  std::vector<string> list;
  while (getline(cin, cmd)) {
    Log now{};
    if (cmd.length() > 1024) {
      cout << "Invalid\n";
      continue;
    }
    SpiltCommand(cmd, list);
    if (list.empty()) {
      continue;
    } else if (list[0] == "quit" || list[0] == "exit") {
      if (list.size() > 1) {
        cout << "Invalid\n";
      } else {
        break;
      }
    } else if (list[0] == "su") {
      if (!user_stack.empty()) {
        strcpy(now.id, user_stack.back().c_str());
      } else {
        strcpy(now.id, "visitor");
      }
      now.op = kSu;
      now.privilege = MyUser.cur_privilege;
      switch (list.size()) {
      case 2: {
        selected_book_stack.push_back(MyBook.selected_book_idx);
        if (MyUser.Login(list[1], "")) {
          user_stack.push_back(list[1]);
          MyLog.Push(now);
        } else {
          selected_book_stack.pop_back();
          cout << "Invalid\n";
        }
        break;
      }
      case 3: {
        selected_book_stack.push_back(MyBook.selected_book_idx);
        if (MyUser.Login(list[1], list[2])) {
          user_stack.push_back(list[1]);
          MyLog.Push(now);
        } else {
          selected_book_stack.pop_back();
          cout << "Invalid\n";
        }
        break;
      }
      default:
        cout << "Invalid\n";
      }
    } else if (list[0] == "logout") {
      if (list.size() != 1 || MyUser.cur_privilege < 1 || user_stack.empty())
        cout << "Invalid\n";
      else {
        if (!user_stack.empty()) {
          strcpy(now.id, user_stack.back().c_str());
          now.op = kLogout;
          now.privilege = MyUser.cur_privilege;
        }
        MyLog.Push(now);
        user_stack.pop_back();
        MyBook.selected_book_idx = selected_book_stack.back();
        selected_book_stack.pop_back();
        if (user_stack.empty()) {
          MyUser.cur_privilege = 0;
          MyUser.cur_idx = 0;
        } else {
          char id[id_len + 1];
          strcpy(id, user_stack.back().c_str());
          MyUser.EnforcingLogin(id);
        }
      }
    } else if (list[0] == "register") {
      if (list.size() != 4) {
        cout << "Invalid\n";
        continue;
      }
      if (!user_stack.empty()) {
        strcpy(now.id, user_stack.back().c_str());
      } else {
        strcpy(now.id, "visitor");
      }
      now.op = kRegister;
      now.privilege = MyUser.cur_privilege;
      if (IsId(list[1]) && IsPassword(list[2]) && IsUserName(list[3])) {
        auto x = new Account(list[1], list[2], list[3], 1);
        if (MyUser.Signup(*x)) {
          delete x;
          MyLog.Push(now);
          continue;
        }
        delete x;
        cout << "Invalid\n";
      } else {
        cout << "Invalid\n";
      }
    } else if (list[0] == "passwd") {
      if (!user_stack.empty()) {
        strcpy(now.id, user_stack.back().c_str());
        now.op = kSu;
        now.privilege = MyUser.cur_privilege;
      }
      char id[id_len + 1]{};
      char password[password_len + 1]{};
      char new_password[password_len + 1]{};
      switch (list.size()) {
      case 3: {
        if (!IsPassword(list[2])) {
          cout << "Invalid\n";
          break;
        }
        strcpy(id, list[1].c_str());
        strcpy(new_password, list[2].c_str());
        if (!MyUser.ChangePassword(id, "", new_password)) {
          cout << "Invalid\n";
        } else {
          MyLog.Push(now);
        }
        break;
      }
      case 4: {
        if (!IsPassword(list[3])) {
          cout << "Invalid\n";
          break;
        }
        strcpy(id, list[1].c_str());
        strcpy(password, list[2].c_str());
        strcpy(new_password, list[3].c_str());
        if (!MyUser.ChangePassword(id, password, new_password)) {
          cout << "Invalid\n";
        } else {
          MyLog.Push(now);
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
      if (!IsInt(list[3])) {
        cout << "Invalid\n";
        continue;
      }
      if (!user_stack.empty()) {
        strcpy(now.id, user_stack.back().c_str());
        now.op = kUseradd;
        now.privilege = MyUser.cur_privilege;
      }
      const int privilege_ = StringToInt(list[3]);
      if (IsId(list[1]) && IsPassword(list[2]) && IsUserName(list[4]) &&
          IsPrivilege(privilege_)) {
        auto x = new Account(list[1], list[2], list[4], privilege_);
        if (MyUser.AddUser(*x)) {
          delete x;
          MyLog.Push(now);
          continue;
        }
        delete x;
        cout << "Invalid\n";
      } else {
        cout << "Invalid\n";
      }
    } else if (list[0] == "delete") {
      if (!IsId(list[1])) {
        cout << "Invalid\n";
        continue;
      }
      if (find(user_stack.begin(), user_stack.end(), list[1]) !=
          user_stack.end()) {
        cout << "Invalid\n";
        continue;
      }
      if (!user_stack.empty()) {
        strcpy(now.id, user_stack.back().c_str());
        now.op = kDelete;
        now.privilege = MyUser.cur_privilege;
      }
      char id_[id_len + 1]{};
      strcpy(id_, list[1].c_str());
      if (list.size() != 2) {
        cout << "Invalid\n";
        continue;
      }
      if (!MyUser.Delete(id_)) {
        cout << "Invalid\n";
      } else {
        MyLog.Push(now);
      }
    } else if (list[0] == "show") { // 这里注意可能有两种命令
      if (list.size() > 1 && list[1] == "finance") {
        if (MyUser.cur_privilege < 7) {
          cout << "Invalid\n";
          continue;
        }
        switch (list.size()) {
        case 2: {
          MyFinance.FinanceShow(MyFinance.info_len);
          break;
        }
        case 3: {
          if (!IsInt(list[2])) {
            cout << "Invalid\n";
            break;
          }
          const long long count = StringToInt(list[2]);
          if (!IsQuantity(count)) {
            cout << "Invalid\n";
            break;
          }
          if (!MyFinance.FinanceShow(count)) {
            cout << "Invalid\n";
          }
          break;
        }
        default:
          cout << "Invalid\n";
        }
      } else {
        if (MyUser.cur_privilege < 1) {
          cout << "Invalid\n";
          continue;
        }
        if (!user_stack.empty()) {
          strcpy(now.id, user_stack.back().c_str());
          now.op = kShow;
          now.privilege = MyUser.cur_privilege;
        }
        switch (list.size()) {
        case 1: {
          MyBook.AllShow();
          MyLog.Push(now);
          break;
        }
        case 2: {
          std::pair<string, string> p;
          if (!SpiltString(list[1], p)) {
            cout << "Invalid\n";
            break;
          }
          string &s = p.second;
          if (p.first == "-ISBN") {
            if (!IsIsbn(s)) {
              cout << "Invalid\n";
            } else {
              MyBook.IsbnShow(s);
              MyLog.Push(now);
            }
          } else if (p.first == "-name") {
            if (s.length() <= 2 || s.front() != '\"' || s.back() != '\"') {
              cout << "Invalid\n";
              break;
            }
            s = s.substr(1, s.length() - 2);
            if (!IsName(s)) {
              cout << "Invalid\n";
              break;
            }
            MyBook.NameShow(s);
            MyLog.Push(now);
          } else if (p.first == "-author") {
            if (s.length() <= 2 || s.front() != '\"' || s.back() != '\"') {
              cout << "Invalid\n";
              break;
            }
            s = s.substr(1, s.length() - 2);
            if (!IsAuthor(s)) {
              cout << "Invalid\n";
              break;
            }
            char author[name_len + 1]{};
            strcpy(author, s.data());
            MyBook.AuthorShow(author);
            MyLog.Push(now);
          } else if (p.first == "-keyword") {
            if (s.length() <= 2 || s.front() != '\"' || s.back() != '\"') {
              cout << "Invalid\n";
              break;
            }
            s = s.substr(1, s.length() - 2);
            if (!IsKeyword(s)) {
              cout << "Invalid\n";
              break;
            }
            MyBook.KeywordShow(s);
            MyLog.Push(now);
          } else {
            cout << "Invalid\n";
          }
          break;
        }
        default:
          cout << "Invalid\n";
        }
      }
    } else if (list[0] == "buy") {
      if (MyUser.cur_privilege < 1 || list.size() != 3) {
        cout << "Invalid\n";
        continue;
      }
      if (!IsInt(list[2])) {
        cout << "Invalid\n";
        continue;
      }
      if (!user_stack.empty()) {
        strcpy(now.id, user_stack.back().c_str());
        now.op = kBuy;
        now.privilege = MyUser.cur_privilege;
      }
      const long long quantity = StringToInt(list[2]);
      if (IsIsbn(list[1]) && IsQuantity(quantity)) {
        const double income = MyBook.Buy(list[1], quantity);
        if (income == -1) {
          cout << "Invalid\n";
        } else {
          now.finance = income;
          MyLog.Push(now);
          cout << std::fixed << std::setprecision(2) << income << '\n';
          MyFinance.Write(income);
        }
      } else {
        cout << "Invalid\n";
      }
    } else if (list[0] == "select") {
      if (MyUser.cur_privilege < 3 || list.size() != 2) {
        cout << "Invalid\n";
        continue;
      }
      if (!user_stack.empty()) {
        strcpy(now.id, user_stack.back().c_str());
        now.op = kSelect;
        now.privilege = MyUser.cur_privilege;
      }
      if (IsIsbn(list[1])) {
        MyBook.Select(list[1]);
        MyLog.Push(now);
      } else {
        cout << "Invalid\n";
      }
    } else if (list[0] == "modify") {
      if (MyUser.cur_privilege < 3 || list.size() == 1 || list.size() > 6 ||
          MyBook.selected_book_idx == 0) {
        cout << "Invalid\n";
        continue;
      }
      bool isvalid = true;
      bool is_isbn = false, is_name = false, is_author = false,
           is_keyword = false, is_price = false;
      std::pair<string, string> p[list.size()];
      for (int i = 1; i < list.size(); i++) {
        if (!SpiltString(list[i], p[i])) {
          cout << "Invalid\n";
          isvalid = false;
          break;
        }
        if (p[i].first == "-ISBN") {
          if (is_isbn || !IsIsbn(p[i].second)) {
            cout << "Invalid\n";
            isvalid = false;
            break;
          }
          is_isbn = true;
        } else if (p[i].first == "-name") {
          if (p[i].second.length() < 2 || p[i].second.front() != '\"' ||
              p[i].second.back() != '\"') {
            cout << "Invalid\n";
            isvalid = false;
            break;
          }
          p[i].second = p[i].second.substr(1, p[i].second.length() - 2);
          if (is_name || !IsName(p[i].second)) {
            cout << "Invalid\n";
            isvalid = false;
            break;
          }
          is_name = true;
        } else if (p[i].first == "-author") {
          if (p[i].second.length() < 2 || p[i].second.front() != '\"' ||
              p[i].second.back() != '\"') {
            cout << "Invalid\n";
            isvalid = false;
            break;
          }
          p[i].second = p[i].second.substr(1, p[i].second.length() - 2);
          if (is_author || !IsAuthor(p[i].second)) {
            cout << "Invalid\n";
            isvalid = false;
            break;
          }
          is_author = true;
        } else if (p[i].first == "-keyword") {
          if (p[i].second.length() < 2 || p[i].second.front() != '\"' ||
              p[i].second.back() != '\"') {
            cout << "Invalid\n";
            isvalid = false;
            break;
          }
          p[i].second = p[i].second.substr(1, p[i].second.length() - 2);
          if (is_keyword || p[i].second.length() > keyword_len) {
            cout << "Invalid\n";
            isvalid = false;
            break;
          }
          is_keyword = true;
          std::vector<string> keywords;
          SpiltKeyword(p[i].second, keywords);
          std::unordered_set<string> copy;
          for (const auto &word : keywords) {
            if (!IsKeyword(word)) {
              cout << "Invalid\n";
              isvalid = false;
              break;
            }
            copy.insert(word);
          }
          if (!isvalid)
            break;
          if (keywords.size() != copy.size()) {
            cout << "Invalid\n";
            isvalid = false;
            break;
          }
        } else if (p[i].first == "-price") {
          if (is_price || !IsPrice(StringToDouble(p[i].second))) {
            cout << "Invalid\n";
            isvalid = false;
            break;
          }
          is_price = true;
        } else {
          cout << "Invalid\n";
          isvalid = false;
          break;
        }
      }
      if (!isvalid)
        continue;
      if (is_isbn) {
        for (int i = 1; i < list.size(); i++) {
          if (p[i].first == "-ISBN") {
            if (!MyBook.IsbnModify(p[i].second)) {
              cout << "Invalid\n";
              isvalid = false;
              break;
            }
          }
        }
      }
      if (!isvalid)
        continue;
      if (!user_stack.empty()) {
        strcpy(now.id, user_stack.back().c_str());
        now.op = kModify;
        now.privilege = MyUser.cur_privilege;
      }
      MyLog.Push(now);
      for (int i = 1; i < list.size(); i++) {
        if (p[i].first == "-name") {
          MyBook.NameModify(p[i].second);
        }
        if (p[i].first == "-author") {
          MyBook.AuthorModify(p[i].second);
        }
        if (p[i].first == "-keyword") {
          MyBook.KeywordModify(p[i].second);
        }
        if (p[i].first == "-price") {
          MyBook.PriceModify(StringToDouble(p[i].second));
        }
      }
    } else if (list[0] == "import") {
      if (MyUser.cur_privilege < 3 || list.size() != 3) {
        cout << "Invalid\n";
        continue;
      }
      if (!IsInt(list[1]) || !IsDouble(list[2])) {
        cout << "Invalid\n";
        continue;
      }
      if (!user_stack.empty()) {
        strcpy(now.id, user_stack.back().c_str());
        now.op = kImport;
        now.privilege = MyUser.cur_privilege;
      }
      const long long x = StringToInt(list[1]);
      const double cost = StringToDouble(list[2]);
      if (!IsQuantity(x) || !IsTotalCost(cost)) {
        cout << "Invalid\n";
        continue;
      }
      if (!MyBook.Import(x, cost)) {
        cout << "Invalid\n";
        continue;
      }
      now.finance = -cost;
      MyFinance.Write(-cost);
      MyLog.Push(now);
    } else if (list[0] == "report") {
      if (list.size() != 2 || MyUser.cur_privilege != 7) {
        cout << "Invalid\n";
        continue;
      }
      if (list[1] == "finance") {
        MyLog.ReportFinance();
      } else if (list[1] == "employee") {
        MyLog.ReportEmployee();
      } else {
        cout << "Invalid\n";
      }
    } else if (list[0] == "log") {
      if (list.size() != 1 || MyUser.cur_privilege != 7) {
        cout << "Invalid\n";
        continue;
      }
      MyLog.ReportLog();
    } else {
      cout << "Invalid\n";
    }
  }
  MyUser.Save();
  MyBook.Save();
  MyFinance.Save();
  MyLog.Save();
  return 0;
}