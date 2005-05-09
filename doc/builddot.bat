@echo off
:-
:-  Build PNG from dot file

dot -Tpng %1.dot -o%1.png
