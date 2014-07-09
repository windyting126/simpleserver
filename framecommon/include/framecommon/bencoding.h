// bencoding.h: interface for the bencoding class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __BENCODING_H
#define __BENCODING_H

//#pragma warning(disable:4786)

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cassert>

#ifndef __int64
#define __int64	long long
#endif

namespace bencoding
{
    class var;

    enum var_type
    {
        vt_null, // No type
        vt_int,    // Integer
        vt_str,    // String
        vt_lst,    // List
        vt_dic,    // Dictionary
    };

    typedef std::string             bc_str;
    typedef signed __int64          bc_int;
    typedef std::vector<var>        bc_lst;
    typedef std::map<bc_str, var>   bc_dic;
    
    var_type vt( const bc_str & );
    var_type vt( const bc_int & );
    var_type vt( const bc_lst & );
    var_type vt( const bc_dic & );

    class var  
    {
    public:
        var();
        virtual ~var();

        void clear();

        template<class _Ty>
        var( const _Ty & r )
        {
            reset();
            assign(r);
        }

        var( const var & r )
        {
            reset();
            if ( this != &r )
            {
                assign(r);
            }
        }
    
        operator bc_str&();
        operator bc_int();
        operator bc_lst&();
        operator bc_dic&();

        template<class _Ty>
        var & operator=( const _Ty & r )
        {
            assign(r);
            return *this;
        }

        //template<>
        var & operator=( const var & r )
        {
            if ( this != &r )
            {
                assign(r);
            }
            return *this;
        }

        template<class _Ty>
        bool operator==( const _Ty & r) const
        {
            return ((_type==vt(r))&&(*((_Ty*)&_value)==r));
        }

//        template<>
        bool operator==(const var & r ) const
        {
            if ( this == &r )
            {
                return true;
            }

            if ( _type == r._type )
            {
                switch ( _type )
                {
                case vt_str:
                    return *((bc_str*)&_value)==*((bc_str*)&r._value);
                    break;
                case vt_int:
                    return *((bc_int*)&_value)==*((bc_int*)&r._value);
                    break;
                case vt_lst:
                    return *((bc_lst*)&_value)==*((bc_lst*)&r._value);
                    break;
                case vt_dic:
                    return *((bc_dic*)&_value)==*((bc_dic*)&r._value);
                    break;
                case vt_null:
                    return true;
                    break;
                default:
//                    assert(NULL);
                    return true;
                    break;
                }
            }
            else
            {
                return false;
            }
        }

        template<class _Ty>
        bool operator!=( const _Ty & r) const
        {
            return !(*this==r);
        }

        // method for list or dictionary type
        var_type type() const;
        unsigned int size() const;
        var & operator[]( unsigned int ); // get list element
        var & operator[]( const char * ); // get dictionary element
        var & operator[]( const bc_str & ); // get dictionary element
        unsigned int count( const char * ); // get dictionary element count by key, return 1 or 0
        unsigned int count( const bc_str & ); // get dictionary element count by key, return 1 or 0

        enum in_format_type
        {
            ifmt_cod, // input fomrat of bencoding
        };

        enum out_format_type
        {
            ofmt_cod, // output format of bencoding
            ofmt_des, // output format of description
        };

        var & format( in_format_type );
        var & format( out_format_type );

        friend std::ostream & operator<<( std::ostream &, const var & );
        friend std::istream & operator>>( std::istream &, var & );

        static const char * dic_prefix;
        static const char * dic_suffix;
        static const char * dic_break;
        static const char * dic_mapping;
        static const char * lst_prefix;
        static const char * lst_suffix;
        static const char * lst_break;
        static const char * str_prefix;
        static const char * str_suffix;
        static const char * str_tab;

    protected:
        
        void reset();

        template<class _Ty>    void assign( const _Ty & r )
        {
            clear();    
            _type = vt(r);
            new ((void *)&_value) _Ty(r);
        }

        //template<> void assign( const char* r )
        //{
        //    clear();
        //    _type = vt_str;
        //    new ((void *)&_value) bc_str(r);
        //}
        
        //template<> 
        void assign( const var & r )
        {
            clear();
            if ( r._type != vt_null )
            {
                _type = r._type;
                switch( r._type )
                {
                case vt_str:
                    new ((void *)&_value) bc_str(*((bc_str*)(&r._value)));
                    break;
                case vt_int:
                    new ((void *)&_value) bc_int(*((bc_int*)(&r._value)));
                    break;
                case vt_lst:
                    new ((void *)&_value) bc_lst(*((bc_lst*)(&r._value)));
                    break;
                case vt_dic:
                    new ((void *)&_value) bc_dic(*((bc_dic*)(&r._value)));
                    break;
                default:
//                    assert(NULL);
                    break;
                }
            }
        }

    private:

        in_format_type    _in_format;
        out_format_type    _out_format;

        union tag_value // define this union to get max size between str,int,lst,dic
        {
            char _str[sizeof(bc_str)];
            char _int[sizeof(bc_int)];
            char _lst[sizeof(bc_lst)];
            char _dic[sizeof(bc_dic)];
        };
        
        var_type    _type;
        char        _value[sizeof(tag_value)];
    };

    std::ostream & operator<<( std::ostream &, const bencoding::var & );
    std::istream & operator>>( std::istream &, bencoding::var & );
}


#endif // !defined(AFX_BENCODING_H__2B3E6AF1_B7FC_48D7_AA3A_69AFA3B00FB0__INCLUDED_)
