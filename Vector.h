#pragma once

#include "Base.h"

namespace mat_vec {

// Умножение всех элементов вектора на число слева (k * v)
Vector operator*(double k, const Vector &v);

class Vector {
public:
  // Конструирует вектор размера size со значениями value
  explicit Vector(size_t size, double value = 0);
  
  // Конструктор копирования
  Vector(const Vector &src);
  
  // Оператор присваивания
  Vector &operator=(const Vector &rhs);
  
  // Деструктор
  ~Vector();

  // Возвращает размер вектора
  size_t size() const;
  
  // Доступ к n-му элементу вектора
  double operator[](size_t n) const;
  double &operator[](size_t n);

  // L2 норма вектора
  double norm() const;
  
  // Возвращает новый вектор, полученный нормализацией текущего (this)
  Vector normalized() const;
  
  // Нормализует текущий вектор
  void normalize();

  // Поэлементное сложение векторов
  Vector operator+(const Vector &rhs) const;
  Vector &operator+=(const Vector &rhs);

  // Поэлементное вычитание векторов
  Vector operator-(const Vector &rhs) const;
  Vector &operator-=(const Vector &rhs);

  // Поэлементное умножение векторов
  Vector operator^(const Vector &rhs) const;
  Vector &operator^=(const Vector &rhs);

  // Скалярное произведение
  double operator*(const Vector &rhs) const;

  // Умножение всех элементов вектора на скаляр справа (v * k)
  Vector operator*(double k) const;
  Vector &operator*=(double k);

  // Деление всех элементов вектора на скаляр
  Vector operator/(double k) const;
  Vector &operator/=(double k);

  // Умножение вектора на матрицу
  Vector operator*(const Matrix &mat) const;
  Vector &operator*=(const Matrix &mat);

  // Поэлементное сравнение
  bool operator==(const Vector &rhs) const;
  bool operator!=(const Vector &rhs) const;

private:
};
} // namespace mat_vec
