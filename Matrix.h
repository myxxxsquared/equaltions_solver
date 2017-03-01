#pragma once

namespace mylib{

	template<typename T>
	class Matrix
	{
	public:
		Matrix(int l1, int l2)
			:len1(l1), len2(l2), data(new T[l1*l2])
		{
			for (int i = 0; i < l1*l2; i++)
				data[i] = 0;
		}


		Matrix(int l1, int l2, T* d)
			:len1(l1), len2(l2), data(new T[l1*l2])
		{
			try
			{
				for (int i = 0; i < l1*l2; i++)
					data[i] = d[i];
			}
			catch (...)
			{
				delete[] data;
				data = nullptr;
				throw;
			}
		}

		~Matrix()
		{
			if (data)
				delete[] data;
			data = nullptr;
		}

		Matrix(const Matrix& obj)
		{
			len1 = obj.len1;
			len2 = obj.len2;

			data = new T[len1 * len2];

			try
			{
				for (int i = 0; i < len1 * len2; i++)
					data[i] = obj.data[i];
			}
			catch (...)
			{
				delete[] data;
				data = nullptr;
				throw;
			}
		}

		Matrix(Matrix&& obj)
		{
			len1 = obj.len1;
			len2 = obj.len2;
			data = obj.data;
			obj.data = nullptr;
		}

		Matrix& operator=(const Matrix& obj)
		{
			if (this == &obj)
				return *this;

			if (data)
				delete[] data;

			len1 = obj.len1;
			len2 = obj.len2;
			data = new T[len1 * len2];

			try
			{
				for (int i = 0; i < len1 * len2; i++)
					data[i] = obj.data[i];
			}
			catch (...)
			{
				delete[] data;
				data = nullptr;
				throw;
			}
			return *this;
		}

		Matrix& operator=(Matrix&& obj)
		{
			if (this == &obj)
				return *this;
			if (data)
				delete[] data;
			len1 = obj.len1;
			len2 = obj.len2;
			data = obj.data;
			obj.data = nullptr;
			return *this;
		}

		T& item(int i, int j)
		{
			if (i >= len1 || i < 0 || j >= len2 || j < 0)
				throw std::logic_error("Invalid Index.");
			return data[i*len2 + j];
		}

		const T& item(int i, int j) const
		{
			if (i >= len1 || i < 0 || j >= len2 || j < 0)
				throw std::logic_error("Invalid Index.");
			return data[i*len2 + j];
		}

		Matrix operator+(const Matrix & right) const
		{
			if (len1 != right.len1 || len2 != right.len2)
				throw std::logic_error("Invaild Dim");

			Matrix m(len1, len2);
			for (int i = 0; i < len1 * len2; i++)
				m.data[i] = data[i] + right.data[i];

			return m;
		}

		Matrix operator*(const Matrix & right) const
		{
			if (len2 != right.len1)
				throw std::logic_error("Invaild Dim");

			Matrix m(len1, right.len2);

			for (int i = 0; i < len1; i++)
				for (int j = 0; j < right.len2; j++)
					for (int k = 0; k < len2; k++)
						m.data[i*right.len2 + j] += data[i*len2 + k] * right.data[k*right.len2 + j];
			//m.item(i, j) += item(i, k)*right.item(k, j);
			return m;
		}

		Matrix transposition() const
		{
			Matrix m(len2, len1);
			for (int i = 0; i < len1; i++)
				for (int j = 0; j < len2; j++)
					m.item(j, i) = item(i, j);
			return m;
		}

		operator T*(){ return data; }
		operator const T*() const{ return data; }

	private:
		T* data;
		int len1, len2;
	};

}