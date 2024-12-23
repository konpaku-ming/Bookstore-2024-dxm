#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include "book.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unistd.h>
using std::cout;
using std::string;
using std::vector;
constexpr int max_size = 500;
class BookDatabase;

// 图书的总数据库
class BookManage {
  friend BookDatabase;

private:
  std::string file_name;
  std::fstream book_data;
  int total = 0; // 留一个int存储总本数

public:
  BookManage() = default;

  ~BookManage() { book_data.close(); }

  void initialize(string FN = "") {
    if (access(file_name.c_str(), F_OK) == 0) {
      // 检查文件是否存在
      book_data.seekg(0);
      book_data.read(reinterpret_cast<char *>(&total), sizeof(int));
      return;
    }
    if (FN != "") {
      file_name = FN;
    }
    book_data.open(file_name, std::ios_base::out);
    book_data.close();
    book_data.open(file_name, std::ios_base::in | std::ios_base::out);
  }

  void Read(Book &dest, const int n) {
    book_data.seekg((n - 1) * sizeof(Book) + sizeof(int));
    book_data.read(reinterpret_cast<char *>(&dest), sizeof(Book));
  }

  void Update(Book &new_book, const int n) {
    // 更新第n本书的信息
    book_data.seekp((n - 1) * sizeof(Book) + sizeof(int));
    book_data.write(reinterpret_cast<char *>(&new_book), sizeof(Book));
  }

  int Push(Book &new_book) {
    // 在尾部写入新书
    total++;
    book_data.seekp((total - 1) * sizeof(Book) + sizeof(int));
    book_data.write(reinterpret_cast<char *>(&new_book), sizeof(Book));
    return total;
  }
};

class BookDatabase {
private:
  BookManage book_system;
  unordered_map<string, int> isbn_map{};
  unordered_map<string, vector<int>> name_map{};
  unordered_map<string, vector<int>> author_map{};
  unordered_map<string, vector<int>> keyword_map{};

public:
  int selected_book_idx = 0;

  BookDatabase() = default;

  ~BookDatabase() = default;

  void Init() {
    // 初始化，给文件一个名字
    book_system.initialize("booksystem");
  }

  void Restore() {
    // 把一部分外存信息写回内存
    if (access("all_book", F_OK) != 0) {
      // 检查文件是否存在
      return;
    }
    auto now = new Book;
    for (int i = 1; i <= book_system.total; i++) {
      book_system.Read(*now, i);
      isbn_map[now->GetIsbn()] = i;
      name_map[now->GetName()].push_back(i);
      author_map[now->GetAuthor()].push_back(i);
      std::vector<string> keywords;
      SpiltKeyword(now->GetKeyword(), keywords);
      for (int j = 0; i < keywords.size(); j++) {
        keyword_map[keywords[i]].push_back(i);
      }
    }
    delete now;
  }

  void Save() {
    book_system.book_data.seekp(0);
    book_system.book_data.write(reinterpret_cast<char *>(&book_system.total),
                                sizeof(int));
  }

  void AllShow() {
    if (book_system.total == 0) {
      cout << "\n";
      return;
    }
    const int size_ = book_system.total;
    Book temp[size_];
    for (int i = 1; i <= size_; i++) {
      book_system.Read(temp[i - 1], i);
    }
    std::sort(temp, temp + size_, cmp);
    for (int i = 0; i < size_; i++) {
      cout << temp[i].GetIsbn() << "\t" << temp[i].GetName() << "\t"
           << temp[i].GetAuthor() << "\t" << temp[i].GetKeyword() << "\t"
           << std::fixed << std::setprecision(2) << temp[i].GetPrice() << "\t"
           << temp[i].GetQuantity() << "\n";
    }
  }

  void IsbnShow(const string &isbn) {
    if (isbn_map.find(isbn) == isbn_map.end()) {
      cout << "\n";
      return;
    }
    const int idx = isbn_map[isbn];
    auto temp = new Book;
    book_system.Read(*temp, idx);
    cout << temp->GetIsbn() << "\t" << temp->GetName() << "\t"
         << temp->GetAuthor() << "\t" << temp->GetKeyword() << "\t"
         << std::fixed << std::setprecision(2) << temp->GetPrice() << "\t"
         << temp->GetQuantity() << "\n";
    delete temp;
  }

  void NameShow(const string &name) {
    if (name_map.find(name) == name_map.end()) {
      cout << "\n";
      return;
    }
    if (name_map[name].empty()) {
      cout << "\n";
      return;
    }
    const int size_ = name_map[name].size();
    Book temp[size_];
    for (int i = 0; i < size_; i++) {
      book_system.Read(temp[i], name_map[name][i]);
    }
    std::sort(temp, temp + size_, cmp);
    for (int i = 0; i < size_; i++) {
      cout << temp[i].GetIsbn() << "\t" << temp[i].GetName() << "\t"
           << temp[i].GetAuthor() << "\t" << temp[i].GetKeyword() << "\t"
           << std::fixed << std::setprecision(2) << temp[i].GetPrice() << "\t"
           << temp[i].GetQuantity() << "\n";
    }
  }

  void AuthorShow(const string &author) {
    if (author_map.find(author) == author_map.end()) {
      cout << "\n";
      return;
    }
    if (author_map[author].empty()) {
      cout << "\n";
      return;
    }
    const int size_ = author_map[author].size();
    Book temp[size_];
    for (int i = 0; i < size_; i++) {
      book_system.Read(temp[i], author_map[author][i]);
    }
    std::sort(temp, temp + size_, cmp);
    for (int i = 0; i < size_; i++) {
      cout << temp[i].GetIsbn() << "\t" << temp[i].GetName() << "\t"
           << temp[i].GetAuthor() << "\t" << temp[i].GetKeyword() << "\t"
           << std::fixed << std::setprecision(2) << temp[i].GetPrice() << "\t"
           << temp[i].GetQuantity() << "\n";
    }
  }

  void KeywordShow(const string &keyword) {
    if (keyword_map.find(keyword) == keyword_map.end()) {
      cout << "\n";
      return;
    }
    if (keyword_map[keyword].empty()) {
      cout << "\n";
      return;
    }
    const int size_ = keyword_map[keyword].size();
    Book temp[size_];
    for (int i = 0; i < size_; i++) {
      book_system.Read(temp[i], keyword_map[keyword][i]);
    }
    std::sort(temp, temp + size_, cmp);
    for (int i = 0; i < size_; i++) {
      cout << temp[i].GetIsbn() << "\t" << temp[i].GetName() << "\t"
           << temp[i].GetAuthor() << "\t" << temp[i].GetKeyword() << "\t"
           << std::fixed << std::setprecision(2) << temp[i].GetPrice() << "\t"
           << temp[i].GetQuantity() << "\n";
    }
  }

  double Buy(const string &isbn, const long long x) {
    if (isbn_map.find(isbn) == isbn_map.end()) {
      return -1;
    }
    const int idx = isbn_map[isbn];
    auto temp = new Book;
    book_system.Read(*temp, idx);
    const bool flag = temp->ModifyQuantity(x);
    if (flag) {
      book_system.Update(*temp, idx);
      delete temp;
      return temp->GetPrice() * x;
    }
    delete temp;
    return -1;
  }

  void Select(const string &isbn) {
    if (isbn_map.find(isbn) == isbn_map.end()) {
      Book new_book = {isbn, "", "", "", 0, 0};
      selected_book_idx = book_system.Push(new_book);
      isbn_map[isbn] = selected_book_idx;
      return;
    }
    selected_book_idx = isbn_map[isbn];
  }

  bool Import(const long long x, double cost) {
    if (selected_book_idx == 0) {
      return false;
    }
    auto temp = new Book;
    book_system.Read(*temp, selected_book_idx);
    temp->ModifyQuantity(-x);
    book_system.Update(*temp, selected_book_idx);
    return true;
  }

  bool IsbnModify(const string &isbn) {
    auto temp = new Book;
    book_system.Read(*temp, selected_book_idx);
    const string pre_isbn = temp->GetIsbn();
    if (isbn_map.find(isbn) != isbn_map.end()) {
      delete temp;
      return false;
    }
    temp->ModifyIsbn(isbn);
    auto it = isbn_map.find(pre_isbn);
    isbn_map.erase(it);
    isbn_map[isbn] = selected_book_idx;
    book_system.Update(*temp, selected_book_idx);
    delete temp;
    return true;
  }

  void NameModify(const string &name) {
    auto temp = new Book;
    book_system.Read(*temp, selected_book_idx);
    const string s = temp->GetName();
    if (!s.empty()) {
      auto v = name_map.find(s)->second;
      auto iter = std::remove(v.begin(), v.end(), selected_book_idx);
      v.erase(iter, v.end());
    }
    name_map[name].push_back(selected_book_idx);
    temp->ModifyName(name);
    book_system.Update(*temp, selected_book_idx);
    delete temp;
  }

  void AuthorModify(const string &author) {
    auto temp = new Book;
    book_system.Read(*temp, selected_book_idx);
    const string s = temp->GetAuthor();
    if (!s.empty()) {
      auto v = author_map.find(s)->second;
      auto iter = std::remove(v.begin(), v.end(), selected_book_idx);
      v.erase(iter, v.end());
    }
    author_map[author].push_back(selected_book_idx);
    temp->ModifyAuthor(author);
    book_system.Update(*temp, selected_book_idx);
    delete temp;
  }

  void KeywordModify(const string &keyword) {
    auto temp = new Book;
    book_system.Read(*temp, selected_book_idx);
    string s = temp->GetKeyword();
    if (!s.empty()) {
      std::vector<string> keywords;
      SpiltKeyword(s, keywords);
      for (const auto &i : keywords) {
        auto v = keyword_map.find(i)->second;
        auto iter = std::remove(v.begin(), v.end(), selected_book_idx);
        v.erase(iter, v.end());
      }
    }
    std::vector<string> new_keywords;
    SpiltKeyword(keyword, new_keywords);
    for (const auto &j : new_keywords) {
      keyword_map[j].push_back(selected_book_idx);
    }
    temp->ModifyKeyword(keyword);
    book_system.Update(*temp, selected_book_idx);
    delete temp;
  }

  void PriceModify(const double x) {
    auto temp = new Book;
    book_system.Read(*temp, selected_book_idx);
    temp->ModifyPrice(x);
    book_system.Update(*temp, selected_book_idx);
    delete temp;
  }
};

#endif //FILE_STORAGE_H
