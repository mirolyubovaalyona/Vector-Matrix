#include <iostream>
#include <cmath>
#include "Vector.h"
#include "Matrix.h"

namespace mat_vec {

	// -Конструирует вектор размера size со значениями value
	Vector::Vector(size_t size, double value): _size(size){
		data = new double [_size];
		for (int i = 0; i < _size; i++) 
			data[i] = value;
	}

	// -Конструктор копирования
	Vector::Vector(const Vector& src): Vector(src.size()) {
		for (int i = 0; i < src._size ; i++) data[i] = src.data[i];
	}

	// -Оператор присваивания
	Vector& Vector::operator=(const Vector& rhs) {
		_size = rhs._size;
		delete[] data;
		data = new double[_size];
		for (int i = 0; i < rhs._size; i++) data[i] = rhs.data[i];
		return *this;
	}

	// -Деструктор
	Vector::~Vector() {
		delete[] data;
	}

	//- Возвращает размер вектора
	size_t Vector::size() const { return _size; }

	//- Доступ к n-му элементу вектора
	double Vector::operator[](size_t n) const { return data[n]; }
	double& Vector::operator[](size_t n){ return data[n]; }

	// -L2 норма вектора
	double Vector::norm() const{
		double l2 = 0;
		for (int i = 0; i < _size; i++) l2+=pow(data[i],2);
		return sqrt(l2);
	}

	// -Возвращает новый вектор, полученный нормализацией текущего (this)
	Vector Vector::normalized() const{
		Vector c(*this);
		c.normalize();
		return c;
	}

	// -Нормализует текущий вектор
	void Vector::normalize() {
		double l2=norm();
		for (int i = 0; i < _size; i++) data[i] /= l2;
	}

	// -Поэлементное сложение векторов
	Vector Vector::operator+(const Vector& rhs) const{
		Vector c = *this;
		c += rhs;
		return c;
	}
	Vector& Vector::operator+=(const Vector& rhs){
		for (int i = 0; i < _size; i++) data[i] += rhs.data[i];
		return *this;
	}

	// -Поэлементное вычитание векторов
	Vector Vector::operator-(const Vector& rhs) const{
		Vector c = *this;
		c -= rhs;
		return c;
	}
	Vector& Vector::operator-=(const Vector& rhs){
		for (int i = 0; i < _size; i++) data[i] -= rhs.data[i];
		return *this;
	}

	// -Поэлементное умножение векторов
	Vector Vector::operator^(const Vector& rhs) const{
		Vector c = *this;
		c ^= rhs;
		return c;
	}
	Vector& Vector::operator^=(const Vector& rhs){
		for (int i = 0; i < _size; i++) data[i] *= rhs.data[i];
		return *this;
	}

	// -Скалярное произведение
	double Vector::operator*(const Vector& rhs) const{
		double ab = 0;
		for (int i = 0; i < _size; i++) ab += data[i] * rhs.data[i];
		return ab;
	}

	// -Умножение всех элементов вектора на скаляр справа (v * k)
	Vector Vector::operator*(double k) const{
		Vector res= *this;
		res *= k;
		return res;
	}
	Vector& Vector::operator*=(double k){
		for (int i = 0; i < _size; i++) data[i] *= k;
		return *this;
	}

	// -Деление всех элементов вектора на скаляр
	Vector Vector::operator/(double k) const{
		Vector res= *this;
		res /= k;
		return res;
	}
	Vector& Vector::operator/=(double k){
		for (int i = 0; i < _size; i++) data[i] /= k;
		return *this;
	}

	// -Умножение вектора на матрицу
	Vector Vector::operator*(const Matrix& mat) const{
		Vector c = Vector(_size, 0);
		for (int i = 0; i <mat._size.first; i++) 
			for (int j = 0; j < mat._size.second; j++) 
				c.data[i] += mat.a[i][j] * data[j];
		return c;
	}
	Vector& Vector::operator*=(const Matrix& mat){
		Vector c = Vector(_size, 0);
		for (int i = 0; i < mat._size.first; i++) 
			for (int j = 0; j < mat._size.second; j++) 
				c.data[i] += mat.a[i][j] * data[j];
		*this = c;
		return *this;
	}

	// -Поэлементное сравнение
	bool Vector::operator==(const Vector& rhs) const{
		return !(*this != rhs);
	}
	bool Vector::operator!=(const Vector& rhs) const{
		for (int i = 0; i < _size; i++) if (this->data[i] == rhs.data[i]) return false;
		return true;
	}
}