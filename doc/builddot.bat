@echo off
:-
:-  Build PNG from dot file

set FILE=%1
set FILE=%FILE:.dot=%
dot -Tpng %FILE%.dot -o%FILE%.png
