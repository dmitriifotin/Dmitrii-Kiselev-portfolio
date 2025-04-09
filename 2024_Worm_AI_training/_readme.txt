Training a worm model to learn to move in a physics engine

The goal of the project was to create a worm-like model in a CoppeliaSim (physics engine) and write a script for it to learn to move.
This was a basic introduction to AI training. The training was setup to reward displacement and record each simulation results for future runs. Ultimately, the model preferred shifting weight from one side to the other and displacing itself around its center of mass. This may be due to a significant number of drives and sensors involved, which made the learning curve very steep for the actual worm-like movement of shifting forward. Either a more "evolutionary" approach could be used by scaling the model down and gradually adding components, or by introducing a more refined feedback/reward system.

MCU: n/a
Script: Python
Components: n/a
Timeline: 3 months