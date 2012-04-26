/* Author: Matthew Millard
/*
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include <OpenSim\Actuators\ActiveForceLengthCurve.h>//"ActiveForceLengthCurve.h"
#include <OpenSim\Actuators\ForceVelocityCurve.h>    //"ForceVelocityCurve.h"

#include <SimTKsimbody.h>
#include <ctime>
#include <string>
#include <stdio.h>

using namespace std;
using namespace OpenSim;
using namespace SimTK;

void testActiveForceLengthCurve();
void testForceVelocityCurve();

int main(int argc, char* argv[])
{
	

	try {
            SimTK_START_TEST("Testing Serializable Curves");
            testActiveForceLengthCurve();
            testForceVelocityCurve();
            SimTK_END_TEST();
    }
    catch (OpenSim::Exception ex)
    {
        cout << ex.getMessage() << endl;
		cin.get();
        return 1;
    }
    catch (const std::exception& ex)
    {
        cout << ex.what() << endl;
		cin.get();		
		return 1;
    }
    catch (...)
    {
        cout << "UNRECOGNIZED EXCEPTION" << endl;
		cin.get();
        return 1;
    }

	

    cout << "\n Serializable Curve Testing completed successfully.\n";
	return 0;
}

void testActiveForceLengthCurve()
{
        //cout <<"**************************************************"<<endl;
        cout <<"____________________________________________________"<<endl;
        cout <<"1. Testing: ActiveForceLengthCurve "<<endl;       
        cout <<"____________________________________________________"<<endl;

        cout <<"    a. default construction" <<endl;
        ActiveForceLengthCurve falCurve1;
        //falCurve1.setName("default_ActiveForceLengthCurve");
        falCurve1.print("default_ActiveForceLengthCurve.xml");

        cout <<"    b. serialization & deserialization" <<endl;
        ActiveForceLengthCurve falCurve2;
        falCurve2.setMaxActiveFiberLength(2);
        falCurve2.setTransitionFiberLength(0.8);
        falCurve2.setMinActiveFiberLength(0);
        falCurve2.setMinValue(0.3);
        falCurve2.setShallowAscendingSlope(0.5);


        //These next few lines are just to read the object in, and repopulate
        //falCurve2 with the properties from the file ... and its a little 
        //awkward to use.
     
        
        Object* tmpObj = Object::
                       makeObjectFromFile("default_ActiveForceLengthCurve.xml");
        falCurve2 = *dynamic_cast<ActiveForceLengthCurve*>(tmpObj);
        delete tmpObj;
        
        SimTK_TEST(falCurve2 == falCurve1);        
        remove("default_ActiveForceLengthCurve.xml");        

        falCurve2.setMaxActiveFiberLength(2);
        falCurve2.setTransitionFiberLength(0.8);
        falCurve2.setMinActiveFiberLength(0);
        falCurve2.setMinValue(0.3);
        falCurve2.setShallowAscendingSlope(0.5);

        cout <<"    c. assignment operator" <<endl;
        falCurve2=falCurve1;
                
        SimTK_TEST(falCurve1==falCurve2);

        falCurve2.setMaxActiveFiberLength(2);
        falCurve2.setTransitionFiberLength(0.8);
        falCurve2.setMinActiveFiberLength(0);
        falCurve2.setMinValue(0.3);
        falCurve2.setShallowAscendingSlope(0.5);

        cout <<"    d. copy constructor" <<endl;
        ActiveForceLengthCurve falCurve2p5(falCurve2);
        SimTK_TEST(falCurve2==falCurve2p5);

        cout << "Passed: default construction, serialization" << endl;
        cout << "         assignment operator, copy constructor" << endl;

        //cout <<"**************************************************"<<endl;
        cout <<"2. Testing API constructor" << endl;
        ActiveForceLengthCurve falCurve3(0.5, 0.75,1.5,0.75,0.01,"testMuscle");
        double falVal  = falCurve3.calcValue(1.0);
        double dfalVal = falCurve3.calcDerivative(1.0,1);
        cout << "Passed: Testing API constructor" << endl;

        //cout <<"**************************************************"<<endl;
        cout <<"3. Testing get/set methods:" << endl;

        falCurve2.setMinActiveFiberLength(0);
        falCurve2.setTransitionFiberLength(0.8);
        falCurve2.setMaxActiveFiberLength(2);        
        falCurve2.setMinValue(0.3);
        falCurve2.setShallowAscendingSlope(0.5);

        SimTK_TEST(falCurve2.getMinActiveFiberLength() == 0.0);
        SimTK_TEST(falCurve2.getTransitionFiberLength()== 0.8);
        SimTK_TEST(falCurve2.getMaxActiveFiberLength() == 2.0);
        SimTK_TEST(falCurve2.getMinValue() == 0.3);
        SimTK_TEST(falCurve2.getShallowAscendingSlope() == 0.5);

        cout << "Passed: Testing get/set methods" << endl;

        //cout <<"**************************************************"<<endl;
        cout <<"4. Testing Services for connectivity:" << endl;        
        ActiveForceLengthCurve falCurve4;
        falCurve4.setName("falCurve");

        cout <<"    a. calcValue" << endl;
            double tol = sqrt(SimTK::Eps);
            double value = falCurve4.calcValue(1.0);
            SimTK_TEST_EQ_TOL(value, 1, tol);
        cout <<"    b. calcDerivative" << endl;
            double dvalue= falCurve4.calcDerivative(1.0,1);
            SimTK_TEST_EQ_TOL(dvalue, 0, tol);

        cout <<"    c. getCurveDomain" << endl;
            SimTK::Vec2 tmp = falCurve4.getCurveDomain();
            SimTK_TEST(tmp(0) == falCurve4.getMinActiveFiberLength() &&
                       tmp(1) == falCurve4.getMaxActiveFiberLength());

        cout <<"    d. printMuscleCurveToCSVFile" << endl;
            falCurve4.printMuscleCurveToCSVFile("");
            std::string fname = falCurve4.getName();
            fname.append(".csv");
            remove(fname.c_str());

        cout << "Passed: Testing Services for connectivity" << endl;                            

        ///cout <<"**************************************************"<<endl;
        cout <<"Service correctness is tested by underlying utility class"<<endl;
        cout <<"MuscleCurveFunction, and MuscleCurveFunctionFactory"<<endl;
        //cout <<"**************************************************"<<endl;

        cout <<"**************************************************"<<endl;
        cout <<"          TESTING ActiveForceLengthCurve          "<<endl;
        cout <<"                    COMPLETED                     "<<endl;
        cout <<"**************************************************"<<endl;

}

void testForceVelocityCurve()
{
        
        cout <<"____________________________________________________"<<endl;
        cout <<"1. Testing ForceVelocityCurve"<<endl;       
        cout <<"____________________________________________________"<<endl;

        cout <<"    a. default construction" <<endl;
        ForceVelocityCurve fvCurve1;
        fvCurve1.print("default_ForceVelocityCurve.xml");

        cout <<"    b. serialization & deserialization" <<endl;
        ForceVelocityCurve fvCurve2;
        //change all of the properties to something other than the default
        fvCurve2.setConcentricCurviness(0.5);
        fvCurve2.setConcentricMinSlope(0);
        fvCurve2.setEccentricCurviness(0.5);
        fvCurve2.setEccentricMinSlope(0);
        fvCurve2.setMaxEccentricVelocityForceMultiplier(2.0);
        fvCurve2.setIsometricMaxSlope(10);      


        //These next few lines are just to read the object in, and repopulate
        //fvCurve2 with the properties from the file ... and its a little 
        //awkward to use.     
        
        Object* tmpObj = Object::          
                       makeObjectFromFile("default_ForceVelocityCurve.xml");
        fvCurve2 = *dynamic_cast<ForceVelocityCurve*>(tmpObj);
        
        delete tmpObj;

        SimTK_TEST(fvCurve2 == fvCurve1);       

        remove("default_ForceVelocityCurve.xml");
              
        fvCurve2.setConcentricCurviness(0.5);
        fvCurve2.setConcentricMinSlope(0);
        fvCurve2.setEccentricCurviness(0.5);
        fvCurve2.setEccentricMinSlope(0);
        fvCurve2.setMaxEccentricVelocityForceMultiplier(2.0);
        fvCurve2.setIsometricMaxSlope(10);

        cout <<"    c. assignment operator" <<endl;
        fvCurve2=fvCurve1;
        
        
        SimTK_TEST(fvCurve1==fvCurve2);

        fvCurve2.setConcentricCurviness(0.5);
        fvCurve2.setConcentricMinSlope(0);
        fvCurve2.setEccentricCurviness(0.5);
        fvCurve2.setEccentricMinSlope(0);
        fvCurve2.setMaxEccentricVelocityForceMultiplier(2.0);
        fvCurve2.setIsometricMaxSlope(10);

        cout <<"    d. copy constructor" <<endl;
        ForceVelocityCurve fvCurve2p5(fvCurve2);
        SimTK_TEST(fvCurve2==fvCurve2p5);

        cout << "*Passed: default construction, limited serialization" << endl;
        cout << "         assignment operator, copy constructor" << endl;

        //cout <<"**************************************************"<<endl;
        cout <<"2. Testing API constructor" << endl;
        ForceVelocityCurve fvCurve3(0,5,0,1.8,0.1,0.75,"testMuscle");
        double falVal  = fvCurve3.calcValue(1.0);
        double dfalVal = fvCurve3.calcDerivative(1.0,1);
        cout << "Passed: Testing API constructor" << endl;

        //cout <<"**************************************************"<<endl;
        cout <<"3. Testing get/set methods:" << endl;

        fvCurve2.setConcentricCurviness(0.5);
        fvCurve2.setConcentricMinSlope(0);
        fvCurve2.setEccentricCurviness(0.6);
        fvCurve2.setEccentricMinSlope(0.1);
        fvCurve2.setMaxEccentricVelocityForceMultiplier(2.0);
        fvCurve2.setIsometricMaxSlope(10);

        SimTK_TEST(fvCurve2.getConcentricCurviness()                    == 0.5);
        SimTK_TEST(fvCurve2.getConcentricMinSlope()                     == 0  );
        SimTK_TEST(fvCurve2.getEccentricCurviness()                     == 0.6);
        SimTK_TEST(fvCurve2.getEccentricMinSlope()                      == 0.1);
        SimTK_TEST(fvCurve2.getMaxEccentricVelocityForceMultiplier()    == 2.0);
        SimTK_TEST(fvCurve2.getIsometricMaxSlope()                      ==  10);

        cout << "Passed: Testing get/set methods" << endl;

        //cout <<"**************************************************"<<endl;
        cout <<"4. Testing Services for connectivity:" << endl;        
        ForceVelocityCurve fvCurve4;
        fvCurve4.setName("fvCurve");

        cout <<"    a. calcValue" << endl;
            double tol = sqrt(SimTK::Eps);
            double value = fvCurve4.calcValue(0);
            SimTK_TEST_EQ_TOL(value, 1, tol);
        cout <<"    b. calcDerivative" << endl;
            double dvalue= fvCurve4.calcDerivative(0,1);
            SimTK_TEST_EQ_TOL(dvalue, 5, tol);

        cout <<"    c. getCurveDomain" << endl;
            SimTK::Vec2 tmp = fvCurve4.getCurveDomain();
            SimTK_TEST(tmp(0) == -1.0 &&
                       tmp(1) == 1.0);

        cout <<"    d. printMuscleCurveToCSVFile" << endl;
            fvCurve4.setConcentricCurviness(0.5);
            fvCurve4.setEccentricCurviness(1.0);
            fvCurve4.printMuscleCurveToCSVFile("");
            std::string fname = fvCurve4.getName();
            fname.append(".csv");
            remove(fname.c_str());

        cout << "Passed: Testing Services for connectivity" << endl;                            

        //cout <<"**************************************************"<<endl;
        cout <<"Service correctness is tested by underlying utility class"<<endl;
        cout <<"MuscleCurveFunction, and MuscleCurveFunctionFactory"<<endl;
        //cout <<"**************************************************"<<endl;

        cout <<"**************************************************"<<endl;
        cout <<"          TESTING ForceVelocityCurve              "<<endl;
        cout <<"                    COMPLETED                     "<<endl;
        cout <<"**************************************************"<<endl;

}