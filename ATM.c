#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 10
#define CASH_LIMIT 5000
#define INITIAL_BALANCE 50000

int cash_balance = INITIAL_BALANCE;
pthread_mutex_t balance_mutex;
pthread_cond_t client_ready;

void *atm_thread(void *arg) {
  int id = *((int *) arg);
  int password;
  int choice;
  int amount;

  pthread_mutex_lock(&balance_mutex);
  pthread_cond_wait(&client_ready, &balance_mutex);
  pthread_mutex_unlock(&balance_mutex);

  printf("Thread %d: Enter your password: ", id);
  scanf("%d", &password);
  printf("Thread %d: Enter your choice (1 - Check Balance, 2 - Withdraw): ", id);
  scanf("%d", &choice);

  pthread_mutex_lock(&balance_mutex);
  if (choice == 1) {
    printf("Thread %d: Your balance is %d\n", id, cash_balance);
  } else if (choice == 2) {
    printf("Thread %d: Enter the amount to withdraw: ", id);
    scanf("%d", &amount);
    if (amount > cash_balance) {
      printf("Thread %d: Insufficient balance\n", id);
    } else if (cash_balance - amount < CASH_LIMIT) {
      printf("Thread %d: ATM is out of cash\n", id);
    } else {
      cash_balance -= amount;
      printf("Thread %d: Successfully withdrew %d. Your balance is %d\n", id, amount, cash_balance);
    }
  }
  pthread_mutex_unlock(&balance_mutex);

  return NULL;
}

int main(void) {
  pthread_t threads[NUM_THREADS];
  pthread_mutex_init(&balance_mutex, NULL);
  pthread_cond_init(&client_ready, NULL);

  int i;
  int thread_ids[NUM_THREADS];

  for (i = 0; i < NUM_THREADS; i++) {
    thread_ids[i] = i;
    pthread_create(&threads[i], NULL, atm_thread, &thread_ids[i]);
  }

  for (i = 0; i < NUM_THREADS; i++) {
    pthread_cond_signal(&client_ready);
    pthread_join(threads[i], NULL);
  }

  pthread_cond_destroy(&client_ready);
  pthread_mutex_destroy(&balance_mutex);
  return 0;
}