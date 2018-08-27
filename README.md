# Metabopolis

Metabopolis is a new layout algorithm for biological pathways, which has been developed by [Hsiang-Yun Wu][yw], [Martin Nöllenburg][mn], [Filipa Sousa][fs], and
[Ivan Viola][iv], in the group [Research Division of Computer Graphics][rdcg] at [TU Wien][tuwien] in Vienna, Austria. The c++ library was developed by Hsiang-Yun Wu.

## How to cite?

TBA.

## Installing

### Prerequisites

- [cmake 3.4+][cmake]
- [qt 5.8][qt5]
- [cplex 128][cplex]
- [hola][hola]
- [Boost Graph Library 1.58][boost]
- [cgal][cgal]
- [tinyxml][tinyxml]
- [sbml][sbml]
- [sbgnml][sbgnml]


### Metabopolis

Clone the Metabopolis repository into a directory `METABOPOLIS`.

```
$ cd METABOPOLIS
$ mkdir build
$ cd build
$ cmake ../
$ make
```
### Tested environment

- Ubuntu 16.04 LTS
- Mac OS 10.12

## Using Metabopolis

```
$ ./metabopolis
```

Step 1:
  Select the target folder of a dataset by pressing button "Browse"
  Defined: User defined duplicated metabolites. Duplication of metabolites is defined from ~/Desktop/gitroot/tools/bionet/xml/metabolite_frequency.txt
  Threshold: User defined threshold for metabolites duplication

Step 2:
  Edit the relationship graph
  Delete an edge: Shift Key + Mouse Right Button on an Edge
  Add an edge: Shift Key + Mouse Left Button on one Node and the other
  Confirm button: Compute graph layout
  Adjust button: Adjust box size

Step 3:
  Compute button: Compute the orthogonal layout
  HOLA: human-like orthogonal layout
  yFiles: compact orthogonal algorithm from yFiles package

Step 4:
  Compute button: Compute the connectivity between nodes in different categories

Step 5:
  Interaction mode
  Local: Connection of metabolites in one category
  Global: Connection of metabolites in multiples categories
  Both: Both
  Bundled: Bundled edges
  Extended: Extended edges

Remarks:

  - After loading the file folder, press "Batch" to run the program with all default settings.
  - Press "Export" to export images, which will be stored at `METABOPOLIS/svg/`

## Acknowledgement

The project leading to this submission has received funding from the European Union Horizon 2020 research and innovation programme under the Marie Sklodowska-Curie grant agreement No. 747985 and from the Vienna Science and Technology Fund (WWTF) through project VRG11-010.

[cmake]: https://cmake.org/
[hola]: http://www.infotech.monash.edu.au/about/schools/caulfield/
[cplex]: https://www.ibm.com/products/ilog-cplex-optimization-studio
[tinyxml]: http://www.grinninglizard.com/tinyxml/
[qt5]: https://www.qt.io/
[boost]: https://www.boost.org/
[cgal]: https://www.cgal.org/
[sbml]: https://sbml.org/
[sbgnml]: https://github.com/sbgn

[rdcg]: https://www.cg.tuwien.ac.at/
[tuwien]: https://www.tuwien.ac.at/
[yw]: https://www.cg.tuwien.ac.at/staff/HsiangYunWu.html
[mn]: https://www.ac.tuwien.ac.at/people/noellenburg/
[fs]: https://archaea.univie.ac.at/research/filipa-sousa-lab/
[iv]: https://www.cg.tuwien.ac.at/staff/IvanViola.html
