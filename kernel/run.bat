@echo off
set trace=0
:next
if %1.==. goto end

if %1==t1 set trace=1
if %1==t2 set trace=2
if %1==t3 set trace=3

if %1==s1 start "Master" amq_server -c cluster-001 --port 5001 --trace %trace%
if %1==s2 start "Backup" amq_server -c cluster-002 --port 5002 --trace %trace%

if %1==r1 start "Reader one" reader -s localhost:5001 -Q q.shared 
if %1==r2 start "Reader two" reader -s localhost:5002 -Q q.shared

if %1==p1 start "Private one" reader -s localhost:5001 -Q q.private -P 1
if %1==p2 start "Private two" reader -s localhost:5002 -Q q.private -P 1

if %1==w1 start "Writer one" writer -s localhost:5001 -M 5
if %1==w2 start "Writer two" writer -s localhost:5002 -M 5
shift
goto next
:end
