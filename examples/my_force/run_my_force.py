"""Run a PyChaste node-based cell simulation that uses MyForce from the project bindings.

Before running this, build and install your project's Python bindings (see the README in
this directory), then activate the virtualenv:

    source .virtualenv/bin/activate
    python examples/my_force/run_my_force.py

Replace ``myproject`` below with your project's name.
"""

import chaste
import chaste.cell_based
import chaste.mesh

chaste.init()

# The project bindings, which provide MyForce. Replace with your project name.
import myproject

# A templated class is wrapped once per dimension. cppwg names the 2D instantiation of
# MyForce<DIM> as MyForce_2 (the dimensions follow an underscore). If this name is wrong
# for your build, run: print([n for n in dir(myproject) if "MyForce" in n])
MyForce_2 = myproject.MyForce_2


def main():
    # Choose where results are written.
    chaste.core.OutputFileHandler("Python/MyForce")

    # The cell-cycle models need the simulation clock to exist before cells are created.
    chaste.cell_based.SimulationTime.Instance().SetStartTime(0.0)

    # Build a small node-based cell population on a honeycomb mesh.
    generator = chaste.mesh.HoneycombMeshGenerator(5, 5)
    generating_mesh = generator.GetMesh()

    mesh = chaste.mesh.NodesOnlyMesh_2()
    mesh.ConstructNodesWithoutMesh(generating_mesh, 1.5)

    transit_type = chaste.cell_based.TransitCellProliferativeType()
    cell_generator = chaste.cell_based.CellsGenerator["UniformCellCycleModel", "2"]()
    cells = cell_generator.GenerateBasicRandom(mesh.GetNumNodes(), transit_type)

    cell_population = chaste.cell_based.NodeBasedCellPopulation_2(mesh, cells)

    # Set up an off-lattice simulation.
    simulator = chaste.cell_based.OffLatticeSimulation_2_2(cell_population)
    simulator.SetOutputDirectory("Python/MyForce")
    simulator.SetSamplingTimestepMultiple(12)
    simulator.SetEndTime(1.0)

    # A standard spring force keeps neighbouring cells interacting...
    spring_force = chaste.cell_based.GeneralisedLinearSpringForce_2_2()
    simulator.AddForce(spring_force)

    # ...and our custom force from the project bindings pushes every cell in +x.
    my_force = MyForce_2(1.0)
    simulator.AddForce(my_force)

    simulator.Solve()

    print(f"Simulation complete with {cell_population.GetNumRealCells()} cells.")


if __name__ == "__main__":
    main()
