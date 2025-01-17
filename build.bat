@echo off

if not exist build\ mkdir build
pushd build

REM Uncomment one of these to run an example. Try it! They're great documentation.
REM cl /Zi ..\bastd\examples\memory.c

REM This builds your application with debug symbols.
cl /Zi ..\bastd\examples\memory.c

popd