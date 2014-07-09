// bencoding.cpp: implementation of the bencoding class.
//
//////////////////////////////////////////////////////////////////////

#include "bencoding.h"
#include <cstring>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace bencoding
{
	var_type vt( const bc_str & )
	{
		return vt_str;
	}
	var_type vt( const bc_int & )
	{
		return vt_int;
	}
	var_type vt( const bc_lst & )
	{
		return vt_lst;
	}
	var_type vt( const bc_dic & )
	{
		return vt_dic;
	}

	const char * var::dic_prefix	= "{";
	const char * var::dic_suffix	= "}";
	const char * var::dic_break		= ";";
	const char * var::dic_mapping	= "=";
	const char * var::lst_prefix	= "[";
	const char * var::lst_suffix	= "]";
	const char * var::lst_break		= ", ";
	const char * var::str_prefix	= "'";
	const char * var::str_suffix	= "'";
	
	var::var()
	{
		reset();
	}
	
	var::~var()
	{
		clear();
	}
	
	void var::reset()
	{
		_in_format = ifmt_cod;
		_out_format = ofmt_des;
		_type = vt_null;
		memset( _value, 0, sizeof(_value) );
	}
	
	void var::clear()
	{
		if ( _type != vt_null )
		{
			switch( _type )
			{
			case vt_str:
				((bc_str*)&_value)->~bc_str();
				break;
			case vt_int:				
				((bc_int*)&_value)->~bc_int();
				break;
			case vt_lst:
				((bc_lst*)&_value)->~bc_lst();
				break;
			case vt_dic:
				((bc_dic*)&_value)->~bc_dic();
				break;
			default:
//				assert(NULL);
				break;
			}
			
			_type = vt_null;
			memset( _value, 0, sizeof(_value) );
		}
	}

	var::operator bc_str&()
	{
		if ( _type != vt_str )
		{
			clear();
			_type = vt_str;
			new ((void *)&_value) bc_str();
		}		
		return *((bc_str*)&_value);
	}
	
	var::operator bc_int()
	{
		if ( _type != vt_int )
		{
			clear();
			_type = vt_int;
			memset(_value, 0, sizeof(_value));
		}		
		return *((bc_int*)&_value);
	}
	
	var::operator bc_lst&()
	{
		if ( _type != vt_lst )
		{
			clear();
			_type = vt_lst;
			new ((void *)&_value) bc_lst();
		}		
		return *((bc_lst*)&_value);
	}

	var::operator bc_dic&()
	{
		if ( _type != vt_dic )
		{
			clear();
			_type = vt_dic;
			new ((void *)&_value) bc_dic();
		}		
		return *((bc_dic*)&_value);
	}

	var_type var::type() const
	{
		return _type;
	}

	unsigned int var::size() const
	{
		if ( _type == vt_lst )
		{
			return ((bc_lst*)&_value)->size();
		}
		else if ( _type == vt_dic )
		{
			return ((bc_dic*)&_value)->size();
		}
		else
		{
			return 0;
		}
	}

	var & var::operator[]( unsigned int index )
	{
		bc_lst & l = *this;
		return l[index];
	}

	var & var::operator[]( const char * key )
	{
		bc_dic & d = *this;
		return d[key];
	}

	var & var::operator[]( const bc_str & key )
	{
		bc_dic & d = *this;
		return d[key];
	}

    unsigned int var::count( const char * key )
    {
        bc_dic & d = *this;
        return d.count(key);
    }
    unsigned int var::count( const bc_str & key )
    {
        bc_dic & d = *this;
        return d.count(key);
    }
	
	var & var::format( in_format_type io_format )
	{
		_in_format = io_format;
		return *this;
	}
	
	var & var::format( out_format_type io_format )
	{
		_out_format = io_format;
		return *this;
	}
	
	std::ostream & operator<<( std::ostream & out, const var & r )
	{
		if ( var::ofmt_des == r._out_format )
		{
			switch ( r._type )
			{
			case vt_str:
				return out << var::str_prefix << *((bc_str*)&r._value) << var::str_suffix;
				break;
			case vt_int:
				return out << *((bc_int*)&r._value);
				break;
			case vt_lst:
				{
					out << var::lst_prefix;
					bc_lst & lst = *((bc_lst*)&r._value);
					bc_lst::iterator it = lst.begin();
					for ( ; it!=lst.end(); )
					{
						out << it->format(var::ofmt_des);
						it++;
						if ( it==lst.end() )
						{
							break;
						}
						out << var::lst_break;
					}
					out << var::lst_suffix;
				}
				break;
			case vt_dic:
				{
					out << var::dic_prefix;
					bc_dic & dic = *((bc_dic*)&r._value);
					bc_dic::iterator it = dic.begin();
					for ( ; it!=dic.end(); )
					{
						out << it->first;
						out << var::dic_mapping;
						out << it->second.format(var::ofmt_des);
						it++;
						if ( it==dic.end() )
						{
							break;
						}
						out << var::dic_break;
					}
					out << var::dic_suffix;
				}
				break;
			default:
				break;
			}
		}
		else if ( var::ofmt_cod == r._out_format )
		{
			if ( r._type == vt_str )
			{
				bc_str & s = *((bc_str*)&r._value);
				out << s.size();
				out << ':';
				out.write( s.c_str(), s.size() ); 
			}
			else if ( r._type == vt_int )
			{
				out << 'i';
				out << *((bc_int*)&r._value);
				out << 'e';
			}
			else if ( r._type == vt_lst )
			{
				out << 'l';
				bc_lst & l = *((bc_lst*)&r._value);
				bc_lst::iterator it = l.begin();
				for ( ; it!=l.end(); it++ )
				{
					out << (*it).format(var::ofmt_cod);
				}
				out << 'e';
			}
			else if ( r._type == vt_dic )
			{
				out << 'd';
				bc_dic & d = *((bc_dic*)&r._value);
				bc_dic::iterator it = d.begin();
				for ( ; it!=d.end(); it++ )
				{
					const bc_str & s = it->first;
					out << s.size();
					out << ':';
					out.write( s.c_str(), s.size() ); 
					
					out << it->second.format(var::ofmt_cod);
				}
				out << 'e';
			}
			else
			{
				// output nothing
			}
		}
		else
		{
//			assert( NULL );
		}
		
		return out;
	}	
	
	std::istream & operator>>( std::istream & in, var & r )
	{
		assert( var::ifmt_cod == r._in_format );
		
		r.clear();
		
		if ( !in.good() )
		{
			return in; // in stream is not good.
		}
		
		if ( var::ifmt_cod == r._in_format )
		{
			char c;
			if ( (in>>c).fail() )
			{
				return in;
			}
			
			if ( c >= '0' && c <= '9' ) // it's integer, so it's a string
			{
				in.unget();
				int i;
				if ( (in>>i).fail() )
				{
					return in;
				}
				
				if ( (in>>c).fail() )
				{
					return in;
				}
				
				if ( c != ':' )
				{
					in.setstate( std::istream::failbit );
					return in;
				}
				
				std::string s( i, ' ');
				if ( in.read( (char *)s.c_str(), i ).fail() )
				{					
					return in;
				}

				r = s;
			}
			else if ( c == 'i' )
			{
				r = 0;
				bc_int i = 0;
				if ( (in >> i).fail() )
				{
					return in;
				}
				if ( (in >> c).fail() )
				{
					return in;
				}
				if ( c != 'e' )
				{
					in.setstate( std::istream::failbit );
					return in;
				}
				r = i;
			}
			else if ( c == 'l' )
			{
				bc_lst & l = r;
				do
				{
                    if ( (in >> c).fail() ) // "le" means an empty list
                    {
                        return in;
                    }
                    if ( c == 'e' ) // end of list
                    {
                        break;
                    }
                    in.unget();

					var v;
					if ( (in >> v.format(var::ifmt_cod)).fail() )
					{
						return in;
					}
					l.push_back(v);
					
					if ( (in >> c).fail() )
					{
						return in;
					}
					if ( c == 'e' ) // end of list
					{
						break;
					}
					in.unget();
				}
				while(true);
			}
			else if ( c == 'd' )
			{
				bc_dic & d = r;
				do
				{
                    if ( (in >> c).fail() ) // "de" means an empty dictionary
                    {
                        return in;
                    }
                    if ( c == 'e' )
                    {
                        break;
                    }
                    in.unget();

					// read string key
					int i;
					if ( (in >> i).fail() )
					{
						return in;
					}
					if ( (in >> c).fail() )
					{
						return in;
					}
					if ( c != ':' )
					{
						in.setstate( std::istream::failbit );
						return in;
					}
					std::string key( i, ' ' );
					if ( in.read( (char *)key.c_str(), i ).fail() )
					{
						return in;
					}
					
					var v;
					if ( (in >> v.format(var::ifmt_cod)).fail() )
					{
						return in;
					}
					d.insert( std::pair<std::string, var>(key,v) );
					
					if ( (in >> c).fail() )
					{
						return in;
					}
					if ( c == 'e' )
					{
						break;
					}
					in.unget();
				}
				while(true);
			}
			else
			{
				in.setstate( std::istream::failbit ); // set in stream state is fail
			}		
		}
		else
		{
//			assert( NULL );
		}
		
		return in;
	}
}
