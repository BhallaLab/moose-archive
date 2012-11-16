/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "header.h"
#include "../shell/Shell.h"
#include "Dinfo.h"

Cinfo::Cinfo( const string& name,
				const Cinfo* baseCinfo,
				Finfo** finfoArray,
				unsigned int nFinfos,
				DinfoBase* d,
				const string* doc,
				unsigned int numDoc,
				ThreadExecBalancer internalThreadBalancer
)
		: name_( name ), baseCinfo_( baseCinfo ), dinfo_( d ),
			numBindIndex_( 0 ),
			internalThreadBalancer_( internalThreadBalancer )
{
	if ( cinfoMap().find( name ) != cinfoMap().end() ) {
		cout << "Warning: Duplicate Cinfo name " << name << endl;
	}
	init( finfoArray, nFinfos );
	cinfoMap()[ name ] = this;
	doc_.clear();
	// cout << "Doing initCinfo for " << name << " with numDoc = " << numDoc << endl;
	if ( doc && numDoc ) {
		for ( unsigned int i = 0; i < numDoc - 1; i += 2 ) {
			const string argName = doc[i];
			const string argVal = doc[i+1];
			// cout << "in initCinfo for " << name << ", doc[" << i << "] = " << doc[i] << ", " << doc[i+1] << endl;
			// doc_[ doc[i] ] = doc[i+i];
			doc_[ argName ] = argVal;
		}
	}
}

Cinfo::Cinfo()
		: name_( "dummy" ), baseCinfo_( 0 ), dinfo_( 0 ),
			numBindIndex_( 0 )
{;}

Cinfo::Cinfo( const Cinfo& other )
		: name_( "dummy" ), baseCinfo_( 0 ), dinfo_( 0 ),
			numBindIndex_( 0 )
{;}

Cinfo::~Cinfo()
{
	delete dinfo_;
}

////////////////////////////////////////////////////////////////////
// Initialization funcs
////////////////////////////////////////////////////////////////////

/**
 * init: initializes the Cinfo. Must be called just once
 */
void Cinfo::init( Finfo** finfoArray, unsigned int nFinfos )
{
	if ( baseCinfo_ ) {
		// Copy over base Finfos.
		numBindIndex_ = baseCinfo_->numBindIndex_;
		finfoMap_ = baseCinfo_->finfoMap_;
		funcs_ = baseCinfo_->funcs_;
		postCreationFinfos_ = baseCinfo_->postCreationFinfos_;
	} 
	for ( unsigned int i = 0; i < nFinfos; i++ ) {
		registerFinfo( finfoArray[i] );
	}
}

FuncId Cinfo::registerOpFunc( const OpFunc* f )
{
	FuncId ret = funcs_.size();
	funcs_.push_back( f );
	return ret;
}

void Cinfo::overrideFunc( FuncId fid, const OpFunc* f )
{
	assert ( funcs_.size() > fid );
	funcs_[ fid ] = f;
}

BindIndex Cinfo::registerBindIndex()
{
	return numBindIndex_++;
}

void Cinfo::registerFinfo( Finfo* f )
{
		finfoMap_[ f->name() ] = f;
		f->registerFinfo( this );
		if ( dynamic_cast< DestFinfo* >( f ) ) {
			destFinfos_.push_back( f );
		}
		else if ( dynamic_cast< SrcFinfo* >( f ) ) {
			srcFinfos_.push_back( f );
		}
		else if ( dynamic_cast< ValueFinfoBase* >( f ) ) {
			valueFinfos_.push_back( f );
		}
		else if ( dynamic_cast< LookupValueFinfoBase* >( f ) ) {
			lookupFinfos_.push_back( f );
		}
		else if ( dynamic_cast< SharedFinfo* >( f ) ) {
			sharedFinfos_.push_back( f );
		}
		else if ( dynamic_cast< FieldElementFinfoBase* >( f ) ) {
			fieldElementFinfos_.push_back( f );
		}
}

void Cinfo::registerPostCreationFinfo( const Finfo* f )
{
	postCreationFinfos_.push_back( f );
}

void Cinfo::postCreationFunc( Id newId, Element* newElm ) const
{
	for ( vector< const Finfo* >::const_iterator i =
		postCreationFinfos_.begin();
		i != postCreationFinfos_.end(); ++i )
		(*i)->postCreationFunc( newId, newElm );
}

// Static function called by init()
void Cinfo::makeCinfoElements( Id parent )
{
	static Dinfo< Cinfo > dummy;
	vector< unsigned int > dims( 1, 0 );

	for ( map< string, Cinfo* >::iterator i = cinfoMap().begin(); 
		i != cinfoMap().end(); ++i ) {
		Id id = Id::nextId();
		char* data = reinterpret_cast< char* >( i->second );
		DataHandler* dh = new ZeroDimHandler( &dummy, data );
			// (new Dinfo< Cinfo >(), data );
		new Element( id, Cinfo::initCinfo(), i->first, dh );
		Shell::adopt( parent, id );
		// cout << "Cinfo::makeCinfoElements: parent= " << parent << ", Id = " << id << ", name = " << i->first << endl;
	}
}

//////////////////////////////////////////////////////////////////////
// Look up operations.
//////////////////////////////////////////////////////////////////////

/**
 * Looks up Finfo from name.
 */
const Finfo* Cinfo::findFinfo( const string& name ) const
{
	map< string, Finfo*>::const_iterator i = finfoMap_.find( name );
	if ( i != finfoMap_.end() )
		return i->second;
	return 0;
}

/**
 * looks up OpFunc by FuncId
 */
const OpFunc* Cinfo::getOpFunc( FuncId fid ) const {
	if ( fid < funcs_.size () )
		return funcs_[ fid ];
	return 0;
}

/*
FuncId Cinfo::getOpFuncId( const string& funcName ) const {
	map< string, FuncId >::const_iterator i = opFuncNames_.find( funcName );
	if ( i != opFuncNames_.end() ) {
		return i->second;
	}
	return 0;
}
*/

////////////////////////////////////////////////////////////////////////
// Miscellaneous
////////////////////////////////////////////////////////////////////////

unsigned int Cinfo::numBindIndex() const
{
	return numBindIndex_;
}

const map< string, Finfo* >& Cinfo::finfoMap() const
{
	return finfoMap_;
}

const DinfoBase* Cinfo::dinfo() const
{
	return dinfo_;
}

bool Cinfo::isA( const string& ancestor ) const
{
	if ( ancestor == "Neutral" ) return 1;
	const Cinfo* base = this;
	while( base && base != Neutral::initCinfo() ) {
		if ( ancestor == base->name_ )
			return 1;
		base = base->baseCinfo_;
	}
	return 0;
}

void Cinfo::reportFids() const
{
	for ( map< string, Finfo*>::const_iterator i = finfoMap_.begin();
		i != finfoMap_.end(); ++i ) {
		const DestFinfo* df = dynamic_cast< const DestFinfo* >(
			i->second );
		if ( df ) {
			cout << df->getFid() << "	" << df->name() << endl;
		}
	}
}

ThreadExecBalancer Cinfo::internalThreadBalancer() const
{
	return internalThreadBalancer_;
}

////////////////////////////////////////////////////////////////////////
// Private functions.
////////////////////////////////////////////////////////////////////////


/*
map< OpFunc, FuncId >& Cinfo::funcMap()
{
	static map< OpFunc, FuncId > lookup_;
	return lookup_;
}
*/

////////////////////////////////////////////////////////////////////////
// MOOSE class functions.
////////////////////////////////////////////////////////////////////////

const Cinfo* Cinfo::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ReadOnlyValueFinfo< Cinfo, string > docs(
			"docs",
			"Documentation",
			&Cinfo::getDocs
		);

		static ReadOnlyValueFinfo< Cinfo, string > baseClass(
			"baseClass",
			"Name of base class",
			&Cinfo::getBaseClass
		);

		//////////////////////////////////////////////////////////////
		// FieldElementFinfo definitions for different kinds of Finfos
		// Assume up to 64 of each.
		//////////////////////////////////////////////////////////////
		static FieldElementFinfo< Cinfo, Finfo > srcFinfo( "srcFinfo",
			"SrcFinfos in this Class",
			Finfo::initCinfo(),
			&Cinfo::getSrcFinfo,
			&Cinfo::setNumFinfo, // Dummy
			&Cinfo::getNumSrcFinfo,
			64
		);
		static FieldElementFinfo< Cinfo, Finfo > destFinfo( "destFinfo",
			"DestFinfos in this Class",
			Finfo::initCinfo(),
			&Cinfo::getDestFinfo,
			&Cinfo::setNumFinfo, // Dummy
			&Cinfo::getNumDestFinfo,
			64
		);
		static FieldElementFinfo< Cinfo, Finfo > valueFinfo( "valueFinfo",
			"ValueFinfos in this Class",
			Finfo::initCinfo(),
			&Cinfo::getValueFinfo,
			&Cinfo::setNumFinfo, // Dummy
			&Cinfo::getNumValueFinfo,
			64
		);
		static FieldElementFinfo< Cinfo, Finfo > lookupFinfo( "lookupFinfo",
			"LookupFinfos in this Class",
			Finfo::initCinfo(),
			&Cinfo::getLookupFinfo,
			&Cinfo::setNumFinfo, // Dummy
			&Cinfo::getNumLookupFinfo,
			64
		);
		static FieldElementFinfo< Cinfo, Finfo > sharedFinfo( "sharedFinfo",
			"SharedFinfos in this Class",
			Finfo::initCinfo(),
			&Cinfo::getSharedFinfo,
			&Cinfo::setNumFinfo, // Dummy
			&Cinfo::getNumSharedFinfo,
			64
		);
		static FieldElementFinfo< Cinfo, Finfo > fieldElementFinfo( 
			"fieldElementFinfo",
			"fieldElementFinfos in this Class",
			Finfo::initCinfo(),
			&Cinfo::getFieldElementFinfo,
			&Cinfo::setNumFinfo, // Dummy
			&Cinfo::getNumFieldElementFinfo,
			64
		);

    static string doc[] =
    {
        "Name", "Cinfo",
        "Author", "Upi Bhalla",
        "Description", "Class information object."
    };

	static Finfo* cinfoFinfos[] = {
		&docs,				// ReadOnlyValue
		&baseClass,		// ReadOnlyValue
		&srcFinfo,			// FieldElementFinfo
		&destFinfo,			// FieldElementFinfo
		&valueFinfo,			// FieldElementFinfo
		&lookupFinfo,		// FieldElementFinfo
		&sharedFinfo,		// FieldElementFinfo
		&fieldElementFinfo,		// FieldElementFinfo
	};

	static Cinfo cinfoCinfo (
		"Cinfo",
		Neutral::initCinfo(),
		cinfoFinfos,
		sizeof( cinfoFinfos ) / sizeof ( Finfo* ),
		new Dinfo< Cinfo >(),
        doc,
        sizeof(doc)/sizeof(string)        
	);

	return &cinfoCinfo;
}

static const Cinfo* cinfoCinfo = Cinfo::initCinfo();


///////////////////////////////////////////////////////////////////
// Field functions
///////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// Below we have a set of functions for getting various categories of
// Finfos. These also return the base class finfos. The baseclass finfos 
// come first, then the new finfos. This is a bit messy because it changes
// the indices of the new finfos, but I shouldn't be looking them up 
// by index anyway.
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void Cinfo::setNumFinfo( unsigned int val ) // Dummy function
{
	;
}

////////////////////////////////////////////////////////////////////
const string& Cinfo::srcFinfoName( BindIndex bid ) const
{
	static const string err = "";
	for ( vector< Finfo* >::const_iterator i = srcFinfos_.begin(); 
		i != srcFinfos_.end(); ++i ) {
		const SrcFinfo* sf = dynamic_cast< const SrcFinfo* >( *i );
		assert( sf );
		if ( sf->getBindIndex() == bid ) {
			return sf->name();
		}
	}
	cout << "Error: Cinfo::srcFinfoName( " << bid << " ): not found\n";
	return err;
}

const string& Cinfo::destFinfoName( FuncId fid ) const
{
	static const string err = "";
	for ( vector< Finfo* >::const_iterator i = destFinfos_.begin(); 
		i != destFinfos_.end(); ++i ) {
		const DestFinfo* df = dynamic_cast< const DestFinfo* >( *i );
		assert( df );
		if ( df->getFid() == fid ) {
			return df->name();
		}
	}
	cout << "Error: Cinfo::destFinfoName( " << fid << " ): not found\n";
	return err;
}

