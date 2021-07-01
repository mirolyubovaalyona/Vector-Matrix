#include <iostream>
#include "Matrix.h"
#include "Vector.h"
#include <utility>

namespace mat_vec {

	// -Конструирует матрицу с размерами size x size, заполненную value
	Matrix::Matrix(size_t size, double value ):Matrix(size, size, value) {	}

	// -Возвращает единичную матрицу
	Matrix Matrix::eye(std::size_t size) {
		Matrix m(size);
		for (int i = 0; i < size; ++i) m.a[i][i] = 1;
		return m;
		
	}

	// -Возвращает матрицу с размерами rows x cols, заполненную value
	Matrix::Matrix(size_t rows, size_t cols, double value) {
		_size = { rows, cols};
		a = new double* [_size.first];
		for (int i = 0; i < _size.first; i++) {
			a[i] = new double[_size.second];
			for (int j = 0; j < _size.second; j++) a[i][j] = value;
		}

	}


	// -Конструктор копирования
	Matrix::Matrix(const Matrix& src){
		_size=src._size;
		//for (int i = 0; i < _size.first; i++) delete[] a[i];
		//delete[] a;
		a = new double*[_size.first];
		for (int i = 0; i < src._size.first; i++) a[i] = new double[_size.second];
		for (int i = 0; i < src._size.first; i++) for (int j = 0; j < src._size.second; j++) 
			a[i][j] = src.a[i][j];
	}

	// -Оператор присваивания
	Matrix& Matrix::operator=(const Matrix& rhs) {
		_size = rhs._size;
		for (int i = 0; i < _size.first; i++) delete[] a[i];
		delete[] a;
		a = new double* [_size.first]; 
			
		for (int i = 0; i < rhs._size.first; i++) {
			a[i] = new double[_size.second];
			for (int j = 0; j < rhs._size.second; j++)
				a[i][j] = rhs.a[i][j];
		}
		return *this;
	}

	// -Деструктор
	Matrix::~Matrix() {
		for (int i = 0; i < _size.first; i++) delete[] a[i];
		delete[] a;
	}

	// - Изменяет ширину и высоту матрицы, не изменяя при этом
	// порядок следования элементов от левого верхнего к правому нижнему:
	//
	// [1 2 3] -> [1 2]
	// [4 5 6] -> [3 4]
	//            [5 6]
	void Matrix::reshape(size_t rows, size_t cols){
		Matrix c = *this;
		for (int i = 0; i < _size.first; i++) delete[] a[i];
		delete[] a;
		int k = 0;
		_size = { cols, rows};
		a = new double* [_size.first];
		for (int i = 0; i < _size.first; i++) {
			a[i] = new double[_size.second];
			for (int j = 0; j < _size.second; j++) a[i][j] = c.a[k % c._size.first][k%c._size.second];
			k++;
		}
	}

	

	// -Возвращает пару {rows, cols} -- количество строк и столбцов матрицы
	std::pair<size_t, size_t> Matrix::shape() const { return _size; }

	// -Возвращает элемент на позиции [row, col]
	double Matrix::get(size_t row, size_t col) const { return a[row][col];}

	// -Поэлементное сложение
	Matrix Matrix::operator+(const Matrix& rhs) const {
		Matrix c =*this;
		c += rhs;
		return c;
	}
	Matrix& Matrix::operator+=(const Matrix& rhs){
		for (int i = 0; i < _size.first; i++) for (int j = 0; j < _size.second; j++)
			a[i][j] += rhs.a[i][j];
		return *this;
	}
	
	// -Поэлементное вычитание
	Matrix Matrix::operator-(const Matrix& rhs) const{

		Matrix c = *this;
		c -= rhs;
		return c;
	}
	Matrix& Matrix::operator-=(const Matrix& rhs){
		for (int i = 0; i < _size.first; i++) for (int j = 0; j < _size.second; j++) 
			a[i][j] -= rhs.a[i][j];
		return *this;
	}

	// -Матричное умножение
	Matrix Matrix::operator*(const Matrix& rhs) const{
		Matrix c = *this;
		c *= rhs;
		return c;
	}
	Matrix& Matrix::operator*=(const Matrix& rhs){
		for (int i = 0; i < _size.first; i++) for (int j = 0; j < _size.second; j++) 
			a[i][j] *= rhs.a[i][j];
		return *this;
	}

	// -Умножение всех элементов матрицы на константу
	Matrix Matrix::operator*(double k) const{
		Matrix c = *this;
		c *= k;
		return c;
	}
	Matrix& Matrix::operator*=(double k){
		for (int i = 0; i < _size.first; i++) for (int j = 0; j < _size.second; j++) a[i][j] *= k;
		return *this;
	}

	// -Деление всех элементов матрицы на константу
	Matrix Matrix::operator/(double k) const{
		Matrix c = *this;
		c /= k;
		return c;
	}
	Matrix& Matrix::operator/=(double k){
		for (int i = 0; i < _size.first; i++) for (int j = 0; j < _size.second; j++) a[i][j] /= k;
		return *this;
	}

	// -Возвращает новую матрицу, полученную транспонированием текущей (this)
	Matrix Matrix::transposed() const{
		Matrix c = *this;
		c.transpose();
		return c;
	}

	//  -Транспонирует текущую матрицу
	void Matrix::transpose(){
		Matrix c = *this;
		for (int i = 0; i < _size.first; i++) delete[] a[i];
		delete[] a;
		int k=0;
		_size = { c._size.second, c._size.first };
		a = new double* [_size.first];
		for (int i = 0; i < _size.first; i++) {
			a[i] = new double[_size.second];
			for (int j = 0; j < _size.second; j++) a[i][j] = c.a[k % c._size.second][k % c._size.first];
			k++;
		}
	}

	//Определитель
	double Matrix::det() const{
		const double EPS = 1E-9;
		double d = 1;
		double c;
		for (int i=0; i<_size.first; ++i) {
			int k = i;
			for (int j=i+1; j< _size.first; ++j)
				if (abs (a[j][i]) > abs (a[k][i]))
					k = j;
			if (abs (a[k][i]) < EPS) {
				d = 0;
				break;
			}
			for (int jjj = 0; i < _size.first; ++jjj) {
				c = a[i][jjj];
				a[i][jjj]= a[k][jjj];
				a[k][jjj] = c;
			}
			if (i != k)
				d = -d;
			d *= a[i][i];
			for (int j=i+1; j< _size.first; ++j)
				a[i][j] /= a[i][i];
			for (int j=0; j< _size.first; ++j)
				if (j != i && abs (a[j][i]) > EPS)
					for (int k=i+1; k<_size.first; ++k)
						a[j][k] -= a[i][k] * a[j][i];
		}
		return d;
	
	}

	// Обратная матрица
	Matrix Matrix::inv() const{
		Matrix c = *this;
		Matrix dd = *this;
		double d = dd.det();
		c.transpose();
		c *= (1 / d);
		return c;
	}

	// -УМножение матрицы на вектор
	Vector Matrix::operator*(const Vector& vec) const{
		Vector c = Vector(vec._size, 0);
		for (int i = 0; i < _size.first; i++) for (int j = 0; j < _size.second; j++) c.data[j]+=a[i][j]*vec.data[j];
		return c;
	}

	// -Поэлементное сравнение
	bool Matrix::operator==(const Matrix& rhs) const{
		for (int i = 0; i < _size.first; i++) 
			for (int j = 0; j < _size.second; j++)
				if (this->a[i][j] != rhs.a[i][j])
					return false;
		return true;
	}
	bool Matrix::operator!=(const Matrix& rhs) const{
		return !(*this == rhs);
	}
}