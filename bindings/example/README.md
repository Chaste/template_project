# Bindings Example: a new C++ Force used from Python

This example adds a new C++ cell-based `Force` to your project, exposes it through the
project's Python bindings, and then uses it in a [PyChaste](https://chaste.github.io/)
simulation driven from Python.

It assumes you have already created your project from this template and answered **yes** to
the Python bindings prompt and **yes** to the cell-based prompt in `setup_project.py`, so that the `bindings/` folder is present, containing a `config.yaml`,
a `CMakeLists.txt` and a `package/` subfolder.

Run every command below from your project's root directory, and replace `myproject` with
your project's name throughout.

## Add the force to your project's source

Copy the example force class into your project's `src/` directory:

```sh
cp bindings/example/MyForce.?pp src/
```

[`MyForce`](MyForce.hpp) subclasses
`AbstractForce<DIM>` and is templated over the spatial dimension. Its `AddForceContribution()` applies a constant force in the
positive x-direction to every node in the population.

## Expose the force to the Python bindings

Tell [cppwg](https://github.com/Chaste/cppwg) (the wrapper generator) to wrap the new class by editing
`bindings/config.yaml` and adding `MyForce` to `classes:`.

`MyForce` subclasses `AbstractForce`, which is wrapped in PyChaste, so tell
cppwg this by adding the `chaste._pychaste_all` module under `imports:` and
listing `AbstractForce` under `external_bases`:

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

Cross-package inheritance works even though
`AbstractForce` is wrapped in PyChaste, which is a separate Python package from
the user project's Python package. Listing `AbstractForce` under `external_bases`
tells cppwg it is registered outside the module, and specifying `chaste._pychaste_all`
as an import ensures that `AbstractForce` is registered before `MyForce`.
Without this, Python would not recognise `MyForce` as an `AbstractForce`.
As a result, for example, `OffLatticeSimulation.AddForce(my_force)` would reject
the force.

Because `MyForce` is templated over `<unsigned DIM>`, it is wrapped once per
dimension. The setting `discover_template_instantiations: True` in `config.yaml`
picks up the explicit instantiations at the bottom of [`MyForce.cpp`](MyForce.cpp),
`template class MyForce<1>;` and so on. The class is exposed in Python as
`MyForce_1`, `MyForce_2` (2D) and `MyForce_3` (3D).

## Compile and install the bindings

With `CHASTE_SOURCE_DIR` pointing at your Chaste source:

```sh
scripts/configure.sh         # only needed the first time
scripts/compile.sh           # rebuilds the project and its bindings
bindings/scripts/install.sh  # installs PyChaste + your project into .virtualenv/
```


## Use the force in a Python simulation

Activate the virtualenv

```sh
source .virtualenv/bin/activate
```

The example [`run_my_force.py`](run_my_force.py) script builds a small node-based cell population and runs
an off-lattice simulation that adds both a standard spring force and our custom `MyForce`:

```python
import chaste
import chaste.cell_based
import chaste.mesh

chaste.init()

import myproject  # provides MyForce_2

# The cell-cycle models need the simulation clock to exist before cells are created.
chaste.SimulationTime.Instance().SetStartTime(0.0)

# Build a small node-based cell population. The same distance sets how far apart
# cells interact and where the force below is cut off, so they must agree.
cutoff_length = 1.5
generator = chaste.HoneycombMeshGenerator(5, 5)
mesh = chaste.mesh.NodesOnlyMesh_2()
mesh.ConstructNodesWithoutMesh(generator.GetMesh(), cutoff_length)

transit_type = chaste.TransitCellProliferativeType()
cell_generator = chaste.CellsGenerator["UniformCellCycleModel", "2"]()
cells = cell_generator.GenerateBasicRandom(mesh.GetNumNodes(), transit_type)
cell_population = chaste.cell_based.NodeBasedCellPopulation_2(mesh, cells)

# Run an off-lattice simulation using a standard force and our custom force.
simulator = chaste.cell_based.OffLatticeSimulation_2_2(cell_population)
simulator.SetOutputDirectory("Python/MyForce")
simulator.SetEndTime(1.0)
spring_force = chaste.cell_based.PathmanathanInteractionForce_2_2()
spring_force.SetCutOffLength(cutoff_length)
simulator.AddForce(spring_force)
simulator.AddForce(myproject.MyForce_2(1.0))  # <-- our new force, from C++
simulator.Solve()
```

Edit the `import myproject` line to your project name, then run it:

```sh
python bindings/example/run_my_force.py
```

You should see the simulation run to completion and print the number of cells. The custom
force pushes the whole population in the x-direction over the course of the simulation,
confirming that your new C++ class is callable from Python.

> If `myproject.MyForce_2` is not found, list the generated names with
> `print([n for n in dir(myproject) if "MyForce" in n])`. The dimension suffix depends on
> how the class is templated (see the note in the [bindings README](../README.md)).

## Troubleshooting
See the [bindings README](../README.md#troubleshooting-the-bindings) for steps to fix problems with adding Python bindings.

## Next steps

* Give `MyForce` more parameters or a different `AddForceContribution()` and rebuild.
* Add more of your own classes to `bindings/config.yaml` the same way.
* See the [PyChaste tutorials](https://chaste.github.io/pychaste/tutorials/) for more
  complete cell-based simulations.
