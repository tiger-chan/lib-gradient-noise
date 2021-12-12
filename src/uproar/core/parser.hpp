#ifndef UPROAR_CORE_SCHEMA_PARSER_HPP
#define UPROAR_CORE_SCHEMA_PARSER_HPP

#include <string_view>

#ifndef TC_STDINT
#	define TC_STDINT
using int8 = signed char;
using int16 = signed short;
using int32 = signed int;
using int64 = signed long long;

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;
#endif

namespace tc {
	namespace schema {
		enum lexer_result {
			LR_null = 0,
			LR_no_match = 1,
			LR_success = 2,
			LR_complete = 3,
		};

		using token_kind = uint32;
		using code_point = uint32;
		using code_point_matcher = bool (*)(code_point cp);

		struct lexer_transaction;
		using lexer_matcher = bool (*)(lexer_transaction &);
		struct grammer_matcher {
			lexer_matcher match;
			token_kind kind;
		};

		bool is_match(lexer_transaction &ctx, code_point_matcher matcher, code_point &out);
		bool is_match(lexer_transaction &ctx, code_point cmp, code_point &out);
		bool is_match(lexer_transaction &ctx, std::initializer_list<code_point> any_of, code_point &out);
		bool is_match(lexer_transaction &ctx, std::string_view cmp);

		struct lexer_token {
			token_kind kind{ UINT32_MAX };
			uint32 line{ 0 };
			uint32 column{ 0 };

			uint64 size{ 0 };
			const char *data{ nullptr };
		};

		struct schema_lexer;

		struct lexer_transaction {
			lexer_transaction(schema_lexer &lexer);
			lexer_transaction(const lexer_transaction &other);
			lexer_transaction &operator=(const lexer_transaction &other);
			void begin();
			code_point peek() const;
			void new_line();
			void advance();
			bool eof();
			lexer_token commit(token_kind kind);

		private:
			schema_lexer *lexer{ nullptr };
			uint32 line{ 0 };
			uint32 column{ 0 };
			const char *pos{ nullptr };
		};

		struct schema_lexer {
			template<typename Grammar>
			lexer_result read(lexer_token *);

			const char *first{ nullptr };
			const char *last{ nullptr };
			uint32 line{ 0 };
			uint32 column{ 0 };
		};

		struct parser_interface;
		namespace json {
			void parse(parser_interface &interface, std::string_view content);
		}
	}    // namespace schema
}    // namespace tc

#include "parser.inl"

#endif    // UPROAR_CORE_SCHEMA_PARSER_HPP