# Walkthrough: a new C++ Force used from Python

This walkthrough adds a new C++ cell-based `Force` to your project, exposes it through the
project's Python bindings, and then uses it in a [PyChaste](https://chaste.github.io/)
simulation driven from Python.

It assumes you have already created your project from this template and answered **yes** to
the Python bindings prompt and **yes** to the cell-based prompt in `setup_project.py`, so that `dynamic/config.yaml`,
`dynamic/CMakeLists.txt` and `src/py/` are present.

Run every command below from your project's root directory, and replace `myproject` with
your project's name throughout.

## 1. Add the force to your project's source

Copy the example force class into your project's `src/` directory:

```sh
cp examples/my_force/MyForce.hpp examples/my_force/MyForce.cpp src/
```

[`MyForce`](MyForce.hpp) is templated over the spatial dimension and subclasses
`AbstractForce<DIM>`. Its `AddForceContribution()` applies a constant force, in the
positive x-direction, to every node in the population — a deliberately simple force whose
only job is to demonstrate the C++ → Python workflow.

## 2. Expose the force to the Python bindings

Tell cppwg to wrap the new class by editing `dynamic/config.yaml`:

* add its header under `source_includes:`

  ```yaml
  source_includes:
    - SmartPointers.hpp
    - Hello_ProjectName.hpp
    - MyForce.hpp
  ```

* under the `all` module, add the `MyForce` to `classes:`, then tell cppwg that
  `AbstractForce` is wrapped in PyChaste by adding the `chaste._pychaste_all`
  module under `imports:` and listing `AbstractForce` under `external_bases:`

  ```yaml
  modules:
    - name: all
      imports: #<-- new
        - chaste._pychaste_all #<-- new
      external_bases: #<-- new
        - AbstractForce #<-- new
      source_locations:
        - src/
      classes:
        - name: Hello_ProjectName
        - name: MyForce #<-- new
```

  This is what makes the cross-module inheritance work: `MyForce` subclasses
  `AbstractForce`, which is wrapped in PyChaste (a different package). Listing
  `AbstractForce` under `external_bases` tells cppwg it is registered there, so
  cppwg references it as the base of `MyForce`; `imports` then imports
  `chaste._pychaste_all` so that base type is registered before `MyForce`.
  Without this, `OffLatticeSimulation.AddForce(my_force)` would reject the force
  because Python would not recognise `MyForce` as an `AbstractForce`.

  > Use the latest version of [cppwg](https://github.com/Chaste/cppwg) to
  > ensure it has cross-module inheritance support.

Because `MyForce` is templated over `<unsigned DIM>`, it is wrapped once per
dimension and exposed in Python as `MyForce_2` (2D) and `MyForce_3` (3D).

## 3. Compile and install the bindings

With `CHASTE_SOURCE_DIR` pointing at your Chaste source:

```sh
scripts/configure.sh         # only needed the first time
scripts/compile.sh           # rebuilds the project and its bindings
scripts/bindings_install.sh  # installs PyChaste + your project into .virtualenv/
```

## 4. Activate the virtualenv

```sh
source .virtualenv/bin/activate
```

## 5. Use the force in a Python simulation

A ready-to-run script lives next to this walkthrough at
[`run_my_force.py`](run_my_force.py). It builds a small node-based cell population and runs
an off-lattice simulation that adds both a standard spring force and our custom `MyForce`:

```python
import chaste
import chaste.cell_based
import chaste.mesh

chaste.init()

import myproject  # provides MyForce_2

# The cell-cycle models need the simulation clock to exist before cells are created.
chaste.cell_based.SimulationTime.Instance().SetStartTime(0.0)

# Build a small node-based cell population.
generator = chaste.mesh.HoneycombMeshGenerator(5, 5)
mesh = chaste.mesh.NodesOnlyMesh_2()
mesh.ConstructNodesWithoutMesh(generator.GetMesh(), 1.5)

transit_type = chaste.cell_based.TransitCellProliferativeType()
cell_generator = chaste.cell_based.CellsGenerator["UniformCellCycleModel", "2"]()
cells = cell_generator.GenerateBasicRandom(mesh.GetNumNodes(), transit_type)
cell_population = chaste.cell_based.NodeBasedCellPopulation_2(mesh, cells)

# Run an off-lattice simulation using a standard force and our custom force.
simulator = chaste.cell_based.OffLatticeSimulation_2_2(cell_population)
simulator.SetOutputDirectory("Python/MyForce")
simulator.SetEndTime(1.0)
simulator.AddForce(chaste.cell_based.GeneralisedLinearSpringForce_2_2())
simulator.AddForce(myproject.MyForce_2(1.0))  # <-- our new force, from C++
simulator.Solve()
```

Edit the `import myproject` line to your project name, then run it:

```sh
python examples/my_force/run_my_force.py
```

You should see the simulation run to completion and print the number of cells. The custom
force pushes the whole population in the x-direction over the course of the simulation,
confirming that your new C++ class is callable from Python.

> If `myproject.MyForce_2` is not found, list the generated names with
> `print([n for n in dir(myproject) if "MyForce" in n])` — the dimension suffix depends on
> how the class is templated (see the note in the top-level README).

## Troubleshooting
See the main [README](../README.md#troubleshooting-the-bindings) for steps to fix problems with adding Python bindings.

## Next steps

* Give `MyForce` more parameters or a different `AddForceContribution()` and rebuild.
* Add more of your own classes to `dynamic/config.yaml` the same way.
* See the [PyChaste tutorials](https://chaste.github.io/docs/python-tutorials/) for more
  complete cell-based simulations.
