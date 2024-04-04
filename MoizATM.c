#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 10
#define ATM_CASH_LIMIT 5000
#define ATM_INITIAL_CASH 50000

int atm_cash = ATM_INITIAL_CASH;
sem_t atm_mutex;

void* atm_client(void* arg) {
  int id =  (int) arg;
  int password;
  int choice;
  int amount;

  printf("'\nATM client %d: enter password: ", id);
  scanf("%d", &password);

  // Check password
  if (password != 1234) {
    printf("ATM client %d: invalid password\n", id);
    return NULL;
  }

  printf("ATM client %d: select operation (1 - withdraw, 2 - check balance): ", id);
  scanf("%d", &choice);

  if (choice == 1) {
    printf("ATM client %d: enter amount to withdraw: ", id);
    scanf("%d", &amount);

    // Check if ATM has enough cash
    sem_wait(&atm_mutex);
    if (atm_cash < amount) {
      printf("ATM client %d: ATM is out of cash\n", id);
      sem_post(&atm_mutex);
      return NULL;
    }

    // Withdraw cash
    atm_cash -= amount;
    printf("ATM client %d: withdrew %d\n", id, amount);
    sem_post(&atm_mutex);
  } else if (choice == 2) {
    printf("ATM client %d: balance = %d\n", id, atm_cash);
  } else {
    printf("ATM client %d: invalid operation\n", id);
  }

  return NULL;
}

int main(int argc, char** argv) {
  pthread_t threads[NUM_THREADS];
  int thread_ids[NUM_THREADS];
  sem_init(&atm_mutex, 0, 1);

  // Create threads
  for (int i = 0; i < NUM_THREADS; i++) {
    thread_ids[i] = i + 1;
    pthread_create(&threads[i], NULL, atm_client, &thread_ids[i]);
  }

  // Wait for threads to finish
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}