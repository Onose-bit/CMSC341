/*Title: course.cpp
  Author: Onosetale Okooboh
  Date: 02/09/2025
  Description: This file implements the functions in centcom.h dictating the operations of an elevator
*/
#include "centcom.h"

CentCom::CentCom(int numElevators, int buildingID) {
    m_id = buildingID;
    m_numElevators = numElevators;
    m_elevatorsList = new Elevator*[numElevators];
    for (int i = 0; i < numElevators; i++) {
        m_elevatorsList[i] = nullptr;
    }
}

CentCom::~CentCom() {
    for (int i = 0; i < m_numElevators; i++) {
        delete m_elevatorsList[i];
    }
    delete[] m_elevatorsList;
}

bool CentCom::addElevator(int ID, int bottomFloor, int topFloor) {
    if (ID < 0 || ID >= m_numElevators || m_elevatorsList[ID] != nullptr)
        return false;
    m_elevatorsList[ID] = new Elevator(ID);
    m_elevatorsList[ID]->setUp(bottomFloor, topFloor);
    return true;
}

Elevator* CentCom::getElevator(int ID) {
    if (ID < 0 || ID >= m_numElevators)
        return nullptr;
    return m_elevatorsList[ID];
}

bool CentCom::setSecure(int ID, int floorNum, bool yes_no) {
    Elevator* elevator = getElevator(ID);
    if (!elevator){ 
        return false;
    }

    // Set security status on the matching floor in the main floor list.
    Floor* temp = elevator->m_bottom;
    while (temp) {
        if (temp->m_floorNum == floorNum) {
            temp->m_secured = yes_no;
            return true;
        }
        temp = temp->m_next;
    }
    return false;
}

bool CentCom::clearEmergency(int ID) {
    Elevator* elevator = getElevator(ID);
    if (!elevator)
        return false;
    elevator->pushEmergency(false);  // Clears emergency
    return elevator->processNextRequest();  // Resume processing requests
}

//-------------------- Elevator Class --------------------

Elevator::Elevator(int ID) {
    m_id = ID;
    m_bottom = nullptr;
    m_top = nullptr;
    m_upRequests = nullptr;
    m_downRequests = nullptr;
    m_currentFloor = nullptr;
    m_moveState = IDLE;
    m_doorState = CLOSED;
    m_emergency = false;
    m_load = 0;
    m_lastMoveState = IDLE; // Remember last direction
}

Elevator::~Elevator() {
    clear();
}

void Elevator::clear() {
    // Delete main floor list
    Floor* temp = m_bottom;
    while (temp) {
        Floor* next = temp->m_next;
        delete temp;
        temp = next;
    }
    m_bottom = m_top = m_currentFloor = nullptr;
    
    // Clear request lists
    while (m_upRequests) {
        Floor* next = m_upRequests->m_next;
        delete m_upRequests;
        m_upRequests = next;
    }
    while (m_downRequests) {
        Floor* next = m_downRequests->m_next;
        delete m_downRequests;
        m_downRequests = next;
    }
}

/* 
   setUp: Clears any existing floors and then inserts floors.
   To satisfy the rule "insert nodes at the head", we iterate from lastFloor down to firstFloor.
   This way, each new floor is inserted at the head and the final order is ascending.
*/
void Elevator::setUp(int firstFloor, int lastFloor) {
    if (firstFloor > lastFloor)
        return;
    
    clear(); // Remove existing floors and requests

    // Insert floors from lastFloor down to firstFloor.
    for (int i = lastFloor; i >= firstFloor; i--) {
        insertFloor(i);
    }
    
    m_currentFloor = m_bottom; // Start at the lowest floor.
}

/*
   insertFloor: Inserts a new floor node at the head of the main floor list.
   (This meets the requirement, and due to setUp iterating in descending order, the list is sorted ascending.)
*/
bool Elevator::insertFloor(int floor) {
    const int MIN_FLOOR = -10;
    const int MAX_FLOOR = 100;
    if (floor < MIN_FLOOR || floor > MAX_FLOOR)
        return false;
    Floor* newFloor = new Floor(floor);
    
    // Insert at head
    newFloor->m_next = m_bottom;
    if (m_bottom) {
        m_bottom->m_previous = newFloor;
    } else {
        m_top = newFloor;  // If list was empty, newFloor is also the top.
    }
    m_bottom = newFloor;
    return true;
}

/*
   checkSecure: Searches the main floor list for the given floor number and returns its security status.
*/
bool Elevator::checkSecure(int floor) {
    Floor* temp = m_bottom;
    while (temp) {
        if (temp->m_floorNum == floor)
            return temp->m_secured;
        temp = temp->m_next;
    }
    return false;
}

/*
   pushEmergency: Sets m_emergency to true if pushed; clearing emergency is done by pushEmergency(false).
*/
void Elevator::pushEmergency(bool pushed) {
    if (pushed)
        m_emergency = true;
    else
        m_emergency = false;
}

/*
   pushButton: Registers a floor request.
   It first validates that the floor exists in the main floor list,
   rejects secured floors, and avoids duplicate requests.
   The request is added to the appropriate request list (up or down) based on m_currentFloor.
*/
bool Elevator::pushButton(int floor) {
    // Validate that the floor exists in the main floor list.
    Floor* temp = m_bottom;
    while (temp) {
        if (temp->m_floorNum == floor)
            break;
        temp = temp->m_next;
    }
    if (!temp)
        return false; // Floor does not exist.

    if (!m_currentFloor)
        return false; // Safety check for null current floor.

    // Reject requests for secured floors.
    if (checkSecure(floor))
        return false;

    // Determine which request list to use (UP or DOWN).
    Floor** requestList = (floor > m_currentFloor->m_floorNum) ? &m_upRequests : &m_downRequests;

    // Check for duplicate requests before adding.
    temp = *requestList;
    while (temp) {
        if (temp->m_floorNum == floor)
            return false; // Request already exists.
        temp = temp->m_next;
    }

    // Create new request node.
    Floor* newRequest = new Floor(floor);

    // Insert the new request in sorted order:
    if (!*requestList) {
        // If the list is empty, newRequest becomes the head.
        *requestList = newRequest;
    } else {
        Floor* curr = *requestList;
        Floor* prev = nullptr;
        if (floor > m_currentFloor->m_floorNum) {
            // UP requests: sort in ascending order.
            while (curr && curr->m_floorNum < floor) {
                prev = curr;
                curr = curr->m_next;
            }
        } else {
            // DOWN requests: sort in descending order.
            while (curr && curr->m_floorNum > floor) {
                prev = curr;
                curr = curr->m_next;
            }
        }
        if (!prev) {
            // Insert at the beginning.
            newRequest->m_next = *requestList;
            (*requestList)->m_previous = newRequest;
            *requestList = newRequest;
        } else {
            // Insert between prev and curr.
            newRequest->m_next = curr;
            newRequest->m_previous = prev;
            prev->m_next = newRequest;
            if (curr) {
                curr->m_previous = newRequest;
            }
        }
    }

    // If elevator is idle, set move state and close door (since we're starting to move).
    if (m_moveState == IDLE) {
        m_moveState = (floor > m_currentFloor->m_floorNum) ? UP : DOWN;
        m_doorState = CLOSED;
    }

    return true;
}

// Returns the UP request with the smallest floor number greater than m_currentFloor.
Floor* Elevator::getNextUpRequest() {
    Floor* candidate = nullptr;
    Floor* temp = m_upRequests;
    while (temp) {
        if (temp->m_floorNum > m_currentFloor->m_floorNum) {
            if (!candidate || temp->m_floorNum < candidate->m_floorNum) {
                candidate = temp;
            }
        }
        temp = temp->m_next;
    }
    return candidate;
}

// Returns the DOWN request with the largest floor number less than m_currentFloor.
Floor* Elevator::getNextDownRequest() {
    Floor* candidate = nullptr;
    Floor* temp = m_downRequests;
    while (temp) {
        if (temp->m_floorNum < m_currentFloor->m_floorNum) {
            if (!candidate || temp->m_floorNum > candidate->m_floorNum) {
                candidate = temp;
            }
        }
        temp = temp->m_next;
    }
    return candidate;
}

// Removes the specified candidate node from the given request list.
void Elevator::removeRequestFromList(Floor* candidate, Floor*& list) {
    if (!candidate) return;
    if (list == candidate) {
        list = candidate->m_next;
        if (list) list->m_previous = nullptr;
    } else {
        if (candidate->m_previous) {
            candidate->m_previous->m_next = candidate->m_next;
        }
        if (candidate->m_next) {
            candidate->m_next->m_previous = candidate->m_previous;
        }
    }
    delete candidate;
}

// Traverses the main floor list to set m_currentFloor to the node whose m_floorNum equals targetFloor.
void Elevator::updateCurrentFloor(int targetFloor) {
    Floor* temp = m_bottom;
    while (temp) {
        if (temp->m_floorNum == targetFloor) {
            m_currentFloor = temp;
            break;
        }
        temp = temp->m_next;
    }
}
/*
   processNextRequest: Processes the next floor request.
   It checks emergency and overload conditions.
   It then selects the next target floor based on the current direction (UP if available; otherwise, DOWN).
   The elevator's current floor is updated by searching the main floor list.
   The move state is maintained until all requests in the original direction are processed.
   The door is opened (m_doorState set to OPEN) when idle.
*/
bool Elevator::processNextRequest() {
    if (m_emergency)
        return false;  // Do not process if emergency is active
    if (m_load > LOADLIMIT)
        return false;  // Do not move if overloaded

    // If no requests, set idle and open door.
    if (!m_upRequests && !m_downRequests) {
        m_moveState = IDLE;
        m_doorState = OPEN;
        return false;
    }
    
    // If currently idle, decide direction based on available requests.
    if (m_moveState == IDLE) {
        if (m_upRequests)
            m_moveState = UP;
        else if (m_downRequests)
            m_moveState = DOWN;
        m_lastMoveState = m_moveState;
        m_doorState = CLOSED;  // Close door when starting to move.
    }
    
    // Process UP direction
    if (m_moveState == UP) {
        Floor* candidate = getNextUpRequest();
        if (!candidate) {
            // No valid UP request; if there are DOWN requests, switch direction.
            if (m_downRequests) {
                m_moveState = DOWN;
                m_lastMoveState = DOWN;
            } else {
                m_moveState = IDLE;
                m_doorState = OPEN;
                return false;
            }
        } else {
            int targetFloor = candidate->m_floorNum;
            removeRequestFromList(candidate, m_upRequests);
            updateCurrentFloor(targetFloor);
            // After processing, if no further requests remain, set idle.
            if (!m_upRequests && !m_downRequests) {
                m_moveState = IDLE;
                m_doorState = OPEN;
            }
            return true;
        }
    }
    
    // Process DOWN direction
    if (m_moveState == DOWN) {
        Floor* candidate = getNextDownRequest();
        if (!candidate) {
            // No valid DOWN request; if there are UP requests, switch direction.
            if (m_upRequests) {
                m_moveState = UP;
                m_lastMoveState = UP;
            } else {
                m_moveState = IDLE;
                m_doorState = OPEN;
                return false;
            }
        } else {
            int targetFloor = candidate->m_floorNum;
            removeRequestFromList(candidate, m_downRequests);
            updateCurrentFloor(targetFloor);
            if (!m_upRequests && !m_downRequests) {
                m_moveState = IDLE;
                m_doorState = OPEN;
            }
            return true;
        }
    }
    
    return false;
}

void Elevator::enter(int load) {
    if (m_load + load <= LOADLIMIT)
        m_load += load;
}

int Elevator::exit(int load) {
    m_load -= load;
    if (m_load < 0)
        m_load = 0;
    return m_load;
}
// Debugging function to print elevator status
void Elevator::dump(){
    if (m_moveState == IDLE) cout << "Elevator " << m_id << " is idle.";
    else if (m_moveState == UP) cout << "Elevator " << m_id << " is moving up.";
    else if (m_moveState == DOWN) cout << "Elevator " << m_id << " is moving down.";
    cout << endl;
    if (m_emergency == true) cout << "someone pushed the emergency button!" << endl;
    if (m_top != nullptr){
        Floor *temp = m_top;
        cout << "Top" << endl;
        while(temp->m_previous != nullptr){
            cout << temp->m_floorNum;
            if (temp->m_floorNum == m_currentFloor->m_floorNum) cout << " current ";
            if (temp->m_secured == true) cout << " secured ";
            cout << endl;
            temp = temp->m_previous;
        }
        cout << temp->m_floorNum;
        if (temp->m_floorNum == m_currentFloor->m_floorNum) cout << " current ";
        if (temp->m_secured == true) cout << " secured ";
        cout << endl;
        cout << "Bottom" << endl;
    }
}
