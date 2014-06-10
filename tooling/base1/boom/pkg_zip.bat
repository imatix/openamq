@echo off
:-
:-  pkg_zip.bat - Boom ZIP packager
:-
:-  Copyright (c) 1996-2009 iMatix Corporation
:-
:-  This program is free software; you can redistribute it and/or modify
:-  it under the terms of the GNU General Public License as published by
:-  the Free Software Foundation; either version 2 of the License, or (at
:-  your option) any later version.
:-
:-  This program is distributed in the hope that it will be useful, but
:-  WITHOUT ANY WARRANTY; without even the implied warranty of
:-  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
:-  General Public License for more details.
:-
:-  For information on alternative licensing for OEMs, please contact
:-  iMatix Corporation.
:-

:-  %1 is the name of the archive to create
if .%1==. goto exit
if exist %1.zip del %1.zip
zip -rq %1 %1
:exit
