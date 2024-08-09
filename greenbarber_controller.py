import random
import time

# Height of the segment in pixels
SEGMENT_HEIGHT = 100
# Width of the segment in pixels
SEGMENT_WIDTH = 100

# Number of tries to validate the aromatic plant while centering the cut module
AROMATICS_VALIDATION_THRESHOLD = 3


class Point:
    def __init__(self, x=0, y=0, z=0):
        """
        Initialize a point with x, y, and z coordinates.
        """
        self.x = x
        self.y = y
        self.z = z

    def __repr__(self):
        """
        Return a string representation of the point.
        """
        return f"Point(x={self.x}, y={self.y}, z={self.z})"


class GreenBarberController:
    segment_counter = 0
    harvest_history = {}
    harvested_plants_counter = 0

    def __init__(self):
        """
        Initialize the GreenBarber robot's controller.
        Set up modules, 3axis bridge, harvesting module, and direction module.
        """
        print("Initializing GreenBarber robot... 🤖🌾")

        # Step 1: Initialize all modules
        if not self.init_modules():
            print("Failed to initialize modules. Exiting...")
            return

        # Step 2: Calibrate the 3-axis bridge
        if not self.calibrate_3axis_bridge():
            print("Failed to calibrate 3-axis bridge. Exiting...")
            return

        # Step 3: Calibrate the Harvesting module
        if not self.calibrate_harvesting_module():
            print("Failed to calibrate Harvesting module. Exiting...")
            return

        # Step 4: Calibrate the Direction module
        if not self.calibrate_direction_module():
            print("Failed to calibrate Direction module. Exiting...")
            return

        print("GreenBarber robot successfully initialized. 🚀")

    def init_modules(self) -> bool:
        """
        Initialize the various modules of the GreenBarber robot.

        Returns:
            bool: True if all modules were successfully initialized, False otherwise.
        """
        print("Initializing modules...")

        # TODO: Initialize specific modules here
        return True

    def calibrate_3axis_bridge(self):
        """
        Calibrate the 3-axis bridge for accurate orientation and positioning.

        Returns:
            bool: True if the 3-axis bridge was successfully calibrated, False otherwise.
        """
        print("Calibrating 3-axis bridge...")

        # TODO: Implement calibration logic for the 3-axis bridge here
        return True

    def calibrate_harvesting_module(self) -> bool:
        """
        Calibrate the Harvesting module.

        Returns:
            bool: True if the Harvesting module was successfully calibrated, False otherwise.
        """
        print("Calibrating Harvesting module...")
        # TODO: Implement calibration logic for the Harvesting module here
        return True

    def calibrate_direction_module(self) -> bool:
        """
        Calibrate the Direction module to ensure accurate navigation.
        """
        print("Calibrating Direction module...")
        # Implement calibration logic for the Direction module here
        return True

    def get_plants_coordinates(self) -> list[Point]:
        """
        Get plants coordinates.

        Returns:
            Point: The coordinates of the plants in the current segment. 
        """
        # TODO: Implement logic to get the coordinates of the plants in the current segment
        n_plants = random.randint(0, 3)
        plants_positions = [Point(random.randint(0, SEGMENT_WIDTH),
                                  random.randint(0, SEGMENT_HEIGHT)) for _ in range(n_plants)]
        return plants_positions

    def set_3axis(self, x: int = 0, y: int = 0, z: int = 0) -> bool:
        """
        Set the 3-axis bridge to the specified coordinates.

        Args:
            x (int): The x-coordinate.
            y (int): The y-coordinate.
            z (int): The z-coordinate.

        Returns:
            bool: True if the 3-axis bridge was successfully set to the specified coordinates, False otherwise.
        """
        # TODO: call the 3-axis bridge module to set the coordinates
        # TODO: I'm assuming -1 values to not move a specific axis, e.g., set_3axis(-1, 0, -1) to move only the y-axis to 0
        print(f"Setting 3-axis bridge to coordinates: x={x}, y={y}, z={z}")
        return True

    def is_cut_module_centered(self) -> bool:
        """
        Check if the cut module is centered.

        Returns:
            bool: True if the cut module is centered, False otherwise.
        """
        # TODO: This functions need to verify if the cutting module is centered
        # in the aromatic plant. To do this, it needs to compute the middle of
        # the square identified by the AromaticTracker and the current position
        # of the cutting module. If the cutting module is not centered, it needs
        # to move the 3-axis bridge to the center of the square (with some minor
        # adjustments).
        return True

    def is_y_position_in_front(self) -> bool:
        """
        Check if the Y position is in front, i.e., y = 0.

        Returns:
            bool: True if the Y position is in front, False otherwise.
        """
        # TODO: call the 3-axis bridge module to get the current y position
        curr_y = 1
        return curr_y == 0

    def is_aromatic_valid(self) -> bool:
        """
        Check if the aromatic plant is valid.

        Returns:
            bool: True if the aromatic plant is valid, False otherwise.
        """
        # TODO: call the AromaticsRecognizer module to check if the plant is good
        # to be harvested
        return True

    def harvest_plant(self) -> bool:
        """
        Harvest the plant in the current segment.

        Returns:
            bool: True if the plant was successfully harvested, False otherwise.
        """
        # TODO: call the Harvesting module to harvest the plant
        print("Harvesting plant...")
        return True

    def update_harvest_history(self, plant_positions: Point):
        """
        Updates Harvest history with the coordinates of the harvested plant in the current segment.

        Example:
            {
                0: [plant_positions00 [Point], plant_positions01 [Point], ...],
                1: [plant_positions10 [Point], plant_positions11 [Point], ...],
                ...
            }
        """
        if self.harvest_history.get(self.segment_counter) is None:
            self.harvest_history[self.segment_counter] = []
        self.harvest_history[self.segment_counter].append((plant_positions))

        self.harvested_plants_counter += 1

    def run(self):
        """
        Main loop to control the GreenBarber robot.
        """
        i = 0
        try:
            while i <= 1:
                plants_coordinates = self.get_plants_coordinates()

                print("Found plants at coordinates:", plants_coordinates)
                if plants_coordinates:  # hasAromaticsInArea?
                    for plant_position in plants_coordinates:
                        self.set_3axis(plant_position.x, plant_position.y)

                        isAromaticValid = self.is_aromatic_valid()
                        if not isAromaticValid:
                            continue

                        tries = 0
                        while not self.is_cut_module_centered():
                            self.set_3axis()  # TODO: To what position?
                            isAromaticValid = self.is_aromatic_valid()
                            if not isAromaticValid:
                                continue
                            if tries >= AROMATICS_VALIDATION_THRESHOLD:
                                # TODO: Mark that plant as notPlant
                                break
                            tries += 1

                        self.harvest_plant()
                        self.update_harvest_history(plant_position)
                    self.segment_counter += 1
                else:
                    isYPositionInFront = self.is_y_position_in_front()
                    if isYPositionInFront:
                        self.set_3axis(-1, 0, -1)
                    else:
                        self.set_3axis(-1, SEGMENT_HEIGHT, -1)

                time.sleep(3)  # Adjust loop timing as needed
                i += 1
        except KeyboardInterrupt:
            self.shutdown_robot()

    def shutdown_robot(self):
        """
        Method to safely shut down the GreenBarber robot.
        """
        print("Shutting down GreenBarber robot...")

        # TODO: Shutdown logic here
        print("GreenBarber robot successfully shutted down.")
        pass


if __name__ == "__main__":
    controller = GreenBarberController()
    controller.run()
