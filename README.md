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
```
Pilot 1: T1, T2
Pilot 2: T3, T4
Pilot 3: T5, T6
```

But the optimum assignment would use only two pilots:
```
Pilot 1: T1, T3, T5, T2
Pilot 2: T4, T6
```

With version 2, a feasible assignment would be:
```
Pilot 1: T1, T5, T2
Pilot 2: T3, T4, T6
```

In the last example, the pilot 1 would go from CDG to LGW as a passenger in T3.


The minimum flow problem
-------------------------

Most of the work is based on:

> Algorithm Design by Jon Kleinberg, Eva Tardos: Chapter 7 (Network flow)

This project uses the reductions stated on this book.

When you want to find the maximum flow on a network, you start with an empty network and you use a max-flow algorithm from *s* to *t* to augment the flow.
To find the minimum flow that is a feasible flow, i.e. it satisfies all the lower bounds, capacity, and flow conservation constraints,
you twist the approach: you start with a feasible flow, **f** and you reduce it by applying a max-flow algorithm from *t* to *s*.
Think of it as if supplying *air* from *t* to *s*, instead of pumping *fluid* or *resource* from *s* to *t*.
The resulting *air flow* is maximum, with value **f'**.
The optimum flow will be `Fopt = f - f'`. Applying a max-flow algorithm again on an empty network knowing the optimum flow value will lead you to a feasible circulation that is minimum.