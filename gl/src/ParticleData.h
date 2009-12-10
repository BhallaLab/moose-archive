/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef PARTICLEDATA_H
#define PARTICLEDATA_H

// An instance of the following struct ParticleData represents a
// collection of particles which are all of the same type; therefore
// the individual particles are all represented by the same color,
// size and shape. So a simulation will contain as many such instances
// as there are types of particle, and each instance will store only
// the 3D co-ordinates of each particular particle of that type. This
// information will be obtained from Smoldyn via a MOOSE interface.

struct ParticleData
{
	// note that the following colors are specified over [0,1],
	// and are not w.r.t any particular colormap file
	double colorRed;
	double colorBlue;
	double colorGreen;

	// if diameter == 0, particles herein will be represented
	// as points, otherwise, as spheres
	double diameter;

	// the length of coordinates will be a multiple of 3;
	// each consecutive 3-tuple will represent x, y, z for a single
	// particle to be drawn as points or spheres
	std::vector< double > coords;

	template< typename Archive >
	void serialize( Archive& ar, const unsigned int version)
	{
		ar & colorRed;
		ar & colorBlue;
		ar & colorGreen;
		ar & diameter;
		ar & coords;
	}
};

#endif // PARTICLEDATA_H
