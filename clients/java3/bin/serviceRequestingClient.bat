setlocal
call setupClasspath.bat
%JAVA_HOME%\bin\java -Damqj.logging.level=INFO -cp %CP% org.openamq.requestreply1.ServiceRequestingClient localhost 7654 guest guest /test serviceQ1 2000
