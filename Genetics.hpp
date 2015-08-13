#ifndef TOOLBOX_GENETICS_HPP
#define TOOLBOX_GENETICS_HPP

/*
 * Toolbox/Genetics.hpp
 *
 * A simple placeholder file for easier inclusion into projects.
 */

/*****************************************************************************
 * Genetics Overview:
 *
 * - Once genetic traits are determined, an "Adam" and "Eve" genome are
 *   created.  The genomes can be used to produce a Gamete (sperm/egg/etc.).
 *   Those gametes can then be combined to create an Embryo. Once all gametes
 *   have been combined, the embryo can gestate into an Organism.  The
 *   can then produce new gametes to begin the process again.
 *
 * - The genomes mimic mamalian biology (primarily) but contain support to
 *   have any number of parents.  A single gamete can generate an embryo to
 *   effectively "clone" an organism, or any number of gametes can fertilize
 *   the embryo to allow for even greater diversity.
 *
 * - The ability to add sex-linked genes is possible.  (Male pattern baldness,
 *   color blindness, etc., can all be expressed.)
 *
 * - At the organism stage there are helper functions to retrieve the
 *   phenotypes expressed, since the genome typically contains more
 *   information than is actually expressed in the organism.
 *
 * - An example program is provided in <Toolbox/Genetics/example>.
 *
 ****************************************************************************/

/****************************************************************************/
/****************************************************************************/

#include <Toolbox/Genetics/Organism.hpp>
#include <Toolbox/Genetics/Trainer.hpp>


#endif // TOOLBOX_GENETICS_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


