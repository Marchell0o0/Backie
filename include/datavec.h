#ifndef DATAVEC_H
#define DATAVEC_H

#include "template.h"

template <typename T> class DataVec {
public:
	virtual bool readVec(fs::path &path);

	// Standard vector methods that could be overloaded in child classes

	// non-const methods have to call saveVec to save changes into json
	virtual typename std::vector<T>::iterator
	erase(typename std::vector<T>::iterator pos) {
		typename std::vector<T>::iterator next = vec.erase(pos);
		saveVec();
		return next;
	}
	virtual typename std::vector<T>::iterator
	erase(typename std::vector<T>::iterator first,
		  typename std::vector<T>::iterator last) {
		typename std::vector<T>::iterator next = vec.erase(first, last);
		saveVec();
		return next;
	}

	virtual void push_back(const T &value) {
		vec.push_back(value);
		saveVec();
	}
	virtual void change(typename std::vector<T>::iterator pos, const T &value) {
		(*pos) = value;
		saveVec();
	}

	// const methods
	virtual int size() const { return vec.size(); }
	virtual bool empty() const { return vec.empty(); }
	virtual typename std::vector<T>::iterator begin() { return vec.begin(); }
	virtual typename std::vector<T>::const_iterator begin() const {
		return vec.begin();
	}
	virtual typename std::vector<T>::iterator end() { return vec.end(); }
	virtual typename std::vector<T>::const_iterator end() const {
		return vec.end();
	}

protected:
	bool saveVec();
	std::vector<T> vec;
	fs::path path;
	DataVec() {}
};

class TemplateVec : public DataVec<Template> {
public:
	// override for task scheduler interface
	bool readVec(fs::path &path) override;

	std::vector<Template>::iterator
	erase(std::vector<Template>::iterator pos) override;

	std::vector<Template>::iterator
	erase(std::vector<Template>::iterator first,
		  std::vector<Template>::iterator last) override;

	void push_back(const Template &value) override;

	void change(std::vector<Template>::iterator pos,
				const Template &newObject) override;

	// singleton
	TemplateVec(TemplateVec &other) = delete;
	void operator=(const TemplateVec &) = delete;
	static TemplateVec &getInstance() {
		static TemplateVec instance;
		return instance;
	}

private:
	TemplateVec() {}
};

#endif // DATAVEC_H
