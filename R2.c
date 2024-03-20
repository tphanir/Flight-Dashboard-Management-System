#include <stdio.h>
#include <stdlib.h>


/* ASSUMPTIONS */
// 1. TIME is in 24hrs format
// 2. User enters valid TIME



/* STATUS_CODE enum */
typedef enum{FAILURE, SUCCESS} STATUS_CODE;



// STRUCTURES

/* TIME structure */
typedef struct time_tag
{
    int min;
    int hour;
} TIME;

/* FlightPlanList_Node structure*/
typedef struct flight_plan_node_tag
{
    int flightID;
    TIME departure;
    TIME ETA;
    struct flight_plan_node_tag *next;
} FlightPlanNode;

/* BucketList_Node structure */
typedef struct bucket_node_tag
{
    FlightPlanNode *f;
    int bucketID;
    TIME beginningETA;
    TIME endETA;
    struct bucket_node_tag *next;
    struct bucket_node_tag *prev;
} BucketNode;



// HELPER FUNCTIONS

int maxtime(TIME A, TIME B)
{
    int bool;

    if(A.hour == 23 && B.hour == 0)
    {
        bool = 1;
    }
    else if(A.hour == 0 && B.hour == 23)
    {
        bool = -1;
    }

    else if(A.hour < B.hour)
    {
        bool = 1;
    }
    else if(A.hour == B.hour)
    {
        if(A.min == B.min)
        {
            bool = 0;
        }
        else if(A.min > B.min)
        {
            bool = -1;
        }
        else
        {
            bool = 1;
        }
    }
    else
    {
        bool = -1;
    }

    return bool;
}

int timedeff(TIME A, TIME B)
{
    int diff;
    diff = (A.hour - B.hour) * 60 + (A.min - B.min);
    return diff;
}

void printTime(TIME t)
{
  printf("%02d : %02d", t.hour, t.min);
}

void visitFlightPlanList(FlightPlanNode *flptr)
{
  FlightPlanNode *ptr = flptr;

  if(ptr == NULL)
  {
      printf("\nNo FLIGHT PLANS.\n");
  }
  else
  {
      while(ptr != NULL)
      {
          printf("\nFLIGHT ID - %d",ptr->flightID);
          printf("\nDEPARTURE TIME - ");
          printTime(ptr->departure);
          printf("\nETA %10s - ", "");
          printTime(ptr->ETA);
          ptr = ptr->next;
          printf("\n");
      }
  }
}

void visitBucketList(BucketNode *last)
{
  BucketNode *ptr = last->next;

  do
  {
      printf("\n---------------------------\n");
      printf("BUCKET ID : %d\n",ptr->bucketID);
      printf("\nBEG ETA INTERVAL - ");
      printTime(ptr->beginningETA);
      printf("\nEND ETA INTERVAL - ");
      printTime(ptr->endETA);
      printf("\n");
      visitFlightPlanList(ptr->f);
      ptr = ptr->next;
      printf("\n---------------------------\n");
      printf("\n");
  }
  while(ptr != last->next);

}

BucketNode* searchForBucket(BucketNode *last, FlightPlanNode *fptr)
{
    TIME flightETA;
    BucketNode *ptr;

    ptr = last;
    flightETA = fptr->ETA;

    while(!(timedeff(flightETA,ptr->beginningETA) >= 0 && timedeff(flightETA,ptr->beginningETA) <= 59))
    {
        ptr = ptr->next;
    }
    return ptr;
}

void insertFlightPlanInBucket(BucketNode *bptr, FlightPlanNode *fptr)
{
    FlightPlanNode *ptr;
    TIME flightDeparture;

    ptr = bptr->f;
    flightDeparture = fptr->departure;

    if(ptr == NULL)
    {
        bptr->f = fptr;
        fptr->next = NULL;
    }

    else
    {
        FlightPlanNode *prev = NULL;
        while(ptr != NULL && maxtime(ptr->departure, flightDeparture) == 1)
        {
            prev = ptr;
            ptr = ptr->next;
        }

        if(prev == NULL)
        {
            fptr->next = ptr;
            bptr->f = fptr;
        }
        else
        {
            fptr->next = ptr;
            prev->next = fptr;
        }
    }
}

void searchAndInsertFlightPlan(BucketNode *last, FlightPlanNode *fptr)
{

    BucketNode *bptr;

    bptr = searchForBucket(last, fptr);

    insertFlightPlanInBucket(bptr, fptr);
}

STATUS_CODE cancelFlightPlan(BucketNode *last, int flightID)
{
    STATUS_CODE sc;
    BucketNode *ptr;
    FlightPlanNode *curr, *prev;
    int found;

    found = 0;
    ptr = last;

    do
    {
        curr = ptr->f;
        prev = NULL;

        while(curr != NULL && !found)
        {
            if(curr->flightID == flightID)
            {
                found = 1;
            }
            else
            {
                prev = curr;
                curr = curr->next;
            }
        }
        if(!found)
        {
            ptr = ptr->next;
        }
    }
    while(ptr != last && !found);

    if(found)
    {
        sc = SUCCESS;
        if(prev == NULL)
        {
            ptr->f = curr->next;
        }
        else
        {
            prev->next = curr->next;
        }
        free(curr);
    }
    else
    {
        sc = FAILURE;
    }

    return sc;
}

STATUS_CODE flightPlanStatus(BucketNode *last, int flightID)
{
    BucketNode *ptr;
    FlightPlanNode *curr, *prev;
    STATUS_CODE sc;
    int found;

    found = 0;
    ptr = last;

    do
    {
        curr = ptr->f;
        prev = NULL;

        while(curr != NULL && !found)
        {
            if(curr->flightID == flightID)
            {
                found = 1;
            }
            else
            {
                prev = curr;
                curr = curr->next;
            }
        }
        if(!found)
        {
            ptr = ptr->next;
        }
    }
    while(ptr != last && !found);

    if(found)
    {
        sc = SUCCESS;
        printf("\nFLIGHT ID : %d", curr->flightID);
        printf("\nDeparture Time - ");
        printTime(curr->departure);
        printf("\nETA - ");
        printTime(curr->ETA);
        printf("\n");
    }
    else
    {
        sc = FAILURE;
    }

    return sc;
}

BucketNode* readBucket()
{
    FILE *fptr;
    BucketNode *dummy, *nptr, *result;

    fptr = fopen("BucketList.dat", "r");

    result = dummy = (BucketNode *)malloc(sizeof(BucketNode));

    while(!feof(fptr))
    {
        nptr = (BucketNode *)malloc(sizeof(BucketNode));
        nptr->f = NULL;
        fscanf(fptr, "%d, %d, %d, %d, %d", &nptr->bucketID, &nptr->beginningETA.hour, &nptr->beginningETA.min,
                                     &nptr->endETA.hour, &nptr->endETA.min);
        dummy->next = nptr;
        nptr->prev = dummy;
        dummy = nptr;
    }
    nptr = result;
    result = result->next;
    dummy->next = result;
    result->prev = dummy;
    free(nptr);
    return dummy; //dummy == last
}

void readFlightPlanIntoBucket(BucketNode *last)
{
    FILE *fptr;
    fptr = fopen("FlightPlans.dat", "r");

    while(!feof(fptr))
    {
        FlightPlanNode *nptr = (FlightPlanNode*)malloc(sizeof(FlightPlanNode));
        fscanf(fptr, "%d, %d, %d, %d, %d",
               &nptr->flightID, &nptr->departure.hour, &nptr->departure.min, &nptr->ETA.hour, &nptr->ETA.min);
        searchAndInsertFlightPlan(last, nptr);
    }

    fclose(fptr);
}

void changeTimings(BucketNode **last, TIME t)
{

    BucketNode *ptr, *checkpoint;
    FlightPlanNode *fptr, *curr, *prev, *nptr;
    int counter;

    counter = 0;
    ptr = *last;

    while(!(maxtime(ptr->beginningETA, t) >= 0 && maxtime(ptr->endETA, t) <= 0))
    {
        ptr = ptr->next;
    }

    if(timedeff(t, ptr->beginningETA) > 30)
    {
        ptr = ptr->next;
    }

    checkpoint = ptr->prev;

    do
    {
        ptr->beginningETA.hour = (t.hour + counter) % 24;
        ptr->beginningETA.min = t.min;
        ptr->endETA.min = (t.min + 59);
        if(ptr->endETA.min >= 60)
        {
            ptr->endETA.min %= 60;
            ptr->endETA.hour = (ptr->beginningETA.hour + 1) % 24;
        }
        counter += 1;
        ptr = ptr->next;
    }
    while(ptr != checkpoint->next);

    ptr = *last;

    do
    {
        fptr = ptr->f;
        prev = NULL;
        curr = fptr;

        while(curr != NULL)
        {
            if(maxtime(ptr->endETA, curr->ETA) == 1)
            {
                if(prev == NULL)
                {
                    nptr = curr;
                    curr = curr->next;
                    ptr->f = curr;
                }
                else
                {
                    nptr = curr;
                    prev->next = curr->next;
                    curr = curr->next;
                }
                insertFlightPlanInBucket(ptr->next, nptr);
            }
            else if(maxtime(ptr->beginningETA, curr->ETA) == -1)
            {

                if(prev == NULL)
                {
                    nptr = curr;
                    curr = curr->next;
                    ptr->f = curr;
                }
                else
                {
                    nptr = curr;
                    prev->next = curr->next;
                    curr = curr->next;
                }
                insertFlightPlanInBucket(ptr->prev, nptr);
            }
            else
            {
                prev = curr;
                curr = curr->next;
            }
        }
        ptr = ptr->next;
    }
    while(ptr != *last);

    *last = checkpoint;
}



// DRIVER FUNCTION

void driver(BucketNode *last)
{
    int done = 0;
    int input;

    printf("---------------------------------------------------------");
    printf("\n%43s\n", "WELCOME TO DIGITAL DASHBOARD");
    printf("---------------------------------------------------------\n");
    printf("\nCHOOSE from the following - \n");
    printf("\n1. INSERT NEW FLIGHT PLAN");
    printf("\n2. SHOW STATUS OF A FLIGHT PLAN");
    printf("\n3. CANCEL FLIGHT PLAN");
    printf("\n4. UPDATE BUCKETLIST");
    printf("\n5. SHOW ENTIRE DASHBOARD");
    printf("\n6. EXIT\n");
    printf("\n---------------------------------------------------------\n");

    while(!done)
    {
        printf("Enter chosen OPTION number : ");
        scanf("%d", &input);
        printf("\n");
        if(input == 1)
        {
            FlightPlanNode *nptr;
            nptr = (FlightPlanNode *)malloc(sizeof(FlightPlanNode));

            printf("Enter FLIGHT ID : ");
            scanf("%d", &nptr->flightID);
            printf("Enter DEPARTURE TIME : ");
            scanf("%d %d", &nptr->departure.hour, &nptr->departure.min);
            printf("Enter ETA : ");
            scanf("%d %d", &nptr->ETA.hour, &nptr->ETA.min);
            searchAndInsertFlightPlan(last, nptr);

            printf("FLIGHT PLAN INSERTION was successful.\n");
            printf("OPERATION STATUS : SUCCESSFUL\n");
        }
        else if(input == 2)
        {
            int flightID;
            printf("Enter FLIGHT ID : ");
            scanf("%d", &flightID);

            if(!flightPlanStatus(last, flightID))
            {
                printf("\nFLIGHT PLAN with given FLIGHT ID not found.\n");
                printf("OPERATION STATUS : FAILED\n");
            }
            else
            {
                printf("\nOPERATION STATUS : SUCCESSFUL\n");
            }
        }
        else if(input == 3)
        {
            int flightID;

            printf("Enter FLIGHT ID : ");
            scanf("%d", &flightID);

            if(!cancelFlightPlan(last, flightID))
            {

                printf("FLIGHT PLAN with given FLIGHT ID not found.\n");
                printf("\nFLIGHT PLAN CANCELLATION was unsuccessful.");
                printf("\nOPERATION STATUS : FAILED\n");
            }
            else
            {
                printf("\nFLIGHT PLAN CANCELLATION was successful.");
                printf("\nOPERATION STATUS : SUCCESSFUL\n");
            }
        }
        else if(input == 4)
        {
            TIME X;
            printf("Enter time : ");
            scanf("%d %d", &X.hour, &X.min);
            changeTimings(&last, X);
            printf("\nOPERATION STATUS : SUCCESSFUL\n");
        }
        else if(input == 5)
        {
            visitBucketList(last);
        }
        else if(input == 6)
        {
            done = 1;
            printf("---------------------------------------------------------\n");
            printf("%30s\n", "THANK YOU");
            printf("---------------------------------------------------------\n");
        }
        else
        {
            printf("INVALID OPTION SELECTED!\n");
            printf("Choose a valid OPTION.\n");
        }
        if(done != 1) {
            printf("\n---------------------------------------------------------\n");
            printf("\n1. INSERT NEW FLIGHT PLAN");
            printf("\n2. SHOW STATUS OF A FLIGHT PLAN");
            printf("\n3. CANCEL FLIGHT PLAN");
            printf("\n4. UPDATE BUCKETLIST");
            printf("\n5. SHOW ENTIRE DASHBOARD");
            printf("\n6. EXIT\n");
            printf("\n---------------------------------------------------------\n");
        }
    }
}



// MAIN FUNCTION

void main()
{
    BucketNode *last;
    last = readBucket();
    readFlightPlanIntoBucket(last);
    driver(last);
}
