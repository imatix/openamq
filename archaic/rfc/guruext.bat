@echo off
:- generate PDF for gurudoc file

call mkgdl %1
call gsl3 -quiet -tpl:latex_simple -gdl:%1 gurudoc

:- twice, so TOC is correctly built
pdflatex %1.tex
pdflatex %1.tex

