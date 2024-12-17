#ifndef BOOK_H
#define BOOK_H
#include <cstring>
#include <string>
using std::string;
constexpr int isbn_len = 20, keyword_len = 60, name_len = 60;
class Book {
private:
  char isbn[isbn_len + 1]{}, keyword[keyword_len + 1]{}, name[name_len + 1]{},
      author[name_len + 1]{};
  long long quantity = 0; // 库存数量
  double price = 0;
  double total_cost = 0;

public:
  Book();

  Book(const string &isbn_, const string &keyword_ = "",
       const string &name_ = "", const string &author_ = "",
       long long quantity_, double price_);

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

  void ModifyQuantity(long long);

  bool operator>(const Book &a, const Book &b) const {
    return strcmp(a.isbn, b.isbn) > 0;
  }

  bool operator<(const Book &a, const Book &b) const {
    return strcmp(a.isbn, b.isbn) < 0;
  }

  bool operator==(const Book &a, const Book &b) const {
    return strcmp(a.isbn, b.isbn) == 0;
  }

  bool KeywordJudge(const string &) const;
};
#endif // BOOK_H
