#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <map>

#include "asserts.hpp"
#include "unit_test.hpp"

namespace test {

	namespace 
	{
		typedef std::map<std::string, unit_test> test_map;
		test_map& get_test_map()
		{
			static test_map map;
			return map;
		}
	}

	int register_test(const std::string& name, unit_test test)
	{
		get_test_map()[name] = test;
		return 0;
	}

	bool run_tests(const std::vector<std::string>* tests)
	{
		boost::posix_time::ptime mst1 = boost::posix_time::microsec_clock::local_time();

		std::vector<std::string> all_tests;
		if(!tests) {
			for(test_map::const_iterator i = get_test_map().begin(); i != get_test_map().end(); ++i) {
				all_tests.push_back(i->first);
			}

			tests = &all_tests;
		}

		int npass = 0, nfail = 0;
		for(const auto& test : *tests) {
			try {
				get_test_map()[test]();
				LOG_INFO("TEST " << test << " PASSED");
				++npass;
			} catch(failure_exception&) {
				LOG_ERROR("TEST " << test << " FAILED!!");
				++nfail;
			}
		}

		if(nfail) {
			LOG_ERROR(npass << " TESTS PASSED, " << nfail << " TESTS FAILED");
			return false;
		} else {
			boost::posix_time::ptime mst2 = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration msdiff = mst2 - mst1;
			LOG_INFO("ALL " << npass << " TESTS PASSED IN " << msdiff.total_milliseconds() << "ms");
			return true;
		}
	}
}
