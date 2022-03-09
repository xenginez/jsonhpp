#include <iostream>

#include "jsonhpp.hpp"

using namespace std;

int main()
{
	const char * json = R"(
{
	"object" : {
		"array" : [
			null,
			"string",
			123456,
			654.321,
			true,
			false
		],
		"null" : null,
		"string" : "string_string",
		"int" : 123456,
		"float" : 654.321,
		"bool_true" : true,
		"bool_false" : false
	}
}
)";

	{
		jsonhpp::document doc;
		jsonhpp::read( doc, json );

		std::cout << doc["object"]["array"][1].get_string() << std::endl;

		std::cout << std::endl;
		jsonhpp::write( doc, std::cout, 4 );

		std::cout << std::endl;
		std::cout << std::endl;
		jsonhpp::write( doc, std::cout );
	}



	{
		jsonhpp::document doc;

		doc = { 1, 2, 3 };

		std::cout << std::endl;
		std::cout << std::endl;
		jsonhpp::write( doc, std::cout );

		doc = {
			"aaa", 1,
			"bbb", 2,
			"ccc", 3,
		};

		std::cout << std::endl;
		std::cout << std::endl;
		std::string str;
		jsonhpp::write( doc, str, 4 );
		std::cout << str << std::endl;
	}

	return 0;
}
