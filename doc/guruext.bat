@echo off
:- generate PDF for gurudoc file

set FILE=%1
set FILE=%FILE:.txt=%

call mkgdl %FILE%
call gsl3 -quiet -tpl:latex_simple -gdl:%FILE% gurudoc

:- twice, so TOC is correctly built
pdflatex %FILE%.tex
pdflatex %FILE%.tex

:- clean-up intermediary files
del %FILE%.aux %FILE%.gdl %FILE%.log %FILE%.tex %FILE%.toc

