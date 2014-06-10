Latex template for Gurudoc
--------------------------

Packages required on Debian:

- tetex-base
- tetex-bin
- tetex-extra
- and libkpathsea3

Pdf creation steps:

- make the gdls (mkgdl)
- call gurudoc with the latex template: gsl3 -tpl:latex_simple -gdl:<name> gurudoc
- (optional but recommended) Install report.cls locally or globally
  globally at: /usr/share/texmf/tex/latex/base/ (overwrite)
- call pdflatex 2 times: pdflatex --interaction batchmode <name>.tex (the first 
  time the TOC is generated)

The use of '--interaction batchmode' will suppress the trace output from pdflatex, 
this should not be used when creating or debugging a document.
