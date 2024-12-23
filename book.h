#ifndef BOOK_H
#define BOOK_H
#include <cstring>
#include <string>
#include <vector>
using std::string;
constexpr int isbn_len = 20, keyword_len = 60, name_len = 60;

class Book {
private:
  char isbn[isbn_len + 1]{}, keyword[keyword_len + 1]{}, name[name_len + 1]{},
      author[name_len + 1]{};
  long long quantity = 0; // 库存数量
  double price = 0;

public:
  Book();

  Book(const string &isbn_, const string &keyword_, const string &name_,
       const string &author_, long long quantity_, double price_);

  ~Book();

  string GetIsbn() const;

  string GetName() const;

  string GetAuthor() const;

  string GetKeyword() const;

  double GetPrice() const;

  long long GetQuantity() const;

  void ModifyIsbn(const string &);

  void ModifyName(const string &);

  void ModifyAuthor(const string &);

  void ModifyKeyword(const string &);

  void ModifyPrice(double);

  bool ModifyQuantity(long long);
};

bool IsInt(const string &s);

bool IsDouble(const string &s);

bool IsIsbn(const string &s);

bool IsName(const string &s);

bool IsAuthor(const string &s);

bool IsKeyword(const string &s);

bool IsQuantity(long long x);

bool IsTotalCost(double x);

bool IsPrice(double x);

void SpiltKeyword(const string &str, std::vector<string> &list, char c = '|');

bool cmp(const Book &a, const Book &b);
#endif // BOOK_H
