#include "account.h"

#include <cstring>
bool IsChar(const char ch) {
  // 数字，字母，下划线
  if (ch == '_' || (ch > 47 && ch < 58) || (ch > 64 && ch < 91) ||
      (ch > 96 && ch < 123)) {
    return true;
  }
  return false;
}

bool IsId(const string &s) { // 检查是否为合法的Id
  if (s.empty() || s.length() > id_len) {
    return false;
  }
  for (int i = 0; i < s.length(); i++) {
    if (!IsChar(s[i]))
      return false;
  }
  return true;
}

bool IsName(const string &s) { // 检查是否为合法的用户名
  if (s.empty() || s.length() > name_len) {
    return false;
  }
  for (int i = 0; i < s.length(); i++) {
    if (!isprint(s[i]))
      return false;
  }
  return true;
}

bool IsPassword(const string &s) { // 检查是否为合法的密码
  if (s.empty() || s.length() > password_len) {
    return false;
  }
  for (int i = 0; i < s.length(); i++) {
    if (!IsChar(s[i]))
      return false;
  }
  return true;
}

bool IsPrivilege(const int x) {
  if (x == 1 || x == 3 || x == 7) {
    return true;
  }
  return false;
}

Account::Account() = default;

Account::Account(const string &id_, const string &password_,
                 const string &name_, const int privilege_) {
  strcpy(id, id_.data());
  strcpy(password, password_.data());
  strcpy(name, name_.data());
  privilege = privilege_;
}

Account::~Account() = default;

string Account::GetPassword() const { return password; }

string Account::GetId() const { return id; }

string Account::GetName() const { return name; }

int Account::GetPrivilege() const { return privilege; }