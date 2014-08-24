#include <sstream>
#include "asserts.hpp"
#include "json.hpp"
#include "node.hpp"
#include "unit_test.hpp"

node::node()
	: type_(NODE_TYPE_NULL), i_(0), f_(0.0f), b_(false)
{
}

node::node(const node& rhs) 
	: type_(rhs.type()), i_(0), f_(0.0f), b_(false)
{
	switch(type_) {
	case NODE_TYPE_NULL:				 break;
	case NODE_TYPE_INTEGER:	i_ = rhs.i_; break;
	case NODE_TYPE_FLOAT:	f_ = rhs.f_; break;
	case NODE_TYPE_BOOL:	b_ = rhs.b_; break;
	case NODE_TYPE_STRING:	s_ = rhs.s_; break;
	case NODE_TYPE_MAP:		m_ = rhs.m_; break;
	case NODE_TYPE_LIST:	l_ = rhs.l_; break;
	case NODE_TYPE_FUNCTION: fn_ = rhs.fn_; break;
	default:
		ASSERT_LOG(false, "Unrecognised type in copy constructor: " << type_);
	}
}

node::node(int64_t n)
	: type_(NODE_TYPE_INTEGER), i_(n), f_(0.0f), b_(false)
{
}

node::node(int n)
	: type_(NODE_TYPE_INTEGER), i_(n), f_(0.0f), b_(false)
{
}

node::node(unsigned n)
	: type_(NODE_TYPE_INTEGER), i_(n), f_(0.0f), b_(false)
{
}

node::node(float f)
	: type_(NODE_TYPE_FLOAT), i_(0), f_(f), b_(false)
{
}

node::node(double f)
: type_(NODE_TYPE_FLOAT), i_(0), f_(static_cast<float>(f)), b_(false)
{
}

node::node(const std::string& s)
	: type_(NODE_TYPE_STRING), i_(0), f_(0.0f), s_(s), b_(false)
{
}

node::node(const char* s)
	: type_(NODE_TYPE_STRING), i_(0), f_(0.0f), s_(s), b_(false)
{
}

node::node(const std::map<node,node>& m)
	: type_(NODE_TYPE_MAP), i_(0), f_(0.0f), m_(m), b_(false)
{
}

node::node(const std::vector<node>& l)
	: type_(NODE_TYPE_LIST), i_(0), f_(0.0f), l_(l), b_(false)
{
}

node::node(node_map* m)
	: type_(NODE_TYPE_MAP), i_(0), f_(0.0f), b_(false)
{
	m_.swap(*m);
}

node::node(node_list* l)
	: type_(NODE_TYPE_LIST), i_(0), f_(0.0f), b_(false)
{
	l_.swap(*l);
}

node::node(node_fn fn)
	: type_(NODE_TYPE_FUNCTION), i_(0), f_(0.0f), b_(false), fn_(fn)
{
}

node node::from_bool(bool b)
{
	node n;
	n.type_ = NODE_TYPE_BOOL;
	n.b_ = b;
	return n;
}


std::string node::type_as_string() const
{
	switch(type_) {
	case NODE_TYPE_NULL:
		return "null";
	case NODE_TYPE_INTEGER:
		return "int";
	case NODE_TYPE_FLOAT:
		return "float";
	case NODE_TYPE_BOOL:
		return "bool";
	case NODE_TYPE_STRING:
		return "string";
	case NODE_TYPE_MAP:
		return "map";
	case NODE_TYPE_LIST:
		return "list";
	case NODE_TYPE_FUNCTION:
		return "function";
	}
	ASSERT_LOG(false, "Unrecognised type converting to string: " << type_);
}

int64_t node::as_int() const
{
	switch(type()) {
	case NODE_TYPE_INTEGER:
		return i_;
	case NODE_TYPE_FLOAT:
		return int64_t(f_);
	case NODE_TYPE_BOOL:
		return b_ ? 1 : 0;
	}
	ASSERT_LOG(false, "as_int() type conversion error from " << type_as_string() << " to int");
	return 0;
}

std::string node::as_string() const
{
	switch(type()) {
	case NODE_TYPE_STRING:
		return s_;
	case NODE_TYPE_INTEGER: {
		std::stringstream s;
		s << i_;
		return s.str();
	}
	case NODE_TYPE_FLOAT: {
		std::stringstream s;
		s << f_;
		return s.str();
	}
	}
	ASSERT_LOG(false, "as_string() type conversion error from " << type_as_string() << " to string");
	return "";
}

float node::as_float() const
{
	switch(type()) {
	case NODE_TYPE_INTEGER:
		return float(i_);
	case NODE_TYPE_FLOAT:
		return f_;
	case NODE_TYPE_BOOL:
		return b_ ? 1.0f : 0.0f;
	}
	ASSERT_LOG(false, "as_float() type conversion error from " << type_as_string() << " to float");
	return 0;
}

bool node::as_bool() const
{
	switch(type()) {
	case NODE_TYPE_INTEGER:
		return i_ ? true : false;
	case NODE_TYPE_FLOAT:
		return f_ == 0.0f ? false : true;
	case NODE_TYPE_BOOL:
		return b_;
	case NODE_TYPE_STRING:
		return s_.empty() ? false : true;
	case NODE_TYPE_LIST:
		return l_.empty() ? false : true;
	case NODE_TYPE_MAP:
		return m_.empty() ? false : true;
	}
	ASSERT_LOG(false, "as_bool() type conversion error from " << type_as_string() << " to boolean");
	return 0;
}

const node_list& node::as_list() const
{
	ASSERT_LOG(type() == NODE_TYPE_LIST, "as_list() type conversion error from " << type_as_string() << " to list");
	return l_;
}

std::vector<std::string> node::as_list_strings() const
{
	std::vector<std::string> res;
	ASSERT_LOG(type() == NODE_TYPE_LIST, "as_list_strings() type conversion error from " << type_as_string() << " to list");
	res.reserve(l_.size());
	for(auto& n : l_) {
		ASSERT_LOG(n.is_string(), "type conversion error from " << type_as_string() << " to string");
		res.emplace_back(n.as_string());
	}
	return res;
}

const node_map& node::as_map() const
{
	ASSERT_LOG(type() == NODE_TYPE_MAP, "as_map() type conversion error from " << type_as_string() << " to map");
	return m_;
}

node_list& node::as_mutable_list()
{
	ASSERT_LOG(type() == NODE_TYPE_LIST, "as_mutable_list() type conversion error from " << type_as_string() << " to list");
	return l_;
}

node_map& node::as_mutable_map()
{
	ASSERT_LOG(type() == NODE_TYPE_MAP, "as_mutable_map() type conversion error from " << type_as_string() << " to map");
	return m_;
}

const node_fn node::as_function() const
{
	ASSERT_LOG(type() == NODE_TYPE_FUNCTION, "as_function() type conversion error from " << type_as_string() << " to function");
	return fn_;
}

bool node::operator<(const node& n) const
{
	if(type() != n.type()) {
		return type_ < n.type();
	}
	switch(type()) {
	case NODE_TYPE_NULL:
		return true;
	case NODE_TYPE_BOOL:
		return b_ < n.b_;
	case NODE_TYPE_INTEGER:
		return i_ < n.i_;
	case NODE_TYPE_FLOAT:
		return f_ < n.f_;
	case NODE_TYPE_STRING:
		return s_ < n.s_;
	case NODE_TYPE_MAP:
		return m_.size() < n.m_.size();
	case NODE_TYPE_LIST:
		for(int i = 0; i != l_.size() && i != n.l_.size(); ++i) {
			if(l_[i] < n.l_[i]) {
				return true;
			} else if(l_[i] > n.l_[i]) {
				return false;
			}
		}
		return l_.size() < n.l_.size();
	case NODE_TYPE_FUNCTION:
	default:
		break;
	}
	ASSERT_LOG(false, "operator< unknown type: " << type_as_string());
	return false;
}

bool node::operator>(const node& n) const
{
	return !(*this < n);
}

const node& node::operator[](size_t n) const
{
	ASSERT_LOG(type() == NODE_TYPE_LIST, "Tried to index node that isn't a list, was: " << type_as_string());
	ASSERT_LOG(n < l_.size(), "Tried to index a list outside of list bounds: " << n << " >= " << l_.size());
	return l_[n];
}

const node& node::operator[](const node& v) const
{
	if(type() == NODE_TYPE_LIST) {
		return l_[size_t(v.as_int())];
	} else if(type() == NODE_TYPE_MAP) {
		auto it = m_.find(v);
		ASSERT_LOG(it != m_.end(), "Couldn't find key in map");
		return it->second;
	} else {
		ASSERT_LOG(false, "Tried to index a node that isn't a list or map: " << type_as_string());
	}
}

const node& node::operator[](const std::string& key) const
{
	ASSERT_LOG(type() == NODE_TYPE_MAP, "Tried to index node that isn't a map, was: " << type_as_string());
	auto it = m_.find(node(key));
	ASSERT_LOG(it != m_.end(), "Couldn't find key(" << key << ") in map");
	return it->second;
}

node node::operator()(const node& args)
{
	ASSERT_LOG(type() == NODE_TYPE_FUNCTION, "Tried to execute node that wasn't a function, was: " << type_as_string());
	return fn_(args);
}

node node::operator()(node args)
{
	ASSERT_LOG(type() == NODE_TYPE_FUNCTION, "Tried to execute node that wasn't a function, was: " << type_as_string());
	return fn_(args);
}

bool node::has_key(const node& v) const
{
	if(type() == NODE_TYPE_LIST) {
		return v.as_int() < l_.size() ? true : false;
	} else if(type() == NODE_TYPE_MAP) {
		return m_.find(v) != m_.end() ? true : false;
	} else {
		ASSERT_LOG(false, "Tried to index a node that isn't a list or map: " << type_as_string());
	}
	return false;
}

bool node::has_key(const std::string& key) const
{
	ASSERT_LOG(type() == NODE_TYPE_MAP, "Tried to index node that isn't a map, was: " << type_as_string());
	return m_.find(node(key)) != m_.end() ? true : false;
}

bool node::operator==(const std::string& s) const
{
	return *this == node(s);
}

bool node::operator==(int64_t n) const
{
	return *this == node(n);
}

bool node::operator==(const node& n) const
{
	if(type() != n.type()) {
		if(type() == NODE_TYPE_FLOAT || n.type() == NODE_TYPE_FLOAT) {
			if(!is_numeric() && !is_null() || !n.is_numeric() && !n.is_null()) {
				return false;
			}
			return is_numeric() == n.is_numeric();
		}
		return false;
	}
	switch(type()) {
	case NODE_TYPE_NULL:
		return n.is_null();
	case NODE_TYPE_BOOL:
		return b_ == n.b_;
	case NODE_TYPE_INTEGER:
		return i_ == n.i_;
	case NODE_TYPE_FLOAT:
		return f_ == n.f_;
	case NODE_TYPE_STRING:
		return s_ == n.s_;
	case NODE_TYPE_MAP:
		return m_ == n.m_;
	case NODE_TYPE_LIST:
		if(l_.size() != n.l_.size()) {
			return false;
		}
		for(size_t ndx = 0; ndx != l_.size(); ++ndx) {
			if(l_[ndx] != n.l_[ndx]) {
				return false;
			}
		}
		return true;
	}
	return false;

}

bool node::operator!=(const node& n) const
{
	return !operator==(n);
}

void node::write_json(std::ostream& os, bool pretty, int indent) const
{
	switch(type()) {
	case NODE_TYPE_NULL:
		os << "null";
		break;
	case NODE_TYPE_BOOL:
		os << b_ ? "true" : "false";
		break;
	case NODE_TYPE_INTEGER:
		os << i_;
		break;
	case NODE_TYPE_FLOAT:
		os << f_;
		break;
	case NODE_TYPE_STRING:
		for(auto it = s_.begin(); it != s_.end(); ++it) {
			if(*it == '"') {
				os << "\\\"";
			} else if(*it == '\\') {
				os << "\\\\";
			} else if(*it == '/') {
				os << "\\/";
			} else if(*it == '\b') {
				os << "\\b";
			} else if(*it == '\f') {
				os << "\\f";
			} else if(*it == '\n') {
				os << "\\n";
			} else if(*it == '\r') {
				os << "\\r";
			} else if(*it == '\t') {
				os << "\\t";
			} else if(*it > 128) {
				uint8_t value = uint8_t(*it);
				uint16_t code_point = 0;
				if(value & 0xe0) {
					code_point = (value & 0x1f) << 12;
					value = uint8_t(*it++);
					code_point |= (value & 0x7f) << 6;
					value = uint8_t(*it++);
					code_point |= (value & 0x7f);
				} else if(value & 0xc0) {
					code_point = (value & 0x3f) << 6;
					value = uint8_t(*it++);
					code_point |= (value & 0x7f);
				}
			} else {
				os << *it;
			}
		}
		break;
	case NODE_TYPE_MAP:
		os << pretty ? "{\n" + std::string(' ', indent) : "{";
		for(auto pr = m_.begin(); pr != m_.end(); ++pr) {
			if(pr != m_.begin()) {
				os << pretty ? ",\n" + std::string(' ', indent) : ",";
			}
			pr->first.write_json(os, pretty, indent + 4);
			os << pretty ? ": " : ":";
			pr->second.write_json(os, pretty, indent + 4);
		}
		os << pretty ? "\n" + std::string(' ', indent) + "}" : "}";
	case NODE_TYPE_LIST:
		os << pretty ? "[\n" + std::string(' ', indent) : "[";
		for(auto it = l_.begin(); it != l_.end(); ++it) {
			if(it != l_.begin()) {
				os << pretty ? ",\n" + std::string(' ', indent) : ",";
			}
			it->write_json(os, pretty, indent + 4);
		}
		os << pretty ? "\n" + std::string(' ', indent) + "]" : "]";
		break;
	}
}

unsigned node::num_elements() const
{
	switch(type()) {
		case NODE_TYPE_NULL:
			return 0;
		case NODE_TYPE_BOOL:
		case NODE_TYPE_INTEGER:
		case NODE_TYPE_FLOAT:
		case NODE_TYPE_FUNCTION:
			return 1;
		case NODE_TYPE_STRING:
			return s_.size();
		case NODE_TYPE_MAP:
			return m_.size();
		case NODE_TYPE_LIST:
			return l_.size();
		default:
			ASSERT_LOG(false, "Unrecognised type: " << type_);
	}
	return 0;
}

std::ostream& operator<<(std::ostream& os, const node& n)
{
	n.write_json(os);
	return os;
}

UNIT_TEST(node_tests)
{
	CHECK_EQ(node("abcde"), node("abcde"));
}
