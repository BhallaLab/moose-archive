/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _FTYPE2_H
#define _FTYPE2_H

/**
 * The Ftype2 handles 2-argument functions.
 */
template < class T1, class T2 > class Ftype2: public Ftype
{
		public:
			unsigned int nValues() const {
				return 2;
			}
			
			bool isSameType( const Ftype* other ) const {
				return ( dynamic_cast< const Ftype2< T1, T2 >* >( other ) != 0 );
			}
			
			static bool isA( const Ftype* other ) {
				return ( dynamic_cast< const Ftype2< T1, T2 >* >( other ) != 0 );
			}

			size_t size() const
			{
				return sizeof( T1 ) + sizeof( T2 );
			}

			/**
			 * This variant works for most kinds of Finfo,
			 * so we make a default version.
			 * In a few cases we need to specialize it because
			 * we need extra info from the Finfo.
			 * This function makes the strong assumption that
			 * we will NOT look up any dynamic finfo, and will
			 * NOT use the conn index in any way. This would
			 * fail because the conn index given is a dummy one.
			 * We do a few assert tests in downstream functions
			 * for any such attempts
			 * to search for a Finfo based on the index.
			 */
			virtual bool set(
				Element* e, const Finfo* f, T1 v1, T2 v2 ) const {

				void (*set)( const Conn&, T1 v1, T2 v2 ) =
					reinterpret_cast< 
						void (*)( const Conn&, T1 v1, T2 v2 )
					>(
									f->recvFunc()
					);
				Conn c( e, MAXUINT );
				set( c, v1, v2 );
				return 1;
			}

			/**
			 * This is a virtual function that takes a string,
			 * converts it to two values, and assigns it to a field.
			 * Returns true on success.
			 * It will run into trouble if the contents are strings
			 * with spaces or commas.
			 */
			bool strSet( Element* e, const Finfo* f, const string& s )
					const
			{
				string::size_type pos = s.find_first_of( ", 	" );
				if ( pos == string::npos )
						return 0;
				if ( pos < 1 )
						return 0;
				string s1 = s.substr( 0, pos );
				pos = s.find_last_of( ", 	" );
				if ( pos >=  s.length() - 1 )
						return 0;
				string s2 = s.substr( pos );
				T1 val1;
				if ( str2val( s1, val1 ) ) {
					T2 val2;
					if ( str2val( s2, val2 ) )
						return this->set( e, f, val1, val2 );
				}

				return 0;
			}

			static const Ftype* global() {
				static Ftype* ret = new Ftype2< T1, T2 >();
				return ret;
			}

			RecvFunc recvFunc() const {
				return 0;
			}

			RecvFunc trigFunc() const {
				return 0;
			}
    std::string getTemplateParameters()
    {
        std::string s = "";
        std::string type1(typeid(T1).name());
        std::string type2(typeid(T2).name());
        s = Ftype::full_type(type1)+" "+Ftype::full_type(type2);                     
        return s;
    }
    
    
};

#endif // _FTYPE2_H
