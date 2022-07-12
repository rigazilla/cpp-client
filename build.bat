@echo on
if [%generator%] == [] call:usage generator
if [%JAVA_HOME%] == [] call:usage JAVA_HOME
if [%INFINISPAN_VERSION%] == [] call:usage INFINISPAN_VERSION
if [%SWIG_DIR%] == [] call:usage SWIG_DIR
if [%SWIG_EXECUTABLE%] == [] call:usage SWIG_EXECUTABLE
if [%MVN_PROGRAM%] == [] call:usage MVN_PROGRAM
if [%PROTOBUF_LIBRARY%] == [] call:usage PROTOBUF_LIBRARY
if [%PROTOBUF_INCLUDE_DIR%] == [] call:usage PROTOBUF_INCLUDE_DIR

set JBOSS_HOME=%cd%\infinispan-server-%INFINISPAN_VERSION%
set JBOSS_ZIP=infinispan-server-%INFINISPAN_VERSION%.zip
set JBOSS_URL=http://download.jboss.org/infinispan/%INFINISPAN_VERSION%/%JBOSS_ZIP%

if not exist %JBOSS_HOME% (
  if not exist %JBOSS_ZIP% (
     powershell.exe -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; (new-object System.Net.WebClient).DownloadFile('%JBOSS_URL%','%JBOSS_ZIP%')"
   )
"C:\Program Files\7-zip\7z.exe" x %JBOSS_ZIP% 
)
copy test\data\*.* %JBOSS_HOME%\server\conf /y
xcopy /E /I /Y %JBOSS_HOME%\server %JBOSS_HOME%\server1\

