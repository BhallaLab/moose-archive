/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HINES_MATRIX_H
#define _HINES_MATRIX_H

struct JunctionStruct
{
	JunctionStruct( unsigned int i, unsigned int r ) :
		index( i ),
		rank( r )
	{ ; }

	bool operator< ( const JunctionStruct& other ) const {
		return ( index < other.index );
	}

	unsigned int index;
	unsigned int rank;
};

class HinesMatrix
{
public:
	void setup(
		const vector< vector< unsigned int > >& children,
		const vector< double >& Ga,
		const vector< double >& CmByDt );
	
	double getA( unsigned int row, unsigned int col );
	double getB( unsigned int row );
	double getVMid( unsigned int row );

protected:
	typedef vector< double >::iterator vdIterator;
	
	unsigned int              nCompt_;
	vector< JunctionStruct >  junction_;
	vector< double >          HS_;
	vector< double >          HJ_;
	vector< double >          HJCopy_;
	vector< double >          VMid_;
	vector< vdIterator >      operand_;
	vector< vdIterator >      backOperand_;
	int                       stage_;

private:
	void makeJunctions( );
	void makeMatrix( );
	void makeOperands( );
	void refresh( );

	vector< vector< unsigned int > > const  *children_;
	vector< double > const                  *Ga_;
	vector< double > const                  *CmByDt_;
	vector< vector< unsigned int > >         coupled_;
	map< unsigned int, vdIterator >          operandBase_;
	map< unsigned int, unsigned int >        groupNumber_;
};

#endif // _HINES_MATRIX_H
