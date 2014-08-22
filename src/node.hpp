#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <cstdint>

class node;
typedef std::map<node,node> node_map;
typedef std::vector<node> node_list;
typedef std::function<node(const node&)> node_fn;

class node
{
public:
	enum node_type
	{
		NODE_TYPE_NULL,
		NODE_TYPE_BOOL,
		NODE_TYPE_INTEGER,
		NODE_TYPE_FLOAT,
		NODE_TYPE_STRING,
		NODE_TYPE_MAP,
		NODE_TYPE_LIST,
		NODE_TYPE_FUNCTION,
	};

	node();
	node(const node&);
	explicit node(int);
	explicit node(unsigned);
	explicit node(int64_t);
	explicit node(float);
	explicit node(double);
	explicit node(const char*);
	explicit node(const std::string&);
	explicit node(const node_map&);
	explicit node(const node_list&);
	explicit node(node_map*);
	explicit node(node_list*);
	explicit node(node_fn fn);

	node_type type() const { return type_; }
	std::string type_as_string() const;

	static node from_bool(bool b);

	std::string as_string() const;
	int64_t as_int() const;
	float as_float() const;
	bool as_bool() const;
	const node_list& as_list() const;
	const node_map& as_map() const;
	const node_fn as_function() const;

	node_list& as_mutable_list();
	node_map& as_mutable_map();

	unsigned num_elements() const;

	bool is_string() const { return type_ == NODE_TYPE_STRING; }
	bool is_null() const { return type_ == NODE_TYPE_NULL; }
	bool is_bool() const { return type_ == NODE_TYPE_BOOL; }
	bool is_numeric() const { return is_int() || is_float(); }
	bool is_int() const { return type_ == NODE_TYPE_INTEGER; }
	bool is_float() const { return type_ == NODE_TYPE_FLOAT; }
	bool is_map() const { return type_ == NODE_TYPE_MAP; }
	bool is_list() const { return type_ == NODE_TYPE_LIST; }
	bool is_function() const { return type_ == NODE_TYPE_FUNCTION; }

	bool operator<(const node&) const;
	bool operator>(const node&) const;

	bool operator==(const node&) const;
	bool operator!=(const node&) const;

	bool operator==(const std::string&) const;
	bool operator==(int64_t) const;

	const node& operator[](size_t n) const;
	const node& operator[](const node& v) const;
	const node& operator[](const std::string& key) const;

	node operator()(const node& args);
	node operator()(node args);

	bool has_key(const node& v) const;
	bool has_key(const std::string& key) const;
			
	void write_json(std::ostream& s, bool pretty=true, int indent=0) const;
protected:
private:
	node_type type_;

	bool b_;
	int64_t i_;
	float f_;
	std::string s_;
	node_map m_;
	node_list l_;
	node_fn fn_;
};

std::ostream& operator<<(std::ostream& os, const node& n);
