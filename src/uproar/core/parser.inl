#include "mapper.hpp"
#include "parser.hpp"

namespace tc {
	namespace schema {
		namespace grammar {
			namespace json {
				// https://datatracker.ietf.org/doc/html/rfc7159#section-2

				/**
				 * @brief These are the six structural characters:
				 * 
				 * begin-array     = ws %x5B ws  ; [ left square bracket
				 * begin-object    = ws %x7B ws  ; { left curly bracket
				 * end-array       = ws %x5D ws  ; ] right square bracket
				 * end-object      = ws %x7D ws  ; } right curly bracket
				 * name-separator  = ws %x3A ws  ; : colon
				 * value-separator = ws %x2C ws  ; , comma
				 * 
				 */
				static bool is_begin_array(lexer_transaction &ctx) {
					code_point c;
					return is_match(ctx, '[', c);
				}

				/**
				 * @brief These are the six structural characters:
				 * 
				 * begin-array     = ws %x5B ws  ; [ left square bracket
				 * begin-object    = ws %x7B ws  ; { left curly bracket
				 * end-array       = ws %x5D ws  ; ] right square bracket
				 * end-object      = ws %x7D ws  ; } right curly bracket
				 * name-separator  = ws %x3A ws  ; : colon
				 * value-separator = ws %x2C ws  ; , comma
				 * 
				 */
				static bool is_begin_object(lexer_transaction &ctx) {
					code_point c;
					return is_match(ctx, '{', c);
				}

				/**
				 * @brief These are the six structural characters:
				 * 
				 * begin-array     = ws %x5B ws  ; [ left square bracket
				 * begin-object    = ws %x7B ws  ; { left curly bracket
				 * end-array       = ws %x5D ws  ; ] right square bracket
				 * end-object      = ws %x7D ws  ; } right curly bracket
				 * name-separator  = ws %x3A ws  ; : colon
				 * value-separator = ws %x2C ws  ; , comma
				 * 
				 */
				static bool is_end_array(lexer_transaction &ctx) {
					code_point c;
					return is_match(ctx, ']', c);
				}

				/**
				 * @brief These are the six structural characters:
				 * 
				 * begin-array     = ws %x5B ws  ; [ left square bracket
				 * begin-object    = ws %x7B ws  ; { left curly bracket
				 * end-array       = ws %x5D ws  ; ] right square bracket
				 * end-object      = ws %x7D ws  ; } right curly bracket
				 * name-separator  = ws %x3A ws  ; : colon
				 * value-separator = ws %x2C ws  ; , comma
				 * 
				 */
				static bool is_end_object(lexer_transaction &ctx) {
					code_point c;
					return is_match(ctx, '}', c);
				}

				/**
				 * @brief These are the six structural characters:
				 * 
				 * begin-array     = ws %x5B ws  ; [ left square bracket
				 * begin-object    = ws %x7B ws  ; { left curly bracket
				 * end-array       = ws %x5D ws  ; ] right square bracket
				 * end-object      = ws %x7D ws  ; } right curly bracket
				 * name-separator  = ws %x3A ws  ; : colon
				 * value-separator = ws %x2C ws  ; , comma
				 * 
				 */
				static bool is_name_separator(lexer_transaction &ctx) {
					code_point c;
					return is_match(ctx, ':', c);
				}

				/**
				 * @brief These are the six structural characters:
				 * 
				 * begin-array     = ws %x5B ws  ; [ left square bracket
				 * begin-object    = ws %x7B ws  ; { left curly bracket
				 * end-array       = ws %x5D ws  ; ] right square bracket
				 * end-object      = ws %x7D ws  ; } right curly bracket
				 * name-separator  = ws %x3A ws  ; : colon
				 * value-separator = ws %x2C ws  ; , comma
				 * 
				 */
				static bool is_value_separator(lexer_transaction &ctx) {
					code_point c;
					return is_match(ctx, ',', c);
				}

				/**
				 * @brief Insignificant whitespace is allowed before or after any of the six structural characters.
				 * ws = *(
				 *  %x20 /              ; Space
				 *  %x09 /              ; Horizontal tab
				 *  %x0A /              ; Line feed or New line
				 *  %x0D )              ; Carriage return
				 */
				bool is_whitespace(code_point c) {
					const char insignificant_ws[] = " \t\n\r";
					for (auto i : insignificant_ws) {
						if (c == i) {
							return true;
						}
					}
					return false;
				}

				bool is_new_line(code_point c) {
					const char insignificant_ws[] = "\n\r";
					for (auto i : insignificant_ws) {
						if (c == i) {
							return true;
						}
					}
					return false;
				}

				static bool is_whitespace(lexer_transaction &ctx) {
					code_point c;
					int32_t i{ 0 };
					while (is_match(ctx, &grammar::json::is_whitespace, c)) {
						if (grammar::json::is_new_line(c)) {
							ctx.new_line();
						}
						++i;
					}
					return i > 0;
				}

				/**
				 * A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true
				 * 
				 * The literal names MUST be lowercase.  No other literal names are allowed.
				 * 
				 * value = false / null / true / object / array / number / string
				 * false = %x66.61.6c.73.65   ; false
				 * null  = %x6e.75.6c.6c      ; null
				 * true  = %x74.72.75.65      ; true
				 */
				static bool is_literal_null(lexer_transaction &ctx) {
					return is_match(ctx, "null");
				}

				/**
				 * A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true
				 * 
				 * The literal names MUST be lowercase.  No other literal names are allowed.
				 * 
				 * value = false / null / true / object / array / number / string
				 * false = %x66.61.6c.73.65   ; false
				 * null  = %x6e.75.6c.6c      ; null
				 * true  = %x74.72.75.65      ; true
				 */
				static bool is_literal_true(lexer_transaction &ctx) {
					return is_match(ctx, "true");
				}

				/**
				 * A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true
				 * 
				 * The literal names MUST be lowercase.  No other literal names are allowed.
				 * 
				 * value = false / null / true / object / array / number / string
				 * false = %x66.61.6c.73.65   ; false
				 * null  = %x6e.75.6c.6c      ; null
				 * true  = %x74.72.75.65      ; true
				 */
				static bool is_literal_false(lexer_transaction &ctx) {
					return is_match(ctx, "false");
				}

				/**
				 * number = [ minus ] int [ frac ] [ exp ]
				 * decimal-point = %x2E       ; .
				 * digit1-9 = %x31-39         ; 1-9
				 * e = %x65 / %x45            ; e E
				 * exp = e [ minus / plus ] 1*DIGIT
				 * frac = decimal-point 1*DIGIT
				 * int = zero / ( digit1-9 *DIGIT )
				 * minus = %x2D               ; -
				 * plus = %x2B                ; +
				 * zero = %x30                ; 0
				 * 
				 */
				static bool is_digit(code_point c) {
					return '0' <= c && c <= '9';
				}

				/**
				 * number = [ minus ] int [ frac ] [ exp ]
				 * decimal-point = %x2E       ; .
				 * digit1-9 = %x31-39         ; 1-9
				 * e = %x65 / %x45            ; e E
				 * exp = e [ minus / plus ] 1*DIGIT
				 * frac = decimal-point 1*DIGIT
				 * int = zero / ( digit1-9 *DIGIT )
				 * minus = %x2D               ; -
				 * plus = %x2B                ; +
				 * zero = %x30                ; 0
				 * 
				 */
				static bool is_digit1_9(code_point c) {
					return '0' < c && c <= '9';
				}

				/**
				 * number = [ minus ] int [ frac ] [ exp ]
				 * decimal-point = %x2E       ; .
				 * digit1-9 = %x31-39         ; 1-9
				 * e = %x65 / %x45            ; e E
				 * exp = e [ minus / plus ] 1*DIGIT
				 * frac = decimal-point 1*DIGIT
				 * int = zero / ( digit1-9 *DIGIT )
				 * minus = %x2D               ; -
				 * plus = %x2B                ; +
				 * zero = %x30                ; 0
				 * 
				 */
				static bool is_integer(lexer_transaction &ctx) {
					code_point c;
					if (is_match(ctx, '0', c)) {
						return true;
					}

					if (!is_match(ctx, is_digit1_9, c)) {
						return false;
					}

					while (is_match(ctx, is_digit, c)) {
						// Do nothing with the value
					}

					return true;
				}

				/**
				 * number = [ minus ] int [ frac ] [ exp ]
				 * decimal-point = %x2E       ; .
				 * digit1-9 = %x31-39         ; 1-9
				 * e = %x65 / %x45            ; e E
				 * exp = e [ minus / plus ] 1*DIGIT
				 * frac = decimal-point 1*DIGIT
				 * int = zero / ( digit1-9 *DIGIT )
				 * minus = %x2D               ; -
				 * plus = %x2B                ; +
				 * zero = %x30                ; 0
				 * 
				 */
				static bool is_fractional(lexer_transaction &ctx) {
					code_point c;
					if (!is_match(ctx, '.', c)) {
						return false;
					}

					if (!is_match(ctx, is_digit, c)) {
						return false;
					}

					while (is_match(ctx, is_digit, c)) {
						// Do nothing with the value
					}

					return true;
				}

				/**
				 * number = [ minus ] int [ frac ] [ exp ]
				 * decimal-point = %x2E       ; .
				 * digit1-9 = %x31-39         ; 1-9
				 * e = %x65 / %x45            ; e E
				 * exp = e [ minus / plus ] 1*DIGIT
				 * frac = decimal-point 1*DIGIT
				 * int = zero / ( digit1-9 *DIGIT )
				 * minus = %x2D               ; -
				 * plus = %x2B                ; +
				 * zero = %x30                ; 0
				 * 
				 */
				static bool is_exponent(lexer_transaction &ctx) {
					code_point c;
					if (!is_match(ctx, { 'e', 'E' }, c)) {
						return false;
					}

					is_match(ctx, { '-', '+' }, c);

					if (!is_match(ctx, is_digit, c)) {
						return false;
					}

					while (is_match(ctx, is_digit, c)) {
						// Do nothing with the value
					}

					return true;
				}

				/**
				 * number = [ minus ] int [ frac ] [ exp ]
				 * decimal-point = %x2E       ; .
				 * digit1-9 = %x31-39         ; 1-9
				 * e = %x65 / %x45            ; e E
				 * exp = e [ minus / plus ] 1*DIGIT
				 * frac = decimal-point 1*DIGIT
				 * int = zero / ( digit1-9 *DIGIT )
				 * minus = %x2D               ; -
				 * plus = %x2B                ; +
				 * zero = %x30                ; 0
				 * 
				 */
				static bool is_number(lexer_transaction &ctx) {
					code_point c;
					// [ minus ] optional
					is_match(ctx, '-', c);
					if (!is_integer(ctx)) {
						return false;
					}

					lexer_transaction sub_ctx{ ctx };
					if (is_fractional(sub_ctx)) {
						ctx = sub_ctx;
					}

					if (is_exponent(sub_ctx)) {
						ctx = sub_ctx;
					}

					return true;
				}

				/**
				 * string = quotation-mark *char quotation-mark
				 * char = unescaped /
				 * escape (
				 *  %x22 /          ; "    quotation mark  U+0022
				 *  %x5C /          ; \    reverse solidus U+005C
				 *  %x2F /          ; /    solidus         U+002F
				 *  %x62 /          ; b    backspace       U+0008
				 *  %x66 /          ; f    form feed       U+000C
				 *  %x6E /          ; n    line feed       U+000A
				 *  %x72 /          ; r    carriage return U+000D
				 *  %x74 /          ; t    tab             U+0009
				 *  %x75 4HEXDIG )  ; uXXXX                U+XXXX
				 * 
				 * escape = %x5C              ; \
				 * quotation-mark = %x22      ; "
				 * unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
				 * 
				 */
				bool is_wildcard(code_point c) {
					return c != '\0';
				}

				/**
				 * string = quotation-mark *char quotation-mark
				 * char = unescaped /
				 * escape (
				 *  %x22 /          ; "    quotation mark  U+0022
				 *  %x5C /          ; \    reverse solidus U+005C
				 *  %x2F /          ; /    solidus         U+002F
				 *  %x62 /          ; b    backspace       U+0008
				 *  %x66 /          ; f    form feed       U+000C
				 *  %x6E /          ; n    line feed       U+000A
				 *  %x72 /          ; r    carriage return U+000D
				 *  %x74 /          ; t    tab             U+0009
				 *  %x75 4HEXDIG )  ; uXXXX                U+XXXX
				 * 
				 * escape = %x5C              ; \
				 * quotation-mark = %x22      ; "
				 * unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
				 * 
				 */
				static bool is_string(lexer_transaction &ctx) {
					code_point c;
					if (!is_match(ctx, '"', c)) {
						return false;
					}

					code_point prev = c;
					while (is_match(ctx, grammar::json::is_wildcard, c)) {
						if (c == '"' && prev != '\\') {
							return true;
						}
						prev = c;
					}

					return false;
				}
			}    // namespace json
		}    // namespace grammar

		bool is_match(lexer_transaction &ctx, code_point_matcher matcher, code_point &out) {
			if (ctx.eof() || !matcher(ctx.peek())) {
				return false;
			}
			out = ctx.peek();
			return ctx.advance(), true;
		}

		bool is_match(lexer_transaction &ctx, code_point cmp, code_point &out) {
			if (ctx.eof() || cmp != ctx.peek()) {
				return false;
			}
			out = ctx.peek();
			return ctx.advance(), true;
		}

		bool is_match(lexer_transaction &ctx, std::initializer_list<code_point> any_of, code_point &out) {
			for (auto i : any_of) {
				if (is_match(ctx, i, out)) {
					return true;
				}
			}
			return false;
		}

		bool is_match(lexer_transaction &ctx, std::string_view cmp) {
			code_point c;
			for (uint32 i = 0; i < cmp.size(); ++i) {
				if (!is_match(ctx, cmp[i], c)) {
					return false;
				}
			}
			return true;
		}

		lexer_transaction::lexer_transaction(schema_lexer &lexer)
			: lexer{ &lexer } {
		}

		lexer_transaction::lexer_transaction(const lexer_transaction &other)
			: lexer{ other.lexer }
			, line{ other.line }
			, column{ other.column }
			, pos{ other.pos } {
		}

		lexer_transaction &lexer_transaction::operator=(const lexer_transaction &other) {
			lexer = other.lexer;
			line = other.line;
			column = other.column;
			pos = other.pos;
			return *this;
		}

		void lexer_transaction::begin() {
			pos = lexer->first;
			line = lexer->line;
			column = lexer->column;
		}

		code_point lexer_transaction::peek() const {
			return static_cast<code_point>(*pos);
		}

		void lexer_transaction::new_line() {
			column = 0;
			++line;
		}

		void lexer_transaction::advance() {
			++column, ++pos;
		}

		bool lexer_transaction::eof() {
			return pos == lexer->last;
		}

		lexer_token lexer_transaction::commit(token_kind kind) {
			lexer_token token{
				kind,
				lexer->line,
				lexer->column,
				uint64(pos - lexer->first),
				lexer->first,
			};

			lexer->line = line;
			lexer->column = column;
			lexer->first = pos;

			return token;
		}

		template<typename Grammar>
		lexer_result schema_lexer::read(lexer_token *token) {
			if (!token) {
				return LR_null;
			}

			auto g = Grammar{};
			lexer_transaction transaction{ *this };
			for (const auto &matcher : g) {
				transaction.begin();
				if (matcher.match(transaction)) {
					*token = transaction.commit(matcher.kind);
					return LR_success;
				}
			}

			return transaction.eof() ? LR_complete : LR_no_match;
		}

		struct json_grammer {
			using grammar_list = std::vector<grammer_matcher>;
			using iterator = grammar_list::iterator;
			enum json_grammar {
				JG_whitespace = 0,
				JG_null /*= 1*/,
				JG_true /*= 2*/,
				JG_false /*= 3*/,
				JG_begin_array /*= 4*/,
				JG_begin_object /*= 5*/,
				JG_end_array /*= 6*/,
				JG_end_object /*= 7*/,
				JG_name_separator /*= 8*/,
				JG_value_separator /*= 9*/,
				JG_number /*= 10*/,
				JG_string /*= 11*/,
			};

			iterator begin() {
				return std::begin(matchers);
			}
			iterator end() {
				return std::end(matchers);
			}

			static bool filter(token_kind kind) {
				return kind == JG_whitespace;
			}

			inline static grammar_list matchers{
				{ &grammar::json::is_whitespace, JG_whitespace },
				{ &grammar::json::is_literal_false, JG_false },
				{ &grammar::json::is_literal_null, JG_null },
				{ &grammar::json::is_literal_true, JG_true },
				{ &grammar::json::is_begin_object, JG_begin_object },
				{ &grammar::json::is_begin_array, JG_begin_array },
				{ &grammar::json::is_end_object, JG_end_object },
				{ &grammar::json::is_end_array, JG_end_array },
				{ &grammar::json::is_name_separator, JG_name_separator },
				{ &grammar::json::is_value_separator, JG_value_separator },
				{ &grammar::json::is_number, JG_number },
				{ &grammar::json::is_string, JG_string },
			};
		};

		struct json_parser {
			void parse_value(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name);
			void parse_struct(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name);
			void parse_array(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name);
			void parse_string(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name);
			void parse_number(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name);
			void parse_null(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name);
			void parse_true(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name);
			void parse_false(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name);
		};

		void parse(parser_interface &interface, std::string_view content) {
			std::vector<lexer_token> tokens;
			tokens.reserve(100);
			{
				schema_lexer lexer{ content.data(), content.data() + content.size() };

				lexer_result result;
				lexer_token token;
				while ((result = lexer.read<json_grammer>(&token)) == LR_success) {
					if (json_grammer::filter(token.kind)) {
						continue;
					}
					tokens.emplace_back(token);
				}

				if (result != LR_complete) {
					// There was a parse error of some form.
					return;
				}
			}

			if (tokens.empty()) {
				// There wasn't anything to parse, also probably a failure?
				return;
			}

			// Perform actual parsing to internal structs here.
			std::string_view name{ "" };
			json_parser parser{};
			lexer_token *cursor = tokens.data();
			parser.parse_value(interface, cursor, tokens.data() + tokens.size(), "");
		}

		void json_parser::parse_value(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name) {
			switch (cursor->kind) {
				case json_grammer::JG_begin_object: {
					parse_struct(interface, cursor, tend, name);
				} break;
				case json_grammer::JG_begin_array: {
					parse_array(interface, cursor, tend, name);
				} break;
				case json_grammer::JG_string: {
					parse_string(interface, cursor, tend, name);
				} break;
				case json_grammer::JG_number: {
					parse_number(interface, cursor, tend, name);
				} break;
				case json_grammer::JG_false: {
					parse_false(interface, cursor, tend, name);
				} break;
				case json_grammer::JG_true: {
					parse_true(interface, cursor, tend, name);
				} break;
				case json_grammer::JG_null: {
					parse_null(interface, cursor, tend, name);
				} break;
			}
		}

		void json_parser::parse_struct(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name) {
			interface.begin_object(name);
			++cursor;

			std::string_view value_name;
			while (cursor < tend && cursor->kind != json_grammer::JG_end_object) {
				switch (cursor->kind) {
					case json_grammer::JG_string: {
						// Strip quotes and set as name
						value_name = std::string_view{ cursor->data + 1, cursor->size - 2 };
						if (++cursor == tend || cursor->kind != json_grammer::JG_name_separator) {
							return;    // Error parsing
						}

						parse_value(interface, ++cursor, tend, value_name);
					} break;
					default: {
						return;    // Parse error only strings are valid in this context.
					}
				}

				if (cursor != tend) {
					if (cursor->kind == json_grammer::JG_value_separator) {
						++cursor;
						continue;
					}

					if (cursor->kind != json_grammer::JG_end_object) {
						return;    // Error parsing
					}
				}
			}

			interface.end_object(name);
			++cursor;
		}

		void json_parser::parse_array(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name) {
			interface.begin_array(name);
			++cursor;

			int32 i = 0;
			while (cursor < tend && cursor->kind != json_grammer::JG_end_array) {
				interface.begin_array_element(i++);
				parse_value(interface, cursor, tend, "");
				interface.end_array_element();

				if (cursor < tend) {
					if (cursor->kind == json_grammer::JG_value_separator) {
						++cursor;
						continue;
					}

					if (cursor->kind != json_grammer::JG_end_array) {
						return;    // Error parsing
					}
				}
			}

			interface.end_array(name);
			++cursor;
		}

		void json_parser::parse_string(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name) {
			// Strip quotes
			interface.set_value<std::string>(name, std::string(cursor->data + 1, cursor->size - 2));
			++cursor;
		}

		void json_parser::parse_number(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name) {
			std::string_view value{cursor->data, cursor->size};

			bool is_negative = value[0] == '-';
			if (is_negative) {
				value = value.substr(1, value.size() - 1);
			}

			// integer portion
			int64 integer = 0;
			for (uint32 i = 0; i < value.size(); ++i) {
				if (grammar::json::is_digit(value[i])) {
					int32 val(value[i] - '0');
					integer *= 10;
					integer += val;
				}
				else {
					value = value.substr(i);
					break;
				}
			}

			// fraction if present.
			int64 frac = 0;
			int32 point = 0;
			if (value[0] == '.') {
				for (uint32 i = 1; i < value.size(); ++i) {
					if (grammar::json::is_digit(value[i])) {
						int32 val(value[i] - '0');
						frac *= 10;
						frac += val;
						++point;
					}
					else {
						value = value.substr(i);
						break;
					}
				}
			}

			// exponent if present
			int64 exponent = 0;
			if (value[0] == 'e' || value[0] == 'E') {
				for (uint32 i = 1; i < value.size(); ++i) {
					if (grammar::json::is_digit(value[i])) {
						int32 val(value[i] - '0');
						exponent *= 10;
						exponent += val;
					}
				}
			}

			int32 mulitplier = is_negative ? -1 : 1;

			if (point) {
				double val = static_cast<double>(frac);
				for (int32 i = 0; i < point; ++i) {
					val *= 0.1;
				}

				val += integer;

				for (int32 i = 0; i < exponent; ++i) {
					val *= 10;
				}
				val *= mulitplier;
				interface.set_value<double>(name, val);
			}
			else {
				for (int32 i = 0; i < exponent; ++i) {
					integer *= 10;
				}
				integer *= mulitplier;
				interface.set_value<int64>(name, integer);
			}

			
			++cursor;
		}

		void json_parser::parse_null(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name) {
			interface.set_null(name);
			++cursor;
		}

		void json_parser::parse_true(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name) {
			interface.set_value<bool>(name, true);
			++cursor;
		}

		void json_parser::parse_false(parser_interface &interface, lexer_token *&cursor, lexer_token *tend, const std::string_view &name) {
			interface.set_value<bool>(name, false);
			++cursor;
		}
	}    // namespace schema
}    // namespace tc
