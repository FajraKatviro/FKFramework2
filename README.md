FKFramework2
============

Cross-platform Qt-based object management tool, mostly targeted as a game engine.


=========Unique features=========

*Built-in object replication
All objects, created by server infrastructure and needed by another (client or server) infrastructure, automatically replicates to that infrastructure.
Infrastructures scalable. Many users can use 1 client (i.e. hot-sit), many service servers can exist in 1 session (i.e. multiserver rooms).
Infrastructures customizible. I.e. use client infrastructure as server itself to reduce memory cost for single player only games.

*Native QML integration
Objects can have any QML referent (using non-gui, quick (2d) or entety (3d) components). Referenced objects can be interacted both from C++ and QML.
3d objects powered by Qt3D engine.

=========Major features=========

*Room modules
Application level provides infrastructure setting
Game logic encapsulated in dynamic library plugin (room module)
Different room modules can be loaded by the same application and the same modules can be loaded by any compatitable application

*Easy property management
Objects have short-syntax properties with default setters/getters (powered by clang llvm)
Properties automatically replicates into editor      

*Single divided implementation
Game implementation provides both server and client logic in 1 room module, divided into client and server part if needed (servant concept)

*Built-in account management
Application requires accound record for clients and provides realm infrastructure for that purpose

=========Other features=========

*Room update
Room modules can be downloaded or updated from server in run-time

*Application tests
Extentable test system for infrastructure level

*Single api for orders
GUI orders and command line orders share the same api calls

*Tools integration
Editing tools can be created as a room module and use the same object classes as main room module
