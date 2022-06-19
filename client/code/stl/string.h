#pragma once

namespace jc::string
{
	namespace fn
	{
		static constexpr uint32_t INIT		= 0x401680;
		static constexpr uint32_t CTOR_DTOR = 0x401110;
		static constexpr uint32_t ASSIGN	= 0x4015D0;

	}
}

namespace jc::stl
{
	struct string
	{
		char data[0x1C];

		void alloc()
		{
			jc::this_call(jc::string::fn::CTOR_DTOR, this, 0);
		}

		string() { alloc(); }
		~string() { jc::this_call(jc::string::fn::CTOR_DTOR, this, 1); }

		string(const char* str)
		{
			alloc();

			jc::this_call(jc::string::fn::INIT, this, str, strlen(str));
		}

		//const char* c_str() const  { return std::as_const(data); }

		string& operator=(const char* str)
		{
			string right(str);

			jc::this_call(jc::string::fn::ASSIGN, this, &right, 0, -1);

			return *this;
		}

		string& operator=(const string& str)
		{
			jc::this_call(jc::string::fn::ASSIGN, this, &str, 0, -1);
			return *this;
		}
	};
}