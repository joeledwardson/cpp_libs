# C++ Libraries

## crslib
Pdcurses (windows)/NCurses (linux) library for creating an asyncronous input/output window in command prompt/terminal
MUST add -lncurses to compile options

## vislib
WINDOWS ONLY
visual library, implements chilli directx framework
also supports creating menus, input boxes etc..

## stdlib
general collection of formatting/error files

## netlib
core networking library, utilising UDP non-blocking sockets. Implementation initial class is BNP::IPComm/BNP::IPComm_HostType. Supports multiple connections and
orders packets receieved from contacts
FOR LINUX:
MUST add -lncurses to compile options

## gmelib
game library. Contains base types for a game, with a countdown class and a game networking adaptaion from netlib

## svrlib
Game server library: hosting a general game-type server - expands on gmelib

## cltlib
WINDOWS ONLY
visual client library for connecting to an unspecified game-type server (in coorporation with svrlib and gmelib)
