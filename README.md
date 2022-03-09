# jsonhpp
![badge](https://img.shields.io/badge/license-MIT-blue)

c++17标准的美观易用的json库，仅单头文件

***
[JSON][json-org] 是一种轻量级的数据交换格式。它可以表示布尔、数字、字符串、数组以及对象集合。

[json-org]: http://json.org/

jsonhpp 是一个C++库，仅单头文件，包括序列化和反序列化到字符串，使其成为存储用户文件的便捷格式。
***

## 特点

- c++17标准
- 自定义allocator
- 自定义JSON输入和JSON输出


***
## 示例

```
// JSON
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
```

```
// C++
jsonhpp::document doc;
jsonhpp::read( doc, json ); // JSON解析

std::cout << doc["object"]["array"][1].get_string() << std::endl;

std::cout << std::endl;
jsonhpp::write( doc, std::cout, 4 ); // 格式化输出到流中

std::cout << std::endl;
std::cout << std::endl;
jsonhpp::write( doc, std::cout ); // 紧凑型输出到流中
```

```
// C++
jsonhpp::document doc;

doc = { 1, 2, 3 }; // 数组

std::cout << std::endl;
std::cout << std::endl;
jsonhpp::write( doc, std::cout );

doc = {
	"aaa", 1,
	"bbb", 2,
	"ccc", 3,
}; // 对象

std::cout << std::endl;
std::cout << std::endl;
std::string str;
jsonhpp::write( doc, str, 4 ); // 格式化输出到字符串中
std::cout << str << std::endl;
```
***
## 输出效果
```
// Console
string

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

{"object":{"array":[null,"string",123456,654.321,true,false],"null":null,"string":"string_string","int":123456,"float":654.321,"bool_true":true,"bool_false":false}}

[1,2,3]

{
    "aaa" : 1,
    "bbb" : 2,
    "ccc" : 3
}
```