# Synchronisation

The course of the practical work is subdivided into three parts. The first part consists in validating the knowledge acquired in the previous task. You are asked to find a bug in the code provided in relation to the management of activities. The second part is dedicated to the manipulation and implementation of synchronisation mechanisms. The last part will focus on the management of interblocks.

**Importing the Git project from the practical work :**

1. Go to View->Command Palette menu or type "Shift + Control + p" => Enter.
2. Enter 'clone' and run the command git: clone
3. Enter URL **https://gitlab-df.imt-atlantique.fr/fila2-os/TP6.git**
4. Save the repository in Documents/sextant
5. At the message appearing at the bottom right, click on "Add to Workspace"

![Capture16](images/Capture16.PNG)

**Attention ⚠️**

* It is possible that on some systems (Linux, Mac Os) an error like this appears at compile time :
* i386-elf-ld: section .eh_frame VMA [00000000002011a0,00000000002011d3] overlaps section .bss VMA [000000000020119e,0000000000241fff]
* The problem comes from an overlap when linking the different .o files. To correct this problem, simply shift the beginning of the .bss section by modifying the support/sextant.lds file line 86
*.bss SIZEOF(.rodata) + ADDR(.rodata) +10000 :

## 1. Question on practical work n-1

### 1.1 Preemption (30 minutes)

Before exploiting the Thread class introduced at the end of the last session, we will analyse again the techniques used in the context of the scheduling of activities in a preemptive mode. Two activities are created in question 1 and must display a message: 'thread 1 scheduled', 'thread 2 scheduled'. The messages are not displayed, why?

## 2. La Synchronisation

### 2.1 Producer, consumer (30 minutes)

In this question we ask you to synchronise a producer/consumer via a semaphore. A producer produces data to be read. A consumer reads the data. In our case, the data produced by our consumer is a simple character. This character is written in an array at location n (a buffer). The value of n is incremented by the producer after each piece of data produced. The producer produces only 70 data. A consumer can only read the data produced. Use a single semaphore to ensure this behaviour.

**Remarks:**

* The producer and consumer codes are in the Applications/ProdCons directory
* The Semaphore code in sextant/Synchronisation/Semaphore

### 2.2 A rolling story (15 minutes)

We now want the producer to be able to produce an infinite amount of data. To do this we will implement a circular buffer. When it reaches the 70th data item, the Producer starts writing again at the beginning of the table. However, it can only write to a cell in the table if the data has been read by the consumer. has been read by the consumer. Propose a solution on paper to this new problem (do not implement it), you will probably need a second semaphore.

### 2.3 A date (30 minutes)

In the Application directory we provide you with a Hello class that displays a string on the screen. As in a previous tutorial, we want to be able to display the string Hello World on the screen. However, this display is produced by interleaving the executions of two activities. In the last tutorial, we showed that the preemptive scheduling system no longer allowed us to synchronise our activities.

Use two semaphores (sem1 and sem2) to coordinate the display of the two activities. Here you need to modify the code in Hello.cpp and in main. What we are asking you to implement is called a rendezvous.

## 3. Inside

### 3.1 Spinlock (15 minutes)

Observe how the spinlock works. We remind you that the spinlock is a simple synchronization mechanism based on active waiting.

Why does our code loop indefinitely on the execution of a single Activity? Add the necessary code to the semaphore class to ensure that it works correctly. correct operation.

**Indication:**

* protect the assignment to the value variable in the P() and V() methods using the class variable

### 3.2 Mutex (45 minutes)

Develop a Mutex in the associated directory. A mutex can be seen as a specialized (inheritance) semaphore. Indeed, a mutex is a semaphore initialized to 1. The mutex class must have two methods (at least) lock() (taking su mutex) and unlock() (freeing the mutex). A mutex can only be freed by the activity that took it.

**Indication:**

* Use among others the function thread_get_current()

### 3.3 A semaphorized keyboard (30 minutes)

As you can see, the keyboard driver codes are programmed on an active wait basis. Revisit these codes to no longer use active waiting based on the notion of semaphore.

### 3.4 A semaphorized keyboard that works (30 minutes)

As seen, when within main you call clavier->getchar(), your system throws an error. How do you explain it?

**Indice:**

* Blocking passive waiting, Scheduler

## End of the session