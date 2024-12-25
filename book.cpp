#include "book.h"

#include <cstring>
bool IsInt(const string &s) {
  if (s.empty() || s.length() > 10) {
    return false;
  }
  for (int i = 0; i < s.length(); i++) {
    if (!isdigit(s[i]))
      return false;
  }
  return true;
}

bool IsDouble(const string &s) {
  if (s.empty() || s.length() > 13) {
    return false;
  }
  if (s.front() == '.') {
    return false;
  }
  bool is_point = false;
  for (int i = 0; i < s.length(); i++) {
    if (!isdigit(s[i])) {
      if (s[i] == '.' && !is_point) {
        is_point = true;
      } else
        return false;
    }
  }
  return true;
}

bool IsIsbn(const string &s) {
  // 检查是否为合法的ISBN
  if (s.empty() || s.length() > isbn_len) {
    return false;
  }
  for (int i = 0; i < s.length(); i++) {
    if (!isprint(s[i]))
      return false;
  }
  return true;
}

bool IsName(const string &s) {
  // 检查是否为合法的书名
  if (s.empty() || s.length() > name_len) {
    return false;
  }
  for (int i = 0; i < s.length(); i++) {
    if (!isprint(s[i]) || s[i] == '\"')
      return false;
  }
  return true;
}

bool IsAuthor(const string &s) {
  // 检查是否为合法的作者名
  if (s.empty() || s.length() > name_len) {
    return false;
  }
  for (int i = 0; i < s.length(); i++) {
    if (!isprint(s[i]) || s[i] == '\"')
      return false;
  }
  return true;
}

bool IsKeyword(const string &s) {
  // 检查是否为一个关键词
  if (s.empty() || s.length() > keyword_len) {
    return false;
  }
  for (int i = 0; i < s.length(); i++)
    if (s[i] == '|' || !isprint(s[i]) || s[i] == '\"')
      return false;
  return true;
}

bool IsQuantity(const long long x) {
  // 检查是否为合法的Quantity
  if (x <= 0) {
    return false;
  }
  return true;
}

bool IsTotalCost(const double x) {
  // 检查是否为合法的total_cost
  if (x <= 0) {
    return false;
  }
  return true;
}

bool IsPrice(const double x) {
  // 检查是否为合法的price
  if (x < 0) {
    return false;
  }
  return true;
}

Book::Book() = default;

Book::Book(const string &isbn_, const string &keyword_, const string &name_,
           const string &author_, const long long quantity_,
           const double price_) {
  // 在添加图书时就判断参数是否合法，此处传入的均为合法参数
  strcpy(isbn, isbn_.data());
  strcpy(keyword, keyword_.data());
  strcpy(name, name_.data());
  strcpy(author, author_.data());
  quantity = quantity_;
  price = price_;
}

Book::~Book() = default;

string Book::GetIsbn() const { return isbn; }

string Book::GetName() const { return name; }

string Book::GetAuthor() const { return author; }

string Book::GetKeyword() const { return keyword; }

double Book::GetPrice() const { return price; }

long long Book::GetQuantity() const { return quantity; }

void Book::ModifyIsbn(const string &s) {
  memset(isbn, 0, sizeof(isbn));
  strcpy(isbn, s.data());
}

void Book::ModifyName(const string &s) {
  memset(name, 0, sizeof(name));
  strcpy(name, s.data());
}

void Book::ModifyAuthor(const string &s) {
  memset(author, 0, sizeof(author));
  strcpy(author, s.data());
}

void Book::ModifyKeyword(const string &s) {
  memset(keyword, 0, sizeof(keyword));
  strcpy(keyword, s.data());
}

void Book::ModifyPrice(const double x) { price = x; }

bool Book::ModifyQuantity(const long long x) {
  if (quantity < x) {
    return false;
  }
  quantity -= x;
  return true;
}

void SpiltKeyword(const string &str, std::vector<string> &list, char c) {
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

bool cmp(const Book &a, const Book &b) { return a.GetIsbn() < b.GetIsbn(); }