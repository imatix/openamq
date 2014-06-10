OPF - The Object Property Framework

OPF is a simple framework for generating classes that hold serialised
object properties, which usually means configuration data saved in XML
files. This tool is not currently written using XNF, but is a simple GSL
script.

An OPF file defines a set of configuration properties - see example.opf.

From the OPF file, we generate:

 - a class that manages a set of configuration properties, providing an
   API that lets you get and set values.

 - documentation.

You can also add plugins to generate custom outputs:

    <plugin name = "random_gsl_script" />

Which may look something like this (random_gsl_script.gsl):

    .output "some_generated_output"
    .for property by name
    .   if type = "Bool"
    .   elsif metat = "shorts" | metat = "string" 
    .   else
    .   endif
    .endfor

The usage of the generated classes will be documented at some point.

OPF is Copyright (c) 1996-2009 iMatix Corporation

OPF forms part of the iMatix "Base/2" technology toolkit.  The OPF
source code is provided under the GNU General Public License (GPL) for
free software developers, and the iMatix General Terms of Business for
commercial developers.

Source files output from the OPF code generation process are subject to
the BSD license, unless otherwise stated.  For the avoidance of doubt,
please see the individual source files for copying conditions.

OPF is maintained by iMatix Corporation.  For more information please
see http://www.imatix.com/.
