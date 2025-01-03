#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <cstring>
#include <string>
using std::string;
constexpr int id_len = 30, password_len = 30, username_len = 30;

class Account {
private:
  char id[id_len + 1]{}, password[password_len + 1]{}, name[username_len + 1]{};
  int privilege = 0;

public:
  Account();

  Account(const string &id_, const string &password_, const string &name_,
          int privilege_);

  ~Account();

  string GetPassword() const;

  string GetId() const;

  string GetName() const;

  int GetPrivilege() const;

  void ModifyPassword(char new_password[password_len + 1]);
};

bool IsChar(char ch);

bool IsId(const string &s);

bool IsUserName(const string &s);

bool IsPassword(const string &s);

bool IsPrivilege(int x);

#endif // ACCOUNT_H
