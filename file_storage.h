#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

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
  std::string file_name;
  std::fstream book_data;
  int total = 0; // 留一个int存储总本数

public:
  BookManage() = default;

  ~BookManage() { book_data.close(); }

  void initialize(string FN = "") {
    /*
    if (access(file_name.c_str(), F_OK) == 0) {
      // 检查文件是否存在
      return;
    }
    */
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
  int block_number = 0;
  std::fstream linked_block;

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
    if (access("block", F_OK) != 0) {
      // 检查文件是否存在
      linked_block.open("block", std::ios::out);
      linked_block.close();
      return;
    }
    linked_block.open("block", std::ios::in | std::ios::out);
    linked_block.seekg(0);
    linked_block.read(reinterpret_cast<char *>(&block_number), sizeof(int));
    linked_block.seekg(sizeof(int));
    linked_block.read(reinterpret_cast<char *>(&head), sizeof(Block));
    auto now = &head;
    for (int i = 1; i < block_number; i++) {
      linked_block.seekg(sizeof(int) + i * sizeof(Block));
      now->next = new Block;
      now = now->next;
      linked_block.read(reinterpret_cast<char *>(now), sizeof(Block));
    }
    linked_block.close();
  }

  void Save() {
    // 把内存信息转到文件里
    linked_block.open("block", std::ios::in | std::ios::out);
    linked_block.seekp(0);
    linked_block.write(reinterpret_cast<char *>(&block_number), sizeof(int));
    linked_block.seekp(sizeof(int));
    linked_block.write(reinterpret_cast<char *>(&head), sizeof(Block));
    Block *now = head.next;
    int num = 1;
    while (now != nullptr) {
      auto it = now;
      linked_block.seekp(sizeof(int) + num * sizeof(Block));
      linked_block.write(reinterpret_cast<char *>(now), sizeof(Block));
      now = now->next;
      delete it;
    }
    linked_block.close();
  }

  void Insert(Book &x) {
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
          book_system.Read(*temp, cur->index[i]);
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
            block_number++;
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
          block_number++;
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

  void AllShow() {
    if (book_system.total == 0) {
      cout << "\n";
      return;
    }
    Block *cur = &head;
    auto temp = new Book;
    while (cur != nullptr) {
      for (int i = 0; i < cur->size; i++) {
        book_system.Read(*temp, cur->index[i]);
        cout << temp->GetIsbn() << "\t" << temp->GetName() << "\t"
             << temp->GetAuthor() << "\t" << temp->GetKeyword() << "\t"
             << std::fixed << std::setprecision(2) << temp->GetPrice() << "\t"
             << temp->GetQuantity() << "\n";
      }
      cur = cur->next;
    }
    delete temp;
  }

  void IsbnShow(char isbn[isbn_len + 1]) {
    bool flag = false;
    if (book_system.total == 0) {
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
          book_system.Read(*temp, cur->index[i]);
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

  void NameShow(char name[name_len + 1]) {
    bool flag = false;
    if (book_system.total == 0) {
      cout << "\n";
      return;
    }
    Block *cur = &head;
    auto temp = new Book;
    while (cur != nullptr) {
      for (int i = 0; i < cur->size; i++) {
        book_system.Read(*temp, cur->index[i]);
        if (strcmp(name, temp->GetName().data()) == 0) {
          // 相同书名
          flag = true;
          cout << temp->GetIsbn() << "\t" << temp->GetName() << "\t"
               << temp->GetAuthor() << "\t" << temp->GetKeyword() << "\t"
               << std::fixed << std::setprecision(2) << temp->GetPrice() << "\t"
               << temp->GetQuantity() << "\n";
        }
      }
      cur = cur->next;
    }
    if (!flag)
      cout << "\n";
  }

  void AuthorShow(char author[name_len + 1]) {
    bool flag = false;
    if (book_system.total == 0) {
      cout << "\n";
      return;
    }
    Block *cur = &head;
    auto temp = new Book;
    while (cur != nullptr) {
      for (int i = 0; i < cur->size; i++) {
        book_system.Read(*temp, cur->index[i]);
        if (strcmp(author, temp->GetAuthor().data()) == 0) {
          // 相同作者
          flag = true;
          cout << temp->GetIsbn() << "\t" << temp->GetName() << "\t"
               << temp->GetAuthor() << "\t" << temp->GetKeyword() << "\t"
               << std::fixed << std::setprecision(2) << temp->GetPrice() << "\t"
               << temp->GetQuantity() << "\n";
        }
      }
      cur = cur->next;
    }
    if (!flag)
      cout << "\n";
  }

  void KeywordShow(const string &s) {
    bool flag = false;
    if (book_system.total == 0) {
      // 在链表头存入
      cout << "\n";
      return;
    }
    Block *cur = &head;
    auto temp = new Book;
    while (cur != nullptr) {
      for (int i = 0; i < cur->size; i++) {
        book_system.Read(*temp, cur->index[i]);
        if (temp->KeywordJudge(s)) {
          // 是关键字
          flag = true;
          cout << temp->GetIsbn() << "\t" << temp->GetName() << "\t"
               << temp->GetAuthor() << "\t" << temp->GetKeyword() << "\t"
               << std::fixed << std::setprecision(2) << temp->GetPrice() << "\t"
               << temp->GetQuantity() << "\n";
        }
      }
      cur = cur->next;
    }
    if (!flag)
      cout << "\n";
  }

  double Buy(char isbn[isbn_len + 1], const long long x) {
    if (book_system.total == 0) {
      return -1;
    }
    Block *cur = &head;
    while (cur != nullptr) {
      if (strcmp(isbn, cur->max_isbn) > 0) {
        cur = cur->next;
      } else {
        auto temp = new Book;
        for (int i = 0; i < cur->size; i++) {
          book_system.Read(*temp, cur->index[i]);
          if (strcmp(isbn, temp->GetIsbn().data()) < 0) {
            // 当前位置的ISBN大于目标isbn
            delete temp;
            return -1;
          }
          if (strcmp(isbn, temp->GetIsbn().data()) == 0) {
            // 相同isbn
            const bool flag = temp->ModifyQuantity(x);
            book_system.Update(*temp, cur->index[i]);
            delete temp;
            if (flag) {
              return temp->GetPrice() * x;
            }
            return -1;
          }
        }
        delete temp;
        return -1;
      }
    }
    return false;
  }

  bool Select(char isbn[isbn_len + 1]) {
    if (book_system.total == 0) {
      return false;
    }
    Block *cur = &head;
    while (cur != nullptr) {
      if (strcmp(isbn, cur->max_isbn) > 0) {
        cur = cur->next;
      } else {
        auto temp = new Book;
        for (int i = 0; i < cur->size; i++) {
          book_system.Read(*temp, cur->index[i]);
          if (strcmp(isbn, temp->GetIsbn().data()) < 0) {
            // 当前位置的ISBN大于目标isbn
            delete temp;
            return false;
          }
          if (strcmp(isbn, temp->GetIsbn().data()) == 0) {
            // 相同isbn
            selected_book_idx = cur->index[i];
            delete temp;
            return true;
          }
        }
        delete temp;
        return false;
      }
    }
    return false;
  }

  bool Import(long long x, double cost) {
    if (selected_book_idx == 0) {
      return false;
    }
    auto temp = new Book;
    book_system.Read(*temp, selected_book_idx);
    temp->ModifyQuantity(-x);
    book_system.Update(*temp, selected_book_idx);
    // TODO cost要写到日志系统
    return true;
  }

  bool IsbnModify(const string &isbn) {
    auto temp = new Book;
    book_system.Read(*temp, selected_book_idx);
    if (temp->GetIsbn() == isbn) {
      delete temp;
      return false;
    }
    temp->ModifyIsbn(isbn);
    book_system.Update(*temp, selected_book_idx);
    delete temp;
    return true;
  }

  void NameModify(const string &name) {
    auto temp = new Book;
    book_system.Read(*temp, selected_book_idx);
    temp->ModifyName(name);
    book_system.Update(*temp, selected_book_idx);
    delete temp;
  }

  void AuthorModify(const string &author) {
    auto temp = new Book;
    book_system.Read(*temp, selected_book_idx);
    temp->ModifyAuthor(author);
    book_system.Update(*temp, selected_book_idx);
    delete temp;
  }

  void KeywordModify(const string &keyword) {
    auto temp = new Book;
    book_system.Read(*temp, selected_book_idx);
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
