@echo off

if not exist build\ mkdir build
pushd build

set debugflags=/nologo /W3 /MTd /Od /Zi /RTC1 /fsanitize=address /link /incremental:no shell32.lib

REM Uncomment one of these to run an example. Try it! They're great documentation.
REM cl ..\bastd\examples\memory.c %debugflags%
REM cl ..\bastd\examples\fib_slice.c %debugflags%
REM cl ..\bastd\examples\string.c %debugflags%
REM cl ..\bastd\examples\buffer.c %debugflags%
REM cl ..\bastd\examples\job_random.c %debugflags%

REM This builds your application with debug symbols.
cl ..\main.c %debugflags%

popd