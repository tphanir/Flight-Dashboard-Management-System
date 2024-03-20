# Flight-Dashboard-Management-System
Linked Lists Implementation

## Objective
To implement an in-air flight-management platform for an air-traffic control (ATC) system. It includes a digital dashboard that maintains in-air flights bucketed in 60-minute intervals based on expected time of arrival (ETA). The project includes functions to insert new flight-plans, cancel flight-plans for emergency situations, show status of a particular flight-plan, and show flight-plans in 1-hour time periods from a given time.
![image](https://github.com/tphanir/Flight-Dashboard-Management-System/assets/125972587/483d2357-f9f2-4afa-a54f-cd2d776e6526)

## Approach
1. Data Structures Used:
   * Circular Doubly Linked List for Buckets: This was chosen due to the dynamic nature of the bucket lists caused by the rearrangement of flight-plans in 1-hour periods.
   * Singly Linked List for Flights: Used to maintain the list of flight-plans within each bucket.
2. Additionally, a TIME structure is defined to represent time in the system. This structure consists of two integer fields, min for minutes and hour for hours, allowing for accurate representation and manipulation of time-related data within the digital dashboard for the ATC system.
```
typedef struct time_tag
{
    int min;
    int hour;
} TIME;
```
```
typedef struct flight_plan_node_tag
{
    int flightID;
    TIME departure;
    TIME ETA;
    struct flight_plan_node_tag *next;
} FlightPlanNode;
```
```
typedef struct bucket_node_tag
{
    FlightPlanNode *f;
    int bucketID;
    TIME beginningETA;
    TIME endETA;
    struct bucket_node_tag *next;
    struct bucket_node_tag *prev;
} BucketNode;
```
## Working

