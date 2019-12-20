#pragma once

#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <utility>

template <typename T, int MINCAP=0>
class Vector {
	int m_size, m_cap;
	union DataUnion {
		DataUnion() {}
		~DataUnion() {}
		T *ext;
		T local[MINCAP];
	} m_data;
public:

	Vector() : m_size(0), m_cap(MINCAP) { }

	Vector(const T *ptr, size_t len)
		: Vector()
	{
		reserve(len);
		for (size_t i = 0; i < len; i++) {
			new (&(*this)[i]) T(ptr[i]);
		}
		m_size = len;
	}

	Vector(Vector &&v)
		: m_size(v.m_size), m_cap(v.m_cap), m_data(v.m_data)
	{
		v.m_cap = MINCAP;
		v.m_size = 0;
	}

	~Vector()
	{
		T *p = data();
		for (size_t i = 0; i < size(); i++) {
			(&(*this)[i])->~T();
		}
		if (p != m_data.local) {
			free(p);
		}
	}

	void operator=(const Vector &v) = delete;
	void operator=(Vector &v) = delete;
	void operator=(Vector &&v)
	{
		m_size = v.m_size;
		m_cap = v.m_cap;
		m_data = v.m_data;
		v.m_cap = MINCAP;
		v.m_size = 0;
	}

	T *data() const { return (T*)(m_cap <= MINCAP ? m_data.local : m_data.ext); }
	size_t size() const { return m_size; }
	size_t capacity() const { return m_cap; }

	using iterator = T*;
	iterator begin() { return data(); }
	iterator end() { return data() + size(); }

	using const_iterator = const T*;
	const_iterator cbegin() const { return data(); }
	const_iterator cend() const { return data() + size(); }
	const_iterator begin() const { return data(); }
	const_iterator end() const { return data() + size(); }

	T &operator[](int i) { return data()[i]; }
	const T &operator[](int i) const { return data()[i]; }

	void reserve(int n)
	{
		if (n <= m_cap)
			return;

		T *prev = data();
		if (MINCAP == 0 && m_cap == 0) {
			m_cap = 16;
		}
		do m_cap += (m_cap+3)/4;
		while (n > m_cap);
		assert(m_cap > MINCAP);

		if (prev == m_data.local) {
			//DEBUG("resize from local to {}", m_cap);
			T *ext = (T*)malloc(m_cap * sizeof(T));
			for (size_t i = 0; i < size(); i++) {
				new (&ext[i]) T(std::move(prev[i]));
				(&prev[i])->~T();
			}
			//memcpy(ext, prev, size() * sizeof(T));
			m_data.ext = ext;
		} else {
			//DEBUG("resize from ext to {}", m_cap);
			m_data.ext = (T*)realloc((void*)prev, m_cap * sizeof(T));
		}
	}

	void set_size(size_t n)
	{
		m_size = n;
		assert(m_size <= m_cap);
	}

	void truncate(size_t n)
	{
		assert(n <= size());
		for (size_t i = n; i < size(); i++)
			(&(*this)[i])->~T();
		m_size = n;
	}

	void resize(int n, T init=T{})
	{
		if (m_size >= n) {
			for (ssize_t i = n; i < m_size; i++)
				(&(*this)[i])->~T();
			m_size = n;
		} else {
			reserve(n);
			T *d = data();
			while (m_size < n)
				d[m_size++] = init;
		}
	}

	void clear() { resize(0); }

	size_t push_back(const T &t)
	{
		reserve(m_size+1);
		new (&(*this)[m_size]) T();
		(*this)[m_size++] = t;
		return m_size-1;
	}

	template <typename... Args>
	T &emplace_back(Args... args)
	{
		reserve(m_size+1);
		new (&(*this)[m_size++]) T(std::forward<Args>(args)...);
		return back();
	}

	void pop_back()
	{
		m_size--;
	}

	T &front() { return (*this)[0]; }
	const T &front() const { return (*this)[0]; }

	T &back() { return (*this)[m_size-1]; }
	const T &back() const { return (*this)[m_size-1]; }
};
