@echo off
:-
:-  Build PNG from dot file

set FILE=%1
set FILE=%FILE:.dot=%

:-  Build PNG from dot file
dot -Tpng %FILE%.dot -o%FILE%.png

:- Build PDF from dot file
dot -Tps2 %FILE%.dot -o%FILE%.ps
epstopdf %FILE%.ps

