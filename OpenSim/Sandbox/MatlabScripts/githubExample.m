% /* -------------------------------------------------------------------------- *
%  *                    OpenSim:  testREADME.cpp                                *
%  * -------------------------------------------------------------------------- *
%  * The OpenSim API is a toolkit for musculoskeletal modeling and simulation.  *
%  * See http://opensim.stanford.edu and the NOTICE file for more information.  *
%  * OpenSim is developed at Stanford University and supported by the US        *
%  * National Institutes of Health (U54 GM072970, R24 HD065690) and by DARPA    *
%  * through the Warrior Web program.                                           *
%  *                                                                            *
%  * Copyright (c) 2005-2016 Stanford University and the Authors                *
%  * Author(s): James Dunne                                                    *
%  * Contributor(s): Thomas Uchida, Chris Dembia                                 *
%  *                                                                            *
%  * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
%  * not use this file except in compliance with the License. You may obtain a  *
%  * copy of the License at http://www.apache.org/licenses/LICENSE-2.0.         *
%  *                                                                            *
%  * Unless required by applicable law or agreed to in writing, software        *
%  * distributed under the License is distributed on an "AS IS" BASIS,          *
%  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
%  * See the License for the specific language governing permissions and        *
%  * limitations under the License.                                             *
%  * -------------------------------------------------------------------------- */

%% Matlab version of the Github example

import org.opensim.modeling.*

model = Model();
%model.setUseVisualizer(true);

% origin, and moments and product   s of inertia of zero.
humerus = Body('humerus', 1, Vec3(0), Inertia(0));
radius = Body('radius', 1, Vec3(0), Inertia(0));

% Connect the bodies with pin joints. Assume each body is 1 m long.
shoulder = PinJoint('shoulder', model.getGround(), Vec3(0), Vec3(0), humerus, Vec3(0, 1, 0), Vec3(0));
elbow = PinJoint('elbow', humerus, Vec3(0), Vec3(0), radius, Vec3(0, 1, 0), Vec3(0));

% Add a muscle that flexes the elbow.
biceps = Millard2012EquilibriumMuscle('biceps', 200, 0.6, 0.55, 0);
biceps.addNewPathPoint('origin',humerus, Vec3(0, 0.8, 0));
biceps.addNewPathPoint('insertion', radius,  Vec3(0, 0.7, 0));

% Add a controller that specifies the excitation of the muscle.
brain = PrescribedController();
brain.addActuator(biceps);
% Muscle excitation is 0.3 for the first 0.5 seconds, then increases to 1.
brain.prescribeControlForActuator('biceps', StepFunction(0.5, 3, 0.3, 1));

% Add components to the model.
model.addBody(humerus);    model.addBody(radius);
model.addJoint(shoulder);  model.addJoint(elbow);
model.addForce(biceps);
model.addController(brain);

% Add a console reporter to print the muscle fiber force and elbow angle.
reporter = ConsoleReporter();
reporter.set_report_time_interval(1.0);
reporter.updInput('inputs').connect(biceps.getOutput('fiber_force'));
reporter.updInput('inputs').connect(elbow.getCoordinate().getOutput('value'),'elbow_angle');
model.addComponent(reporter);

% Configure the model.
state = model.initSystem();
% Fix the shoulder at its default angle and begin with the elbow flexed.
shoulder.getCoordinate().setLocked(state, true);
elbow.getCoordinate().setValue(state, 0.5 * pi);
model.equilibrateMuscles(state);

% Add display geometry.
% model.updMatterSubsystem().setShowDefaultGeometry(true);
% viz = model.updVisualizer().updSimbodyVisualizer();
% viz.setBackgroundColor(White);
% % Ellipsoids: 0.5 m radius along y-axis, centered 0.5 m up along y-axis.
% DecorativeEllipsoid geom(Vec3(0.1, 0.5, 0.1)); Vec3 center(0, 0.5, 0);
% viz.addDecoration(humerus.getMobilizedBodyIndex(), Transform(center), geom);
% viz.addDecoration( radius.getMobilizedBodyIndex(), Transform(center), geom);

% Simulate.
manager = Manager(model);
manager.setInitialTime(0); manager.setFinalTime(10.0);

for i = 1 :10
    manager.integrate(state);
end

