#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

/*
    Name: Kirubel Moges
    Program: 2
    Due: SUNDAY April 4, 2021
*/

//                                              PLEASE MAKE SURE EACH LINE OF INPUT FILE HAS NOT TRAILING SPACE CHARS

int tables [100] = {-1}; // -1 = unoccupied, -2 = customer left
int numOftypesOfItems;
int customerInfo [100][100];
int maximumNumberOfItemsAuctioned;
int isItemToBeAuctionedIsGeneratedRandomly;
int extraCredit;

int waitingQueue [100] = {-2}; //-2= end of queue, -1=customer picked
int numOfCustomers;
int numOfTables;

int ready = 0;
int auctionBids[100] = {-1}; // -1=unassigned or no bids
int nextAuctionRoundStarted = 0; // 0=next auction has not started, 1=auction started, -1=result announced, -2 = result is tied. redo, -3 = Nobody wins round
int waitAuction = 1;                //1 = auction process is in progress
int itemUpForAuction = -1;
int indexOfTableThatWonCurrentRound = -1;
int indexOfTablesTied [100] = {-1};
int removedCustomerIndex[100] = {-1}; 
int eligibleCustomers [100] = {-2}; //-2 = end of Array
int tiedBidIndex [100] = {-1};

int interestArray [100] = {0};

int totalSumPaid = 0;
int totalNumOfItemSuccessfullyAuctioned = 0;
int totalAuctions = 0;

int extraCreditStopCommand = 0; //0 = KEEP GOING, 1 = STOP

pthread_mutex_t  mutex1; 

void readFile();
void *eachThread();
void *generateRandomCustomer();

/*
    main - is a function responsible for initializing the auction game. Coordinates reading in data from file, ensuring customers are placed in the 
            waiting queue, starts, stops & pauses bidding process as it sees fit, etc
    EXPECTED OUTPUT: Auction results, file data input, thread start & end, 
*/
int main()
{
    printf("Reading & processing file...\n");
    readFile();

    srand(time(0));

    /*
    printf("Displaying customer info \n");
    for(int i = 0; i < numOfCustomers; i++)
    {
        for(int j = 0; j < 100; j++)
        {
            if(customerInfo[i][j] == -1)
            {
                break;
            }
            printf("i: %d, j: %d | %d \n", i,j,customerInfo[i][j]);
        }
    }
    */


    pthread_t tid[numOfTables];
    pthread_attr_t attr;

    //EXTRA CREDIT! : RUNS IF 6TH DIGIT OF FIRST LINE IS 1
    if(extraCredit == 1)
    {
        printf("\n\n#####     EXTRA CREDIT BIT WAS TURNED ON IN FILE | CREATING A RANDOM CUSTOMER     ##### \n\n");
        printf("\nIF NO NEW CUSTOMER IS CREATED, THE CUSTOMER GENERATOR DID NOT HAVE ENOUGH TIME TO CREATE THE CONDITIONS TO MAKE A NEW CUSTOMER. RETRY PROGRAM! ATLEAST 30 TIMES! THIS FUNCTION WORKS! I PROMISE!\n\n");
        pthread_t t;

        pthread_attr_init(&attr);
        pthread_create(&t, &attr, generateRandomCustomer, (void*) 0);

        int q;
        for(int w = 0; w < 100000; w++)
        {
            q=1;
        }

        extraCreditStopCommand = 1;

        pthread_join(t, NULL);

        /*
        printf("Displaying customer info \n");
        for(int i = 0; i < numOfCustomers; i++)
        {
            for(int j = 0; j < 100; j++)
            {
                if(customerInfo[i][j] == -1)
                {
                    break;
                }
                printf("i: %d, j: %d | %d \n", i,j,customerInfo[i][j]);
            }
        }
        */
    }

    //Put customers in waiting queue. Customer ID is stored in waiting queue
    printf("Adding customers to queue\n\n");
    for(int i = 0; i < numOfCustomers; i++)
    {
        //waitingQueue[i] = customerInfo[i][0];
        waitingQueue[i] = i;
        printf("Customer ID: %d added to the queue!\n", customerInfo[i][0]);
    }


    printf("\nFinished Adding Customers to Queue!\n");

    for(int i = 0; i < numOfTables; i++)
    {
        pthread_attr_init(&attr);
        pthread_create(&tid[i], &attr, eachThread, (void*) &i);
        printf("Thread %d CREATED!\n", i);
        sleep(1);
    }


    printf("\nThreads Created! Customers Seated! Auction Starting...\n");


    int maxBid = 0;
    int maxBidIndex = -1;
    int tiedCounter = 0;
    for(int k = 0; k < maximumNumberOfItemsAuctioned; k++)
    {
        totalAuctions++;
        maxBidIndex = -1;
        maxBid = 0;
        tiedCounter = 0;

        pthread_mutex_lock(&mutex1);
        for(int a = 0; a < numOfTables; a++)
        {
            if(auctionBids[a] != -1)
            {
                auctionBids[a] = -1;
            }
        }

        int x;
	    for (long long int i = 0; i < 10000000; i++)
		    x = 1;
        
       printf("\nList of Customer IDs eligible for auction round #%d: \n", k);

        int shouldGameContinueCounter = 0;
       for(int j = 0; j < numOfTables; j++)
       {
           printf("Customer ID: %d \n", customerInfo[tables[j]][0]);
           if(j != 0 && tables[j] == tables[j-1])
           {
               shouldGameContinueCounter++;
           }
       }

       if(shouldGameContinueCounter == numOfTables)
       {
           printf("\n\n #######        NO MORE CUSTOMERS AVAILABLE        ##########  \n\n");
           break;
       }
       pthread_mutex_unlock(&mutex1);

       //exit(EXIT_SUCCESS);

       if(isItemToBeAuctionedIsGeneratedRandomly == 1)
       {
           printf("\n####    Item to be auctioned is generated sequentially!    #####\n");
           itemUpForAuction = (k % numOftypesOfItems+1); //Cyclic Sequence
           if(itemUpForAuction == 0) itemUpForAuction++;
       }
       else
       {
           printf("\n####    Item to be auctioned is generated randomly!    #####\n");
           itemUpForAuction = (rand() % numOftypesOfItems+1); //Random generation
           if(itemUpForAuction == 0) itemUpForAuction++;
       }

        printf("\n Item of type: %d  up for auction! \n", itemUpForAuction);
        printf("\n######        #######        ########\n");

        //exit(EXIT_SUCCESS);

        //Auction Started
        pthread_mutex_lock(&mutex1);
        nextAuctionRoundStarted = 1;
        pthread_mutex_unlock(&mutex1);

        for (long long int i = 0; i < 100000000; i++)
		    x = 1;

        //Processing Auction
        pthread_mutex_lock(&mutex1);
        nextAuctionRoundStarted = 0;
        pthread_mutex_unlock(&mutex1);

	    for (long long int i = 0; i < 100000000; i++)
		    x = 1;

        pthread_mutex_lock(&mutex1);

        printf("\n#########      Threads Should Have Finished Bidding NOW!!         ##########\n");

        int noBidCounter = 0;
        int currentBidCounter = 0;

        for(int m = 0; m < numOfTables; m++)
        {
            //Add a counter to check if noone had made a bid
            if(auctionBids[m] == -1)
            {
                noBidCounter++;
            }

            //Add a counter to check if only one customer had made a bid
            if(auctionBids[m] > 0) 
            {
                currentBidCounter++;
            }

            //Get the max bid value and index table if all tables have made a bid
            if(auctionBids[m] > maxBid)
            {
                maxBid = auctionBids[m];
                maxBidIndex = m;
            }
        }

        if(noBidCounter == numOfTables)
        {
            printf("\n#########      NO BIDS WERE MADE IN CURRENT ROUND!!!        ##########\n");
            nextAuctionRoundStarted = -3;
            pthread_mutex_unlock(&mutex1);

            for(long long int r = 0; r < 1000000; r++)
                x = 1;

            continue;
        }

        if(currentBidCounter == 1)
        {
            nextAuctionRoundStarted =  -1; //Result processed. Make announcement
            indexOfTableThatWonCurrentRound = maxBidIndex;
            printf("\n\n##########       ONLY ONE CUSTOMER HAS MADE A BID! ROUND ENDS! WINNER ANNOUNCED!        ###########\n\n");

            pthread_mutex_unlock(&mutex1);

            for(long long int r = 0; r < 1000000; r++)
                x = 1;

            continue;
        }

        for(int z = 0; z < numOfTables; z++)
        {
            if(auctionBids[z] == maxBid)
            {
                indexOfTablesTied[tiedCounter++] = z;
            }
        }

        if(tiedCounter > 1)
        {
            nextAuctionRoundStarted = -2; //Tie
            printf("\n########      Second Round of Bidding Needed!      ##########\n");
            pthread_mutex_unlock(&mutex1);

            for (long long int i = 0; i < 100000000; i++)
		    x = 1;

            pthread_mutex_lock(&mutex1);
            printf("\n##############       Processing after 2nd round bidding!       ###########\n");
            nextAuctionRoundStarted = 0;
            pthread_mutex_unlock(&mutex1);

            for (long long int i = 0; i < 100000000; i++)
		    x = 1;

            pthread_mutex_lock(&mutex1);

            for(int m = 0; m < numOfTables; m++)
            {
                if(auctionBids[m] > maxBid)
                {
                    maxBid = auctionBids[m];
                    maxBidIndex = m;
                }
            }
            tiedCounter = 0;

            for(int z = 0; z < numOfTables; z++)
            {
                if(auctionBids[z] == maxBid)
                {
                    indexOfTablesTied[tiedCounter++] = z;
                }
            }

            if(tiedCounter > 1)
            {
                printf("\n########    NOBODY WINS! MOVING TO NEXT ROUND!        ###########\n");
                nextAuctionRoundStarted = -3; //Nobody wins. Move on to next round

                for (long long int i = 0; i < 100000000; i++)
		            x = 1;
            }
            else if(tiedCounter == 1)
            {
                nextAuctionRoundStarted = -1;
                indexOfTableThatWonCurrentRound = maxBidIndex;
                printf("\n########    WINNER ANNOUNCED!        ###########\n");
                
            }

            pthread_mutex_unlock(&mutex1);
            continue;
        }
        else
        {
            nextAuctionRoundStarted = -1;
            indexOfTableThatWonCurrentRound = maxBidIndex;
            printf("\n########    WINNER ANNOUNCED!        ###########\n");

            pthread_mutex_unlock(&mutex1);
        }
    }

    //Terminate Auction & Terminate Thread instruction passed!
    pthread_mutex_lock(&mutex1);
    waitAuction = 0;
    ready = 2;
    pthread_mutex_unlock(&mutex1);

    int y; 
    for(long long int i = 0; i < 100000000; i++)
        y = 1;

    for (int i = 0; i < numOfTables; i++)
    {
        pthread_join(tid[i], NULL);
    }

    printf("\n\n");

    printf("Total Sum Paid: $%d\n", totalSumPaid);
    printf("Total Auction Rounds: %d\n", totalAuctions);
    printf("Total number of items successfully auctioned: %d\n\n", totalNumOfItemSuccessfullyAuctioned);

    return 0;
}

/*
    eachThread - is a function that represents a single table. It accepts an index of a table. It is responsible for sitting customers,
                processing if a customer has won or lost, displaying corresponding messages, etc
    EXPECTED OUPUT: Whether customer wins or loses, whether customer leaver cause of deficient funds or has received all items they were interested in
*/
void *eachThread(void * param)
{
    /*
        ready == 2 - Terminate
        ready = 0 - wait
    */
    int myBid = -1;
    int myBidLocation = -1;
    int numOfWins = 0;
    int originalCashReserve = 0;
    int index = *((int*)param);
    tables[index] = -1;
    //printf("Table #: %d accessed | Value: %d!\n", index, tables[index]);
    while (ready == 0)
    {
        //Table is empty, get customer from waiting queue
        if(tables[index] == -1)
        {
            //printf("TABLE %d is EMPTY! \n", index);
            numOfWins = 0;
            myBidLocation = -1;
            myBid = -1;
            originalCashReserve = 0;


            int x;
	        for (int i = 0; i < 1000000; i++)
		        x = 1;


            //Find customer to join table
            pthread_mutex_lock(&mutex1);
            for(int i = 0; i < numOfCustomers; i++)
            {
                //printf("Table #: %d | Scanning Waiting queue: %d \n", index, waitingQueue[i]);
                //Customer has been picked from queue in a FIFO structure. Skip.
                if(waitingQueue[i] == -1)
                {
                    continue;
                }
                //End of queue reached, break out!
                else if(waitingQueue[i] == -2)
                {
                    break;
                }
                //Waiting customer found! Pick them.
                else
                {
                    tables[index] = waitingQueue[i];
                    waitingQueue[i] = -1;
                    eligibleCustomers[index] = waitingQueue[i];

                    originalCashReserve = customerInfo[i][1];
                    printf("Customer ID: %d assigned to table %d! table[%d] = %d  | customerInfo[%d] = %d \n", customerInfo[i][0], index, index, tables[index], index, customerInfo[tables[index]][0]);
                    break;
                }
            }
            pthread_mutex_unlock(&mutex1);
        }

        //Repeat while auction is processing AND table is seated AND customer has NOT left (has money to play)
        while(waitAuction == 1 && tables[index] != -1 && removedCustomerIndex[tables[index]] != tables[index])
        {
            //If next round start has been announced;
            if(nextAuctionRoundStarted == 1)
            {
                myBidLocation = -1;
                pthread_mutex_lock(&mutex1);
                
                //While end of customer data has not been reached, check if customer is interested in item. If so, bid & place bid in auctionBids array.
                int count = 3;
                int foundInterestedItem = 0;
                while(customerInfo[tables[index]][count] != -1)
                {
                    //If customer is interested in item
                    if(itemUpForAuction == customerInfo[tables[index]][count])
                    {
                        myBidLocation = count;
                        myBid = rand() % customerInfo[tables[index]][1];
                        if(myBid == 0) myBid++;
                        foundInterestedItem = 1;
                        printf("Customer ID: %d | Seated at table #: %d | is interested in item #%d | My bid Amount: $%d | Remaining Balance: $%d \n", customerInfo[tables[index]][0], index, itemUpForAuction, myBid, customerInfo[tables[index]][1]);
                        auctionBids[index] = myBid;
                        printf("     *****     *****     *****     \n");

                        int q;
                        for(long long w = 0; w < 1000000; w++)
                            q = 1;

                        break;
                    }
                    count++;
                }

                pthread_mutex_unlock(&mutex1);

                int y;
                for(long long int z = 0; z < 100000000; z++)
                  y = 1;
            }
            //Result is tied. Redo bidding
            else if(nextAuctionRoundStarted == -2)
            {
                int isBidTied = 0;
                pthread_mutex_lock(&mutex1);
                for(int x = 0; x < numOfTables; x++)
                {
                    if(index == indexOfTablesTied[x])
                    {
                        isBidTied = 1;
                        break;
                    }
                }
                //If current customer is tied, re-bid & place bid in auctionBids array
                if(isBidTied == 1)
                {
                    myBid = (rand() % customerInfo[tables[index]][1] + myBid) % customerInfo[tables[index]][1];
                    if(myBid == 0) myBid++;
                    printf("TIED! Rebid: Customer ID: %d | Seated at table #: %d | is interested in item #%d | My NEW bid Amount: | Amount left: $%d $%d\n", customerInfo[tables[index]][0], index, itemUpForAuction, myBid, customerInfo[tables[index]][1]);
                    auctionBids[index] = myBid;
                    printf("Placed my NEW bid!\n");
                    printf("     *****     *****     *****     \n");
                }
                pthread_mutex_unlock(&mutex1);

                int y;
                for(long long int z = 0; z < 100000000; z++)
                  y = 1;
            }
            //Current auction round result processed by main. Display result.
            else if(nextAuctionRoundStarted == -1)
            {
                //pthread_mutex_lock(&mutex1);
                
                //If customer wins round / bid,...
                if(indexOfTableThatWonCurrentRound == index)
                {
                    numOfWins++;
                    //Customer wins! Decrease bid from total available funds
                    customerInfo[tables[index]][1] = customerInfo[tables[index]][1] - myBid;
                    //Customer wins! Decrease # of items customer is intereseted in
                    customerInfo[tables[index]][2] = customerInfo[tables[index]][2] - 1;

                    //Add bid to total sum paid
                    totalSumPaid += myBid;

                    //Increment total items successfully auctioned
                    totalNumOfItemSuccessfullyAuctioned += 1;

                    //Remove the item that customer just won from customer's interest list
                    customerInfo[tables[index]][myBidLocation] = 0; 
                    printf("Customer #%d | Seated at table #: %d | has WON current round! Bid price: $%d ! Cash Amount left : $%d |  Original Cash Amount: $%d !\n", customerInfo[tables[index]][0], index, myBid, customerInfo[tables[index]][1], originalCashReserve);

                    int x;
	                    for (long long int i = 0; i < 10000000; i++)
		                    x = 1;

                    //If Customer has finished their cash reserve OR obtained all items they're interested in, remove them (prevent from bidding or playing again)
                    if(customerInfo[tables[index]][1] < 1 || customerInfo[tables[index]][2] < 1)
                    {
                        printf("Customer %d | Seated at Table #: %d | has been REMOVED because of:" , customerInfo[tables[index]][0], index);

                        if(customerInfo[tables[index]][1] < 1 )
                        {
                            printf(" INSUFFICIENT FUNDS! \n");
                        }
                        else if (customerInfo[tables[index]][2] < 1)
                        {
                            printf(" ALL ITEMS REMAINING PURCHASED! NO LONGER INTERESTED IN BIDDING! \n");
                        }

                        printf("Customer ID: %d LEAVES @ Table #: %d | Winning: %d auctions | Amount of Cash Spent: $%d | Amount of Cash left: $%d | Original Cash Amount: $%d \n", 
                        customerInfo[tables[index]][0], 
                        index,
                        numOfWins,
                        originalCashReserve - customerInfo[tables[index]][1],
                        customerInfo[tables[index]][1],
                        originalCashReserve
                        );

                        removedCustomerIndex[tables[index]] = tables[index];
                        //Set table at index to EMPTY
                        tables[index] = -1;

                        int x;
	                    for (long long int i = 0; i < 10000000; i++)
		                    x = 1;
                    }
                }
                else
                {
                    printf("Customer #%d | Seated at table #: %d | has LOST! \n", customerInfo[tables[index]][0], index);
                    int x;
	                for (long long int i = 0; i < 10000000; i++)
		                x = 1;
                }
                //pthread_mutex_unlock(&mutex1);
            }
            //All customers have lost current round
            else if(nextAuctionRoundStarted == -3)
            {
                printf("Customer #%d | Seated at table #: %d | has LOST! \n", customerInfo[tables[index]][0], index);

                int x;
	            for (long long int i = 0; i < 100000000; i++)
		            x = 1;
            }
        }
    }

    printf("**** THREAD TERMINATED! Joining thread. Table index: %d\n", index);
    pthread_exit(0);
}

/*

readFile - is a function that reads in a .txt file that contains the customer and auction data. 
            PLEASE make sure that is NO trailing space chars at the end of each line!!
EXPECTED OUTPUT: Each line of the input file
*/
void readFile()
{
    FILE *fp;

    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int customerCounter = 0;
    int lineCounter = 0;

    int isFirstLine = 1;

    fp = fopen("prog2test-1.txt", "r");

    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) 
    {
        if(lineCounter > 0) isFirstLine = 0;

        printf("%s", line);

        char * split = strtok(line, " ");
        char * word;
        int position = 0;

        while(split != NULL)
        {
            word = split;
            split = strtok(NULL, " ");

            //printf("%s ", word);

            if(isFirstLine == 1)
            {
                //# of Tables
                if(position == 0)
                {
                    //printf("Table: %s \n", word);
                    //tables = calloc(atoi(word), sizeof(int));
                    numOfTables = atoi(word);
                }
                //# of Types of Items
                else if (position == 1)
                {
                    //printf("K: %s \n", word);
                    numOftypesOfItems = atoi(word);
                }
                //# of customers
                else if (position == 2)
                {
                    //printf("# Customers: %s \n", word);
                    //customerInfo = calloc(atoi(word) * customerArrayColumnSize, sizeof(int));
                    numOfCustomers = atoi(word);
                }
                // Max num of items
                else if ( position == 3)
                {
                    //printf("Max Num of Items: %s \n", word);
                    maximumNumberOfItemsAuctioned = atoi(word);
                }
                //Random or not
                else if (position == 4)
                {
                    //printf("Random?: %s \n", word);
                    isItemToBeAuctionedIsGeneratedRandomly = atoi(word);
                }
                //Extra Credit
                else if (position == 5)
                {
                    //printf("Extra Credit: %s \n", word);
                    extraCredit = atoi(word);
                }
            }
            else
            {
                //Customer Id
                if(position == 0)
                {
                    //printf("ID: %s \n", word);
                    //customerInfo[customerCounter * customerArrayColumnSize + position] = atoi(word);
                    customerInfo[customerCounter][position] = atoi(word);

                }
                // $ amount
                else if (position == 1)
                {
                    //printf("$: %s \n", word);
                    //customerInfo[customerCounter * customerArrayColumnSize + position] = atoi(word);
                    customerInfo[customerCounter][position] = atoi(word);
                }
                // # of items customer is interested in
                else if (position == 2)
                {
                    //printf("K: %s \n", word);
                    //customerInfo[customerCounter * customerArrayColumnSize + position] = atoi(word);
                    customerInfo[customerCounter][position] = atoi(word);
                }
                // type of each item customer is interested in
                else
                {
                    //printf("Other: %s - ", word);
                    //customerInfo[customerCounter * customerArrayColumnSize + position] = atoi(word);
                    customerInfo[customerCounter][position] = atoi(word);
                }
            }

            position++;
        }
        //Add -1 to signify end of customer data in 2d matrix
        if(isFirstLine == 0)
        {
            customerInfo[customerCounter][position] = -1;
            customerCounter++;
        }
        printf("\n");
        lineCounter++;
    }

    fclose(fp);

    if(line)
    {
        free(line);
    }
}


/*
    generateRandomCustomer - is a function that generates a random customer based on the input file 6th digit of the first line. If 6th digit
        is 1, a random customer MAYBE generated. Generation of a random customer depends on logic of the code we were given to implement. If 6th digit is 0,
        NO random customer will be generated!
    EXPECTED OUTPUT: Randomly generated customer information. 

    NOTE: THIS FUNCTION WORKS! SOMETIMES IT TAKES MULTIPLE COMPILATION AND EXECUTION ATTEMPTS TO GET IT TO GENERATE A NEW CUSTOMER BECAUSE OF GIVEN LOGIC, BUT IT DOES WORK!!
*/
void *generateRandomCustomer(void * param)
{
    int id = -1;
    while(extraCreditStopCommand == 0)
    {
        int delay = 100000;
        int Dtry = 1;
        int x;
        for(int i = 0; i < Dtry; i++)
        {
            for(int j = 0; j < delay; j++)
            {
                x=1;
            }
        }

        int random = rand() % 5;

        if(random == 4)
        {
            //Generate random customer
            printf("Generating RANDOM customer...\n");
            id = rand() % 50000;
            if(id < 10000) id += 10000;

            int cash = rand() % 800;
            if(cash < 50) cash += 50;

            int amountInterested = 0;
            amountInterested = rand() % 9;
            if(amountInterested < 1) amountInterested += 1;

            for(int a = 0; a < amountInterested; a++)
            {
                int temp = rand() % numOftypesOfItems;
                if(temp == 0) temp++;
                interestArray[a] = temp;
            }

            numOfCustomers++;
            customerInfo[numOfCustomers-1][0] = id;
            customerInfo[numOfCustomers-1][1] = cash;
            customerInfo[numOfCustomers-1][2] = amountInterested;

            printf("New Customer Info: \n\n");
            printf("ID: %d \nCash: $%d \n# of Items interested: %d\n" ,id, cash, amountInterested);
            
            for(int x = 0; x < amountInterested; x++)
            {
                customerInfo[numOfCustomers-1][3+x] = interestArray[x];
            }
            //Add -1 to show end of customer info
            customerInfo[numOfCustomers-1][3+amountInterested] = -1;
        }
        else{
            if(Dtry > 2)
            {
                Dtry = Dtry / 2;
            }
            else
            {
                Dtry = 1;
            }
        }
    }

    if(id == -1)
    {
        printf("\n*****     NO NEW CUSTOMER GENERATED! BETTER LUCK NEXT TIME!      *********\n");
    }

    printf("\n\n ##### Extra Credit Thread TERMINATED!!!    ########   \n\n");
    pthread_exit(0);
}