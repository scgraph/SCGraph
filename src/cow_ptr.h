/* Copyright 2006 Florian Paul Schmidt */

#ifndef INCLUDED_COW_PTR
#define INCLUDED_COW_PTR

#include <iostream>
#include <boost/shared_ptr.hpp>

//#define DEBUG

/**
	Everything that should be handled by the cow ptr and needs 
	to have deep copies made
*/
struct DeepCopyable {
	virtual DeepCopyable *deepCopy() = 0;
	virtual ~DeepCopyable() { } 
};

template<class T>
struct cow_ptr{
	mutable boost::shared_ptr<T> m_t;

	inline cow_ptr(T *t = 0) : m_t(t) {
#ifdef DEBUG
		std::cout << "[COW]: constr." << std::endl;
#endif
        }

	inline cow_ptr(const cow_ptr<T> &other) : m_t(other.m_t) {
#ifdef DEBUG
		std::cout << "[COW]: copy constr." << std::endl;
#endif
	}

	template<class U>
	inline cow_ptr(const cow_ptr<U> &other) : m_t(other.m_t) {
#ifdef DEBUG
		std::cout << "[COW]: copy constr (templ. U)." << std::endl;
#endif
	}

	template<class U>
	inline cow_ptr<T> &operator=(const cow_ptr<U> &other) {
#ifdef DEBUG
		std::cout << "[COW]: assignment" << std::endl;
#endif
		m_t = other.m_t;
	}

        inline const T* inspect() const {
#ifdef DEBUG
		std::cout << "[COW]: inspect" << std::endl;
#endif
                return m_t.get();
        }

        inline const T* operator->() const {
#ifdef DEBUG
		std::cout << "[COW]: ->" << std::endl;
#endif
                return m_t.get();
        }

        inline T* touch() {
		if ( !(m_t.unique()) ) 
		{
#ifdef DEBUG
			std::cout << "[COW]: copy: " << std::endl;
#endif
			m_t = boost::shared_ptr<T>(m_t.get()->deepCopy());
		}

#ifdef DEBUG
		std::cout << "[COW]: non-copy" << std::endl;
#endif
                return m_t.get();
        }
};

#endif

