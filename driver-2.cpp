// UMBC - CMSC 341 - Spring 2025 - Proj1
#include "centcom.h"
int main(){
    /*********************Scenario 1*********************************/
    Elevator anElev(10);
    anElev.setUp(0,12);
    anElev.pushButton(8);
    anElev.dump();// here must move up
    cout << "\n=====================\n" << endl;
    anElev.processNextRequest();
    anElev.dump();// here must be idle and current should be 8
    cout << "\n=====================\n" << endl; 
    anElev.pushButton(2);
    anElev.dump();// here must move down
    cout << "\n=====================\n" << endl;
    anElev.processNextRequest();
    anElev.dump();// here must be idle and current should be 2
    /****************************************************************/
    /*********************Scenario 2*********************************
    CentCom controller(10);
    controller.addElevator(6,-5,12);// set up elevator 6 with floors -5 to 12
    controller.setSecure(6,4,true);
    Elevator * anElev = controller.getElevator(6);
    anElev->dump();// it is idle and current is -5
    cout << "\n=====================\n" << endl;
    anElev->pushButton(2);
    anElev->processNextRequest();
    anElev->dump();// Elevator 6 is idle and current is 2
    cout << "\n=====================\n" << endl;
    anElev->pushButton(4);// this floor is secured
    anElev->processNextRequest();
    anElev->dump();// Elevator 6 is idle, it ignores 4
    cout << "\n=====================\n" << endl;
    anElev->pushButton(12);
    anElev->processNextRequest();
    anElev->dump();// it is idle and current is 12
    ***************************************************************/
   /**********************Scenario 3********************************
    CentCom controller(10);
    controller.addElevator(6,-5,12);// set up elevator 6 with floors -5 to 12
    controller.setSecure(6,4,true);
    Elevator * anElev = controller.getElevator(6);
    anElev->dump();// it is idle and current is -5
    cout << "\n=====================\n" << endl;
    anElev->pushButton(2);
    anElev->pushButton(4);// this floor is secured
    anElev->pushButton(12);
    anElev->processNextRequest();
    anElev->dump();// it is moving up and current is 2
    cout << "\n=====================\n" << endl;
    anElev->processNextRequest();
    anElev->dump();// it is idle and current is 12, it ignores 4
    cout << "\n=====================\n" << endl;
    anElev->processNextRequest();// nothing should happen here
    anElev->dump();// it is idle and current is 12
    ***************************************************************/
   /**********************Scenario 4********************************
    CentCom controller(10);
    controller.addElevator(4,0,12);// set up elevator 4 with floors 0 to 12
    Elevator * anElev = controller.getElevator(4);
    anElev->dump();// it is idle and current is 0
    cout << "\n=====================\n" << endl;
    anElev->pushButton(12);
    anElev->pushButton(8);
    anElev->processNextRequest();
    anElev->dump();// it is moving up and current is 8
    cout << "\n=====================\n" << endl;
    anElev->pushButton(4);
    anElev->processNextRequest();
    anElev->dump();// current is 12, it changes direction to move down
    cout << "\n=====================\n" << endl;
    anElev->pushButton(10);
    anElev->processNextRequest();
    anElev->dump();// it is moving down and current is 10
    cout << "\n=====================\n" << endl;
    anElev->processNextRequest();
    anElev->dump();// it is idle and current is 4
    ***************************************************************/
    return 0;
}