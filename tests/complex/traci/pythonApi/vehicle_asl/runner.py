#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Jakob Erdmann
# @date    2017-01-23
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
import random
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci
import sumolib  # noqa
import traci.constants as tc

sumoBinary = os.environ["SUMO_BINARY"]
#~ sumoBinary = sumolib.checkBinary("sumo-gui")
cmd = [sumoBinary,
        '-n', 'input_net.net.xml',
        '-r', 'input_routes.rou.xml',
        '--lanechange-output', 'lanechanges.xml',
        '--lanechange-output.started',
        '--lanechange-output.ended',
        '--fcd-output', 'fcd.xml',
        '--no-step-log',
        '--begin', '0',
        #~ '--lateral-resolution', '3.2',
        '--step-length', '0.1',
        '--default.action-step-length', '1.0',
        #'-S', '-Q',
        ]


traci.start(cmd)
traci.simulationStep()
vehID = "v0"
traci.vehicle.setLaneChangeMode(vehID, 0b0100000000)
traci.vehicle.changeLane(vehID, 1, 0)
for i in range(10):
    step = traci.simulation.getCurrentTime()
    traci.simulationStep(step + 1000)
traci.vehicle.setSpeed(vehID, 0)
for i in range(10):
    step = traci.simulation.getCurrentTime()
    traci.simulationStep(step + 1000)
traci.vehicle.setSpeed(vehID, -1)
for i in range(10):
    step = traci.simulation.getCurrentTime()
    traci.simulationStep(step + 1000)
# step to in between action steps
step = traci.simulation.getCurrentTime()
traci.simulationStep(step + 500)
traci.vehicle.changeLane(vehID, 0, 0)
for i in range(10):
    step = traci.simulation.getCurrentTime()
    traci.simulationStep(step + 1000)
traci.vehicle.setSpeed(vehID, 0)
for i in range(10):
    step = traci.simulation.getCurrentTime()
    traci.simulationStep(step + 1000)
traci.vehicle.setSpeed(vehID, -1)
for i in range(10):
    step = traci.simulation.getCurrentTime()
    traci.simulationStep(step + 1000)

traci.close()
