# Astrophysical Simulation

## Background

This astrophysical simulation is an N-Body problem that models the continuous interaction of each body(could be a star, planet, or even a galaxy) with every other body. In this case, the interactions are caused by the gravitational force that these bodies exert on each other. This is a dynamic simulation in which the forces will cause the bodies to move.

### Barnes-Hut Algorithm

Instead of naively computing all the n*n interactions that exist corresponding to all pairs of bodies, the Barnes-Hut method takes O(nlog(n)) time to compute direct interactions for pairs of bodies that are sufficiently close; for the effects of far away bodies, the algorithm approximates a group of bodies by a single body whose center of mass is the same as the center of mass of the group of bodies.

The algorithm consists of two phases. The first phase constructs a tree that represents a spatial partitioning of the bodies in the system. Each interior node in the tree represents the center of mass of the bodies in the subtree rooted at that node. 

The second phase computes the force interactions for each body in the system by using the MAC (multipole acceptance criteria), which determines whether the bodies in a subtree are sufficiently far away that their net force on a given body can be approximated by the center of mass of the bodies in a subtree. The velocity and position of the body is then calculated via a Leapfrog-Verlet integrator.

### MAC (Multipole Acceptance Criteria)

Whether a body is far away from a subtree depends on the distance *d* between the body and the center of mass of the subtree, the length *l* of the side of the subtree, and a threshold *theta*.

### Force

The gravitational force between two bodies (or one body and a subtree) is given by

*F = G \* M0 \* M1 \* d / d ^ 3*

where *M0* and *M1* are the mass of the two bodies (or one body and a subtree) and *d* is the distance between the two bodies (or one body and the center of mass of a subtree). To avoid large force values, we use a *rlimit* value such that if the distance between two bodies is shorter than *rlimit*, then *rlimit* is used as the distance between the two bodies. The projected force on each axis can be computed as

*Fx = G \* M0 \* M1 \* dx / d ^ 3*  
*Fy = G \* M0 \* M1 \* dy / d ^ 3*

where *dx* and *dy* is the distance between the two bodies (or one body and the center of mass of a subtree) on the x and y axis respectively.

### Leapfrog-Verlet Integration

When you have the total force on a body you can compute the new position (*Px'*, *Py'*) and velocity (*Vx'*, *Vy'*) of the body given its current position (*Px*, *Py*) and velocity (*Vx*, *Vy*) as follows

*ax = Fx / M0*  
*ay = Fy / M0*  
*Px' = Px + Vx \* dt + 0.5 \* ax \* dt ^ 2*  
*Py' = Py + Vy \* dt + 0.5 \* ay \* dt ^ 2*  
*Vx' = Vx + ax \* dt*   
*Vy' = Vy + ay \* dt*

where *dt* is the timestep.

### Parameters

The following are the values of the various parameters used in this project:

- *G*: 0.0001
- *rlimit*: 0.03
- *dt*: 0.005

When the position of a particle is out of the boundary, the particle is considered to be lost. For lost particles, the particle's mass is set to -1 and the particle is not included in the force calculation any more. The domain is set to the square (0,0) -- (4,4).

## Implementation

This project is implemented in C++ using MPI(Message Passing Interface). A node is designated as the master node. This master node reads from a file and initially scatters the particle data to other nodes as well as itself. In each iteration, this master node creates the Barnes-Hut tree from the particle data and broadcasts it to all other nodes by serializing it. All nodes, including master, calculate forces on all particles assigned to them and update the particle data(positions, velocities and setting mass to -1 if particle moves out of the boundary). Finally, the master gathers the particle data from all nodes(including itself) and the process starts all over again. At the end, the master writes the data to a file.

## Requirements

A C++ compiler that supports the C++17 standard is required. MPICH also needs to be installed.

## Usage

Clone this repository and `cd` into it. Run `make` to compile this project. This will result in an executable named `nbody` within the same directory. This has to be run with the following command line parameters:

- `-i inputfilename` (char*): input file name
- `-o outputfilename` (char*): output file name
- `-s steps` (int): number of iterations
- `-t theta` (double): threshold for MAC
- `-d dt` (double): timestep

It is assumed that the input file has the following format:

- The number of bodies (n)
- A list of n tuples:
    - index
    - x position
    - y position
    - mass
    - x velocity
    - y velocity

The output file will also have the same format. The index of each particle uniquely identifies it and is considered as its name. The program also prints out the elapsed time for the simulation to `stdout`.