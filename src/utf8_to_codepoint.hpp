/*
   Copyright 2014 Kristina Simpson <sweet.kristas@gmail.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#pragma once

#include <iterator>
#include <string>
#include <cstdint>

namespace utils
{
	class utf8_to_codepoint
	{
	public:
		class iterator : public std::iterator<std::forward_iterator_tag, char32_t, std::string::difference_type, const char32_t*, const char32_t&>
		{
		public:
			iterator(std::string::const_iterator it) 
				: it_(it),
				utf8_bitmask_1(0x80),
				utf8_bitmask_2(0x40),
				utf8_bitmask_3(0x20),
				utf8_bitmask_4(0x10)
			{}
			~iterator() {}
			bool operator!= (const iterator& other) const
			{
				return it_ != other.it_;
			}
			char32_t operator*() 
			{
				char32_t codepoint = 0;
				std::string::const_iterator it(it_);
				auto c = *it++;
				if(c & utf8_bitmask_1) {
					if(c & utf8_bitmask_3) {
						if(c & utf8_bitmask_4) {
							// U = (C1 ? 240) * 262,144 + (C2 ? 128) * 4,096 + (C3 ? 128) * 64 + C4 ? 128
							codepoint = (c - 240) * 262144;
							c = *it++;
							codepoint += (c - 128) * 4096;
							c = *it++;
							codepoint += (c - 128) * 64;
							c = *it++;
							codepoint += c - 128;
						} else {
							// U = (C1 ? 224) * 4,096 + (C2 ? 128) * 64 + C3 ? 128
							codepoint = (c - 224) * 4096;
							c = *it++;
							codepoint += (c - 128) * 64;
							c = *it++;
							codepoint += c - 128;
						}
					} else {
						// U = (C1 ? 192) * 64 + C2 ? 128
						codepoint = (c - 192) * 64;
						c = *it++;
						codepoint += c - 128;
					}
				} else {
					// U = C1
					codepoint = c;
				}
				return codepoint;
			}
			iterator& operator++()
			{
				std::string::difference_type offset = 1;
				auto c = *it_;
				if(c & utf8_bitmask_1) {
					if(c & utf8_bitmask_3) {
						if(c & utf8_bitmask_4) {
							offset = 4;
						} else {
							offset = 3;
						}
					} else {
						offset = 2;
					}
				}
				std::advance(it_, offset);
				return *this;
			}
		private:
			std::string::const_iterator it_;
			const uint8_t utf8_bitmask_1;
			const uint8_t utf8_bitmask_2;
			const uint8_t utf8_bitmask_3;
			const uint8_t utf8_bitmask_4;
		};

		iterator begin() const { return iterator(utf8_.begin()); }
		iterator end() const { return iterator(utf8_.end()); }

		utf8_to_codepoint(const std::string& s) : utf8_(s) {}
	private:
		std::string utf8_;
		utf8_to_codepoint();
	};
}
