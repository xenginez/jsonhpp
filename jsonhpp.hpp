/*!
 * \file	jsonhpp.hpp
 *
 * \author	ZhengYuanQing
 * \date	2022/03/08
 * \email	zhengyuanqing.95@gmail.com
 *
 */
#ifndef JSONHPP_HPP__1E85F87A_AD28_4BF2_9739_184AF09C9524
#define JSONHPP_HPP__1E85F87A_AD28_4BF2_9739_184AF09C9524

#include <array>
#include <vector>
#include <string>
#include <variant>
#include <iostream>
#include <charconv>
#include <memory_resource>

namespace jsonhpp
{
	enum class type
	{
		null,
		array,
		string,
		number,
		object,
		boolean,
		unknown,
	};

	class value;
	template< jsonhpp::type T > class element;
	template< typename T > class istream_wrapper;
	template< typename T > class ostream_wrapper;

	template< typename T > value __read_value( std::pmr::memory_resource * resource, ostream_wrapper< T > & stream );
	template< typename T > void __write_value( const value & val, istream_wrapper< T > & stream, std::uint32_t depth, std::uint32_t tab );
	element< jsonhpp::type::array > * __make_array( std::pmr::memory_resource * resource, const std::initializer_list< value > & list );
	element< jsonhpp::type::object > * __make_object( std::pmr::memory_resource * resource, const std::initializer_list< value > & list );
	element< jsonhpp::type::array > * __copy_array( std::pmr::memory_resource * resource, element< jsonhpp::type::array > * copy );
	element< jsonhpp::type::object > * __copy_object( std::pmr::memory_resource * resource, element< jsonhpp::type::object > * copy );


	template< typename ... T > struct overloaded : T... { using T::operator()...; };
	template< typename ... T > overloaded( T... ) -> overloaded< T... >;

	template<> class istream_wrapper< char * >
	{
	public:
		using string_type = const char *;

	public:
		istream_wrapper( string_type str, std::size_t size )
			:_str( str ), _size( size ), _pos( 0 )
		{}

	public:
		inline char get()
		{
			if ( _pos >= _size ) throw std::out_of_range( "out of range" );

			return _str[_pos++];
		}

		inline char peek() const
		{
			return _str[_pos];
		}

	private:
		std::size_t _pos;
		string_type _str;
		std::size_t _size;
	};
	template<> class ostream_wrapper< char * >
	{
	public:
		using string_type = char *;

	public:
		ostream_wrapper( string_type str, std::size_t size )
			:_str( str ), _size( size ), _pos( 0 )
		{}

	public:
		inline void put( char c )
		{
			if ( _pos >= _size ) throw std::out_of_range( "out of range" );

			_str[_pos++] = c;
		}

		inline std::size_t size() const
		{
			return _pos;
		}

	private:
		std::size_t _pos;
		string_type _str;
		std::size_t _size;
	};
	template<> class istream_wrapper< std::basic_istream< char, std::char_traits< char > > >
	{
	public:
		using string_type = std::basic_istream< char, std::char_traits< char > >;

	public:
		istream_wrapper( string_type & str )
			:_str( str )
		{}

	public:
		inline char get()
		{
			return _str.get();
		}

		inline char peek() const
		{
			return _str.peek();
		}

	private:
		string_type & _str;
	};
	template<> class ostream_wrapper< std::basic_ostream< char, std::char_traits< char > > >
	{
	public:
		using string_type = std::basic_ostream< char, std::char_traits< char > >;

	public:
		ostream_wrapper( string_type & str )
			:_str( str )
		{}

	public:
		inline void put( char c )
		{
			_str.put( c );
		}

		inline std::size_t size() const
		{
			return _str.tellp();
		}

	private:
		string_type & _str;
	};
	template< typename A > class istream_wrapper< std::basic_string< char, std::char_traits< char >, A > >
	{
	public:
		using string_type = const std::basic_string< char, std::char_traits< char >, A >;

	public:
		istream_wrapper( string_type & str )
			:_str( str ), _pos( 0 )
		{}

	public:
		inline char get()
		{
			if ( _pos + 1 == _str.size() )
			{
				throw std::out_of_range( "out of range!" );
			}

			return _str[_pos++];
		}

		inline char peek() const
		{
			return _str[_pos];
		}

	private:
		std::uint64_t _pos;
		string_type & _str;
	};
	template< typename A > class ostream_wrapper< std::basic_string< char, std::char_traits< char >, A > >
	{
	public:
		using string_type = std::basic_string< char, std::char_traits< char >, A >;

	public:
		ostream_wrapper( string_type & str )
			:_str( str )
		{}

	public:
		inline void put( char c )
		{
			_str.push_back( c );
		}

		inline std::size_t size() const
		{
			return _str.size();
		}

	private:
		string_type & _str;
	};

	template< typename T > inline char __stream_get( T & stream )
	{
		return stream.get();
	}
	template< typename T > inline char __stream_peek( T & stream )
	{
		return stream.peek();
	}
	template< typename T > inline void __stream_ignore( T & stream )
	{
		while ( std::isspace( __stream_peek( stream ) ) )
		{
			__stream_get( stream );
		}
	}
	template< typename T > inline void __stream_put( T & stream, char c )
	{
		stream.put( c );
	}
	template< typename T > inline bool __stream_check( T & stream, char c )
	{
		if ( __stream_peek( stream ) != c )
		{
			return false;
		}

		__stream_get( stream );
		return true;
	}
	template< typename T > inline void __stream_puts( T & stream, const char * s )
	{
		while ( *s != '\0' )
		{
			__stream_put( stream, *s++ );
		}
	}
	template< typename T > inline bool __stream_match( T & stream, const char * s )
	{
		while ( *s != '\0' )
		{
			if ( __stream_get( stream ) != *s++ )
			{
				return false;
			}
		}

		return true;
	}
	template< typename T > inline void __stream_put_tab( T & stream, std::uint32_t depth, std::uint32_t tab )
	{
		if ( tab != 0 )
		{
			for ( size_t i = 0; i < depth; i++ )
			{
				for ( size_t j = 0; j < tab; j++ )
				{
					__stream_put( stream, ' ' );
				}
			}
		}
	}

	template<> class element< jsonhpp::type::null >
	{
	public:
		static constexpr jsonhpp::type value_t = jsonhpp::type::null;

	public:
		element() = default;

		element( element && ) = default;

		element( const element & ) = default;

		element( std::pmr::memory_resource * ){}

		element & operator= ( element && ) = default;

		element & operator= ( const element & ) = default;

	public:
		jsonhpp::type get_type() const
		{
			return value_t;
		}

	public:
		template< typename T > void read( istream_wrapper< T > & stream )
		{
			if ( !__stream_match( stream, "null" ) ) throw std::invalid_argument( "is not \'null\'!" );
		}

		template< typename T > void write( ostream_wrapper< T > & stream, std::uint32_t depth, std::uint32_t tab = 0 ) const
		{
			__stream_puts( stream, "null" );
		}
	};
	template<> class element< jsonhpp::type::array >;
	template<> class element< jsonhpp::type::string >
	{
	public:
		static constexpr jsonhpp::type value_t = jsonhpp::type::string;

	public:
		element( element && ) = default;

		element( const element & ) = default;

		element & operator= ( element && ) = default;

		element & operator= ( const element & ) = default;

		element( std::pmr::memory_resource * resource )
			:_value( resource )
		{

		}

		element( std::pmr::memory_resource * resource, const char * str )
			:_value( str, resource )
		{

		}

		element( std::pmr::memory_resource * resource, std::string_view str )
			:_value( str.data(), str.size(), resource )
		{

		}

		template< typename A > element( std::pmr::memory_resource * resource, const std::basic_string< char, std::char_traits< char >, A > & str )
			: _value( str.data(), str.size(), resource )
		{

		}

		element & operator= ( const char * str )
		{
			_value = str;
			return *this;
		}

		element & operator= ( std::string_view str )
		{
			_value.assign( str.data(), str.size() );

			return *this;
		}

		template< typename A > element & operator= ( const std::basic_string< char, std::char_traits< char >, A > & str )
		{
			_value = str;
			return *this;
		}

	public:
		operator const char * ( ) const
		{
			return _value.c_str();
		}

		operator std::string_view() const
		{
			return { _value.data(), _value.size() };
		}

		operator const std::pmr::string & ( ) const
		{
			return _value;
		}

		template< typename A > operator std::basic_string< char, std::char_traits< char >, A >() const
		{
			return { _value.begin(), _value.end() };
		}

	public:
		jsonhpp::type get_type() const
		{
			return value_t;
		}

	public:
		template< typename T > void read( istream_wrapper< T > & stream )
		{
			if ( !__stream_check(stream, '\"' ) ) throw std::invalid_argument( R"(unable to match "!)" );
			{
				while ( __stream_peek( stream ) != '\"' )
				{
					_value.push_back( __stream_get( stream ) );
				}
			}
			if ( !__stream_check( stream, '\"' ) ) throw std::invalid_argument( R"(unable to match "!)" );
		}

		template< typename T > void write( ostream_wrapper< T > & stream, std::uint32_t depth, std::uint32_t tab = 0 ) const
		{
			__stream_put( stream, '\"' );
			for ( const auto & c : _value )
			{
				__stream_put( stream, c );
			}
			__stream_put( stream, '\"' );
		}

	private:
		std::pmr::string _value;
	};
	template<> class element< jsonhpp::type::number >
	{
	public:
		static constexpr jsonhpp::type value_t = jsonhpp::type::number;

	public:
		element( element && ) = default;

		element( const element & ) = default;

		element & operator= ( element && ) = default;

		element & operator= ( const element & ) = default;

		element( float val )
			:_value( val )
		{}

		element( double val )
			:_value( val )
		{}

		element( std::int8_t val )
			:_value( std::int64_t( val ) )
		{}

		element( std::int16_t val )
			:_value( std::int64_t( val ) )
		{}

		element( std::int32_t val )
			:_value( std::int64_t( val ) )
		{}

		element( std::int64_t val )
			:_value( val )
		{}

		element( std::uint8_t val )
			:_value( std::uint64_t( val ) )
		{}

		element( std::uint16_t val )
			:_value( std::uint64_t( val ) )
		{}

		element( std::uint32_t val )
			:_value( std::uint64_t( val ) )
		{}

		element( std::uint64_t val )
			:_value( val )
		{}

		element( std::pmr::memory_resource * ) {}

		element & operator= ( double val )
		{
			_value = val;
			return *this;
		}

		element & operator= ( std::int64_t val )
		{
			_value = val;
			return *this;
		}

		element & operator= ( std::uint64_t val )
		{
			_value = val;
			return *this;
		}

	public:
		operator float() const
		{
			return std::visit( overloaded{
				[]( double val ) -> float { return static_cast<float>( val ); },
				[]( std::int64_t val ) -> float { return static_cast<float>( val ); },
				[]( std::uint64_t val ) -> float { return static_cast<float>( val ); },
				}, _value );
		}

		operator double() const
		{
			return std::visit( overloaded{
				[]( double val ) -> double { return static_cast<double>( val ); },
				[]( std::int64_t val ) -> double { return static_cast<double>( val ); },
				[]( std::uint64_t val ) -> double { return static_cast<double>( val ); },
				}, _value );
		}

		operator std::int8_t() const
		{
			return std::visit( overloaded{
				[]( double val ) -> std::int8_t { return static_cast<std::int8_t>( val ); },
				[]( std::int64_t val ) -> std::int8_t { return static_cast<std::int8_t>( val ); },
				[]( std::uint64_t val ) -> std::int8_t { return static_cast<std::int8_t>( val ); },
				}, _value );
		}

		operator std::int16_t() const
		{
			return std::visit( overloaded{
				[]( double val ) -> std::int16_t { return static_cast<std::int16_t>( val ); },
				[]( std::int64_t val ) -> std::int16_t { return static_cast<std::int16_t>( val ); },
				[]( std::uint64_t val ) -> std::int16_t { return static_cast<std::int16_t>( val ); },
				}, _value );
		}

		operator std::int32_t() const
		{
			return std::visit( overloaded{
				[]( double val ) -> std::int32_t { return static_cast<std::int32_t>( val ); },
				[]( std::int64_t val ) -> std::int32_t { return static_cast<std::int32_t>( val ); },
				[]( std::uint64_t val ) -> std::int32_t { return static_cast<std::int32_t>( val ); },
				}, _value );
		}

		operator std::int64_t() const
		{
			return std::visit( overloaded{
				[]( double val ) -> std::int64_t { return static_cast<std::int64_t>( val ); },
				[]( std::int64_t val ) -> std::int64_t { return static_cast<std::int64_t>( val ); },
				[]( std::uint64_t val ) -> std::int64_t { return static_cast<std::int64_t>( val ); },
				}, _value );
		}

		operator std::uint8_t() const
		{
			return std::visit( overloaded{
				[]( double val ) -> std::uint8_t { return static_cast<std::uint8_t>( val ); },
				[]( std::int64_t val ) -> std::uint8_t { return static_cast<std::uint8_t>( val ); },
				[]( std::uint64_t val ) -> std::uint8_t { return static_cast<std::uint8_t>( val ); },
				}, _value );
		}

		operator std::uint16_t() const
		{
			return std::visit( overloaded{
				[]( double val ) -> std::uint16_t { return static_cast<std::uint16_t>( val ); },
				[]( std::int64_t val ) -> std::uint16_t { return static_cast<std::uint16_t>( val ); },
				[]( std::uint64_t val ) -> std::uint16_t { return static_cast<std::uint16_t>( val ); },
				}, _value );
		}

		operator std::uint32_t() const
		{
			return std::visit( overloaded{
				[]( double val ) -> std::uint32_t { return static_cast<std::uint32_t>( val ); },
				[]( std::int64_t val ) -> std::uint32_t { return static_cast<std::uint32_t>( val ); },
				[]( std::uint64_t val ) -> std::uint32_t { return static_cast<std::uint32_t>( val ); },
				}, _value );
		}

		operator std::uint64_t() const
		{
			return std::visit( overloaded{
				[]( double val ) -> std::uint64_t { return static_cast<std::uint64_t>( val ); },
				[]( std::int64_t val ) -> std::uint64_t { return static_cast<std::uint64_t>( val ); },
				[]( std::uint64_t val ) -> std::uint64_t { return static_cast<std::uint64_t>( val ); },
				}, _value );
		}

	public:
		jsonhpp::type get_type() const
		{
			return value_t;
		}

	public:
		bool is_int() const
		{
			return _value.index() == 1;
		}

		bool is_uint() const
		{
			return _value.index() == 2;
		}

		bool is_float() const
		{
			return _value.index() == 0;
		}

	public:
		template< typename T > void read( istream_wrapper< T > & stream )
		{
			bool f = false;
			char buf[25] = {};
			std::uint32_t i = 0;

			if ( __stream_peek( stream ) == '-' )
			{
				buf[i++] = __stream_get( stream );
			}
			
			if ( __stream_peek( stream ) == '.' )
			{
				f = true;
				buf[i++] = '0';
				buf[i++] = __stream_get( stream );
				while ( __stream_peek( stream ) >= '0' && __stream_peek( stream ) <= '9' )
				{
					buf[i++] = __stream_get( stream );
				}
			}
			else
			{
				while ( ( __stream_peek( stream ) >= '0' && __stream_peek( stream ) <= '9' ) || __stream_peek( stream ) == '.' )
				{
					buf[i++] = __stream_get( stream );
					if ( buf[i - 1] == '.' )
					{
						f = true;
						while ( __stream_peek( stream ) >= '0' && __stream_peek( stream ) <= '9' )
						{
							buf[i++] = __stream_get( stream );
						}
						break;
					}
				}
			}

			if ( f )
			{
				double val;
				std::from_chars( buf, buf + i, val );
				_value = val;
			}
			else if( buf[0] == '-' )
			{
				std::int64_t val;
				std::from_chars( buf, buf + i, val );
				_value = val;
			}
			else
			{
				if ( std::strcmp( "9223372036854775807", buf ) < 0 )
				{
					std::uint64_t val;
					std::from_chars( buf, buf + i, val );
					_value = val;
				}
				else
				{
					std::int64_t val;
					std::from_chars( buf, buf + i, val );
					_value = val;
				}
			}
		}

		template< typename T > void write( ostream_wrapper< T > & stream, std::uint32_t depth, std::uint32_t tab = 0 ) const
		{
			char buf[25] = {};

			std::visit( overloaded{
				[&buf]( double val ) { std::to_chars( buf, buf + 24, val ); },
				[&buf]( std::int64_t val ) { std::to_chars( buf, buf + 24, val ); },
				[&buf]( std::uint64_t val ) { std::to_chars( buf, buf + 24, val ); },
				}, _value );

			__stream_puts( stream, buf );
		}

	private:
		std::variant< double, std::int64_t, std::uint64_t > _value;
	};
	template<> class element< jsonhpp::type::object >;
	template<> class element< jsonhpp::type::boolean >
	{
	public:
		static constexpr jsonhpp::type value_t = jsonhpp::type::boolean;

	public:
		element( bool val )
			:_value( val )
		{}

		element( element && ) = default;

		element( const element & ) = default;

		element( std::pmr::memory_resource * ) {}

		element & operator= ( element && ) = default;

		element & operator= ( const element & ) = default;

		element & operator= ( bool val )
		{
			_value = val;
			return *this;
		}

	public:
		operator bool() const
		{
			return _value;
		}

	public:
		jsonhpp::type get_type() const
		{
			return value_t;
		}

	public:
		template< typename T > void read( istream_wrapper< T > & stream )
		{
			if ( __stream_peek( stream ) == 'f' )
			{
				if ( !__stream_match( stream, "false" ) ) throw std::invalid_argument( "" );
				_value = false;
			}
			else if ( __stream_peek( stream ) == 't' )
			{
				if( !__stream_match( stream, "true" ) ) throw std::invalid_argument( "" );
				_value = true;
			}
		}

		template< typename T > void write( ostream_wrapper< T > & stream, std::uint32_t depth, std::uint32_t tab = 0 ) const
		{
			__stream_puts( stream, _value ? "true" : "false" );
		}

	private:
		bool _value;
	};
	template<> class element< jsonhpp::type::unknown >
	{
		friend class value;

	public:
		virtual ~element() = default;

	protected:
		element() = default;

	protected:
		virtual std::size_t get_size() const = 0;

		virtual value & find_value( std::size_t val ) = 0;

		virtual value & find_value( std::string_view val ) = 0;
	};

	using null_t = element< jsonhpp::type::null >;
	using array_t = element< jsonhpp::type::array >;
	using string_t = element< jsonhpp::type::string >;
	using number_t = element< jsonhpp::type::number >;
	using object_t = element< jsonhpp::type::object >;
	using boolean_t = element< jsonhpp::type::boolean >;
	using unknown_t = element< jsonhpp::type::unknown >;

	class value
	{
	public:
		using element_type = std::variant<
			std::monostate,
			null_t,
			string_t,
			number_t,
			boolean_t,
			array_t *,
			object_t * > ;

	public:
		value( value && val )
		{
			swap( std::forward< value & >( val ) );
		}

		value( const value & val )
			:_element( val._element ), _resource( val._resource )
		{
			std::visit( overloaded{
				[this]( array_t * val ) { _element = __copy_array( _resource, val ); },
				[this]( object_t * val ) { _element = __copy_object( _resource, val ); },
				[]( auto & ) {}
				}, _element );
		}

		value( std::pmr::memory_resource * resource )
			:_resource( resource )
		{}

		value( std::pmr::memory_resource * resource, element_type && val )
			:_element( std::move( val ) ), _resource( resource )
		{}

		value( std::pmr::memory_resource * resource, const element_type & val )
			:_element( val ), _resource( resource )
		{}

		value & operator= ( value && val )
		{
			swap( std::forward< value & >( val ) );
			return *this;
		}

		value & operator= ( const value & val )
		{
			clear();

			_element = val._element;
			_resource = val._resource;

			std::visit( overloaded{
				[this]( array_t * val ) { _element = __copy_array( _resource, val ); },
				[this]( object_t * val ) { _element = __copy_object( _resource, val ); },
				[]( auto & ) {}
				}, _element );

			return *this;
		}

		~value()
		{
			clear();
		}

	public:
		value( float val )
		{
			_element = number_t( val );
		}

		value( double val )
		{
			_element = number_t( val );
		}

		value( std::nullptr_t )
		{
			_element = null_t( _resource );
		}

		value( std::int8_t val )
		{
			_element = number_t( val );
		}

		value( std::int16_t val )
		{
			_element = number_t( val );
		}

		value( std::int32_t val )
		{
			_element = number_t( val );
		}

		value( std::int64_t val )
		{
			_element = number_t( val );
		}

		value( std::uint8_t val )
		{
			_element = number_t( val );
		}

		value( std::uint16_t val )
		{
			_element = number_t( val );
		}

		value( std::uint32_t val )
		{
			_element = number_t( val );
		}

		value( std::uint64_t val )
		{
			_element = number_t( val );
		}

		value( const char * str )
		{
			_element = string_t( _resource, str );
		}

		value( std::string_view str )
		{
			_element = string_t( _resource, str );
		}

		value( const std::initializer_list< value > & list )
		{
			bool is_obj = false;
			if ( list.size() % 2 == 0 )
			{
				is_obj = true;
				for ( auto it = list.begin(); it != list.end(); it += 2 )
				{
					if ( !it->is_string() )
					{
						is_obj = false;
						break;
					}
				}
			}

			if ( is_obj )
				_element = __make_object( _resource, list );
			else
				_element = __make_array( _resource, list );
		}

		template< typename A > value( const std::basic_string< char, std::char_traits< char >, A > & str )
		{
			_element = string_t( _resource, str );
		}

		value & operator= ( float val )
		{
			clear();
			_element = number_t( val );
			return *this;
		}

		value & operator= ( double val )
		{
			clear();
			_element = number_t( val );
			return *this;
		}

		value & operator= ( std::nullptr_t )
		{
			clear();
			_element = null_t( _resource );
			return *this;
		}

		value & operator= ( std::int8_t val )
		{
			clear();
			_element = number_t( val );
			return *this;
		}

		value & operator= ( std::int16_t val )
		{
			clear();
			_element = number_t( val );
			return *this;
		}

		value & operator= ( std::int32_t val )
		{
			clear();
			_element = number_t( val );
			return *this;
		}

		value & operator= ( std::int64_t val )
		{
			clear();
			_element = number_t( val );
			return *this;
		}

		value & operator= ( std::uint8_t val )
		{
			clear();
			_element = number_t( val );
			return *this;
		}

		value & operator= ( std::uint16_t val )
		{
			clear();
			_element = number_t( val );
			return *this;
		}

		value & operator= ( std::uint32_t val )
		{
			clear();
			_element = number_t( val );
			return *this;
		}

		value & operator= ( std::uint64_t val )
		{
			clear();
			_element = number_t( val );
			return *this;
		}

		value & operator= ( const char * str )
		{
			clear();
			_element = string_t( _resource, str );
			return *this;
		}

		value & operator= ( std::string_view str )
		{
			clear();
			_element = string_t( _resource, str );
			return *this;
		}

		template< typename A > value & operator= ( const std::basic_string< char, std::char_traits< char >, A > & str )
		{
			clear();
			_element = string_t( _resource, str );
			return *this;
		}

		value & operator= ( const std::initializer_list< value > & list )
		{
			clear();

			bool is_obj = false;
			if ( list.size() % 2 == 0 )
			{
				is_obj = true;
				for ( auto it = list.begin(); it != list.end(); it += 2 )
				{
					if ( !it->is_string() )
					{
						is_obj = false;
						break;
					}
				}
			}

			if ( is_obj )
				_element = __make_object( _resource, list );
			else
				_element = __make_array( _resource, list );

			return *this;
		}

	public:
		value & operator[]( std::size_t val )
		{
			return reinterpret_cast<unknown_t *>( std::get< element< jsonhpp::type::array > * >( _element ) )->find_value( val );
		}

		value & operator[]( std::string_view val )
		{
			return reinterpret_cast<unknown_t *>( std::get< element< jsonhpp::type::object > * >( _element ) )->find_value( val );
		}

	public:
		void swap( value & val )
		{
			std::swap( _element, val._element );
			std::swap( _resource, val._resource );
		}

	public:
		type get_type() const
		{
			return std::visit( overloaded{
				[]( const std::monostate & ) { return jsonhpp::type::unknown; },
				[]( const null_t & val ) { return val.get_type(); },
				[]( const string_t & val ) { return val.get_type(); },
				[]( const number_t & val ) { return val.get_type(); },
				[]( const boolean_t & val ) { return val.get_type(); },
				[]( const array_t * val ) { return jsonhpp::type::array; },
				[]( const object_t * val ) { return jsonhpp::type::object; },
				}, _element );
		}

		std::pmr::memory_resource * resource() const
		{
			return _resource;
		}

	public:
		bool empty() const
		{
			return get_type() == jsonhpp::type::unknown;
		}

		bool is_null() const
		{
			return get_type() == jsonhpp::type::null;
		}

		bool is_array() const
		{
			return get_type() == jsonhpp::type::array;
		}

		bool is_string() const
		{
			return get_type() == jsonhpp::type::string;
		}

		bool is_number() const
		{
			return get_type() == jsonhpp::type::number;
		}

		bool is_object() const
		{
			return get_type() == jsonhpp::type::object;
		}

		bool is_boolean() const
		{
			return get_type() == jsonhpp::type::boolean;
		}

	public:
		null_t & get_null()
		{
			return std::get< element< jsonhpp::type::null > >( _element );
		}

		array_t & get_array()
		{
			return *std::get< element< jsonhpp::type::array > * >( _element );
		}

		string_t & get_string()
		{
			return std::get< element< jsonhpp::type::string > >( _element );
		}

		number_t & get_number()
		{
			return std::get< element< jsonhpp::type::number > >( _element );
		}

		object_t & get_object()
		{
			return *std::get< element< jsonhpp::type::object > * >( _element );
		}

		boolean_t & get_boolean()
		{
			return std::get< element< jsonhpp::type::boolean > >( _element );
		}

	public:
		const null_t & get_null() const
		{
			return std::get< element< jsonhpp::type::null > >( _element );
		}

		const array_t & get_array() const
		{
			return *std::get< element< jsonhpp::type::array > * >( _element );
		}

		const string_t & get_string() const
		{
			return std::get< element< jsonhpp::type::string > >( _element );
		}

		const number_t & get_number() const
		{
			return std::get< element< jsonhpp::type::number > >( _element );
		}

		const object_t & get_object() const
		{
			return *std::get< element< jsonhpp::type::object > * >( _element );
		}

		const boolean_t & get_boolean() const
		{
			return std::get< element< jsonhpp::type::boolean > >( _element );
		}

	public:
		void set_null()
		{
			clear();
			_element = null_t( _resource );
		}

		void set_int( std::int64_t val )
		{
			clear();
			_element = number_t( val );
		}

		void set_uint( std::uint64_t val )
		{
			clear();
			_element = number_t( val );
		}

		void set_float( double val )
		{
			clear();
			_element = number_t( val );
		}

		void set_string( const char * str )
		{
			clear();
			_element = string_t( _resource, str );
		}

		void set_string( std::string_view str )
		{
			clear();
			_element = string_t( _resource, str );
		}

		template< typename A > void set_string( const std::basic_string< char, std::char_traits< char >, A > & str )
		{
			clear();
			_element = string_t( _resource, str );
		}

		void set_array( std::initializer_list< value > list )
		{
			clear();
			_element = __make_array( _resource, list );
		}

		void set_object( std::initializer_list< value > list )
		{
			clear();
			_element = __make_object( _resource, list );
		}

	public:
		void clear()
		{
			std::visit( overloaded{
				[this]( array_t * val ) 
				{
					auto p = reinterpret_cast<unknown_t *>( val );
					auto size = p->get_size();

					p->~unknown_t();
					_resource->deallocate( p, size );
				},
				[this]( object_t * val )
				{
					auto p = reinterpret_cast<unknown_t *>( val );
					auto size = p->get_size();

					p->~unknown_t();
					_resource->deallocate( p, size ); 
				},
				[this]( auto & ) {}
				}, _element );

			_element = {};
		}

	protected:
		element_type _element;
		std::pmr::memory_resource * _resource = std::pmr::get_default_resource();
	};

	template<> class element< jsonhpp::type::array > : public element< jsonhpp::type::unknown >
	{
	public:
		static constexpr jsonhpp::type value_t = jsonhpp::type::array;

	public:
		using size_type = std::size_t;
		using value_type = jsonhpp::value;
		using container_type = std::pmr::vector< value_type >;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;

	public:
		element() = default;

		element( std::pmr::memory_resource * resource )
		{}

		~element() override = default;

	protected:
		std::size_t get_size() const override
		{
			return sizeof( *this );
		}

		value & find_value( std::size_t val ) override
		{
			return _value[val];
		}

		value & find_value( std::string_view val ) override
		{
			throw std::bad_variant_access();
		}

	public:
		jsonhpp::type get_type() const
		{
			return value_t;
		}

	public:
		value_type & operator[]( size_type val )
		{
			return _value[val];
		}

		const value_type & operator[]( size_type val ) const
		{
			return _value[val];
		}

	public:
		size_type size() const
		{
			return _value.size();
		}

		iterator earse( const_iterator val )
		{
			return _value.erase( val );
		}

		void push_back( const value_type & val )
		{
			_value.push_back( val );
		}

	public:
		iterator begin()
		{
			return _value.begin();
		}

		iterator end()
		{
			return _value.end();
		}

		const_iterator begin() const
		{
			return _value.begin();
		}

		const_iterator end() const
		{
			return _value.end();
		}

	public:
		template< typename T > void read( istream_wrapper< T > & stream )
		{
			if ( !__stream_check( stream, '[' ) ) throw std::invalid_argument( "not matched \'[\'" );
			{
				while ( __stream_peek( stream ) != ']' )
				{
					__stream_ignore( stream );
					{
						_value.push_back( __read_value( _value.get_allocator().resource(), stream ) );
					}
					__stream_ignore( stream );

					__stream_check( stream, ',' );
				}
			}
			if ( !__stream_check( stream, ']' ) ) throw std::invalid_argument( "not matched \']\'" );
		}

		template< typename T > void write( ostream_wrapper< T > & stream, std::uint32_t depth, std::uint32_t tab = 0 ) const
		{
			__stream_puts( stream, "[" );
			for ( size_t i = 0; i < _value.size(); i++ )
			{
				if ( tab != 0 ) __stream_put( stream, '\n' );
				__stream_put_tab( stream, depth + 1, tab );

				__write_value( _value[i], stream, depth + 1, tab );

				if ( i + 1 < _value.size() )
				{
					__stream_put( stream, ',' );
				}
			}

			if ( tab != 0 ) __stream_put( stream, '\n' );
			__stream_put_tab( stream, depth, tab );
			__stream_puts( stream, "]" );
		}

	private:
		container_type _value;
	};
	template<> class element< jsonhpp::type::object > : public element< jsonhpp::type::unknown >
	{
	public:
		static constexpr jsonhpp::type value_t = jsonhpp::type::object;

	public:
		using size_type = std::size_t;
		using value_type = jsonhpp::value;
		using container_type = std::pmr::vector< std::pair< std::pmr::string, value_type > >;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;

	public:
		element() = default;

		element( std::pmr::memory_resource * resource )
		{}

		~element() override = default;

	protected:
		std::size_t get_size() const override
		{
			return sizeof( *this );
		}

		value & find_value( std::size_t val ) override
		{
			throw std::bad_variant_access();
		}

		value & find_value( std::string_view val ) override
		{
			return std::find_if( _value.begin(), _value.end(), [val]( auto & _val ) { return _val.first == val; } )->second;
		}

	public:
		jsonhpp::type get_type() const
		{
			return value_t;
		}

	public:
		value_type & operator[] ( std::string_view key )
		{
			auto it = std::find_if( _value.begin(), _value.end(), [&]( const auto & val ) { return val.first == key; } );
			if ( it != _value.end() )
			{
				return it->second;
			}

			_value.push_back( { std::pmr::string{ key.data(), key.size(), _value.get_allocator() }, value_type( _value.get_allocator().resource() ) } );

			return _value.back().second;
		}

		const value_type & operator[] ( std::string_view key ) const
		{
			auto it = std::find_if( _value.begin(), _value.end(), [&]( const auto & val ) { return val.first == key; } );
			if ( it != _value.end() )
			{
				return it->second;
			}

			_value.push_back( { std::pmr::string{ key.data(), key.size(), _value.get_allocator() }, value_type( _value.get_allocator().resource() ) } );

			return _value.back().second;
		}

	public:
		size_type size() const
		{
			return _value.size();
		}

		iterator earse( const_iterator val )
		{
			return _value.erase( val );
		}

		void insert( std::string_view key, const value_type & val )
		{
			_value.push_back( { { key.data(), key.size(), _value.get_allocator() }, val } );
		}

	public:
		iterator begin()
		{
			return _value.begin();
		}

		iterator end()
		{
			return _value.end();
		}

		const_iterator begin() const
		{
			return _value.begin();
		}

		const_iterator end() const
		{
			return _value.end();
		}

	public:
		template< typename T > void read( istream_wrapper< T > & stream )
		{
			if ( !__stream_check( stream, '{' ) ) throw std::invalid_argument( "not matched \'{\'" );
			{
				while ( __stream_peek( stream ) != '}' )
				{
					std::pmr::string key( _value.get_allocator() );
					value_type val( _value.get_allocator().resource() );

					__stream_ignore( stream );

					if ( !__stream_check( stream, '\"' ) ) throw std::invalid_argument( "not matched key" );
					{
						while ( __stream_peek( stream ) != '\"' )
						{
							key.push_back( __stream_get( stream ) );
						}
					}
					if ( !__stream_check( stream, '\"' ) ) throw std::invalid_argument( "not matched key" );

					__stream_ignore( stream );
					if ( !__stream_check( stream, ':' ) ) throw std::invalid_argument( "not matched \':\'" );

					__stream_ignore( stream );
					{
						val = __read_value( _value.get_allocator().resource(), stream ); 
					}
					__stream_ignore( stream );

					__stream_check( stream, ',' );

					_value.push_back( { std::move( key ), std::move( val ) } );
				}
			}
			if ( !__stream_check( stream, '}' ) ) throw std::invalid_argument( "not matched \'}\'" );
		}

		template< typename T > void write( ostream_wrapper< T > & stream, std::uint32_t depth, std::uint32_t tab = 0 ) const
		{
			__stream_puts( stream, "{" );
			for ( size_t i = 0; i < _value.size(); i++ )
			{
				if ( tab != 0 ) __stream_put( stream, '\n' );
				__stream_put_tab( stream, depth + 1, tab );

				__stream_put( stream, '\"' );
				__stream_puts( stream, _value[i].first.c_str() );
				__stream_put( stream, '\"' );

				if ( tab != 0 ) __stream_put( stream, ' ' );
				__stream_put( stream, ':' );
				if ( tab != 0 ) __stream_put( stream, ' ' );

				__write_value( _value[i].second, stream, depth + 1, tab );

				if ( i + 1 < _value.size() )
				{
					__stream_put( stream, ',' );
				}
			}

			if ( tab != 0 ) __stream_put( stream, '\n' );
			__stream_put_tab( stream, depth, tab );
			__stream_puts( stream, "}" );
		}

	private:
		mutable container_type _value;
	};

	class document : public value
	{
	public:
		using value::value;
		using value::value::operator=;

		document( document && ) = delete;
		document( const document & ) = delete;
		document & operator= ( document && ) = delete;
		document & operator= ( const document & ) = delete;

		document( std::pmr::memory_resource * resource = std::pmr::get_default_resource() )
			:value( resource )
		{}

		document( jsonhpp::value && val )
			:value( std::move( val ) )
		{}

		document( const jsonhpp::value & val )
			:value( val )
		{}
	};

	template< typename T > value __read_value( std::pmr::memory_resource * resource, istream_wrapper< T > & stream )
	{
		value::element_type element;

		__stream_ignore( stream );

		switch ( __stream_peek( stream ) )
		{
		case 'n':
		{
			null_t result( resource );
			result.read( stream );
			element = result;
		}
		break;
		case '[':
		{
			array_t * result = new ( resource->allocate( sizeof( array_t ) ) ) array_t( resource );
			result->read( stream );
			element = result;
		}
		break;
		case '\"':
		{
			string_t result( resource );
			result.read( stream );
			element = result;
		}
		break;
		case '-': case '.': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
		{
			number_t result( resource );
			result.read( stream );
			element = result;
		}
		break;
		case '{':
		{
			object_t * result = new ( resource->allocate( sizeof( object_t ) ) ) object_t( resource );
			result->read( stream );
			element = result;
		}
		break;
		case 'f': case 't':
		{
			boolean_t result( resource );
			result.read( stream );
			element = result;
		}
		break;
		default:
			throw std::invalid_argument( "unknown character" );
			break;
		}

		return { resource, element };
	}
	template< typename T > void __write_value( const value & val, ostream_wrapper< T > & stream, std::uint32_t depth, std::uint32_t tab )
	{
		switch ( val.get_type() )
		{
		case jsonhpp::type::null:
			val.get_null().write( stream, depth, tab );
			break;
		case jsonhpp::type::array:
			val.get_array().write( stream, depth, tab );
			break;
		case jsonhpp::type::string:
			val.get_string().write( stream, depth, tab );
			break;
		case jsonhpp::type::number:
			val.get_number().write( stream, depth, tab );
			break;
		case jsonhpp::type::object:
			val.get_object().write( stream, depth, tab );
			break;
		case jsonhpp::type::boolean:
			val.get_boolean().write( stream, depth, tab );
			break;
		}
	}
	jsonhpp::element< jsonhpp::type::array > * __make_array( std::pmr::memory_resource * resource, const std::initializer_list< value > & list )
	{
		auto result = new ( resource->allocate( sizeof( array_t ) ) ) array_t( resource );
		for ( const auto & it : list )
		{
			result->push_back( it );
		}
		return result;
	}
	jsonhpp::element< jsonhpp::type::object > * __make_object( std::pmr::memory_resource * resource, const std::initializer_list< value > & list )
	{
		auto result = new ( resource->allocate( sizeof( object_t ) ) ) object_t( resource );
		for ( auto it = list.begin(); it != list.end(); it += 2 )
		{
			result->insert( it->get_string(), *( it + 1 ) );
		}
		return result;
	}
	jsonhpp::element< jsonhpp::type::array > * __copy_array( std::pmr::memory_resource * resource, element< type::array > * copy )
	{
		auto result = new ( resource->allocate( sizeof( array_t ) ) ) array_t( resource );
		*result = *copy;
		return result;
	}
	jsonhpp::element< jsonhpp::type::object > * __copy_object( std::pmr::memory_resource * resource, element< type::object > * copy )
	{
		auto result = new ( resource->allocate( sizeof( object_t ) ) ) object_t( resource );
		*result = *copy;
		return result;
	}

	template< typename T > static void read( jsonhpp::document & doc, jsonhpp::istream_wrapper< T > & stream )
	{
		doc = __read_value( doc.resource(), stream );
	}

	template< typename T > static void write( const jsonhpp::document & doc, jsonhpp::ostream_wrapper< T > & stream, std::uint32_t tab = 0 )
	{
		__write_value( doc, stream, 0, tab );
	}


	static void read( jsonhpp::document & doc, const char * json )
	{
		jsonhpp::istream_wrapper< char * > wrapper( json, std::strlen( json ) );

		read( doc, wrapper );
	}

	static void read( jsonhpp::document & doc, std::istream & stream )
	{
		jsonhpp::istream_wrapper< std::istream > wrapper( stream );

		read( doc, wrapper );
	}

	template< typename A > static void read( jsonhpp::document & doc, const std::basic_string< char, std::char_traits< char >, A > & str )
	{
		jsonhpp::istream_wrapper< std::basic_string< char, std::char_traits< char >, A > > wrapper( str );

		read( doc, wrapper );
	}

	static void write( const jsonhpp::document & doc, std::ostream & stream, std::uint32_t tab = 0 )
	{
		jsonhpp::ostream_wrapper< std::ostream > wrapper( stream );

		write( doc, wrapper, tab );
	}

	static void write( const jsonhpp::document & doc, char * data, std::size_t size, std::uint32_t tab = 0 )
	{
		jsonhpp::ostream_wrapper< char * > wrapper( data, size );

		write( doc, wrapper, tab );
	}

	template< typename A > static void write( const jsonhpp::document & doc, std::basic_string< char, std::char_traits< char >, A > & str, std::uint32_t tab = 0 )
	{
		jsonhpp::ostream_wrapper< std::basic_string< char, std::char_traits< char >, A > > wrapper( str );

		write( doc, wrapper, tab );
	}

}

#endif//JSONHPP_HPP__1E85F87A_AD28_4BF2_9739_184AF09C9524
