@rem this fixes a problem with the Wix binary package - version 2.0.3719.0 - where the bitmaps folder is empty
move ..\build\wix\lib\*.bmp ..\build\wix\lib\Bitmaps
move ..\build\wix\lib\*.ico ..\build\wix\lib\Bitmaps