## Introduction

Thread: a single sequential flow of control. 

Multiple threads means that the program has multiple execution points. The programmer can mostly view the threads running simultaneously, but sometimes must be aware the threads are not executed simultaneously (for example: in the case of writing to the same variable, or read and write threads co-exisits).

Threads are executed within a single address space, but each executes on a separate call stack with its own separate local variables.


## Why Use Concurrency?

1. The advent of multiprocessors. Naturally there are multiple execution points.
2. In the case of driving slow devices (disks, I/O, network, etc)
3. To match multi-tasking of human users.
4. When building a distributed system: there are shared resources, such as file server. 
5. Reduce the latency of operations. An example would be to add a node to a balanced tree. We can immediately return to the caller before re-balancing the tree. Do the rebalancing in a separate thread. 


## The Design of a Thread Facility

The primitives provided by the multi-threading facility:

1. thread creation
2. mutual exclusion
3. condition variables (waiting for events)
4. alerts

### Thread creation

"FORK": to create a new thread, and start that thread asynchronously at an invocation of the given procedure with the given arguments. When the procedure returns, the thread dies.

In the following example, the `a(x)` and `b(y)` execute in parallel, and assigns the result of `a(x)` to `q`.

```
VAR t: Thread;
t := FORK(a, x);
p := b(y);
q := JOIN(t);
```

`JOIN` is not necessary in all cases; sometimes threads don't return a result; other times, they use other synchronization arrangement.

### Mutual excluion

Mutex is a primitive that specifies a region of code that only one thread can execute there at any time.

```
TYPE Mutex;
LOCK mutex DO ... statements ... END
```

Mutex offers a binary access mechanism, but sometimes we need a more refined control, for example, we want to read through multiple threads but write only with one thread. This can be achieved through condition variables.

### Condition variables

Together with mutex, can achieve more complicated scheduling policies

```
TYPE Condition;
PROCEDURE Wait(m: Mutex; c: Condition);
PROCEDURE Signal(c: Condition);
PROCEDURE Broadcast(c: Condition);
```

The `Wait` operation atomically unlocks the mutex and blocks the thread (enqueued on the condition variable). The `Signal` operation does nothing unless there is a thread blocked on the condition variable, in which case it awakends at least one such blocked thread. The `Broadcast` operation is like `Signal`, except that it awakens all the threads currently blocked on the condition variable.

The process: The mutex associated with a condition variable protects the shared data. If a thread wants the resource, it locks the mutex and examines the shared data. If the resource is available, the thread continues. If not, the thread unlocks the mutex and blocks, by calling `Wait`. Later, when some other thread makes the resource available, it awakens the first thread by calling `Signal` or `Broadcast`. 

### Alerts

Alert is a mechanism for interrupting a particular thread, causing it to back out of some long-term wait or computation. 

```
EXCEPTION Alerted;
PROCEDURE Alert(t: Thread);
PROCEDURE AlertWait(m: Mutex, c: Condition);
PROCEDURE TestAlert(): BOOLEAN;
```

## Using A Mutex: Accessing Shared Data

### Unprotected data

Problem code:
```
VAR table: ARRAY [0...999] OF REFANY;
VAR i: [0...1000];

PROCEDURE Insert(r: REFANY);
BEGIN
  IF r # NIL THEN
    table[i] := r;  // 1
    i := i + 1;     // 2
  END
END Insert;
```

If one thread A calls `Insert(x)`, thread B calls `Insert(y)`, and the order of execution is that thread A executes (1), then thread B executes (1), then thread A executes (2), and then thread B executes (2). This order will lead to that only y is inserted to the table, and (2) was executed twite, so there will be a NIL slot left in the table.

The correct way is to add a lock around (1) and (2):

```
PROCEDURE Insert(r: REFANY);
BEGIN
  IF r # NIL THEN
    LOCK m DO 
      table[i] := r;  // 1
      i := i + 1;     // 2
    END
  END
END Insert;
```

### Invariants

When the data protected by a mutex is at all complicated, it is convenient to think of the mutex as protecting the invariant of the associated data. 

For example, in the code fragment above, the invariant is that `i` is the index of the first NIL element in "table", and all elements beyond index `i` are NIL. In this case, when thread B executes (1), the invariant is not hold. 

### Cheating

Need to carefully examine the software before assuming that a mutex is not needed for the simple data case. 

### Deadlocks involing only mutexes

One example of deadlocks:
```
  Thread A locks mutex M1
  Thread B locks mutex M2
  Thread A blocks trying to lock M2
  Thread B blocks trying to lock M1
```
In this case, we can apply a partial order to the acquisition of mutexes in the program. Partial order works when there are only mutexes. 

On designing complicated locking policy: *There more complicated locking becomes, the more likely you are to have some unsynchronized access to shared data. Having your program deadlock is always a preferrable risk to having your program give the wrong answer.* :)


### Poor performance through lock conflicts



