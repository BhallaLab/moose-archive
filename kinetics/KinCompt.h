/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/


#ifndef _KinCompt_h
#define _KinCompt_h

/**
 * The KinCompt is a compartment for kinetic calculations. It doesn't
 * really correspond to a single Smoldyn concept, but it encapsulates
 * many of them into the traditional compartmental view. It connects up
 * with one or more surfaces which collectively define its volume and
 * geometry.
 */
class KinCompt
{
	public:
		KinCompt();
		
		///////////////////////////////////////////////////
		// Field assignment functions
		///////////////////////////////////////////////////
		static double getVolume( const Element* e );
		static void setVolume( const Conn* c, double value );
		void innerSetVolume( double value );
		static double getArea( const Element* e );
		static void setArea( const Conn* c, double value );
		void innerSetArea( double value );
		static double getPerimeter( const Element* e );
		static void setPerimeter( const Conn* c, double value );
		void innerSetPerimeter( double value );
		static double getSize( const Element* e );
		static void setSize( const Conn* c, double value );
		void innerSetSize( double value );
		static unsigned int getNumDimensions( const Element* e );
		static void setNumDimensions( const Conn* c, unsigned int value );

		///////////////////////////////////////////////////
		// Message handlers
		///////////////////////////////////////////////////
		static void requestExtent( const Conn* c );
		void innerRequestExtent( const Element* e ) const;

		static void exteriorFunction( const Conn* c, 
			double v1, double v2, double v3 );
		void localExteriorFunction( double v1, double v2, double v3 );

		static void interiorFunction( const Conn* c, 
			double v1, double v2, double v3 );
		void localInteriorFunction( double v1, double v2, double v3 );

	private:

		/**
		 * Size is the variable-unit extent of the compartment. Its
		 * dimensions depend on the numDimensions of the compartment.
		 * So, when numDimensions == 3, size == volume.
		 * It is exactly equivalent to the size field in SBML
		 * compartments.
		 */
		double size_;

		/**
		 * Volume is computed by summing contributions from all surfaces.
		 * It first gets the exterior message from the outside surface.
		 * This assigns the volume, eliminating earlier values.
		 * Then the interior messages subtract from it.
		 */
		double volume_; 

		/**
		 * Surface area of compartment. Exterior message assigns it,
		 * then interior messages _add_ to it if numDimensions == 3,
		 * but _subtract_ from it if numDimensions == 2. Think about it.
		 */
		double area_; 
		
		/**
		 * Perimeter of compartment. Relevant for surfaces only.
		 */
		double perimeter_; 

		/**
		 * Number of dimensions represented by the compartment.
		 * 3 for normal compartments, 2 for membrane surfaces.
		 */
		unsigned int numDimensions_;
};

// Used by the Smoldyn solver
extern const Cinfo* initKinComptCinfo();

#endif // _KinCompt_h
