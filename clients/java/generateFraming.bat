@echo off
@echo Cleaning generated directory
rmdir /s /q generated\java
@echo Running GSL to generate framing code
gsl -script:amq_java ..\..\kernel\amq_frames.xml