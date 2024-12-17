#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <string>
using std::string;
constexpr int id_len = 30, password_len = 30, name_len = 30;

class Account {
private:
  char id[id_len + 1]{}, password[password_len + 1]{}, name[name_len + 1]{};
  int privilege = 0;
  Account();
  Account(const string &id_, const string &password_, const string &name_,
          int privilege_);
  ~Account();

public:
  string GetPassword() const;
  string GetId() const;
  string GetName() const;
  int GetPrivilege() const;
};
#endif // ACCOUNT_H
