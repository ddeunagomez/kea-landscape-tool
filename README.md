# Kea Landscape Tool

The **Kea Landscape Tool** is intended to be a tool to plan actions on environmental landscapes that have been disrupted by either human action, natural disasters or any other reason. This is still a **work in progress** (for my non-existent free time).
It uses ideas of electric circuit theory to model the landscape where animals live and compute the ``effective resistance`` or ``resistance distance`` between their different habitats. **Kea** not only computes this values, but searches for areas where the mobility of species (animals or plants) could be improved to achieve a major overall improvement. This is done using state-of-the-art Local Search algorithms.
The code is original from this project, but the ideas for computing the electrical current across the network are taken from *[Circuitscape](http://www.circuitscape.org/)*. Note that Circuitscape only provides a tool to *measure* the resistance distance of the habitats, and no algorithms to search for weaknesses and address them (like the present tool does).

The work done here was [originally done using proprietary software (namely IBM CPLEX) during my Ph.D](https://people.eng.unimelb.edu.au/pstuckey/papers/cpaior17a.pdf). I retook this project later to migrate it to work with different open source solvers. The code was also substantially cleaned (as what was done at that time was just a prototype) and is **still a work in progress**. The intention is to eventually get to create a user interface using Qt and QML where the users can see the landscape and can interact with it to see which places they believe need to be improved (by reforesting, etc.).

The original work was published in *Diego de Uña, Graeme Gange, Peter Schachte, Peter J. Stuckey. Minimizing Landscape Resistance for Habitat Conservation, In Proceedings of the 14th International Conference on Integration of Artificial Intelligence and Operations Research techniques in Constraint Programming CPAIOR-17* [[Springer-Link]](https://link.springer.com/chapter/10.1007/978-3-319-59776-8_10)  [[Open access PDF]](https://people.eng.unimelb.edu.au/pstuckey/papers/cpaior17a.pdf) . Please refer to that publication for further details.

# Backend Solvers
Currently Kea has interfaces for three solvers which are NOT provided here. You will need to install them, update the Makefile to reference the correct location of the libraries, as well as the ``kea.sh`` script.

 1. [PETSc](https://www.mcs.anl.gov/petsc/)
 2. [Armadillo](http://arma.sourceforge.net/)
 3. [SoPlex](http://soplex.zib.de/)

# Usage
Once all the libs necessary are installed, simply run `./bin/kea.sh --help` and you will see all the options.

# TODO

 1. Importing and documenting all Local Search strategies from the old code (80%)
 2. Add command line options to let the user control conductances (70%)
 3. Read different network formats (50%, supports TextList, like Circuitscape, now)
 4. Creating user interface (0%)
 5. Automate deployment system (CMake?)
