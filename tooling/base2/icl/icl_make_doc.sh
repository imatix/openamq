#!/bin/sh
#
#   icl_make_doc.sh
#
#   Copyright (c) 1996-2009 iMatix Corporation
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or (at
#   your option) any later version.
# 
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
# 
#   For information on alternative licensing for OEMs, please contact
#   iMatix Corporation.
#

#   Prototype script used for generating reference documentation from a project's
#   iCL classes.
#
#   Usage: run in any project directory containing iCL classes, where index.txt
#   is present
#
#   See help for options.
#

# generate PDF for gurudoc file
pdf_render() {
    export PERLLIB=$IBASE/bin

    NAME=$1
    FILE=$2

    mkgdl $FILE.txt
    gsl3 -quiet -tpl:latex_simple -gdl:$FILE gurudoc
    
    # twice, so TOC is correctly built
    pdflatex --interaction batchmode $FILE.tex
    pdflatex --interaction batchmode $FILE.tex
    
    if [ "$NAME" != "$FILE" ]; then
        mv $FILE.pdf $NAME.pdf
    fi
}

# generate html for gurudoc file
html_render() {
    export PERLLIB=$IBASE/bin

    NAME=$1
    FILE=$2

    mkgdl $FILE.txt
    gsl3 -quiet -tpl:html_frameset -gdl:$FILE gurudoc

    if [ "$NAME" != "$FILE" ]; then
        mv $FILE.html $NAME.html
    fi
}

# mark classes to ignore from icl_make_doc.ignore, if present
mark_ignores() {
    lines=$1

    while [ $lines -gt 0 ]; do
        for class in $(head -n $lines icl_make_doc.ignore | tail -n 1); do
            echo > ${class%.*}.ignore
        done

        lines=$(expr $lines - 1)
    done 
}

# Clean up temporary files
cleanup() {
    rm -f index.aux index.gdl index.log index.tex index.toc 
    rm -f $docnamebind.aux $docnamebind.gdl $docnamebind.log $docnamebind.tex $docnamebind.toc $docnamebind.txt
    for class in *.icl; do
        if [ "$class" != "*.icl" ]; then
            idoc="${class%.*}.txt"
            if [ -f $idoc -a ! -f ${class%.*}.ignore ]; then
                rm $idoc
            fi
            rm -f ${class%.*}.ignore
        fi
    done
}

# Main script body

if [ "$#" = "0" -o "$1" = "-h" -o "$1" = "--help" ]; then
    echo "'icl_make_doc' generate API documentation from iCL classes."
    echo
    echo "Usage: icl_make_doc [-v] [-k] TARGET [REFERENCE TITLE]"
    echo 
    echo "Targets:"
    echo "  html              Generate HTML documentation"
    echo "  pdf               Generate PDF documentation"
    echo "  html-standalone   Generate HTML class reference only"
    echo "  pdf-standalone    Generate PDF class reference only"
    echo "  clean             Remove intermediate files"
    echo
    echo "Options:"
    echo "  -v                Be verbose"
    echo "  -k                Keep intermediate files"
    echo
    echo "Reference Title: Change the default documentation title"
    echo
    exit 0
fi
if [ "$1" = "-v" ]; then
    opt_verbose=1
    shift
fi
if [ "$1" = "-k" ]; then
    opt_keep=1
    shift
fi
opt_target="$1"
shift
case "$opt_target" in
    pdf)
        target=pdf
        ;;
    html)
        target=html
        ;;
    pdf-standalone)
        target=pdf
        standalone=yes
        ;;
    html-standalone)
        target=html
        standalone=yes
        ;;
    clean)
        target=clean
        ;;
    *)
        echo "icl_make_doc W: unsupported target '$opt_target'"
        echo "icl_make_doc I: use -h or --help to see available targets"
        exit 1
        ;;
esac

if [ "$1" = "" ]; then
    docname="$(basename $(pwd))"
    docname="$(echo "$docname" | tr [a-z] [A-Z])"
else
    docname="$1"
fi

docnamelwr="$(echo "$docname" | tr [A-Z] [a-z])"
docnamebind="$(echo "$docnamelwr")_api"

# start with clean directory
cleanup
if [ "$target" = "clean" ]; then
    exit 0
fi

echo "icl_make_doc I: Generating class reference ($docnamebind.txt)"

if [ -f icl_make_doc.ignore ]; then
    mark_ignores $(wc -l icl_make_doc.ignore)
fi

for class in *.icl; do
    if [ "$class" != "*.icl" ]; then
        if [ ! -f ${class%.*}.ignore ]; then
            if [ -n "$opt_verbose" ]; then
                gsl -q -abstract:0 -target:doc $class
            else
                gsl -q -abstract:0 -quiet:1 -target:doc $class
            fi
            if [ $? -ne 0 ]; then
                echo "icl_make_doc E: Error processing class $class, aborting"
                exit 1
            fi
        else
            if [ -n "$opt_verbose" ]; then
                echo "icl_make_doc I: Ignoring marked class: ${class%.*}.ignore"
            fi
        fi
    fi
done

if [ -n "$opt_verbose" ]; then
    echo "icl_make_doc I: Generating $docnamebind.txt"
fi

if [ -f index.txt -a "$standalone" = "" ]; then
    docindex="index"
else
    docindex="$docnamebind"
    if [ "$standalone" = "" ]; then
        echo "icl_make_doc W: Index file for project documentation not found (index.txt), the generated document will only contain the class reference"
        standalone="yes"
    fi
fi

if [ "$standalone" != "" ]; then
    cat <<EOM >$docnamebind.txt
gdl
    title     = $docname
    subtitle  = API Documentation
    product   = $docname
    author    = iMatix Corporation <technical@imatix.com>
    date      = $(date --rfc-3339=date)
    copyright = Copyright (c) 1996-2009 iMatix Corporation
    version   = 1.0
end gdl

$docname API Documentation
**************************

EOM
else
    echo > $docnamebind.txt
fi

for class in *.icl; do
    if [ "$class" != "*.icl" ]; then
        idoc="${class%.*}.txt"
        if [ -f $idoc -a ! -f ${class%.*}.ignore ]; then
            echo "[See $idoc]" >> $docnamebind.txt
        fi
    fi
done

if [ "$target" = "pdf" ]; then
    echo "icl_make_doc I: Generating PDF documentation ($docnamelwr.pdf)"

    pdf_render $docnamelwr $docindex 

    if [ ! -f $docnamelwr.pdf ]; then
        echo "icl_make_doc E: PDF generation has failed, please check that you have a complete latex installation"
        echo "icl_make_doc I: Packages required on Debian for pdf generation: tetex-base, tetex-bin, tetex-extra, libkpathsea"
    fi
elif [ "$target" = "html" ]; then
    echo "icl_make_doc I: Generating HTML documentation (index.html)"

    html_render "index" $docindex
fi

# cleanup temporary files
if [ -z "$opt_keep" ]; then
    cleanup
fi

