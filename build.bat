@echo off

if not exist build\ mkdir build
pushd build

REM Uncomment one of these to run an example. Try it! They're great documentation.
REM cl /Zi ..\bastd\examples\memory.c
REM cl /Zi ..\bastd\examples\fib_slice.c 
REM cl /Zi ..\bastd\examples\string.c 

REM This builds your application with debug symbols.
REM cl /Zi ..\main.c

popd