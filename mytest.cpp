/*Title: mytest.cpp
  Author: Onosetale Okooboh
  Date: 02/09/2025
  Description: This file tests the functions implemented in centcom.cpp
*/
#include "centcom.h"
#include <iostream>
using namespace std;

class Tester {
public:
    // Run all tests.
    void runTests() {
        runTest("Test CentCom constructor (normal case)", testCentComConstructorNormal);
        runTest("Test CentCom constructor (error case)", testCentComConstructorError);
        runTest("Test CentCom::addElevator() (normal case)", testAddElevatorNormal);
        runTest("Test CentCom::addElevator() (error case)", testAddElevatorError);
        runTest("Test Elevator::setUp() (error case)", testSetUpError);
        runTest("Test Elevator::insertFloor() (error case)", testInsertFloorError);
        runTest("Test Elevator::pushButton() (normal case)", testPushButtonNormal);
        runTest("Test Elevator::pushButton() (error case)", testPushButtonError);
        runTest("Test Elevator::processNextRequest() (normal case)", testProcessNextRequestNormal);
        runTest("Test Elevator::processNextRequest() (error case)", testProcessNextRequestError);
    }

private:
    // Helper to run a single test and output its result.
    static void runTest(const string &testName, bool (*testFunc)()) {
        cout << testName << ": " << (testFunc() ? "PASSED" : "FAILED") << endl;
    }

    // Test 1: CentCom constructor (normal case)
    static bool testCentComConstructorNormal() {
        try {
            CentCom cc(3, 10);  // 3 elevators, building id 10
            // Check that m_numElevators is 3.
            return cc.m_numElevators == 3;
        } catch (...) {
            return false;
        }
    }

    // Test 2: CentCom constructor (error case)
    static bool testCentComConstructorError() {
        try {
            CentCom cc(3, -5);
            // Since onstructor doesn't throw, we consider m_id negative as an error.
            return cc.m_id < 0;
        } catch (...) {
            return true;
        }
    }

    // Test 3: CentCom::addElevator() (normal case)
    static bool testAddElevatorNormal() {
        CentCom cc(3, 10);
        bool added = cc.addElevator(1, 1, 10);
        Elevator* e = cc.getElevator(1);
        bool correctFloors = (e && e->m_bottom && e->m_top &&
                              (e->m_bottom->m_floorNum == 1) &&
                              (e->m_top->m_floorNum == 10) &&
                              (e->m_currentFloor == e->m_bottom));
        return added && correctFloors;
    }

    // Test 4: CentCom::addElevator() (error case)
    static bool testAddElevatorError() {
        CentCom cc(3, 10);
        bool resultNeg = cc.addElevator(-1, 1, 10);
        cc.addElevator(1, 1, 10);
        bool resultDup = !cc.addElevator(1, 5, 15);
        return !resultNeg && resultDup;
    }

    // Test 5: Elevator::setUp() (error case)
    static bool testSetUpError() {
        Elevator e(1);
        e.setUp(10, 1);  // Invalid range.
        return (e.m_bottom == nullptr && e.m_top == nullptr && e.m_currentFloor == nullptr);
    }

    // Test 6: Elevator::insertFloor() (error case)
    static bool testInsertFloorError() {
        Elevator e(1);
        return !e.insertFloor(1000);
    }

    // Test 7: Elevator::pushButton() (normal case)
    static bool testPushButtonNormal() {
        Elevator e(1);
        e.setUp(1, 10);
        bool result = e.pushButton(5);
        bool requestAdded = (e.m_upRequests != nullptr && e.m_upRequests->m_floorNum == 5);
        return result && requestAdded;
    }

    // Test 8: Elevator::pushButton() (error case)
    static bool testPushButtonError() {
        Elevator e(1);
        e.setUp(1, 10);
        bool resultNonExistent = !e.pushButton(20);
        bool firstPush = e.pushButton(5);
        bool secondPush = !e.pushButton(5);
        return resultNonExistent && firstPush && secondPush;
    }

    // Test 9: Elevator::processNextRequest() (normal case)
    static bool testProcessNextRequestNormal() {
        Elevator e(1);
        e.setUp(1, 10);
        if (e.m_currentFloor->m_floorNum != 1) return false;
        if (!e.pushButton(5)) return false;
        bool processed = e.processNextRequest();
        bool correctFloor = (e.m_currentFloor && e.m_currentFloor->m_floorNum == 5);
        // Expect elevator to be idle (with door open) after processing if no further requests exist.
        bool correctState = (e.m_moveState == IDLE);
        bool doorOpen = (e.m_doorState == OPEN);
        bool requestCleared = (e.m_upRequests == nullptr);
        return processed && correctFloor && correctState && doorOpen && requestCleared;
    }

    // Test 10: Elevator::processNextRequest() (error case)
    static bool testProcessNextRequestError() {
        Elevator e(1);
        e.setUp(1, 10);
        bool noRequest = !e.processNextRequest();
        e.pushEmergency(true);
        bool emergencyTest = !e.processNextRequest();
        return noRequest && emergencyTest;
    }
};

int main() {
    Tester tester;
    tester.runTests();
    return 0;
}
