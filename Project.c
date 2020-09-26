#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

// The maximum number of patients threads.
#define MAX_PATIENTS 50

// Function prototypes…
void *patient( void *num );
void *dentist(void *);

void randwait(int secs);

// Define the semaphores.

// waitingRoom Limits the # of patients allowed to enter the waiting room at one time
sem_t waitingRoom;

// chairs ensures mutually exclusive access to the chair
sem_t chairs;

// dentistFree will be free until a patient arrives
sem_t dentistFree;

// seatBelt is used to make the customer to wait until the patient treatment is done
sem_t seatBelt;

// Flag to stop the dentist thread when all patients have been treated
int allDone = 0;

int main(int argc, char *argv[]) {
pthread_t btid;
pthread_t tid[MAX_PATIENTS];
long RandSeed;
int i, numPatients, numChairs;
int Number[MAX_PATIENTS];

printf("Enter the number of Patients : ");
 scanf("%d",&numPatients) ;
printf("Enter the number of Chairs : "); 
scanf("%d",&numChairs);

// Make sure the number of threads is less than the number of patients we can support
if (numPatients > MAX_PATIENTS) {
printf("The maximum number of Patients is %d.\n", MAX_PATIENTS);
exit(-1);
}

// Initialize the numbers array.
for (i=0; i<MAX_PATIENTS; i++) {
Number[i] = i;
}

// Initialize the semaphores with initial values…
sem_init(&waitingRoom, 0, numChairs);
sem_init(&chairs, 0, 1);
sem_init(&dentistFree, 0, 0);
sem_init(&seatBelt, 0, 0);

// Create the dentist
pthread_create(&btid, NULL, dentist, NULL);

// Create the patients
for (i=0; i<numPatients; i++) {
pthread_create(&tid[i], NULL, patient, (void *)&Number[i]);
sleep(1);
}

// Join each of the threads to wait for them to finish.
for (i=0; i<numPatients; i++) {
pthread_join(tid[i],NULL);
sleep(1);
}

// When all of the patients are finished, kill the dentist thread
allDone = 1;
sem_post(&dentistFree); // Dentist will exit.
pthread_join(btid,NULL);
}

void *patient(void *number) {
int num = *(int *)number;

// Leave for the clinic and take some random amount of time to arrive.
printf("Patient %d leaving for clinic\n", num);
randwait(2);
printf("Patient %d arrived at clinic\n", num);

// Wait for space to open up in the waiting room…
sem_wait(&waitingRoom);
printf("Patient %d entering waiting room.\n", num);

// Wait for the bestodent chair to become free.
sem_wait(&chairs);

// The chair is free so give up your spot in the waiting room.
sem_post(&waitingRoom);

// Patient wants treatment
printf("Patient %d wants service from dentist\n", num);
sem_post(&dentistFree);

// Wait for the dentist to finish treatment
sem_wait(&seatBelt);

// Give up the chair.
sem_post(&chairs);
printf("Patient %d leaving clinic\n", num);
}

void *dentist(void *junk) 
{

while (!allDone) {

printf("The dentist is free\n");
sem_wait(&dentistFree);

// Skip this stuff at the end…
if (!allDone) 
{

// Take a random amount of time to checkup
printf("The patient is getting checkup from dentist\n");
randwait(2);
printf("The checkup is done\n");

// Release the patient
sem_post(&seatBelt);
}
else 
{
printf("The dentist is going home for the day.\n");
}
}
}

void randwait(int secs) {
int len;

// Generate a random number…
len = (int) ((1 * secs) + 1);
sleep(len);
}
