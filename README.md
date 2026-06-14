# Producer-Consumer Problem

## 1. 문제 개요

초기 코드에서는 두 개의 스레드가 공유 변수 x에 동시에 접근하여 증가 및 감소 연산을 수행하였다.

    x = val + 1;
    x = val - 1;

증가 및 감소 연산은 여러 단계로 이루어지므로 두 스레드가 동시에 실행될 경우 Race Condition이 발생할 수 있다.

본 과제에서는 공유 변수 x를 버퍼의 크기로 간주하고 Producer-Consumer 문제로 변환하여 Race Condition을 해결하였다.

또한 다음 조건을 만족하도록 구현하였다.

    0 <= x <= 30

즉,

- x = 0 : 버퍼가 비어있는 상태
- x = 30 : 버퍼가 가득 찬 상태

를 의미한다.

--------------------------------------------------

## 2. 구현 환경

- OS : Ubuntu 20.04 LTS
- Compiler : GCC 9.4.0
- Thread Library : POSIX Threads (pthread)

--------------------------------------------------

## 3. Condition Variable(CV) 버전

### 해결 방법

Condition Variable 버전은 mutex와 condition variable을 이용하여 Producer와 Consumer를 동기화하였다.

사용한 변수는 다음과 같다.

    pthread_mutex_t m;
    pthread_cond_t empty;
    pthread_cond_t fill;

Producer는 x가 MAX(30)에 도달하면 대기하며,

    while(x == MAX)
    {
        pthread_cond_wait(&empty, &m);
    }

Consumer가 값을 감소시키면 다시 실행된다.

Consumer는 x가 0이 되면 대기하며,

    while(x == 0)
    {
        pthread_cond_wait(&fill, &m);
    }

Producer가 값을 증가시키면 다시 실행된다.

또한 mutex를 사용하여 공유 변수 x에 대한 상호 배제를 보장하였다.

### 특징

- Race Condition 제거
- x가 항상 0~30 범위 유지
- Producer와 Consumer가 필요할 때만 대기
- 조건이 만족될 때까지 스레드를 효율적으로 대기시킬 수 있음
- 불필요한 Busy Waiting을 방지할 수 있음

--------------------------------------------------

## 4. Semaphore 버전

### 해결 방법

Semaphore 버전은 mutex semaphore와 counting semaphore를 사용하였다.

사용한 semaphore는 다음과 같다.

    sem_t m;
    sem_t empty;
    sem_t fill;

초기값은 다음과 같다.

    empty = 30
    fill = 0
    m = 1

Producer는

    sem_wait(&empty);
    sem_wait(&m);

를 수행하여 빈 공간을 확보한 후 x를 증가시킨다.

Consumer는

    sem_wait(&fill);
    sem_wait(&m);

를 수행하여 데이터가 존재하는 경우에만 x를 감소시킨다.

이를 통해 x가 0보다 작아지거나 30보다 커지는 상황을 방지하였다.

### 특징

- Race Condition 제거
- x가 항상 0~30 범위 유지
- Counting Semaphore를 이용한 동기화
- 버퍼 상태를 semaphore 값으로 직접 관리할 수 있음
- 하나의 동기화 도구만으로 상호 배제와 자원 관리를 모두 수행 가능

--------------------------------------------------

## 5. 성능 측정 방법

성능 측정을 위해 별도의 성능 측정용 소스 파일을 작성하였다.

### 제출 파일

    producer_consumer_cv.c
    producer_consumer_cv_performance.c

    producer_consumer_semaphore.c
    producer_consumer_semaphore_performance.c

동작 확인용 소스 파일은 x 값이 0~30 범위에서 변화하는 모습을 출력하도록 구현하였다.

성능 측정용 소스 파일은 출력문을 제거하여 I/O 오버헤드가 결과에 영향을 주지 않도록 하였다.

Producer와 Consumer는 각각 동일한 횟수의 연산을 수행하도록 설정하였다.

    #define ITER 1000000

실행 시간은 프로그램 시작부터 모든 스레드 종료 시점까지를 측정하였다.

    clock_gettime(CLOCK_MONOTONIC, ...)

함수를 사용하여 총 실행 시간을 측정하였다.

--------------------------------------------------

## 6. 성능 측정 결과

| Method | Execution Time (sec) |
|----------|----------|
| Condition Variable | 0.201597 |
| Semaphore | 0.197716 |

--------------------------------------------------

## 7. 결과 분석

실험 결과 본 환경(Ubuntu 20.04, GCC 9.4.0)에서는 Condition Variable 방식이 Semaphore 방식보다 더 빠른 실행 시간을 보였다.

본 구현은 실제 버퍼를 사용하지 않고 공유 변수 x의 값만 관리하는 구조로 구현되었다. 따라서 Semaphore 방식에서 사용된 empty, fill semaphore의 추가적인 관리 비용이 실행 시간 증가에 영향을 준 것으로 판단된다.

Condition Variable 방식은 조건이 만족되지 않는 경우에만 대기 상태로 전환되므로 본 실험 환경에서는 더 효율적으로 동작하였다.

다만 실행 결과는 운영체제의 스케줄링 정책, CPU 환경 및 시스템 부하에 따라 달라질 수 있다.

--------------------------------------------------

## 8. 결론 및 느낀점

이를 통해 Race Condition이 발생하는 원인과 이를 해결하기 위한 동기화 기법을 학습할 수 있었다.

Condition Variable과 Semaphore 모두 Producer-Consumer 문제를 해결할 수 있었으며, 공유 변수 x에 대한 상호 배제를 통해 데이터의 일관성을 유지할 수 있었다.

또한 Producer와 Consumer의 실행 조건을 적절히 제어함으로써 x가 항상 0~30 범위를 유지하도록 구현할 수 있었다.

성능 측정 결과 본 실험 환경에서는 Condition Variable 방식이 더 좋은 성능을 보였지만, 두 방식 모두 올바르게 동작하며 운영체제 환경에 따라 결과가 달라질 수 있음을 확인하였다.

이를 통해 동기화 문제에서는 단순히 Race Condition을 해결하는 것뿐만 아니라 성능과 자원 활용 효율 또한 함께 고려해야 한다는 점을 배울 수 있었다.
