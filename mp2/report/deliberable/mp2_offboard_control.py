#!/bin/python3
# Constructed based on example file in mavsdk-python
import asyncio
import mavsdk
import numpy as np
from mavsdk import System
from mavsdk.offboard import (PositionNedYaw, VelocityNedYaw, OffboardError)
import math
import random

velpos = []
lock = asyncio.Lock()

# Record position and velocity
async def record_velpos(drone):
    global velpos, lock
    async for odom in drone.telemetry.position_velocity_ned():
        async with lock:
            vel = [odom.position.north_m, odom.position.east_m, odom.position.down_m]
            pos = [odom.velocity.north_m_s, odom.velocity.east_m_s, odom.velocity.down_m_s]
            velpos.append( vel + pos)
        # print(f"{odom.velocity.north_m_s} {odom.velocity.down_m_s}")

async def run():
    global velpos, lock
    drone = System()
    await drone.connect(system_address="udp://:14540")

    print("Waiting for drone to connect...")
    async for state in drone.core.connection_state():
        if state.is_connected:
            print(f"Drone discovered.")
            break

    print("-- Arming")
    await drone.action.arm()

    print("-- Setting initial setpoint")
    await drone.offboard.set_position_ned(PositionNedYaw(0.0, 0.0, 0.0, 0.0))

    asyncio.ensure_future(record_velpos(drone))

    print("-- Starting offboard")
    try:
        await drone.offboard.start()
    except OffboardError as error:
        print(f"Starting offboard mode failed with error code: {error._result.result}")
        print("-- Disarming")
        await drone.action.disarm()
        return

    await asyncio.sleep(3)

    print("--- Starting Mission")
    # TODO: Implement mission commands here

    # TODO: Implement mission commands here
    # LINEAR MISSION
    await drone.offboard.set_velocity_ned(VelocityNedYaw(0,0,0,0))

    # await drone.offboard.set_position_ned(PositionNedYaw(0.0, 0.0, -10, 0.0))
    # await asyncio.sleep(15)
    # await drone.offboard.set_position_ned(PositionNedYaw(5, 5, -10, 0.0))
    # await asyncio.sleep(15)
    # await drone.offboard.set_position_ned(PositionNedYaw(5, 5, -10, 0.0))
    # await asyncio.sleep(15)

    # CIRCULAR MISSION
    await drone.offboard.set_position_ned(PositionNedYaw(0.0, 0.0, -10, 0.0))
    await asyncio.sleep(5)
    

    step_size = 0.1
    r = 20

    t = 0
    while t < 2 * math.pi:
        x = r * math.cos(t)
        y = r * math.sin(t)
        print('Circle (X, Y) = ({}, {})'.format(x, y))

        await drone.offboard.set_position_ned(PositionNedYaw(x, y, -10, 0.0))
        await asyncio.sleep(1)

        t+= step_size

    await drone.offboard.set_position_ned(PositionNedYaw(0.0, 0.0, -10, 0.0))
    await asyncio.sleep(5)
    print("--- Finished Mission")

    print("--- Starting Land")
    await drone.action.land()
    await asyncio.sleep(10)

    async with lock:
        np.save('traj.npy',np.array(velpos))
        print("Saved the trajectory as traj.npy")
    await asyncio.sleep(10)

    print("-- Stopping offboard")
    try:
        await drone.offboard.stop()
    except OffboardError as error:
        print(f"Stopping offboard mode failed with error code: {error._result.result}")



if __name__ == "__main__":
    loop = asyncio.get_event_loop()
    loop.run_until_complete(run())
