/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2012 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <time.h>
#include <iostream>
#include <fstream>
#include "header.h"
#include "PoolBase.h"
#include "../shell/Wildcard.h"
#include "EnzBase.h"
#include "CplxEnzBase.h"
#include "ReacBase.h"

void writeHeader( ofstream& fout, 
		double simdt, double plotdt, double maxtime, double defaultVol)
{
	time_t rawtime;
	time( &rawtime );

	fout << 
	"//genesis\n"
	"// kkit Version 11 flat dumpfile\n\n";
	fout << "// Saved on " << ctime( &rawtime ) << endl;
	fout << "include kkit {argv 1}\n";
	fout << "FASTDT = " << simdt << endl;
	fout << "SIMDT = " << simdt << endl;
	fout << "CONTROLDT = " << plotdt << endl;
	fout << "PLOTDT = " << plotdt << endl;
	fout << "MAXTIME = " << maxtime << endl;
	fout << "TRANSIENT_TIME = 2\n"
	"VARIABLE_DT_FLAG = 0\n";
	fout << "DEFAULT_VOL = " << defaultVol << endl;
	fout << "VERSION = 11.0\n"
	"setfield /file/modpath value ~/scripts/modules\n"
	"kparms\n\n";

	fout << 
	"initdump -version 3 -ignoreorphans 1\n"
	"simobjdump table input output alloced step_mode stepsize x y z\n"
	"simobjdump xtree path script namemode sizescale\n"
	"simobjdump xcoredraw xmin xmax ymin ymax\n"
	"simobjdump xtext editable\n"
	"simobjdump xgraph xmin xmax ymin ymax overlay\n"
	"simobjdump xplot pixflags script fg ysquish do_slope wy\n"
	"simobjdump group xtree_fg_req xtree_textfg_req plotfield expanded movealone \\\n"
  	"  link savename file version md5sum mod_save_flag x y z\n"
	"simobjdump geometry size dim shape outside xtree_fg_req xtree_textfg_req x y z\n"
	"simobjdump kpool DiffConst CoInit Co n nInit mwt nMin vol slave_enable \\\n"
  	"  geomname xtree_fg_req xtree_textfg_req x y z\n"
	"simobjdump kreac kf kb notes xtree_fg_req xtree_textfg_req x y z\n"
	"simobjdump kenz CoComplexInit CoComplex nComplexInit nComplex vol k1 k2 k3 \\\n"
  	"  keepconc usecomplex notes xtree_fg_req xtree_textfg_req link x y z\n"
	"simobjdump stim level1 width1 delay1 level2 width2 delay2 baselevel trig_time \\\n"
  	"  trig_mode notes xtree_fg_req xtree_textfg_req is_running x y z\n"
	"simobjdump xtab input output alloced step_mode stepsize notes editfunc \\\n"
  	"  xtree_fg_req xtree_textfg_req baselevel last_x last_y is_running x y z\n"
	"simobjdump kchan perm gmax Vm is_active use_nernst notes xtree_fg_req \\\n"
  	"  xtree_textfg_req x y z\n"
	"simobjdump transport input output alloced step_mode stepsize dt delay clock \\\n"
  	"  kf xtree_fg_req xtree_textfg_req x y z\n"
	"simobjdump proto x y z\n"
	"simundump geometry /kinetics/geometry 0 1.6667e-19 3 sphere \"\" white black 0 0 0\n\n";
}


void writeReac( ofstream& fout, Id id,
				string colour, string textcolour,
			 	double x, double y )
{
	string path = id.path();
	size_t pos = path.find( "/kinetics" );
	path = path.substr( pos );
	double kf = Field< double >::get( id, "kf" );
	double kb = Field< double >::get( id, "kb" );

	fout << "simundump kreac " << path << " 0 " << 
			kf << " " << kb << " \"\" " << 
			colour << " " << textcolour << " " << x << " " << y << " 0\n";
}

void writePool( ofstream& fout, Id id,
				string colour, string textcolour,
			 	double x, double y )
{
	string path = id.path();
	size_t pos = path.find( "/kinetics" );
	path = path.substr( pos );
	double diffConst = Field< double >::get( id, "diffConst" );
	double concInit = Field< double >::get( id, "concInit" );
	double conc = Field< double >::get( id, "conc" );
	double nInit = Field< double >::get( id, "nInit" );
	double n = Field< double >::get( id, "n" );
	double size = Field< double >::get( id, "size" );

	fout << "simundump kpool " << path << " 0 " << 
			diffConst << " " <<
			concInit << " " << 
			conc << " " <<
			n << " " <<
			nInit << " " <<
			0 << " " << 0 << " " << // mwt, nMin
			size * NA * 1e-3  << " " << // volscale
			"0 /kinetics/geometry " << 
			colour << " " << textcolour << " " << x << " " << y << " 0\n";
}

Id getEnzMol( Id id )
{
	static const Finfo* enzFinfo = 
			EnzBase::initCinfo()->findFinfo( "enzDest" );
	vector< Id > ret;
	if ( id.element()->getNeighbours( ret, enzFinfo ) > 0 ) 
		return ret[0];
	return Id();
}

Id getEnzCplx( Id id )
{
	static const Finfo* cplxFinfo = 
			CplxEnzBase::initCinfo()->findFinfo( "cplxDest" );
	vector< Id > ret;
	if ( id.element()->getNeighbours( ret, cplxFinfo ) > 0 )
		return ret[0];
	return Id();
}

void writeEnz( ofstream& fout, Id id,
				string colour, string textcolour,
			 	double x, double y )
{
	string path = id.path();
	size_t pos = path.find( "/kinetics" );
	path = path.substr( pos );
	double k1 = 0;
	double k2 = 0;
	double k3 = 0;
	double nInit = 0;
	double concInit = 0;
	double n = 0;
	double conc = 0;
	Id enzMol = getEnzMol( id );
	assert( enzMol != Id() );
	double vol = Field< double >::get( enzMol, "size" ) * NA * 1e-3; 
	unsigned int isMichaelisMenten = 0;
	if ( id.element()->cinfo()->isA( "CplxEnzBase" ) ) {
		k1 = Field< double >::get( id, "k1" );
		k2 = Field< double >::get( id, "k2" );
		k3 = Field< double >::get( id, "k3" );
		Id cplx = getEnzCplx( id );
		assert( cplx != Id() );
		nInit = Field< double >::get( cplx, "nInit" );
		n = Field< double >::get( cplx, "n" );
		concInit = Field< double >::get( cplx, "concInit" );
		conc = Field< double >::get( cplx, "conc" );
	} else {
		k1 = Field< double >::get( id, "numKm" );
		k3 = Field< double >::get( id, "kcat" );
		k2 = 4.0 * k3;
		k1 = (k2 + k3) / k1;
		isMichaelisMenten = 1;
	}

	fout << "simundump kenz " << path << " 0 " << 
			concInit << " " <<
			conc << " " << 
			nInit << " " <<
			n << " " <<
			vol << " " <<
			k1 << " " <<
			k2 << " " <<
			k3 << " " <<
			0 << " " <<
			isMichaelisMenten << " " <<
			"\"\"" << " " << 
			colour << " " << textcolour << " \"\"" << 
			" " << x << " " << y << " 0\n";
}

void writeGroup( ofstream& fout, Id id,
				string colour, string textcolour,
			 	double x, double y )
{
	string path = id.path();
	size_t pos = path.find( "/kinetics" );
	if ( pos == string::npos ) // Might be finding unrelated neutrals
			return;
	path = path.substr( pos );
	fout << "simundump group " << path << " 0 " << 
			colour << " " << textcolour << " x 0 0 \"\" defaultfile \\\n";
	fout << "  defaultfile.g 0 0 0 " << x << " " << y << " 0\n";
}

void writePlot( ofstream& fout, Id id,
				string colour, string textcolour,
			 	double x, double y )
{
	string path = id.path();
	size_t pos = path.substr( 2 ).find( "/" );
	if ( pos == string::npos ) // Might be finding unrelated neutrals
			return;
	path = path.substr( pos + 2 );
	fout << "simundump xplot " << path << " 3 524288 \\\n" << 
	"\"delete_plot.w <s> <d>; edit_plot.D <w>\" " << textcolour << " 0 0 1\n";
		/*
simundump xplot /graphs/conc1/Sub.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc1/Prd.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 60 0 0 1
  */
}

void writeLookupTable( ofstream& fout, Id id,
				string colour, string textcolour,
			 	double x, double y )
{
}

void writeTable( ofstream& fout, Id id,
				string colour, string textcolour,
			 	double x, double y )
{
	ObjId pa = Neutral::parent( id.eref() );
	if ( pa.id.element()->getName().substr( 0, 4 ) == "conc" )
		writePlot( fout, id, colour, textcolour, x , y);
	else
		writeLookupTable( fout, id, colour, textcolour, x , y);
}

void writeGui( ofstream& fout )
{
	fout << "simundump xgraph /graphs/conc1 0 0 99 0.001 0.999 0\n"
	"simundump xgraph /graphs/conc2 0 0 100 0 1 0\n"
	"simundump xgraph /moregraphs/conc3 0 0 100 0 1 0\n"
	"simundump xgraph /moregraphs/conc4 0 0 100 0 1 0\n"
	"simundump xcoredraw /edit/draw 0 -6 4 -2 6\n"
	"simundump xtree /edit/draw/tree 0 \\\n"
	"  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \"edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>\" auto 0.6\n"
	"simundump xtext /file/notes 0 1\n";
}

void writeFooter( ofstream& fout )
{
	fout << "\nenddump\n";
	fout << "complete_loading\n";
}

Id findInfo( Id id )
{
	vector< Id > kids;
	Neutral::children( id.eref(), kids );

	for ( vector< Id >::iterator i = kids.begin(); i != kids.end(); ++i ) {
		Element* e = i->element();
		if ( e->getName() == "info" && e->cinfo()->isA( "Annotator" ) )
			return *i;
	}
	return Id();
}

void getInfoFields( Id id, string& bg, string& fg, 
				double& x, double& y, double side, double dx )
{
	Id info = findInfo( id );
	if ( info != Id() ) {
		bg = Field< string >::get( info, "color" );
		fg = Field< string >::get( info, "textColor" );
		x = Field< double >::get( info, "x" );
		y = Field< double >::get( info, "y" );
	} else {
		bg = "cyan";
		fg = "black";
		x += dx;
		if ( x > side ) {
				x = 0;
				y += dx;
		}
	}
}


string trimPath( const string& path )
{
	size_t pos = path.find( "/kinetics" );
	if ( pos == string::npos ) // Might be finding unrelated neutrals
			return "";
	return path.substr( pos );
}

void storeReacMsgs( Id reac, vector< string >& msgs )
{
	// const Finfo* reacFinfo = PoolBase::initCinfo()->findFinfo( "reacDest" );
	// const Finfo* noutFinfo = PoolBase::initCinfo()->findFinfo( "nOut" );
	static const Finfo* subFinfo = 
			ReacBase::initCinfo()->findFinfo( "toSub" );
	static const Finfo* prdFinfo = 
			ReacBase::initCinfo()->findFinfo( "toPrd" );
	vector< Id > targets;
	
	reac.element()->getNeighbours( targets, subFinfo );
	string reacPath = trimPath( reac.path() );
	for ( vector< Id >::iterator i = targets.begin(); i != targets.end(); ++i ) {
		string s = "addmsg " + trimPath( i->path() ) + " " + reacPath +
			   	" SUBSTRATE n";
		msgs.push_back( s );
		s = "addmsg " + reacPath + " " + trimPath( i->path() ) + 
				" REAC A B";
		msgs.push_back( s );
	}

	targets.resize( 0 );
	reac.element()->getNeighbours( targets, prdFinfo );
	for ( vector< Id >::iterator i = targets.begin(); i != targets.end(); ++i ) {
		string s = "addmsg " + trimPath( i->path() ) + " " + reacPath +
			   	" PRODUCT n";
		msgs.push_back( s );
		s = "addmsg " + reacPath + " " + trimPath( i->path() ) + 
				" REAC B A";
		msgs.push_back( s );
	}
}

void storeMMenzMsgs( Id enz, vector< string >& msgs )
{
	static const Finfo* subFinfo = 
			EnzBase::initCinfo()->findFinfo( "toSub" );
	static const Finfo* prdFinfo = 
			EnzBase::initCinfo()->findFinfo( "toPrd" );
	static const Finfo* enzFinfo = 
			EnzBase::initCinfo()->findFinfo( "enzDest" );
	vector< Id > targets;
	
	string enzPath = trimPath( enz.path() );
	enz.element()->getNeighbours( targets, subFinfo );
	for ( vector< Id >::iterator i = targets.begin(); i != targets.end(); ++i ) {
		string tgtPath = trimPath( i->path() );
		string s = "addmsg " + tgtPath + " " + enzPath + " SUBSTRATE n";
		msgs.push_back( s );
		s = "addmsg " + enzPath + " " + tgtPath + " REAC sA B";
		msgs.push_back( s );
	}

	targets.resize( 0 );
	enz.element()->getNeighbours( targets, prdFinfo );
	for ( vector< Id >::iterator i = targets.begin(); i != targets.end(); ++i ) {
		string tgtPath = trimPath( i->path() );
		string s = "addmsg " + enzPath + " " + tgtPath + " MM_PRD pA";
		msgs.push_back( s );
	}

	targets.resize( 0 );
	enz.element()->getNeighbours( targets, enzFinfo );
	for ( vector< Id >::iterator i = targets.begin(); i != targets.end(); ++i ) {
		string tgtPath = trimPath( i->path() );
		string s = "addmsg " + tgtPath + " " + enzPath + " ENZYME n";
		msgs.push_back( s );
	}
}

void storeCplxEnzMsgs( Id enz, vector< string >& msgs )
{
	static const Finfo* subFinfo = 
			EnzBase::initCinfo()->findFinfo( "toSub" );
	static const Finfo* prdFinfo = 
			EnzBase::initCinfo()->findFinfo( "toPrd" );
	static const Finfo* enzFinfo = 
			EnzBase::initCinfo()->findFinfo( "toEnz" );
	// In GENESIS we don't need to explicitly connect up the enz cplx, so
	// no need to deal with the toCplx msg.
	vector< Id > targets;
	
	string enzPath = trimPath( enz.path() );
	enz.element()->getNeighbours( targets, subFinfo );
	for ( vector< Id >::iterator i = targets.begin(); i != targets.end(); ++i ) {
		string tgtPath = trimPath( i->path() );
		string s = "addmsg " + tgtPath + " " + enzPath + " SUBSTRATE n";
		msgs.push_back( s );
		s = "addmsg " + enzPath + " " + tgtPath + " REAC sA B";
		msgs.push_back( s );
	}

	targets.resize( 0 );
	enz.element()->getNeighbours( targets, prdFinfo );
	for ( vector< Id >::iterator i = targets.begin(); i != targets.end(); ++i ) {
		string tgtPath = trimPath( i->path() );
		string s = "addmsg " + enzPath + " " + tgtPath + " MM_PRD pA";
		msgs.push_back( s );
	}

	targets.resize( 0 );
	enz.element()->getNeighbours( targets, enzFinfo );
	for ( vector< Id >::iterator i = targets.begin(); i != targets.end(); ++i ) {
		string tgtPath = trimPath( i->path() );
		string s = "addmsg " + tgtPath + " " + enzPath + " ENZYME n";
		msgs.push_back( s );
		s = "addmsg " + enzPath + " " + tgtPath + " REAC eA B";
		msgs.push_back( s );
	}
}

void storeEnzMsgs( Id enz, vector< string >& msgs )
{
	if ( enz.element()->cinfo()->isA( "CplxEnzBase" ) ) 
		storeCplxEnzMsgs( enz, msgs );
	else
		storeMMenzMsgs( enz, msgs );
}

void writeMsgs( ofstream& fout, const vector< string >& msgs )
{
	for ( vector< string >::const_iterator i = msgs.begin();
					i != msgs.end(); ++i )
			fout << *i << endl;
}

void storeFuncPoolMsgs( Id enz, vector< string >& msgs )
{
	;
}

void storeTableMsgs( Id enz, vector< string >& msgs )
{
	;
}

void writeKkit( Id model, const string& fname )
{
		vector< Id > ids;
		vector< string > msgs;
		unsigned int num = simpleWildcardFind( model.path() + "/##", ids );
		if ( num == 0 ) {
			cout << "Warning: writeKkit:: No model found on " << model << 
					endl;
			return;
		}
		ofstream fout( fname.c_str(), ios::out );
		writeHeader( fout, 1, 1, 1, 1 );
		writeGui( fout );

		string bg = "cyan";
		string fg = "black";
		double x = 0;
		double y = 0;
		double side = floor( 1.0 + sqrt( static_cast< double >( num ) ) );
		double dx = side / num;
		for( vector< Id >::iterator i = ids.begin(); i != ids.end(); ++i ) {
			getInfoFields( *i, fg, bg, x, y , side, dx );
			if ( i->element()->cinfo()->isA( "PoolBase" ) ) {
				ObjId pa = Neutral::parent( i->eref() );
				// Check that it isn't an enz cplx.
				if ( !pa.element()->cinfo()->isA( "CplxEnzBase" ) ) {
					writePool( fout, *i, fg, bg, x, y );
				}
				if ( i->element()->cinfo()->isA( "FuncPool" ) ) {
					storeFuncPoolMsgs( *i, msgs );
				}
			} else if ( i->element()->cinfo()->isA( "ReacBase" ) ) {
				writeReac( fout, *i, fg, bg, x, y );
				storeReacMsgs( *i, msgs );
			} else if ( i->element()->cinfo()->isA( "EnzBase" ) ) {
				writeEnz( fout, *i, fg, bg, x, y );
				storeEnzMsgs( *i, msgs );
			} else if ( i->element()->cinfo()->name() == "Neutral" ) {
				writeGroup( fout, *i, fg, bg, x, y );
			} else if ( i->element()->cinfo()->isA( "TableBase" ) ) {
				writeTable( fout, *i, fg, bg, x, y );
				storeTableMsgs( *i, msgs );
			}
		}
		writeMsgs( fout, msgs );
		writeFooter( fout );
}