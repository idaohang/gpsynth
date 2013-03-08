/*
 *  Open BEAGLE
 *  Copyright (C) 2001-2007 by Christian Gagne and Marc Parizeau
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact:
 *  Laboratoire de Vision et Systemes Numeriques
 *  Departement de genie electrique et de genie informatique
 *  Universite Laval, Quebec, Canada, G1K 7P4
 *  http://vision.gel.ulaval.ca
 *
 */

/*!
 *  \file   Beagle/SAES/Package.cpp
 *  \brief  Source code of class SAES::Package.
 *  \author Christian Gagne
 *  $Revision: 1.4 $
 *  $Date: 2007/09/17 17:25:49 $
 */

#include "Beagle/SAES.hpp"

using namespace Beagle;


/*!
 *  \brief Construct package with SA-ES objects.
 *  \param inInitSize Size of vectors generated by initialization.
 */
SAES::Package::Package(unsigned int inInitSize) :
		PackageAbstract("SAES-Package"),
		mInitSize(inInitSize)
{ }


/*!
 *  \brief Configure system by adding necessary components into it.
 *  \param ioSystem System to configure.
 */
void SAES::Package::configure(System& ioSystem)
{
	Beagle_StackTraceBeginM();
	Factory& lFactory = ioSystem.getFactory();

	// Add available basic types to the factory
	lFactory.insertAllocator("Beagle::SAES::Algorithm", new SAES::Algorithm::Alloc);
	lFactory.insertAllocator("Beagle::SAES::InitializationOp", new SAES::InitializationOp::Alloc);
	lFactory.insertAllocator("Beagle::SAES::InitializationQROp", new SAES::InitializationQROp::Alloc);
	lFactory.insertAllocator("Beagle::SAES::MutationOp", new SAES::MutationOp::Alloc);
	lFactory.insertAllocator("Beagle::SAES::MutationQROp", new SAES::MutationQROp::Alloc);
	lFactory.insertAllocator("Beagle::SAES::PairVector", new SAES::PairVector::Alloc);
	lFactory.insertAllocator("Beagle::SAES::RecombinationOp", new SAES::RecombinationOp::Alloc);
	lFactory.insertAllocator("Beagle::SAES::RecombinationWeightedOp", new SAES::RecombinationWeightedOp::Alloc);

	// Aliases to short name of operators
	lFactory.aliasAllocator("Beagle::SAES::Algorithm", "SAES-Algorithm");
	lFactory.aliasAllocator("Beagle::SAES::InitializationOp", "SAES-InitializationOp");
	lFactory.aliasAllocator("Beagle::SAES::InitializationQROp", "SAES-InitializationQROp");
	lFactory.aliasAllocator("Beagle::SAES::MutationOp", "SAES-MutationOp");
	lFactory.aliasAllocator("Beagle::SAES::MutationQROp", "SAES-MutationQROp");
	lFactory.aliasAllocator("Beagle::SAES::PairVector", "SAES-PairVector");
	lFactory.aliasAllocator("Beagle::SAES::RecombinationOp", "SAES-RecombinationOp");
	lFactory.aliasAllocator("Beagle::SAES::RecombinationWeightedOp", "SAES-RecombinationWeightedOp");

	// Set some basic concept-type associations in factory
	lFactory.setConcept("Algorithm", "SAES-Algorithm");
	lFactory.setConcept("Genotype", "SAES-PairVector");
	lFactory.setConcept("InitializationOp", "SAES-InitializationOp");
	lFactory.setConcept("MutationOp", "SAES-MutationOp");
	lFactory.setConcept("SelectionOp", "EC-SelectRandomOp");

	// Add quasi-random component
	if(ioSystem.haveComponent("QuasiRandom") == NULL) {
		ioSystem.addComponent(new QuasiRandom);
	}

	// Set default initialization size
	ioSystem.getRegister().setEntryDefaultValue("saes.init.vectorsize",
	        new UInt(mInitSize),
	        uint2str(mInitSize));

	Beagle_StackTraceEndM();
}


/*!
 *  \brief List packages which current package depends.
 *  \return Bag with the packages instanciation on which current package depends.
 *
 *  Package depends on EC::Package.
 */
PackageAbstract::Bag SAES::Package::listDependencies(void)
{
	Beagle_StackTraceBeginM();
	PackageAbstract::Bag lDependencies;
	lDependencies.push_back(new EC::Package());
	return lDependencies;
	Beagle_StackTraceEndM();
}
