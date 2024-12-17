#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include "book.h"
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unistd.h>
using std::cout;
using std::string;
constexpr int max_size = 500;
class BookDatabase;
// 图书的总数据库
class BookManage {
  friend BookDatabase;

private:
  Book book;
  std::string file_name;
  std::fstream book_data;
  int total = 0; // 留一个int存储总本数

public:
  BookManage() = default;

  BookManage(const std::string &FN) {}

  ~BookManage() { book_data.close(); }

  void initialize(string FN = "") {
    if (access(file_name.c_str(), F_OK) == 0) {
      // 检查文件是否存在
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
    book_data.seekg((n - 1) * sizeof(Book));
    book_data.read(reinterpret_cast<char *>(&dest), sizeof(Book));
  }

  void Update(Book &new_book, const int n) {
    // 更新第n本书的信息
    book_data.seekp((n - 1) * sizeof(Book));
    book_data.write(reinterpret_cast<char *>(&new_book), sizeof(Book));
  }

  int Push(Book &new_book) {
    // 在尾部写入新书
    total++;
    book_data.seekp((total - 1) * sizeof(Book));
    book_data.write(reinterpret_cast<char *>(&new_book), sizeof(Book));
    return total;
  }
};

struct Block {
  char max_isbn[isbn_len + 1]{};
  int index[505]{};
  int size = 0;
  Block *next = nullptr;
};

class BookDatabase {
private:
  BookManage book_system;
  Block head;

public:
  BookDatabase() = default;

  ~BookDatabase() = default;

  void init() {
    // 初始化，给文件一个名字
    book_system.initialize("booksystem");
  }

  void insert(Book &x) {
    if (book_system.total == 0) {
      // 在链表头存入
      head.index[head.size] = book_system.Push(x);
      head.size++;
      strcpy(head.max_isbn, x.GetIsbn().data());
      return;
    }
    Block *cur = &head;
    while (cur != nullptr) {
      if (strcmp(x.GetIsbn().data(), cur->max_isbn) > 0 &&
          cur->next != nullptr) {
        cur = cur->next;
      } else {
        auto temp = new Book;
        for (int i = 0; i < cur->size; i++) {
          book_system.Read(*temp, i);
          if (x > *temp) {
            // 大于当前位置
            continue;
          }
          if (x == *temp) {
            // 相同isbn
            delete temp;
            return;
          }
          for (int j = cur->size - 1; j >= i; j--) {
            cur->index[j + 1] = cur->index[j];
          }
          cur->size++;
          cur->index[i] = book_system.Push(x);
          if (cur->size > max_size) {
            auto new_block = new Block;
            new_block->next = cur->next;
            cur->next = new_block;
            int new_size = cur->size / 2;
            strcpy(new_block->max_isbn, cur->max_isbn);
            book_system.Read(*temp, cur->index[new_size - 1]);
            strcpy(cur->max_isbn, temp->GetIsbn().data());
            for (int k = new_size; k < cur->size; k++) {
              new_block->index[new_block->size++] = cur->index[k];
              cur->index[k] = 0;
            }
            cur->size = new_size;
            new_block = nullptr;
          }
          delete temp;
          return;
        }
        strcpy(cur->max_isbn, x.GetIsbn().data());
        cur->index[cur->size] = book_system.Push(x);
        cur->size++;
        if (cur->size > max_size) {
          auto new_block = new Block;
          new_block->next = cur->next;
          cur->next = new_block;
          int new_size = cur->size / 2;
          strcpy(new_block->max_isbn, cur->max_isbn);
          book_system.Read(*temp, cur->index[new_size - 1]);
          strcpy(cur->max_isbn, temp->GetIsbn().data());
          for (int k = new_size; k < cur->size; k++) {
            new_block->index[new_block->size++] = cur->index[k];
            cur->index[k] = 0;
          }
          cur->size = new_size;
          new_block = nullptr;
        }
        delete temp;
        return;
      }
    }
  }

  void isbn_show(char isbn[isbn_len + 1]) {
    bool flag = false;
    if (book_system.total == 0) {
      // 在链表头存入
      cout << "\n";
      return;
    }
    Block *cur = &head;
    while (cur != nullptr) {
      if (strcmp(isbn, cur->max_isbn) > 0 && cur->next != nullptr) {
        cur = cur->next;
      } else {
        auto temp = new Book;
        for (int i = 0; i < cur->size; i++) {
          book_system.Read(*temp, i);
          if (strcmp(isbn, temp->GetIsbn().data()) < 0) {
            if (!flag) {
              cout << "\n";
            }
            delete temp;
            return;
          }
          if (strcmp(isbn, temp->GetIsbn().data()) == 0) {
            flag = true;
            // 相同isbn
            cout << temp->GetIsbn() << "\t" << temp->GetName() << "\t"
                 << temp->GetAuthor() << "\t" << temp->GetKeyword() << "\t"
                 << std::fixed << std::setprecision(2) << temp->GetPrice()
                 << "\t" << temp->GetQuantity() << "\n";
          }
        }
        if (!flag) {
          cout << "\n";
        }
        delete temp;
        return;
      }
    }
  }

  void name_show(char name[name_len + 1]) {

  }
};

#endif // BPT_MEMORYRIVER_HPP
#endif //FILE_STORAGE_H
