Airline scheduling
====================
An approach to the airline scheduling problem using flow networks.

Why?
-------------
This project is part of the Algorithmics subject, part of the Bachelor Degree in Informatics Engineering at the Barcelona School of Informatics.
It is aimed to experiment with max-flow algorithms to solve resource assignment problems.

The problem
------------
Given some airline company flights and its schedulings within a day, determine the **minimum** number of pilots required to serve all the flights.
We are assuming that plane routes and departure / arrival times are already defined, and we only have to assign a pilot to a route.
This problem has two versions:

- Version 1: a transition between flight `i` and `j` is possible if there is a 15-minute margin between the arrival of `i` and the departure of `j`, and these occur at the same airport.
- Version 2: same as version 1, but allowing a pilot to travel *as a passenger* in another flight (of the same company).

Example:

Flight | Origin | Destination | Departure time | Arrival time
-------|--------|-------------|----------------|-------------
1 | BCN | CDG | 0 | 100
2 | CDG | BCN | 450 | 550
3 | CDG | LGW | 150 | 250
4 | LGW | CDG | 600 | 700
5 | LGW | CDG | 300 | 400
6 | CDG | BCN | 750 | 850

With version 1, a feasible assignment would be:
```Pilot 1: T1, T2
Pilot 2: T3, T4
Pilot 3: T5, T6```

But the optimum assignment would use only two pilots:
```Pilot 1: T1, T3, T5, T2
Pilot 2: T4, T6```

With version 2, a feasible assignment would be:
```Pilot 1: T1, T5, T2
Pilot 2: T3, T4, T6```

In the last example, the pilot 1 would go from CDG to LGW as a passenger in T3.


We will formulate the problem as a minimum-flow problem, showing that it is possible to find an optimum assignment with only two executions of a max-flow algorithm.
