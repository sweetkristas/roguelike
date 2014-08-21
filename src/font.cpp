#include <map>

#include "asserts.hpp"
#include "filesystem.hpp"
#include "font.hpp"
#include "notify.hpp"

namespace font
{
	namespace
	{
		const char* default_font_path = "data/fonts/";

		typedef std::pair<std::string, int> font_pair;
		typedef std::map<font_pair, font_ptr> font_map;
		font_map font_table;

		std::map<std::string,std::string>& get_font_list()
		{
			static std::map<std::string,std::string> res;
			return res;
		}

		void font_file_changed(const std::string& file, const boost::asio::dir_monitor_event& ev)
		{
			if(ev.type == boost::asio::dir_monitor_event::modified) {
				// XXX
				auto& res = get_font_list();
				sys::get_unique_files(default_font_path, res);
			}
		}

		const std::string& get_font_path(const std::string& name) 
		{
			auto& res = get_font_list();
			if(res.empty()) {
				sys::get_unique_files(default_font_path, res);
				notify::register_notification_path(default_font_path, font_file_changed);
			}
			auto itor = res.find(name);
			if(itor == res.end()) {
				ASSERT_LOG(false, "Font file not found: " << name);
			}
			return itor->second;
		}
	}

	font_ptr get_font(const std::string& font_name, int size)
	{
		const std::string& font_path = get_font_path(font_name);
		auto it = font_table.find(std::make_pair(font_path, size));
		if(it == font_table.end()) {
			TTF_Font* font = TTF_OpenFont(font_path.c_str(), size);
			ASSERT_LOG(font != NULL, "Unable to open font: " << font_name);
			font_ptr new_font(font, TTF_CloseFont);
			font_table[std::make_pair(font_path, size)] = new_font;
			return new_font;
		}
		return it->second;
	}

	manager::manager()
	{
		ASSERT_LOG(TTF_Init() != -1, "TTF_Init error(): " << TTF_GetError());
	}

	manager::~manager()
	{
		font_table.clear();
		TTF_Quit();
	}
}