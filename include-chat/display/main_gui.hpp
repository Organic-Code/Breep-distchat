#ifndef SYSDIST_CHAT_UI_HPP
#define SYSDIST_CHAT_UI_HPP

/*************************************************************************************
 * MIT License                                                                       *
 *                                                                                   *
 * Copyright (c) 2018 TiWinDeTea                                                     *
 *                                                                                   *
 * Permission is hereby granted, free of charge, to any person obtaining a copy      *
 * of this software and associated documentation files (the "Software"), to deal     *
 * in the Software without restriction, including without limitation the rights      *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell         *
 * copies of the Software, and to permit persons to whom the Software is             *
 * furnished to do so, subject to the following conditions:                          *
 *                                                                                   *
 * The above copyright notice and this permission notice shall be included in all    *
 * copies or substantial portions of the Software.                                   *
 *                                                                                   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER            *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,     *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     *
 * SOFTWARE.                                                                         *
 *                                                                                   *
 *************************************************************************************/

#include <SFML/Graphics/RenderWindow.hpp>
#include <mutex>
#include <functional>
#include <imgui.h>
#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/container_hash/extensions.hpp>

#include "formatted_message.hpp"

namespace display {
	enum class colors {
		red = 0xDC143C,
		blue = 0x00BFFF,
		green = 0x00FF7F,
		cyan = 0xE0FFFF,
		pink = 0xFF69B4,
		yellow = 0xFAFAD2,
		white = 0xFFFFF0,
		system = 0x6495ED
	};

	bool is_instanciated();

	class main_gui {

		using theme_fnct = void (*)();
		struct msg_list {
			void print() {
				for (auto&& msg : messages) {
					msg.print();
				}
			}

			template <typename... Args>
			void emplace_back(Args&&... args) {
				messages.emplace_back(std::forward<Args>(args)...);
			}

			std::vector<formatted_message> messages{};
			bool can_send_messages{};
		};

	public:
		main_gui();

		~main_gui();

		/* return is_open */
		bool display();

		bool is_open() { return window.isOpen(); }

		void add_message(const std::string& source, formatted_message&& msg) {
			std::lock_guard lg{msg_mutex};
			auto source_messages = messages.find(source);
			if (source_messages != messages.end()) {
				source_messages->second.emplace_back(std::move(msg));
			}
			if (!focused_user || source != *focused_user) {
				new_messages.emplace(source);
			}
		}

		void set_textinput_callback(std::function<void(std::string_view)> tic) {
			textinput_callback = std::move(tic);
		}

		void add_user(std::string_view username) {
			std::scoped_lock lock{msg_mutex};
			messages[std::string(username)].can_send_messages = true;
		}

		void remove_user(const std::string& username) {
			std::scoped_lock lock{msg_mutex};
			auto source_messages = messages.find(username);
			if (source_messages != messages.end()) {
				source_messages->second.can_send_messages = false;
			}
		}

		const std::optional<std::string>& get_focused_name() const {
			return focused_user;
		}

	private:
		void update_frame();

		ImVec4 new_message_color{0.502f, 0.075f, 0.256f, 1.f};
		ImVec4 pop_up_new_message_color{0.502f, 0.075f, 0.256f, 1.f};

		theme_fnct new_theme{nullptr};
		theme_fnct current_theme{nullptr};

		sf::RenderWindow window;
		sf::Clock clk;
		const int frame_flags;

		std::mutex msg_mutex{};
		std::unordered_map<std::string, msg_list> messages{};
		std::set<std::string> new_messages{};

		std::string textinput_buffer{};

		std::function<void(std::string_view)> textinput_callback{};

		std::optional<std::string> focused_user{};
	};
}

#endif //SYSDIST_CHAT_UI_HPP
