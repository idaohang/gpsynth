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
 *  \file   beagle/GA/src/PackageBitString.cpp
 *  \brief  Source code of class GA::PackageBitString.
 *  \author Christian Gagne
 *  $Revision: 1.3 $
 *  $Date: 2007/09/17 17:25:49 $
 */

#include "beagle/GA.hpp"

using namespace Beagle;


/*!
 *  \brief Construct package with bit string GA objects.
 *  \param inInitSize Size of vectors generated by initialization.
 */
GA::PackageBitString::PackageBitString(unsigned int inInitSize) :
		Package("GA-PackageBitString"),
		mInitSize(inInitSize)
{ }


/*!
 *  \brief Configure system by adding necessary components into it.
 *  \param ioSystem System to configure.
 */
void GA::PackageBitString::configure(System& ioSystem)
{
	Beagle_StackTraceBeginM();
	Factory& lFactory = ioSystem.getFactory();

	// Add available basic types to the factory
	lFactory.insertAllocator("Beagle::GA::BitString", new GA::BitString::Alloc);
	lFactory.insertAllocator("Beagle::GA::CrossoverOnePointBitStrOp", new GA::CrossoverOnePointBitStrOp::Alloc);
	lFactory.insertAllocator("Beagle::GA::CrossoverTwoPointsBitStrOp", new GA::CrossoverTwoPointsBitStrOp::Alloc);
	lFactory.insertAllocator("Beagle::GA::CrossoverUniformBitStrOp", new GA::CrossoverUniformBitStrOp::Alloc);
	lFactory.insertAllocator("Beagle::GA::InitBitStrOp", new GA::InitBitStrOp::Alloc);
	lFactory.insertAllocator("Beagle::GA::InitBitStrRampedOp", new GA::InitBitStrRampedOp::Alloc);
	lFactory.insertAllocator("Beagle::GA::MutationFlipBitStrOp", new GA::MutationFlipBitStrOp::Alloc);
	lFactory.insertAllocator("Beagle::GA::MutationMoveSequenceOp", new GA::MutationMoveSequenceOp::Alloc);
	lFactory.insertAllocator("Beagle::GA::MutationReverseSequenceOp", new GA::MutationReverseSequenceOp::Alloc);
	lFactory.insertAllocator("Beagle::GA::MutationShuffleSequenceOp", new GA::MutationShuffleSequenceOp::Alloc);
	lFactory.insertAllocator("Beagle::GA::PackageBitString", new GA::PackageBitString::Alloc);

	// Aliases to short name of operators
	lFactory.aliasAllocator("Beagle::GA::BitString", "GA-BitString");
	lFactory.aliasAllocator("Beagle::GA::CrossoverOnePointBitStrOp", "GA-CrossoverOnePointBitStrOp");
	lFactory.aliasAllocator("Beagle::GA::CrossoverTwoPointsBitStrOp", "GA-CrossoverTwoPointsBitStrOp");
	lFactory.aliasAllocator("Beagle::GA::CrossoverUniformBitStrOp", "GA-CrossoverUniformBitStrOp");
	lFactory.aliasAllocator("Beagle::GA::InitBitStrOp", "GA-InitBitStrOp");
	lFactory.aliasAllocator("Beagle::GA::InitBitStrRampedOp", "GA-InitBitStrRampedOp");
	lFactory.aliasAllocator("Beagle::GA::MutationFlipBitStrOp", "GA-MutationFlipBitStrOp");
	lFactory.aliasAllocator("Beagle::GA::MutationMoveSequenceOp", "GA-MutationMoveSequenceOp");
	lFactory.aliasAllocator("Beagle::GA::MutationReverseSequenceOp", "GA-MutationReverseSequenceOp");
	lFactory.aliasAllocator("Beagle::GA::MutationShuffleSequenceOp", "GA-MutationShuffleSequenceOp");
	lFactory.aliasAllocator("Beagle::GA::PackageBitString", "GA-PackageBitString");

	// Set some basic concept-type associations in factory
	lFactory.setConcept("CrossoverOp", "GA-CrossoverUniformBitStrOp");
	lFactory.setConcept("Genotype", "GA-BitString");
	lFactory.setConcept("InitializationOp", "GA-InitBitStrOp");
	lFactory.setConcept("MutationOp", "GA-MutationFlipBitStrOp");

	// Set default initialization size
	ioSystem.getRegister().setEntryDefaultValue("ga.init.numberbits",
	        new UInt(mInitSize),
	        uint2str(mInitSize));

	Beagle_StackTraceEndM("void GA::PackageBitString::configureSystem(System&)");
}


/*!
 *  \brief List packages which current package depends.
 *  \return Bag with the packages instanciation on which current package depends.
 *
 *  PackageBitString depends on PackageBase.
 */
Package::Bag GA::PackageBitString::listDependencies(void)
{
	Beagle_StackTraceBeginM();
	Package::Bag lDependencies;
	lDependencies.push_back(new PackageBase());
	return lDependencies;
	Beagle_StackTraceEndM("Package::Bag GA::PackageBitString::listDependencies(void)");
}
