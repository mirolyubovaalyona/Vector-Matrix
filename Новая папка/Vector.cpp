#include <iostream>
#include <cmath>
#include "Vector.h"
#include "Matrix.h"

namespace mat_vec {

	// -������������ ������ ������� size �� ���������� value
	Vector::Vector(size_t size, double value): _size(size){
		data = new double [_size];
		for (int i = 0; i < _size; i++) 
			data[i] = value;
	}

	// -����������� �����������
	Vector::Vector(const Vector& src): Vector(src.size()) {
		for (int i = 0; i < src._size ; i++) data[i] = src.data[i];
	}

	// -�������� ������������
	Vector& Vector::operator=(const Vector& rhs) {
		_size = rhs._size;
		delete[] data;
		data = new double[_size];
		for (int i = 0; i < rhs._size; i++) data[i] = rhs.data[i];
		return *this;
	}

	// -����������
	Vector::~Vector() {
		delete[] data;
	}

	//- ���������� ������ �������
	size_t Vector::size() const { return _size; }

	//- ������ � n-�� �������� �������
	double Vector::operator[](size_t n) const { return data[n]; }
	double& Vector::operator[](size_t n){ return data[n]; }

	// -L2 ����� �������
	double Vector::norm() const{
		double l2 = 0;
		for (int i = 0; i < _size; i++) l2+=pow(data[i],2);
		return sqrt(l2);
	}

	// -���������� ����� ������, ���������� ������������� �������� (this)
	Vector Vector::normalized() const{
		Vector c(*this);
		c.normalize();
		return c;
	}

	// -����������� ������� ������
	void Vector::normalize() {
		double l2=norm();
		for (int i = 0; i < _size; i++) data[i] /= l2;
	}

	// -������������ �������� ��������
	Vector Vector::operator+(const Vector& rhs) const{
		Vector c = *this;
		c += rhs;
		return c;
	}
	Vector& Vector::operator+=(const Vector& rhs){
		for (int i = 0; i < _size; i++) data[i] += rhs.data[i];
		return *this;
	}

	// -������������ ��������� ��������
	Vector Vector::operator-(const Vector& rhs) const{
		Vector c = *this;
		c -= rhs;
		return c;
	}
	Vector& Vector::operator-=(const Vector& rhs){
		for (int i = 0; i < _size; i++) data[i] -= rhs.data[i];
		return *this;
	}

	// -������������ ��������� ��������
	Vector Vector::operator^(const Vector& rhs) const{
		Vector c = *this;
		c ^= rhs;
		return c;
	}
	Vector& Vector::operator^=(const Vector& rhs){
		for (int i = 0; i < _size; i++) data[i] *= rhs.data[i];
		return *this;
	}

	// -��������� ������������
	double Vector::operator*(const Vector& rhs) const{
		double ab = 0;
		for (int i = 0; i < _size; i++) ab += data[i] * rhs.data[i];
		return ab;
	}

	// -��������� ���� ��������� ������� �� ������ ������ (v * k)
	Vector Vector::operator*(double k) const{
		Vector res= *this;
		res *= k;
		return res;
	}
	Vector& Vector::operator*=(double k){
		for (int i = 0; i < _size; i++) data[i] *= k;
		return *this;
	}

	// -������� ���� ��������� ������� �� ������
	Vector Vector::operator/(double k) const{
		Vector res= *this;
		res /= k;
		return res;
	}
	Vector& Vector::operator/=(double k){
		for (int i = 0; i < _size; i++) data[i] /= k;
		return *this;
	}

	// -��������� ������� �� �������
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

	// -������������ ���������
	bool Vector::operator==(const Vector& rhs) const{
		return !(*this != rhs);
	}
	bool Vector::operator!=(const Vector& rhs) const{
		for (int i = 0; i < _size; i++) if (this->data[i] == rhs.data[i]) return false;
		return true;
	}
}