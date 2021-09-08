Demonstration of Multiverse and ATLAS Address Space Views
=========================================================

This Tutorial was held at [Netsys 2021](https://netsys2021.org/program/workshops_and_tutorials/#tut2)


- [tall/talk.pdf](talk/talk.pdf)
- [Virtual Machine Image](https://lab.sra.uni-hannover.de/Publications/2021/netsys-tutorial/)

**Abstract**: Many software projects allow us to dynamically configure their behavior at runtime. Thereby, the use and modification of global variables is a common pattern to save or reconfigure the selected configuration. The selected configuration is then enforced by dynamically checked conditions (if statements). However, if this global configuration remains stable over long periods of time, it is questionable why one should pay the cost of constant and repeated checking. Dynamic configurability must be possible at low cost.

One approach to this problem is semi-dynamic variability. The basic idea is to let the compiler generate specialized software variants statically and to introduce them into the running program as efficiently as possible at runtime. As a concrete way to implement semi-dynamic variability, this tutorial will introduce Multiverse: Multiverse is a GCC plugin and a run-time library, which allows to specialize single C-functions for certain assignments of global variables. Through run-time binary patching, the respective specializations are anchored in the code segment without loss of efficiency.

In order to perform this run-time code patching efficiently without disturbing the normal operation of the process, we will look at address space views. Here, multiple, synchronized, ‘views’ are established on the address space within a UNIX process. While one thread prepares the code segment for a new multiverse configuration, all other threads continue to run undisturbed.

**About the Speaker**: Christian Dietrich studied computer science at the FAU Erlangen-Nuremberg, before he put a focus on interaction-aware analysis of real-time systems at the Leibniz Universität Hannover, where he defended his PhD thesis in 2019. After being appointed as Juniorprofessor, he is currently establishing the Operating System Group at the Technical University Hamburg. His research interests are virtual-memory handling, dependable real-time systems, and variability and he is currently searching for a co-worker and PhD candidate.

