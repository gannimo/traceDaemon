traceDaemon
===========

The traceDaemon project injects a LSM module that interposes certain system
calls. The parameters of the system calls are tested for specific race
conditions against other software that runs in parallel.

The LSM module guarantees that system calls of all running threads and
applications are redirected to the traceDaemon in user-space. The traceDaemon
then builds a model of each application and checks for valid state transitions
for each file.
