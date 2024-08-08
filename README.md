# GreenBarber 

The Green Barber is a Robot designed to harvest aromatics plants such as lime-thyme as the first target.

## Overall Architecture

- 1. A cart which rides above lines of plants with direction and traction system.
- 2. A cutting module designed to harvest the excess leafs leaving the plant into a rounded shape.
- 3. A vacuum system which sucks the cut leafs into a bag.
- 4. A 3-axis-bridge which positions the harvesting module on the plants.
- 5. An image recognition system which tracks and recognizes the plants allowing to map the aromatics positions.
- 6. A navigation system using frontal cameras designed to align itself with the aromatics lines.
- 7. A GPS module which gives the approximate location in a map and a web interface designed for the user to indicate where is the harvesting targets are and interact with the robot.
- 8. A power management system which produces all the needed voltages by the different modules until 24V.

### Architecture Diagram

![Architecture Diagram](/architecture.png)

- [Link to the Architecture Diagram](https://1drv.ms/u/s!AtIa6l6rtp56qzVm6Xl4qO35xify?e=c8eezd)

- [Link to animated simulation of the harvesting idea](
https://drive.google.com/file/d/14uQVEL2jJQ8OUNeYXhHj3-7X3vHlUGO4/view?usp=drive_link)

## Modules

- 2.
- 3. [CutterModule](./modulo_corte_mega2560proMini/) - C++ code for the cutting module. Project designed to run on an atmega2560 pro mini using the Arduino library environment
- 4. [3axisBridge](./3axisBridge/) - C++ code for the 3-axis-bridge control
- 4. [AromaticsTracker](./AromaticsTracker/)
- 5. [AromaticsRecognizer](./AromaticsRecognizer/)

## Green Barber Aggregated Workflow
![Flow Chart Diagram](/flowChart.png)
- [Link to Flow Chart](https://1drv.ms/u/s!AtIa6l6rtp56t1KFMW9ak1gB99wk?e=hhpVGZ)
- [Link to Draw.io](https://app.diagrams.net)

## Module Workflow

Module workflow: receive harvest instruction -> turn on vacuum, blades and rotation -> harvesting -> stop vacuum, blades and rotation -> load harvest to the bag!

Module workflow: receive xyz coordinate instruction -> xyzToMotorTicks -> motor position with pid control


## IDEAS

There will be multiple cameras (CAM-N) on the harvesting area and an extra camera (CAM-0) in the harvesting module right on top of the plants.

- Phase 1: Image tracking and classification by multiple cameras (and position harvesting module over the plant)

- Phase 2: Confirmation of recognized plant and mechanic loop for finding the exact plant's center and repositioning

Multiple cameras (CAM-N) (video feed -> frames -> hasAromaticsInFrame? -> areTheyReadyToBeHarvested?) -> 
-> combine recognition results with other cameras -> getAromaticPositions -> Give 3-axis-bridge position instruction -> areTheyReadyToBeHarvested? (CAM-0) ->
-> loop: (getCenterAromaticPosition & 3-axis-bridge reposition) -> Harvest!
